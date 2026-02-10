# Shadow BVH 최적화 계획

**목표**: Shadow any-hit 성능 개선 (2가지 항목)  
**작성일**: 2026-02-08  
**상태**: 구현 대기

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

#### 1.2 현재 코드 상태
```c
// bvh_any_hit.c (3 functions)
// - metrics 호출 포함 (metrics_add_bvh_node_visit, metrics_add_bvh_skip, metrics_add_shadow_intersect)
// - child ordering 미적용 (left→right 고정 순회)

1. leaf_any_hit()       - static, 리프 노드 처리
2. node_any_hit()       - static, 재귀 순회 (변수: t_min, t_max)
3. bvh_intersect_any()  - public, 진입점
```

#### 1.3 구현 방향
- **헬퍼 분리**: `get_near_far()`로 near/far 결정 로직 분리 (가독성 향상, `bvh_traverse.c`와 패턴 통일)

**추가할 함수 (2개)**:
```c
static int	ray_goes_positive(t_vec3 dir, int axis)
{
	if (axis == 0)
		return (dir.x > 0);
	if (axis == 1)
		return (dir.y > 0);
	return (dir.z > 0);
}

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

**수정할 함수 (node_any_hit)**:
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

#### 1.4 최종 파일 구조
```
bvh_any_hit.c (5 functions):
1. ray_goes_positive()  - static, NEW
2. get_near_far()       - static, NEW
3. leaf_any_hit()       - static, 기존 유지
4. node_any_hit()       - static, 수정 (child ordering 적용)
5. bvh_intersect_any()  - public, 기존 유지
```

---

### Task 2: Object Count Threshold 적용 (우선순위 High)

**현황**: 모든 씬에서 BVH shadow 사용  
**목표**: 작은 씬 (≤20 objects)에서는 brute-force로 폴백

#### 2.1 수정 파일
- `src/lighting/shadow_test.c`

#### 2.2 현재 코드 (line 79-81)
```c
if (scene->bvh && scene->bvh->enabled && scene->bvh->root)
	return (bvh_intersect_any(scene->bvh, shadow_ray, mag, scene));
return (check_object_shadow(scene, &shadow_ray, &shadow_hit));
```

#### 2.3 수정 코드
```c
// includes/shadow.h에 추가
# define SHADOW_BVH_THRESHOLD 20

// is_in_shadow() 내부:
if (scene->bvh && scene->bvh->enabled && scene->bvh->root
	&& scene->objects.count > SHADOW_BVH_THRESHOLD)
	return (bvh_intersect_any(scene->bvh, shadow_ray, mag, scene));
return (check_object_shadow(scene, &shadow_ray, &shadow_hit));
```

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
3. **헬퍼 분리**: Norm 준수 상태에서 가독성 향상 목적으로 분리

---

## 검증 계획

### 기능 검증
- [ ] S1~S4 씬 렌더링 결과 시각적 비교 (그림자 정확성)
- [ ] BVH 비활성화 시 동일 결과 확인

### 성능 검증
- [ ] S4 씬 5회 실행 중앙값 측정
- [ ] Frame time, Shadow tests, BVH nodes_visited 기록
- [ ] Baseline (26,253ms) 대비 개선율 계산

### 회귀 검증
- [ ] 큰 씬 (objects > 20) 성능 저하 없음 확인

---

*Plan created: 2026-02-08*
