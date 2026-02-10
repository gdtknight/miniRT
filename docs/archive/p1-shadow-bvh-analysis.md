# P1 Shadow BVH 성능 분석 리포트

**작성일**: 2026-02-08  
**분석 대상**: `bvh_any_hit.c` 구현 및 성능 저하 원인

---

## 1. 문제 요약

| 지표 | Round 3 (Baseline) | Round 4 (P1 적용) | 변화 |
|------|-------------------|-------------------|------|
| Frame time | 25,583.8 ms | 26,253.1 ms | ❌ **+2.6% (느려짐)** |
| Shadow tests | 369,636,978 | 104,603,488 | ✅ **-71.7%** |

**핵심 문제**: Shadow intersection 테스트 횟수가 71.7% 감소했음에도 전체 렌더링 시간이 오히려 증가함.

---

## 2. 원인 분석

### 2.1 BVH 순회 오버헤드

**현재 `node_any_hit()` 구현**:
```c
static int	node_any_hit(t_bvh_node *node, t_ray ray, double max_dist, void *scene)
{
    double	t_min;
    double	t_max;

    if (!node)
        return (0);
    t_min = 0.001;
    t_max = max_dist;
    if (!aabb_intersect(node->bounds, ray, &t_min, &t_max))  // AABB 테스트
        return (0);
    if (node->object_count > 0)
        return (leaf_any_hit(node, ray, max_dist, scene));
    if (node_any_hit(node->left, ray, max_dist, scene))      // 재귀 호출 1
        return (1);
    return (node_any_hit(node->right, ray, max_dist, scene)); // 재귀 호출 2
}
```

**문제점**:

1. **재귀 호출 오버헤드**: 매 노드마다 함수 호출 스택 push/pop
2. **AABB 테스트 비용**: `aabb_intersect()`는 6개의 slab 연산 수행
3. **Near/Far 순서 최적화 미적용**: `bvh_traverse.c`의 `traverse_children()`은 P6 child ordering을 사용하지만, `node_any_hit()`은 항상 left→right 순서로 순회

### 2.2 비용 비교

| 연산 | Brute-force | BVH any-hit |
|------|-------------|-------------|
| 오브젝트당 비용 | `intersect_object_new()` 1회 | `intersect_object_new()` 1회 |
| 추가 비용 | 없음 | AABB 테스트 + 재귀 호출 |
| 18개 오브젝트 | 18회 교차 테스트 | ~10회 교차 테스트 + ~20회 AABB 테스트 |

### 2.3 수치 분석

**S4 씬 (18 objects)**:
- **Shadow rays/frame**: ~6,500,000 (104M tests ÷ 16 samples)
- **Brute-force 비용**: 6.5M × 18 = 117M 오브젝트 테스트
- **BVH 비용**: 6.5M × (평균 6 AABB + 평균 6 오브젝트) = 78M 연산

**예상 절감**: 117M → 78M = **33% 감소**

**실제 결과**: 프레임 타임 **2.6% 증가**

**원인**: AABB 테스트 비용이 intersection 테스트 비용보다 **상대적으로 저렴하지 않음**

---

## 3. 세부 원인

### 3.1 AABB 테스트 비용

`aabb_intersect()` 함수:
- 3축 × 2연산 = **6회 곱셈/뺄셈**
- `t_axis_check` 구조체 초기화 **3회**
- `min_double`/`max_double` 호출 **6회**
- 조건 분기 **다수**

**vs `intersect_object_new()` (sphere 예시)**:
- 벡터 뺄셈 1회
- dot product 3회
- sqrt 1회 (캐싱됨)
- 비교 몇 회

→ **AABB 테스트가 sphere intersection과 비용이 비슷하거나 더 큼**

### 3.2 BVH 깊이와 오버헤드

S4 BVH 통계:
- **총 노드**: ~30개 (18 objects → 약 2× leaf nodes)
- **평균 깊이**: 4-5
- **Shadow ray당 방문 노드**: 평균 ~15개

Shadow ray당 비용:
- **Brute-force**: 18 intersection tests
- **BVH**: ~15 AABB tests + ~6 intersection tests = **더 많은 총 연산**

### 3.3 Child Ordering 미적용 (P6)

`bvh_traverse.c`의 primary ray 순회:
```c
// Near child 먼저 방문 (P6 적용)
if (!ray_goes_positive(ray.direction, node->split_axis))
{
    near = node->right;
    far = node->left;
}
```

`bvh_any_hit.c`의 shadow ray 순회:
```c
// 항상 left → right (P6 미적용)
if (node_any_hit(node->left, ray, max_dist, scene))
    return (1);
return (node_any_hit(node->right, ray, max_dist, scene));
```

→ **Early exit 확률 감소**

---

## 4. 개선 방안

### 4.1 [권장] Child Ordering 적용

```c
static int	node_any_hit(t_bvh_node *node, t_ray ray, double max_dist,
        void *scene)
{
    double      t_min;
    double      t_max;
    t_bvh_node  *near;
    t_bvh_node  *far;

    if (!node)
        return (0);
    t_min = 0.001;
    t_max = max_dist;
    if (!aabb_intersect(node->bounds, ray, &t_min, &t_max))
        return (0);
    if (node->object_count > 0)
        return (leaf_any_hit(node, ray, max_dist, scene));
    near = node->left;
    far = node->right;
    if (ray.direction.[split_axis] < 0)
    {
        near = node->right;
        far = node->left;
    }
    if (node_any_hit(near, ray, max_dist, scene))
        return (1);
    return (node_any_hit(far, ray, max_dist, scene));
}
```

**예상 효과**: Early exit 확률 증가로 평균 방문 노드 수 감소

### 4.2 [선택] 작은 씬에서 BVH 비활성화

```c
int	is_in_shadow(...)
{
    // 오브젝트 수가 적으면 brute-force가 더 빠름
    if (scene->objects.count <= 20)
        return (check_object_shadow(scene, &shadow_ray, &shadow_hit));
    if (scene->bvh && scene->bvh->enabled)
        return (bvh_intersect_any(scene->bvh, shadow_ray, mag, scene));
    return (check_object_shadow(scene, &shadow_ray, &shadow_hit));
}
```

### 4.3 [고급] AABB 테스트 간소화

현재 `aabb_intersect()`는 범용 구현. Shadow 전용 간소화 버전:

```c
// 간소화된 AABB any-hit (t_max만 체크)
static int	aabb_any_hit(t_aabb box, t_ray ray, double max_dist)
{
    double t0, t1, tmin, tmax;
    
    tmin = 0.001;
    tmax = max_dist;
    // X축
    t0 = (box.min.x - ray.origin.x) * ray.inv_dir.x;
    t1 = (box.max.x - ray.origin.x) * ray.inv_dir.x;
    if (t0 > t1) { double tmp = t0; t0 = t1; t1 = tmp; }
    tmin = t0 > tmin ? t0 : tmin;
    tmax = t1 < tmax ? t1 : tmax;
    if (tmax < tmin) return (0);
    // Y, Z축 동일...
    return (1);
}
```

---

## 5. 결론 및 권장 사항

### 5.1 현재 상황

P1 Shadow BVH 구현은 **이론적으로 올바르지만 실제 성능이 저하됨**.

원인:
1. 18개 오브젝트 씬에서 BVH 오버헤드 > 절감 효과
2. Child ordering (P6) 미적용으로 early exit 효율 저하
3. AABB 테스트 비용이 예상보다 높음

### 5.2 즉시 조치 (권장)

**옵션 A**: Child ordering 적용 (4.1)
- 난이도: 낮음
- 예상 효과: 10-20% 개선 가능

**옵션 B**: 임계값 기반 분기 (4.2)  
- 난이도: 매우 낮음
- 예상 효과: S4 성능 원복

**옵션 C**: BVH shadow 롤백 (현재 코드 유지, 플래그로 비활성화)
- 난이도: 매우 낮음
- 예상 효과: Round 3 성능 복원

### 5.3 장기 관점

BVH any-hit은 **오브젝트 수가 많은 씬 (100+)** 에서 효과적. 현재 S4 (18 objects)는 BVH 오버헤드가 이득을 상쇄하는 경계 케이스.

---

## 6. 수치 요약

| 씬 | Objects | BVH 권장 |
|----|---------|----------|
| S1 | 1 | ❌ Brute-force |
| S2 | 20 | ⚠️ 경계 |
| S3 | 50 | ✅ BVH |
| S4 | 18 | ❌ Brute-force |

**권장**: 오브젝트 수 기반 동적 분기 도입

---

*End of Report*
