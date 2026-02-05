# Research: BVH Miss Fallback Removal (P0)

## 1. BVH 커버리지 검증

**질문**: BVH가 모든 scene object를 포함하는가?

**결론**: Yes — `scene_build_bvh()`가 `scene->objects.count` 전체를 BVH에 전달.

**근거**:
- `src/spatial/bvh_init.c:72`: `total = scene->objects.count;`
- `src/spatial/bvh_init.c:75-80`: `refs` 배열을 `total` 크기로 할당, 모든 인덱스를 순차 등록
- `bvh_build(bvh, refs, total, scene)` 호출로 전체 오브젝트를 BVH에 빌드

따라서 BVH miss는 "해당 ray가 어떤 오브젝트와도 교차하지 않음"을 의미하며, `check_all_objects()` 폴백도 동일하게 miss를 반환한다.

## 2. BVH enabled + root null 엣지 케이스

**질문**: `scene->bvh->enabled == 1`이면서 `bvh->root == NULL`인 상태가 가능한가?

**결론**: Yes — `bvh_create()`가 즉시 `enabled = 1`을 설정하며, 이후 `bvh_build_recursive()`가 malloc 실패 등으로 NULL을 반환하면 root는 NULL인 채로 enabled가 유지된다.

**코드 근거**:
- `src/spatial/bvh_lifecycle.c:30`: `bvh->enabled = 1;` (bvh_create 내부)
- `src/spatial/bvh_build_core.c:108`: `bvh->root = bvh_build_recursive(...)` — 반환값이 NULL일 수 있음
- `bvh_build()`에서 root가 NULL이어도 enabled를 해제하는 코드가 없음

**영향**: BVH-on 조건을 `scene->bvh && scene->bvh->enabled`만으로 판단하면, root가 NULL일 때 `bvh_intersect()` → 내부 가드(`!bvh->root`)로 0 반환 → 제안된 설계에서 즉시 black 반환. 이는 현재 동작(폴백으로 정상 렌더)에서 회귀.

**대응**: BVH-on 조건에 `scene->bvh->root` 검사를 추가하여, root가 NULL이면 `check_all_objects()` 경로로 진입.

## 3. intersect_tests 메트릭 정의

**질문**: `intersect_tests`는 무엇을 카운트하는가?

**결론**: 오브젝트 교차 테스트만 카운트. AABB 박스 테스트는 별도 메트릭(`nodes_visited`).

**호출 위치**:
- `src/spatial/bvh_traverse.c:43` — BVH leaf 노드에서 오브젝트 교차 시
- `src/render/trace.c:43` — brute-force `check_all_objects()` 내 오브젝트 교차 시

AABB 관련 메트릭은 `metrics_add_bvh_node_visit()`(방문)과 `metrics_add_bvh_skip()`(미교차 건너뜀)으로 별도 추적.

## 4. 변경 설계

### Before (현재 코드)

```c
t_color trace_ray(t_scene *scene, t_ray *ray)
{
    t_hit   hit;
    int     found;

    metrics_add_ray(&scene->metrics);
    found = 0;
    hit.distance = INFINITY;
    if (scene->bvh && scene->bvh->enabled)
    {
        if (bvh_intersect(scene->bvh, *ray, &hit, scene))
            found = 1;
    }
    if (!found)                                    // ← 문제: BVH miss 시 폴백
    {
        if (check_all_objects(scene, ray, &hit))
            found = 1;
    }
    if (found)
        return (apply_lighting(scene, &hit));
    return ((t_color){0, 0, 0});
}
```

### After (수정안)

```c
t_color trace_ray(t_scene *scene, t_ray *ray)
{
    t_hit   hit;

    metrics_add_ray(&scene->metrics);
    hit.distance = INFINITY;
    if (scene->bvh && scene->bvh->enabled && scene->bvh->root)
    {
        if (bvh_intersect(scene->bvh, *ray, &hit, scene))
            return (apply_lighting(scene, &hit));
        return ((t_color){0, 0, 0});
    }
    if (check_all_objects(scene, ray, &hit))
        return (apply_lighting(scene, &hit));
    return ((t_color){0, 0, 0});
}
```

**변경점**:
- `found` 변수 제거 (4 → 3 변수)
- BVH 유효 조건: `scene->bvh && scene->bvh->enabled && scene->bvh->root` (root null 가드 추가)
- BVH 유효 시: `bvh_intersect()` 결과만 사용, 즉시 return
- BVH 무효/disabled 시: `check_all_objects()` 경로 유지 (else 분기)
- 줄 수: 10줄 body (25줄 이하)
- Norm 준수: 변수 1개 (`t_hit`), 인자 2개, return 괄호 포함

## 5. check_all_objects() 처리

`check_all_objects()`는 BVH 무효/disabled 경로에서 여전히 필요하므로 삭제하지 않는다.
BVH 유효 상태에서는 호출되지 않으므로 해당 경로의 metrics 이중 카운트가 해소된다.

## 6. 대안 검토

| 대안 | 판단 | 이유 |
|------|------|------|
| 폴백 유지 + metrics 분리만 | 기각 | 불필요한 CPU 낭비 (miss ray에서 2x 탐색) |
| check_all_objects() 완전 삭제 | 기각 | BVH 무효/disabled 경로 필요 |
| BVH를 항상 enabled로 강제 | 기각 | 디버깅/비교 용도로 off 필요 |
| enabled만 검사 (root 무시) | 기각 | root null 시 all-black 회귀 |
| enabled + root 검사 (현재 설계) | 채택 | 최소 변경, 엣지 케이스 안전, 명확한 분기 |
