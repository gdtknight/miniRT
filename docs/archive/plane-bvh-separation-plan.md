# Plane BVH Separation - Implementation Plan

**Date**: 2026-02-10  
**Issue**: Plane objects have infinite AABB (±1,000,000) which degrades BVH efficiency  
**Goal**: Separate planes from BVH to improve ray traversal performance

---

## Problem Analysis

### Current State

```
BVH Tree Structure:
└── Internal [depth=0] min(-1000000, ...) max(1000000, ...)  ← Plane 때문
    ├── Leaf [...] Objects: [cy-5, pl-2]  ← Plane이 거대 AABB 유발
    └── Leaf [...] Objects: [pl-1]
```

**원인**: `bounds_for_plane()` 함수에서 plane AABB를 ±1,000,000으로 설정
```c
static t_aabb bounds_for_plane(t_plane_data *p)
{
    double large = 1000000.0;
    return aabb_create(
        (t_vec3){-large, -large, -large},
        (t_vec3){large, large, large}
    );
}
```

**결과**:
- 거대한 AABB가 상위 노드로 전파
- 거의 모든 레이가 plane 포함 노드와 교차
- BVH pruning 효과 상실

### Impact

| 씬 타입 | BVH 효과 |
|---------|----------|
| Sphere/Cylinder only | 효과적 |
| 1-2 Planes | 감소 |
| 3+ Planes | 거의 없음 |

---

## Solution Design

### Approach: Plane Separate List

**핵심 아이디어**:
- BVH는 유한 객체(sphere, cylinder)만 포함
- Plane은 별도 리스트로 관리
- 교차 테스트 시 BVH + Plane 순차 검사

```
Before:
  objects[] = [sp-1, sp-2, pl-1, cy-1, pl-2, ...]
  BVH contains ALL objects (including planes with huge AABB)

After:
  finite_objects[] = [sp-1, sp-2, cy-1, ...]  → BVH
  planes[] = [pl-1, pl-2, ...]                → Linear scan
```

### Architecture

```
t_scene
├── objects (all objects, backward compatible)
├── bvh (finite objects only: sphere, cylinder)
├── plane_indices[] (indices of planes in objects[])  ← NEW
└── plane_count  ← NEW
```

**교차 테스트 흐름**:
```
trace_ray():
  1. BVH traverse (sphere, cylinder) → hit1
  2. Linear scan planes → hit2
  3. Return closer hit
```

---

## Critical Issues Identified (from review)

### Issue 1: plane_indices Memory Leak on Rebuild

**문제**: `scene_build_bvh()`가 여러 번 호출될 수 있음
- 기존 plane_indices를 free하지 않고 덮어쓰면 메모리 누수

**해결**:
```c
// collect_plane_indices() 시작 시
if (scene->plane_indices)
{
    free(scene->plane_indices);
    scene->plane_indices = NULL;
}
scene->plane_count = 0;
```

### Issue 2: Empty BVH Handling

**문제**: `finite_count == 0`일 때
- BVH build를 스킵하면 기존 BVH가 남아있을 수 있음
- 잘못된 traversal 발생 가능

**해결**:
```c
if (finite_count == 0)
{
    // 기존 BVH 트리 정리
    if (scene->bvh && scene->bvh->root)
    {
        bvh_node_destroy(scene->bvh->root);
        scene->bvh->root = NULL;
    }
    free(refs);
    return;
}
```

### Issue 3: NULL plane_indices Access

**문제**: plane_count > 0인데 plane_indices == NULL인 경우 (malloc 실패)

**해결**: 모든 plane 접근 시 이중 체크
```c
if (scene->plane_count > 0 && scene->plane_indices)
{
    // plane 검사
}
```

### Issue 4: trace_ray() Logic Inconsistency

**문제**: 현재 계획에서 BVH path에서만 check_planes() 호출
- BVH 없을 때 check_all_objects()로 빠지면 plane 포함됨
- "plane 분리" 의도와 충돌

**해결 방안 두 가지**:

**Option A**: check_all_objects()도 분리 (일관성)
```c
t_color trace_ray(t_scene *scene, t_ray *ray)
{
    t_hit hit;
    int found = 0;

    hit.distance = INFINITY;
    
    // BVH 또는 brute-force로 finite objects 검사
    if (scene->bvh && scene->bvh->enabled && scene->bvh->root)
        found = bvh_intersect(...);
    else
        found = check_finite_objects(...);  // plane 제외
    
    // 항상 plane 별도 검사
    if (check_planes(...))
        found = 1;
    
    if (found)
        return apply_lighting(...);
    return (t_color){0, 0, 0};
}
```

**Option B**: BVH 없을 때는 기존 로직 유지 (단순성)
- BVH disabled 시 check_all_objects() 그대로 사용
- plane 분리 효과는 BVH 활성화 시에만 적용

**권장**: Option B (단순성 우선)
- 복잡도 증가 최소화
- BVH disabled 시에는 어차피 brute-force이므로 분리 의미 없음

---

## Implementation Plan (Revised)

### Phase 1: Data Structure Changes

#### 1.1 Update t_scene (includes/minirt.h)

```c
typedef struct s_scene
{
    t_ambient       ambient;
    t_camera        camera;
    t_light         light;
    t_shadow_config shadow_config;
    t_object_list   objects;
    int             flags;
    t_bvh           *bvh;
    t_metrics       metrics;
    int             *plane_indices;   // NEW: indices of planes
    int             plane_count;      // NEW: number of planes
}   t_scene;
```

#### 1.2 Update scene lifecycle (src/scene/scene.c)

**scene_init_defaults()**:
```c
scene->plane_indices = NULL;
scene->plane_count = 0;
```

**scene_destroy()**:
```c
if (scene->plane_indices)
    free(scene->plane_indices);
```

---

### Phase 2: Plane Index Collection (src/spatial/bvh_init.c)

```c
/**
 * @brief Free existing plane indices and collect new ones.
 *
 * Must be called before each BVH rebuild.
 *
 * @param scene Scene containing objects.
 */
static void collect_plane_indices(t_scene *scene)
{
    int i;
    int plane_idx;

    // Free existing data (메모리 누수 방지)
    if (scene->plane_indices)
    {
        free(scene->plane_indices);
        scene->plane_indices = NULL;
    }
    scene->plane_count = 0;

    // Count planes
    i = 0;
    while (i < scene->objects.count)
    {
        if (scene->objects.items[i].type == OBJ_PLANE)
            scene->plane_count++;
        i++;
    }
    if (scene->plane_count == 0)
        return;
    
    // Allocate indices
    scene->plane_indices = malloc(sizeof(int) * scene->plane_count);
    if (!scene->plane_indices)
    {
        scene->plane_count = 0;  // 실패 시 count도 0으로
        return;
    }
    
    // Fill indices
    plane_idx = 0;
    i = 0;
    while (i < scene->objects.count)
    {
        if (scene->objects.items[i].type == OBJ_PLANE)
            scene->plane_indices[plane_idx++] = i;
        i++;
    }
}
```

---

### Phase 3: BVH Build Modification (src/spatial/bvh_init.c)

```c
/**
 * @brief Fill object references excluding planes.
 *
 * @param scene Scene containing objects.
 * @param refs Array to fill.
 * @return int Number of finite objects.
 */
static int fill_finite_refs(t_scene *scene, t_object_ref *refs)
{
    int i;
    int count;

    i = 0;
    count = 0;
    while (i < scene->objects.count)
    {
        if (scene->objects.items[i].type != OBJ_PLANE)
        {
            refs[count].index = i;
            count++;
        }
        i++;
    }
    return (count);
}

/**
 * @brief Clear existing BVH tree nodes.
 *
 * @param bvh BVH structure (root will be set to NULL).
 */
static void clear_bvh_tree(t_bvh *bvh)
{
    if (bvh && bvh->root)
    {
        bvh_node_destroy(bvh->root);
        bvh->root = NULL;
        bvh->total_nodes = 0;
        bvh->max_depth = 0;
    }
}

void scene_build_bvh(t_scene *scene)
{
    t_object_ref *refs;
    int finite_count;

    if (!(scene->flags & SCENE_BVH_ENABLED))
        return;
    if (scene->objects.count == 0)
        return;
    
    // 1. Collect plane indices (기존 데이터 free 포함)
    collect_plane_indices(scene);
    
    // 2. Allocate refs
    refs = malloc(sizeof(t_object_ref) * scene->objects.count);
    if (!refs)
        return;
    
    // 3. Fill finite object refs (planes 제외)
    finite_count = fill_finite_refs(scene, refs);
    
    // 4. Handle empty finite objects case
    if (finite_count == 0)
    {
        // 기존 BVH 정리 (stale data 방지)
        clear_bvh_tree(scene->bvh);
        free(refs);
        return;
    }
    
    // 5. Create BVH if needed
    if (!scene->bvh)
        scene->bvh = bvh_create();
    else
        clear_bvh_tree(scene->bvh);  // 재빌드 시 기존 트리 정리
    
    // 6. Build BVH
    if (scene->bvh)
        bvh_build(scene->bvh, refs, finite_count, scene);
    
    free(refs);
}
```

---

### Phase 4: Trace Ray Modification (src/render/trace.c)

```c
/**
 * @brief Test ray against planes in scene.
 *
 * @param scene Scene with plane_indices.
 * @param ray Ray to test.
 * @param hit In/out hit record.
 * @return int 1 if hit, 0 otherwise.
 */
static int check_planes(t_scene *scene, t_ray *ray, t_hit *hit)
{
    int         i;
    int         found;
    t_hit       temp;
    t_object    *obj;

    // 안전 체크
    if (scene->plane_count == 0 || !scene->plane_indices)
        return (0);
    
    found = 0;
    i = 0;
    while (i < scene->plane_count)
    {
        obj = &scene->objects.items[scene->plane_indices[i]];
        temp.distance = hit->distance;
        metrics_add_intersect_test(&scene->metrics);
        if (intersect_object_new(ray, obj, &temp))
        {
            *hit = temp;
            found = 1;
        }
        i++;
    }
    return (found);
}

t_color trace_ray(t_scene *scene, t_ray *ray)
{
    t_hit   hit;
    int     found;

    metrics_add_ray(&scene->metrics);
    hit.distance = INFINITY;
    found = 0;
    
    // BVH path: finite objects + separate plane check
    if (scene->bvh && scene->bvh->enabled && scene->bvh->root)
    {
        if (bvh_intersect(scene->bvh, *ray, &hit, scene))
            found = 1;
        if (check_planes(scene, ray, &hit))
            found = 1;
        if (found)
            return (apply_lighting(scene, &hit));
        return ((t_color){0, 0, 0});
    }
    
    // Fallback: brute-force (includes planes)
    if (check_all_objects(scene, ray, &hit))
        return (apply_lighting(scene, &hit));
    return ((t_color){0, 0, 0});
}
```

---

### Phase 5: Shadow Test Modification (src/lighting/shadow_test.c)

```c
/**
 * @brief Test shadow ray against planes.
 *
 * @param scene Scene with plane_indices.
 * @param ray Shadow ray.
 * @param hit Hit record.
 * @return int 1 if occluded, 0 otherwise.
 */
static int check_plane_shadow(t_scene *scene, t_ray *ray, t_hit *hit)
{
    int         i;
    t_object    *obj;

    if (scene->plane_count == 0 || !scene->plane_indices)
        return (0);
    
    i = 0;
    while (i < scene->plane_count)
    {
        obj = &scene->objects.items[scene->plane_indices[i]];
        metrics_add_shadow_intersect(&scene->metrics);
        if (intersect_object_new(ray, obj, hit))
            return (1);
        i++;
    }
    return (0);
}

// is_in_shadow() 내 수정 부분:
if (scene->bvh && scene->bvh->enabled && scene->bvh->root
    && scene->objects.count > SHADOW_BVH_THRESHOLD)
{
    if (bvh_intersect_any(scene->bvh, shadow_ray, mag, scene))
        return (1);
    return (check_plane_shadow(scene, &shadow_ray, &shadow_hit));
}
return (check_object_shadow(scene, &shadow_ray, &shadow_hit));
```

---

## File Changes Summary

| File | Changes | Type |
|------|---------|------|
| `includes/minirt.h` | +2 fields in t_scene | Modify |
| `src/scene/scene.c` | Init/free plane fields | Modify |
| `src/spatial/bvh_init.c` | collect_plane_indices(), fill_finite_refs(), clear_bvh_tree() | Modify |
| `src/render/trace.c` | check_planes(), update trace_ray() | Modify |
| `src/lighting/shadow_test.c` | check_plane_shadow(), update is_in_shadow() | Modify |

**Total**: 5 files, ~100-120 lines added/modified

---

## Expected Results (Conservative Estimates)

### BVH Structure Change

- Plane 노드가 제거되어 트리 구조 개선
- AABB가 실제 객체 범위로 축소
- 정확한 개선율은 씬 구성에 따라 다름

### Performance (검증 필요)

| Metric | Expected Change | Note |
|--------|-----------------|------|
| BVH nodes | 감소 | Plane 노드 제거 |
| Pruning 효율 | 개선 | AABB 타이트해짐 |
| Plane test 비용 | 추가 | Linear O(n) |
| Overall | 개선 가능 | **측정으로 확인 필요** |

**주의**: 성능 개선율은 예측치이며, 실제 벤치마크로 검증 필요

---

## Risks and Mitigations

| Risk | Mitigation |
|------|------------|
| 재빌드 시 메모리 누수 | collect_plane_indices()에서 기존 데이터 free |
| 빈 BVH 처리 | clear_bvh_tree()로 명시적 정리 |
| NULL plane_indices 접근 | 이중 체크 (count && indices) |
| Fallback 경로 일관성 | BVH 없을 때는 기존 로직 유지 (단순성) |
| scene->bvh NULL 상태에서 clear 호출 | clear_bvh_tree()는 null-safe (if bvh && bvh->root) |
| bvh_build() 실패 | plane_indices는 유효하나 분리 효과 없음 (허용) |
| Shadow threshold mismatch | objects.count → 그대로 유지 (아래 설계 결정 참조) |

---

## Design Decisions (from review)

### 1. Shadow BVH Threshold 기준

**현재**: `scene->objects.count > SHADOW_BVH_THRESHOLD`  
**이슈**: Plane 분리 후 BVH 대상은 finite objects만이므로 불일치 발생

**결정**: **기존 objects.count 유지**

이유:
- Threshold 목적: "적은 객체에서 BVH 오버헤드 방지"
- Plane은 교차 테스트 비용이 낮음 (O(1) dot product)
- 전체 객체 수 기준이 더 보수적 (안전)
- finite_count로 변경 시 기존 동작과 다른 결과 발생 가능

대안 (미채택):
```c
// finite_count 기준으로 변경 시
&& (scene->objects.count - scene->plane_count) > SHADOW_BVH_THRESHOLD
```

**향후 재평가 가능**: Plane 수가 많은 씬에서 threshold가 과도하게 보수적일 경우, finite_count 기준으로 변경 고려

### 2. bvh_build() 실패 시 동작

**시나리오**: plane_indices 수집 완료 → bvh_build() 실패 → scene->bvh NULL

**결정**: **허용 (degraded mode)**

- plane_indices는 유효하게 남음
- trace_ray()에서 BVH 없으면 check_all_objects() 사용
- check_all_objects()가 모든 객체(plane 포함) 검사하므로 정확성 유지
- 성능만 저하, 렌더링 결과는 정확

**코드 흐름**:
```
bvh_build() 실패 시:
  trace_ray():
    BVH 조건 불충족 → check_all_objects() (plane 포함)
    plane 분리 효과 없음 (성능 저하)
    렌더링 정확성은 유지됨
```

### 3. 동적 객체 변경

**현재 코드베이스**: 객체 리스트가 런타임에 변경되지 않음
- 파싱 후 objects[] 고정
- scene_build_bvh()는 초기화 시 1회만 호출

**plane_indices 동기화 문제**: 해당 없음 (현재 구조)

**향후 동적 변경 추가 시**:
- scene_build_bvh() 재호출 필요
- collect_plane_indices()가 이미 기존 데이터 free 처리하므로 안전

---

## Testing Plan

### Memory Safety

```bash
valgrind --leak-check=full ./miniRT scenes/valid/valid_us01_basic.rt
# 메모리 누수 없음 확인

# BVH 재빌드 시나리오 (카메라 이동 등)
valgrind --leak-check=full ./miniRT scenes/perf/perf_all_objects.rt
```

### Functional Tests

1. **Plane-only scene**: BVH 비어있음, plane만 렌더링
2. **No-plane scene**: 기존과 동일 동작
3. **Mixed scene**: BVH + plane 모두 정상
4. **Plane closest hit**: Plane이 가장 가까운 경우 정상 처리

### Regression Tests

```bash
for f in scenes/valid/*.rt; do
    ./miniRT "$f" --render-once
    # 기존과 동일한 렌더링 결과
done
```

---

## Implementation Order

1. ☐ Phase 1: Data structure changes
2. ☐ Phase 2: Plane collection with memory safety
3. ☐ Phase 3: BVH build with empty handling
4. ☐ Phase 4: trace_ray() modification
5. ☐ Phase 5: shadow_test modification
6. ☐ Memory leak testing (valgrind)
7. ☐ Functional testing
8. ☐ Performance benchmarking
9. ☐ Commit

---

## Notes

- Plane 교차 테스트는 O(1) dot product로 빠름
- 일반적으로 씬당 plane 수가 적으므로 (< 10) linear scan 비용 미미
- BVH disabled 시에는 기존 brute-force 유지 (불필요한 분기 추가 방지)
- 성능 개선율은 씬 구성에 따라 다르며, 반드시 측정으로 확인해야 함

---

## Pre-Implementation Checklist

구현 시작 전 확인 필수:

- [ ] `clear_bvh_tree(scene->bvh)` 호출 시 scene->bvh가 NULL일 수 있음 (함수 내부에서 null-safe 처리 확인)
- [ ] `collect_plane_indices()` 내 malloc 실패 시 plane_indices = NULL, plane_count = 0 보장
- [ ] bvh_build() 실패 시 degraded mode 동작 허용 (테스트 필요)
- [ ] 동적 객체 변경 미사용 확인 (현재 코드베이스)

---

## Review History

| Date | Reviewer | Status | Key Changes |
|------|----------|--------|-------------|
| 2026-02-10 | Initial | Draft | 기본 설계 |
| 2026-02-10 | Review 1 | Revised | 메모리 누수, 빈 BVH 처리 추가 |
| 2026-02-10 | Review 2 | Revised | Shadow threshold, bvh_build 실패, 동적 객체 설계 결정 추가 |
