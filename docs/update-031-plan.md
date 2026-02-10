# Shadow BVH 최적화 계획

**목표**: Shadow any-hit 성능 개선 (2가지 항목)  
**작성일**: 2026-02-08

---

## 문제 요약

| 지표 | Baseline | 현재 | 변화 |
|------|----------|------|------|
| Frame time | 25,583 ms | 26,253 ms | ❌ +2.6% |
| Shadow tests | 369M | 104M | ✅ -71.7% |

Shadow test 감소에도 불구하고 frame time 증가 → BVH 오버헤드 문제

---

## 작업 항목

### Task 1: Child Ordering 적용 (우선순위 High)

**현황**: `node_any_hit()`이 항상 left→right 순서로 순회  
**목표**: ray direction 기반 near/far 순서로 변경하여 early exit 확률 증가

#### 1.1 수정 파일
- `src/spatial/bvh_any_hit.c`

#### 1.2 구현 계획

**Before** (현재):
```c
static int	node_any_hit(t_bvh_node *node, t_ray ray, double max_dist,
		void *scene)
{
	// ...
	if (node_any_hit(node->left, ray, max_dist, scene))
		return (1);
	return (node_any_hit(node->right, ray, max_dist, scene));
}
```

**After** (수정):
```c
static int	ray_goes_positive(t_vec3 dir, int axis)
{
	if (axis == 0)
		return (dir.x > 0);
	if (axis == 1)
		return (dir.y > 0);
	return (dir.z > 0);
}

static int	node_any_hit(t_bvh_node *node, t_ray ray, double max_dist,
		void *scene)
{
	t_bvh_node	*near;
	t_bvh_node	*far;
	// ... AABB 체크 ...
	
	near = node->left;
	far = node->right;
	if (!ray_goes_positive(ray.direction, node->split_axis))
	{
		near = node->right;
		far = node->left;
	}
	if (node_any_hit(near, ray, max_dist, scene))
		return (1);
	return (node_any_hit(far, ray, max_dist, scene));
}
```

#### 1.3 Norm 제약 확인
- 현재 `node_any_hit()`: 변수 4개 (t_min, t_max, + 암묵적 2개)
- near/far 추가 시 변수 6개 → ❌ 5개 제한 위반
- **해결**: 헬퍼 함수 `get_near_far()`로 near/far 결정 로직 분리

**헬퍼 함수**:
```c
static void	get_near_far(t_bvh_node *node, t_ray ray,
		t_bvh_node **near, t_bvh_node **far)
{
	*near = node->left;
	*far = node->right;
	if (!ray_goes_positive(ray.direction, node->split_axis))
	{
		*near = node->right;
		*far = node->left;
	}
}
```

**수정된 node_any_hit()** (변수 4개 유지):
```c
static int	node_any_hit(t_bvh_node *node, t_ray ray, double max_dist,
		void *scene)
{
	double		t_min;
	double		t_max;
	t_bvh_node	*near;
	t_bvh_node	*far;

	if (!node)
		return (0);
	metrics_add_bvh_node_visit(&((t_scene *)scene)->metrics);
	t_min = 0.001;
	t_max = max_dist;
	if (!aabb_intersect(node->bounds, ray, &t_min, &t_max))
	{
		metrics_add_bvh_skip(&((t_scene *)scene)->metrics);
		return (0);
	}
	if (node->object_count > 0)
		return (leaf_any_hit(node, ray, max_dist, scene));
	get_near_far(node, ray, &near, &far);
	if (node_any_hit(near, ray, max_dist, scene))
		return (1);
	return (node_any_hit(far, ray, max_dist, scene));
}
```

**주의**: 변수 4개 (t_min, t_max, near, far) → Norm 5개 제한 ✅

#### 1.4 최종 파일 구조
```
bvh_any_hit.c (5 functions max):
1. ray_goes_positive()  - static, 기존 bvh_traverse.c에서 복사
2. get_near_far()       - static, near/far 결정 헬퍼  
3. leaf_any_hit()       - static, 리프 노드 처리
4. node_any_hit()       - static, 재귀 순회 (child ordering 적용)
5. bvh_intersect_any()  - public, 진입점
```

---

### Task 2: Object Count Threshold 적용 (우선순위 High)

**현황**: 모든 씬에서 BVH shadow 사용  
**목표**: 작은 씬 (≤20 objects)에서는 brute-force로 폴백

#### 2.1 수정 파일
- `src/lighting/shadow_test.c`

#### 2.2 구현 계획

**Before** (현재):
```c
if (scene->bvh && scene->bvh->enabled && scene->bvh->root)
	return (bvh_intersect_any(scene->bvh, shadow_ray, mag, scene));
return (check_object_shadow(scene, &shadow_ray, &shadow_hit));
```

**After** (수정):
```c
#define SHADOW_BVH_THRESHOLD 20

if (scene->bvh && scene->bvh->enabled && scene->bvh->root
	&& scene->objects.count > SHADOW_BVH_THRESHOLD)
	return (bvh_intersect_any(scene->bvh, shadow_ray, mag, scene));
return (check_object_shadow(scene, &shadow_ray, &shadow_hit));
```

#### 2.3 Norm 제약 확인
- 매크로 정의 또는 상수 20 직접 사용
- 기존 함수 구조 변경 없음 ✅

#### 2.4 Threshold 값 근거
- S4 씬: 18 objects → brute-force 예상
- BVH 오버헤드 손익분기: ~20-30 objects (리서치 리포트 기준)
- 보수적으로 20 선택

---

## 작업 체크리스트

- [ ] Task 1.1: `ray_goes_positive()` 함수 추가 (bvh_any_hit.c)
- [ ] Task 1.2: `get_near_far()` 헬퍼 함수 추가 (bvh_any_hit.c)
- [ ] Task 1.3: `node_any_hit()` child ordering 적용
- [ ] Task 1.4: norminette 검증
- [ ] Task 2.1: `is_in_shadow()` threshold 조건 추가
- [ ] Task 2.2: norminette 검증
- [ ] Task 3: 빌드 검증 (make re)
- [ ] Task 4: 벤치마크 실행 및 결과 비교

---

## 예상 결과

| 씬 | Objects | 적용 경로 | 예상 효과 |
|----|---------|-----------|-----------|
| S1 | 1 | brute-force | 변화 없음 |
| S2 | 20 | brute-force | 오버헤드 제거 |
| S3 | 50 | BVH + ordering | early exit ↑ |
| S4 | 18 | brute-force | **성능 복원 예상** |

---

## 리스크

1. **Child ordering 버그**: near/far 반전 시 그림자 오류 → leaf_any_hit 결과는 동일하므로 논리적 정합성 유지
2. **Threshold 튜닝**: 20이 최적값이 아닐 수 있음 → 추후 프로파일링으로 조정 가능
3. **Norm 준수**: 변수 4개 (t_min, t_max, near, far) + 헬퍼 함수 분리로 해결

---

*Plan created: 2026-02-08*
