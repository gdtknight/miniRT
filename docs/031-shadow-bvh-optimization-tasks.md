# Shadow BVH 최적화 작업목록

**Feature**: 031-shadow-bvh-optimization  
**작성일**: 2026-02-08

---

## Task 1: Child Ordering 적용

### 1.1 ray_goes_positive() 함수 추가
- **파일**: `src/spatial/bvh_any_hit.c`
- **위치**: 파일 상단 (leaf_any_hit 앞)
- **작업**: ray direction 기반 축 방향 판별 함수 추가
- **코드**:
```c
static int	ray_goes_positive(t_vec3 dir, int axis)
{
	if (axis == 0)
		return (dir.x > 0);
	if (axis == 1)
		return (dir.y > 0);
	return (dir.z > 0);
}
```
- **상태**: [ ]

### 1.2 get_near_far() 헬퍼 함수 추가
- **파일**: `src/spatial/bvh_any_hit.c`
- **위치**: ray_goes_positive() 다음
- **작업**: near/far 자식 노드 결정 헬퍼 추가
- **코드**:
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
- **상태**: [ ]

### 1.3 node_any_hit() child ordering 적용
- **파일**: `src/spatial/bvh_any_hit.c`
- **작업**: 
  - 로컬 변수 `near`, `far` 추가
  - `get_near_far()` 호출로 순서 결정
  - left/right 직접 호출 → near/far 호출로 변경
- **Before**:
```c
if (node_any_hit(node->left, ray, max_dist, scene))
	return (1);
return (node_any_hit(node->right, ray, max_dist, scene));
```
- **After**:
```c
get_near_far(node, ray, &near, &far);
if (node_any_hit(near, ray, max_dist, scene))
	return (1);
return (node_any_hit(far, ray, max_dist, scene));
```
- **상태**: [ ]

### 1.4 norminette 검증
- **파일**: `src/spatial/bvh_any_hit.c`
- **명령**: `norminette src/spatial/bvh_any_hit.c`
- **기대**: OK
- **상태**: [ ]

---

## Task 2: Object Count Threshold 적용

### 2.1 SHADOW_BVH_THRESHOLD 매크로 정의
- **파일**: `includes/shadow.h`
- **위치**: `#define SHADOW_H` 다음, 구조체 정의 전
- **코드**:
```c
# define SHADOW_BVH_THRESHOLD 20
```
- **근거**: 프로젝트 스타일상 설정값은 헤더에 정의 (window.h, hud.h 참고)
- **상태**: [ ]

### 2.2 is_in_shadow() threshold 조건 추가
- **파일**: `src/lighting/shadow_test.c`
- **위치**: line 79
- **Before**:
```c
if (scene->bvh && scene->bvh->enabled && scene->bvh->root)
```
- **After**:
```c
if (scene->bvh && scene->bvh->enabled && scene->bvh->root
	&& scene->objects.count > SHADOW_BVH_THRESHOLD)
```
- **상태**: [ ]

### 2.3 norminette 검증
- **파일**: `src/lighting/shadow_test.c`
- **명령**: `norminette src/lighting/shadow_test.c`
- **기대**: OK
- **상태**: [ ]

---

## Task 3: 빌드 검증

### 3.1 전체 빌드
- **명령**: `make re`
- **기대**: 컴파일 성공, 경고 없음
- **상태**: [ ]

---

## Task 4: 기능 검증

### 4.1 그림자 정확성 검증
- **명령**: `./miniRT scenes/test_shadow.rt` (또는 기존 테스트 씬)
- **검증**: 그림자가 올바르게 렌더링되는지 시각적 확인
- **상태**: [ ]

### 4.2 BVH 비활성화 대비 결과 동일성
- **방법**: BVH 비활성화 후 동일 씬 렌더링, 결과 비교
- **기대**: 픽셀 단위 동일 결과
- **상태**: [ ]

---

## Task 5: 성능 검증

### 5.1 S4 벤치마크 실행
- **명령**: `./miniRT scenes/perf/perf_all_objects.rt`
- **반복**: 5회 실행
- **기록 항목**:
  - Frame time (ms)
  - Shadow tests
  - BVH nodes_visited
  - BVH skip_rate
- **상태**: [ ]

### 5.2 성능 비교
- **Baseline**: Frame time 26,253 ms
- **목표**: Frame time ≤ 25,583 ms (Round 3 수준 복원)
- **상태**: [ ]

### 5.3 회귀 검증 (큰 씬)
- **대상**: objects > 20인 씬
- **검증**: 성능 저하 없음 확인
- **상태**: [ ]

---

## 작업 순서

```
1.1 → 1.2 → 1.3 → 1.4 → 2.1 → 2.2 → 2.3 → 3.1 → 4.1 → 4.2 → 5.1 → 5.2 → 5.3
```

---

## 완료 기준

- [ ] 모든 Task 상태 완료
- [ ] norminette 통과
- [ ] 빌드 성공
- [ ] 그림자 정확성 유지
- [ ] S4 Frame time ≤ 25,583 ms

---

*Tasks generated: 2026-02-08*
