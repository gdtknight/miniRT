# Plane BVH Separation - Task List

**Date**: 2026-02-10  
**Plan**: [plane-bvh-separation-plan.md](plane-bvh-separation-plan.md)  
**Branch**: `031-perf-bottleneck-optimization`

---

## Pre-Implementation Checks

- [ ] `clear_bvh_tree()` null-safe 확인 (또는 신규 작성 시 null-safe 구현)
- [ ] `bvh_node_destroy()` 함수 존재 및 동작 확인
- [ ] 현재 `scene_build_bvh()` 구조 확인
- [ ] 동적 객체 변경 미사용 확인

---

## Phase 1: Data Structure Changes

### 1.1 t_scene 구조체 수정
- [ ] `includes/minirt.h` 열기
- [ ] `t_scene` 구조체에 `int *plane_indices` 필드 추가
- [ ] `t_scene` 구조체에 `int plane_count` 필드 추가
- [ ] 빌드 확인 (`make re`)

### 1.2 초기화/해제 코드
- [ ] `src/scene/scene.c` (또는 해당 파일) 열기
- [ ] `scene_init()` 에서 `plane_indices = NULL`, `plane_count = 0` 초기화
- [ ] `scene_destroy()` 에서 `plane_indices` free 추가
- [ ] 빌드 확인

---

## Phase 2: Plane Collection (Memory Safety)

### 2.1 collect_plane_indices() 함수 작성
- [ ] `src/spatial/bvh_init.c` 열기
- [ ] 함수 시그니처: `static void collect_plane_indices(t_scene *scene)`
- [ ] 기존 plane_indices free 로직 추가 (재빌드 안전)
- [ ] plane 개수 카운트 로직
- [ ] malloc 및 실패 처리 (`plane_indices = NULL`, `plane_count = 0`)
- [ ] plane 인덱스 수집 로직
- [ ] Norm 검사 (함수 25줄, 5변수 이내)

### 2.2 단위 테스트
- [ ] plane-only 씬으로 collect 동작 확인
- [ ] no-plane 씬으로 plane_count=0 확인
- [ ] valgrind로 메모리 누수 확인

---

## Phase 3: BVH Build Modification

### 3.1 clear_bvh_tree() 함수 작성
- [ ] 함수 시그니처: `static void clear_bvh_tree(t_bvh *bvh)`
- [ ] null-safe 구현 (`if (bvh && bvh->root)`)
- [ ] `bvh_node_destroy(bvh->root)` 호출
- [ ] `bvh->root = NULL` 설정

### 3.2 fill_finite_refs() 함수 작성
- [ ] 기존 `fill_object_refs()` 참고
- [ ] plane 타입 제외 로직 추가
- [ ] finite object만 BVH refs에 추가

### 3.3 scene_build_bvh() 수정
- [ ] `collect_plane_indices(scene)` 호출 추가
- [ ] finite_count 계산 (`objects.count - plane_count`)
- [ ] `finite_count == 0` 시 `clear_bvh_tree()` 호출 및 early return
- [ ] `fill_finite_refs()` 로 BVH 대상 변경
- [ ] Norm 검사

### 3.4 단위 테스트
- [ ] plane-only 씬: BVH root == NULL 확인
- [ ] mixed 씬: BVH에 plane 미포함 확인
- [ ] `--bvh-vis`로 트리 구조 확인

---

## Phase 4: trace_ray() Modification

### 4.1 check_planes() 함수 작성
- [ ] `src/render/trace.c` 열기
- [ ] 함수 시그니처: `static int check_planes(t_scene *scene, t_ray *ray, t_hit *hit)`
- [ ] 이중 체크: `if (scene->plane_count == 0 || !scene->plane_indices) return 0`
- [ ] plane_indices 순회하며 교차 테스트
- [ ] 가장 가까운 hit 갱신
- [ ] Norm 검사

### 4.2 trace_ray() 수정
- [ ] BVH 경로에서 `check_planes()` 호출 추가
- [ ] BVH hit과 plane hit 중 가까운 것 선택
- [ ] fallback 경로는 기존 유지 (check_all_objects)

### 4.3 단위 테스트
- [ ] plane이 가장 가까운 경우 정상 렌더링
- [ ] sphere가 가장 가까운 경우 정상 렌더링
- [ ] 기존 valid 씬 렌더링 결과 비교

---

## Phase 5: shadow_test Modification

### 5.1 check_plane_shadow() 함수 작성
- [ ] `src/lighting/shadow_test.c` 열기
- [ ] 함수 시그니처: `static int check_plane_shadow(t_scene *scene, t_ray *ray, t_hit *hit)`
- [ ] 이중 체크: `if (scene->plane_count == 0 || !scene->plane_indices) return 0`
- [ ] plane_indices 순회하며 그림자 교차 테스트
- [ ] Norm 검사

### 5.2 is_in_shadow() 수정
- [ ] BVH 경로에서 `bvh_intersect_any()` 후 `check_plane_shadow()` 호출
- [ ] 둘 중 하나라도 hit이면 shadow
- [ ] fallback 경로는 기존 유지

### 5.3 단위 테스트
- [ ] plane 위 그림자 정상 렌더링
- [ ] sphere 위 plane 그림자 정상 렌더링

---

## Phase 6: Memory Testing

- [ ] valgrind 기본 테스트
  ```bash
  valgrind --leak-check=full ./miniRT scenes/valid/valid_us01_basic.rt --render-once
  ```
- [ ] plane-only 씬 메모리 테스트
- [ ] mixed 씬 메모리 테스트
- [ ] 누수 0 확인

---

## Phase 7: Functional Testing

### 7.1 Edge Cases
- [ ] plane-only 씬 (BVH empty)
- [ ] no-plane 씬 (기존과 동일)
- [ ] mixed 씬 (BVH + planes)
- [ ] plane closest hit 케이스

### 7.2 Regression Tests
- [ ] `scenes/valid/*.rt` 전체 렌더링
- [ ] 기존 결과와 비교 (육안 또는 diff)

---

## Phase 8: Performance Benchmarking

- [ ] 기존 성능 측정 (plane 분리 전)
  ```bash
  ./miniRT scenes/perf/perf_all_objects.rt --benchmark
  ```
- [ ] 변경 후 성능 측정
- [ ] `--bvh-vis`로 트리 구조 개선 확인
- [ ] 결과 기록

---

## Phase 9: Commit

- [ ] Norm 최종 확인 (`norminette`)
- [ ] 전체 빌드 확인 (`make re`)
- [ ] 전체 테스트 통과 확인
- [ ] 커밋 메시지 작성
  ```
  perf(bvh): Separate planes from BVH for better pruning
  
  - Add plane_indices/plane_count to t_scene
  - Exclude planes from BVH build (infinite AABB issue)
  - Add check_planes() for trace_ray path
  - Add check_plane_shadow() for shadow path
  - Memory safe: handles rebuild, empty BVH, malloc failure
  
  Co-Authored-By: Copilot <copilot@github.com>
  ```
- [ ] git add & commit
- [ ] push

---

## Summary

| Phase | Tasks | Est. Lines |
|-------|-------|------------|
| 1. Data structures | 4 | ~10 |
| 2. Plane collection | 3 | ~30 |
| 3. BVH build | 4 | ~40 |
| 4. trace_ray | 3 | ~25 |
| 5. shadow_test | 3 | ~20 |
| 6-8. Testing | - | - |
| 9. Commit | 1 | - |
| **Total** | **18 impl tasks** | **~125 lines** |

---

## Notes

- 각 Phase 완료 후 빌드/테스트 필수
- Norm 위반 시 함수 분리 고려
- 성능 개선 미미할 경우 롤백 가능
