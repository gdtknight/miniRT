# Tasks: Performance Bottleneck Optimization (Round 1 + Round 2)

**Input**: Design documents from `/specs/031-perf-bottleneck-optimization/`
**Prerequisites**: plan.md, spec.md, research.md, data-model.md

**Tests**: Not requested — manual benchmark (S1~S4) + norminette + leaks check.

**Organization**: Round 1 (P5→P2→P6→P1, 완료) + Round 2 (PA→PB, 신규) + Validation + Benchmark.

## Format: `[ID] [Story] Description`

---

## Phase 1: Pre-change Verification ✅

**Purpose**: Baseline 상태 확인 및 빌드 검증

- [x] T001 Build project with `make re` and confirm no warnings
- [x] T002 Run `norminette` on all target files

**Checkpoint**: 코드 변경 전 상태 확인 완료.

---

## Phase 2: P5 — inv_dir Precompute (기반 작업) ✅

**Goal**: `t_ray`에 `inv_dir` 필드 추가, AABB 교차에서 나눗셈→곱셈 변환

**Files**: `includes/ray.h`, `src/render/camera.c`, `src/lighting/shadow_test.c`, `src/spatial/aabb.c`

### Implementation

- [x] T003 Add `t_vec3 inv_dir;` field to `t_ray` struct in `includes/ray.h` (after `direction` field)
- [x] T004 In `create_camera_ray()` at `src/render/camera.c`, calculate inv_dir after setting ray.direction:
  ```c
  ray.inv_dir.x = 1.0 / ray.direction.x;
  ray.inv_dir.y = 1.0 / ray.direction.y;
  ray.inv_dir.z = 1.0 / ray.direction.z;
  ```
- [x] T005 In `is_in_shadow()` at `src/lighting/shadow_test.c`, calculate inv_dir for shadow_ray after setting shadow_ray.direction
- [x] T006 Modify `safe_slab_axis()` in `src/spatial/aabb.c` to use `ray.inv_dir` instead of calculating `inv_d`:
  - Remove the `1.0 / ac->ray_dir` calculation
  - Use pre-computed inv_dir passed via new parameter or restructure
- [x] T007 Run `norminette` on modified files and confirm no errors
- [x] T008 Run `make re` and confirm compilation with zero warnings
- [x] T009 Run S1 scene and verify visual output is identical

**Checkpoint**: P5 구현 완료, inv_dir 적용됨.

---

## Phase 3: P2 — Camera Basis Caching ✅

**Goal**: Camera basis를 프레임당 1회만 계산 (dirty flag)

**Files**: `includes/minirt.h`, `src/render/camera.c`, `src/window/window_camera.c`

### Implementation

- [x] T010 Add `t_camera_cache` struct to `includes/minirt.h`:
  ```c
  typedef struct s_camera_cache
  {
      t_vec3  right;
      t_vec3  up;
      double  aspect_ratio;
      double  fov_scale;
      int     valid;
  }   t_camera_cache;
  ```
- [x] T011 Add `t_camera_cache cache;` field to `t_camera` struct in `includes/minirt.h`
- [x] T012 Add `update_camera_cache()` static function in `src/render/camera.c` that:
  - Returns early if `camera->cache.valid` is true
  - Moves `init_camera_calc()` logic into cache update
  - Sets `camera->cache.valid = 1` after calculation
- [x] T013 Modify `create_camera_ray()` to call `update_camera_cache()` and use cached values
- [x] T014 Add `camera->cache.valid = 0;` in `handle_camera_move()` at `src/window/window_camera.c`
- [x] T015 Add `camera->cache.valid = 0;` in `handle_camera_pitch()` at `src/window/window_camera.c`
- [x] T016 Add `camera->cache.valid = 0;` in `handle_camera_reset()` at `src/window/window_camera.c`
- [x] T017 Initialize `cache.valid = 0` in camera initialization (scene creation)
- [x] T018 Run `norminette` on modified files and confirm no errors
- [x] T019 Run `make re` and confirm compilation with zero warnings
- [x] T020 Run S1 scene, move camera, verify rendering still correct

**Checkpoint**: P2 구현 완료, camera cache 적용됨.

---

## Phase 4: P6 — BVH Child Ordering ✅

**Goal**: BVH 순회 시 near/far child 순서 최적화

**Files**: `includes/spatial.h`, `src/spatial/bvh_build_core.c`, `src/spatial/bvh_traverse.c`

### Implementation

- [x] T021 Add `int split_axis;` field to `t_bvh_node` struct in `includes/spatial.h` (after `depth` field)
- [x] T022 In `src/spatial/bvh_build_core.c`, store split_axis when creating internal nodes:
  - Find where `select_split_axis()` is called
  - Store result in `node->split_axis`
- [x] T023 Modify `traverse_children()` in `src/spatial/bvh_traverse.c` to order children by ray direction:
  ```c
  // Determine near/far based on ray direction and split_axis
  // Visit near first, update t_max on hit, prune far if needed
  ```
- [x] T024 Run `norminette` on modified files and confirm no errors
- [x] T025 Run `make re` and confirm compilation with zero warnings
- [x] T026 Run S1–S4 scenes and verify visual output is identical

**Checkpoint**: P6 구현 완료, child ordering 적용됨.

---

## Phase 5: P1 — Shadow Ray BVH + Any-Hit ✅

**Goal**: Shadow ray에 BVH 가속 적용, early exit 구현

**Files**: `includes/spatial.h`, `src/spatial/bvh_any_hit.c` (신규), `src/lighting/shadow_test.c`, `Makefile`

### Implementation

- [x] T027 Add `bvh_intersect_any()` declaration to `includes/spatial.h`:
  ```c
  bool    bvh_intersect_any(t_bvh *bvh, t_ray ray, double max_dist, void *scene);
  ```
- [x] T028 Create new file `src/spatial/bvh_any_hit.c` with:
  - `static bool bvh_leaf_intersect_any()` — 첫 교차 시 true 반환
  - `static bool bvh_node_intersect_any()` — 재귀 순회, early exit
  - `bool bvh_intersect_any()` — 진입점
- [x] T029 Modify `is_in_shadow()` in `src/lighting/shadow_test.c` to use BVH:
  ```c
  // If BVH enabled, use bvh_intersect_any()
  // Fallback to check_object_shadow() if BVH not available
  ```
- [x] T030 Add `src/spatial/bvh_any_hit.c` to `Makefile` SRCS
- [x] T031 Run `norminette` on all new/modified files and confirm no errors
- [x] T032 Run `make re` and confirm compilation with zero warnings
- [x] T033 Run S1–S4 scenes and verify shadows are correct

**Checkpoint**: P1 구현 완료, shadow BVH 적용됨.

---

## Phase 6: Round 1 Validation ✅ (일부 미완료)

**Purpose**: Round 1 전체 변경 후 기능 및 품질 검증

- [x] T034 Run full `norminette` on all modified files
- [x] T035 Run `make re` and confirm final build with zero warnings
- [x] T036 Run S1–S4 scenes and verify visual rendering identical to baseline
- [x] T037 Run memory leak check: `leaks -atExit -- ./miniRT scenes/perf/perf_spheres_20.rt`

**Checkpoint**: Round 1 기능 검증 완료.

---

## Phase 7: Round 1 Benchmark ✅

**Purpose**: Round 1 성능 측정 (2026-02-10 완료)

- [x] T038 Run S1–S4 scenes and record metrics
- [x] T039 Analyze results and identify remaining bottlenecks

**Round 1 Results**:

| Scene | Objects | Frame Time | Shadow Tests | BVH Skip |
|-------|---------|-----------|-------------|----------|
| perf_timing | 3 | 241ms | 634K | 93.7% |
| perf_spheres_20 | 20 | 942ms | 15.4M | 59.0% |
| perf_spheres_50 | 50 | 1,416ms | 2.2M | 46.6% |
| perf_all_objects | 18 | **25,192ms** | **369.6M** | 33.0% |

**발견된 추가 병목**: Plane AABB 오염 + Shadow BVH threshold → Round 2로 진행.

**Checkpoint**: Round 1 benchmark 완료, Round 2 병목 식별됨.

---

## Phase 8: PA — Plane BVH 분리 (Round 2)

**Goal**: Plane을 BVH에서 제외하고 별도 순회하여 BVH 트리 품질 회복

**Files**: `includes/spatial.h`, `src/spatial/bvh_init.c`, `src/spatial/bvh_lifecycle.c`, `src/render/trace.c`, `src/spatial/bvh_any_hit.c`

**Story**: [US1] Shadow Ray 성능 개선 + BVH 트리 품질 회복

### 8.1 구조체 변경

- [x] T042 [US1] Add `t_plane_refs` struct to `includes/spatial.h` (before `t_bvh`):
  ```c
  typedef struct s_plane_refs
  {
      int    *indices;
      int    count;
  }   t_plane_refs;
  ```

- [x] T043 [US1] Add `t_plane_refs plane_refs;` field to `t_bvh` struct in `includes/spatial.h` (after `visualize` field)

- [x] T044 Run `norminette` on `includes/spatial.h` and confirm no errors

### 8.2 BVH 빌드 시 Plane 필터링

- [x] T045 [US1] Modify `scene_build_bvh()` in `src/spatial/bvh_init.c` to separate planes from bounded objects:
  - Count planes vs non-planes in `scene->objects.items`
  - Allocate `plane_refs.indices` with `malloc(sizeof(int) * plane_count)`
  - Fill `plane_refs` with indices of `OBJ_PLANE` objects
  - Fill `refs` array with only non-plane object indices
  - Build BVH with non-plane refs only
  - Store `plane_refs` in `scene->bvh->plane_refs`

- [x] T046 [US1] Add static helper `count_planes()` in `src/spatial/bvh_init.c` that returns plane count from scene objects

- [x] T047 [US1] Add static helper `fill_separated_refs()` in `src/spatial/bvh_init.c` that fills plane indices and bounded refs arrays separately

- [x] T048 Run `norminette` on `src/spatial/bvh_init.c` and confirm no errors (max 5 functions: `get_object_center`, `fill_object_refs` → removed/replaced, `count_planes`, `fill_separated_refs`, `scene_build_bvh`)

### 8.3 Plane 별도 Intersection (Primary Ray)

- [x] T049 [US1] Add `intersect_planes()` static function in `src/render/trace.c` that:
  - Iterates over `bvh->plane_refs.indices`
  - Tests each plane with `intersect_object_new()`
  - Tracks closest hit (updates `hit` if closer)
  - Returns 1 if any hit found, 0 otherwise
  - Calls `metrics_add_intersect_test()` for each test

- [x] T050 [US1] Modify `trace_ray()` in `src/render/trace.c` to test planes after BVH:
  ```c
  // BVH: test bounded objects
  bvh_hit = bvh_intersect(scene->bvh, *ray, &hit, scene);
  // Planes: test separately (always)
  plane_hit = intersect_planes(scene->bvh, *ray, &hit, scene);
  if (bvh_hit || plane_hit)
      return (apply_lighting(scene, &hit));
  ```

- [x] T051 Run `norminette` on `src/render/trace.c` and confirm no errors (max 5 functions: `check_all_objects`, `intersect_planes`, `trace_ray` = 3 functions)

### 8.4 Plane 별도 Shadow Test

- [x] T052 [US1] Add `check_plane_shadow()` static function in `src/lighting/shadow_test.c` that:
  - Iterates over `scene->bvh->plane_refs.indices`
  - Tests each plane with `intersect_object_new()` using shadow ray
  - Returns 1 on first hit (early exit), 0 if no hit
  - Calls `metrics_add_shadow_intersect()` for each test
  - Respects `max_dist` (shadow_hit.distance = mag)

- [x] T053 [US1] Modify `is_in_shadow()` in `src/lighting/shadow_test.c` to test planes after BVH:
  ```c
  // BVH: test bounded objects
  if (scene->bvh && scene->bvh->enabled && scene->bvh->root)
  {
      if (bvh_intersect_any(scene->bvh, shadow_ray, mag, scene))
          return (1);
      return (check_plane_shadow(scene, &shadow_ray, mag));
  }
  return (check_object_shadow(scene, &shadow_ray, &shadow_hit));
  ```

- [x] T054 Run `norminette` on `src/lighting/shadow_test.c` and confirm no errors (max 5 functions: `check_object_shadow`, `check_plane_shadow`, `is_in_shadow` = 3 functions)

### 8.5 메모리 해제

- [x] T055 [US1] Modify `bvh_destroy()` in `src/spatial/bvh_lifecycle.c` to free `plane_refs.indices`:
  ```c
  if (bvh->plane_refs.indices)
      free(bvh->plane_refs.indices);
  ```

- [x] T056 [US1] Initialize `plane_refs` to zero in `bvh_create()` at `src/spatial/bvh_lifecycle.c`:
  ```c
  bvh->plane_refs.indices = NULL;
  bvh->plane_refs.count = 0;
  ```

- [x] T057 Run `norminette` on `src/spatial/bvh_lifecycle.c` and confirm no errors

### 8.6 빌드 검증

- [x] T058 Run `make re` and confirm compilation with zero warnings
- [x] T059 Run `perf_all_objects.rt` and verify planes are visible (not missing from render)
- [x] T060 Run `perf_spheres_20.rt` (plane 없는 씬) and verify rendering unchanged
- [x] T061 Run `perf_timing.rt` and verify rendering unchanged

**Checkpoint**: PA 구현 완료. Plane이 BVH에서 분리되고 별도 순회됨.

---

## Phase 9: PB — Shadow BVH Threshold 하향 (Round 2)

**Goal**: `SHADOW_BVH_THRESHOLD` 20→5로 하향, 소규모 씬 brute-force 유지

**Files**: `includes/shadow.h`, `src/lighting/shadow_test.c`

**Story**: [US1] Shadow Ray 성능 개선

**Dependency**: Phase 8 (PA) 완료 필수

### Implementation

- [x] T062 [US1] Change `#define SHADOW_BVH_THRESHOLD 20` to `5` in `includes/shadow.h`
  - 완전 제거 시 소규모 씬(3 objects) 15% regression 발생 확인
  - threshold=5로 하향: 3개 씬 brute-force 유지, 18+ 씬 BVH 사용

- [x] T063 [US1] `is_in_shadow()` threshold 조건 유지 (값만 변경):
  - `scene->objects.count > SHADOW_BVH_THRESHOLD` 조건 유지
  - BVH 경로에서 `check_plane_shadow()` 호출 추가 (Phase 8에서 적용됨)

- [x] T064 Run `norminette` on `includes/shadow.h` and `src/lighting/shadow_test.c`
- [x] T065 Run `make re` and confirm compilation with zero warnings

**Checkpoint**: PB 구현 완료. Shadow BVH threshold 20→5로 하향.

---

## Phase 10: Round 2 Validation

**Purpose**: PA+PB 적용 후 기능 및 품질 검증

### 렌더링 정확성

- [x] T066 Run `perf_all_objects.rt` and verify:
  - All 4 planes are visible
  - Shadows on planes render correctly
  - Shadows cast by planes onto other objects are correct
  - No visual artifacts

- [x] T067 Run `perf_spheres_20.rt` and verify rendering identical to Round 1

- [x] T068 Run `perf_spheres_50.rt` and verify rendering identical to Round 1

- [x] T069 Run `perf_timing.rt` and verify rendering identical to Round 1

### 코드 품질

- [x] T070 Run full `norminette` on all Round 2 modified files:
  - `includes/spatial.h`
  - `includes/shadow.h`
  - `src/spatial/bvh_init.c`
  - `src/spatial/bvh_lifecycle.c`
  - `src/render/trace.c`
  - `src/lighting/shadow_test.c`

- [x] T071 Run `make re` and confirm final build with zero warnings

### 메모리 검증

- [x] T072 Run memory leak check: `leaks -atExit -- ./miniRT scenes/perf/perf_all_objects.rt`
  - Verify `plane_refs.indices` is freed properly
  - No new leaks introduced

**Checkpoint**: Round 2 기능 검증 완료.

---

## Phase 11: Final Benchmark & Documentation

**Purpose**: Round 2 성능 측정 및 전체 결과 기록

### Benchmark

- [x] T073 Run `perf_timing.rt` (3 objects, 0 planes) — record frame time, shadow tests, BVH skip rate
- [x] T074 Run `perf_spheres_20.rt` (20 objects, 0 planes) — record metrics
- [x] T075 Run `perf_spheres_50.rt` (50 objects, 0 planes) — record metrics
- [x] T076 Run `perf_all_objects.rt` (18 objects, 4 planes) — record metrics. **Key target: frame time 20%+ 개선 (< 20,000ms)**

### 분석 및 기록

- [x] T077 Calculate Round 2 improvement rates vs Round 1 baseline:
  - Frame time 개선율
  - Shadow tests 감소율
  - BVH skip rate 변화
  - perf_timing 성능 저하 여부 확인 (threshold 제거 영향)

- [x] T078 Update `docs/benchmark-results.md` with Round 2 data

**Round 2 Results (threshold=5)**:

| Scene | Objects | Round 1 | Round 2 | 변화 | Shadow Tests |
|-------|---------|---------|---------|------|-------------|
| perf_timing | 3 | 241ms | **241ms** | 0% | 634K → 634K |
| perf_spheres_20 | 20 | 942ms | **1,219ms** | +29.4% | 15.4M → 2.0M |
| perf_spheres_50 | 50 | 1,416ms | **1,432ms** | +1.1% | 2.2M → 2.2M |
| perf_all_objects | 18 | **25,192ms** | **6,067ms** | **-75.9%** | 369.6M → 83.7M |

**핵심 성과**: perf_all_objects **75.9% 개선** (25.2s → 6.1s). BVH skip rate 33.0% → 82.0%.

**spheres_20 regression 원인**: threshold 20→5 하향으로 shadow가 brute-force(15.4M tests) → BVH(2.0M tests + 14.8M node visits)로 전환. BVH 순회 오버헤드가 brute-force보다 큼.

**Checkpoint**: Round 2 benchmark 기록 완료, PA+PB feature complete.

---

## Dependencies & Execution Order

### Phase Dependencies

- **Phase 1–7 (Round 1)**: ✅ 완료
- **Phase 8 (PA: Plane 분리)**: Depends on Round 1 완료
- **Phase 9 (PB: Threshold 제거)**: Depends on Phase 8 (PA 완료 필수)
- **Phase 10 (Validation)**: Depends on Phase 9
- **Phase 11 (Benchmark)**: Depends on Phase 10

### Execution Flow

```text
[Round 1 - 완료]
Phase 1 (Pre-check)          ✅ T001-T002
Phase 2 (P5: inv_dir)        ✅ T003-T009
Phase 3 (P2: camera cache)   ✅ T010-T020
Phase 4 (P6: child ordering) ✅ T021-T026
Phase 5 (P1: shadow BVH)     ✅ T027-T033
Phase 6 (Validation)         ✅ T034-T037
Phase 7 (Benchmark)          ✅ T038-T039

[Round 2 - 신규]
Phase 8 (PA: Plane 분리)
    ├── T042-T044: 구조체 변경
    ├── T045-T048: BVH 빌드 필터링
    ├── T049-T051: Primary ray plane 테스트
    ├── T052-T054: Shadow ray plane 테스트
    ├── T055-T057: 메모리 해제
    └── T058-T061: 빌드 검증
         ↓
Phase 9 (PB: Threshold 제거)
    └── T062-T065: Threshold 제거
         ↓
Phase 10 (Validation)
    ├── T066-T069: 렌더링 정확성
    ├── T070-T071: 코드 품질
    └── T072: 메모리 검증
         ↓
Phase 11 (Benchmark)
    ├── T073-T076: 씬별 벤치마크
    └── T077-T078: 분석 및 기록
```

---

## Notes

### Round 2 Norm 함수 수 확인

| 파일 | 현재 함수 수 | 변경 후 | 비고 |
|------|-------------|---------|------|
| `includes/spatial.h` | N/A | N/A | 구조체 추가만 |
| `src/spatial/bvh_init.c` | 3 | 5 | `count_planes()`, `fill_separated_refs()` 추가, `fill_object_refs()` 제거/교체 |
| `src/spatial/bvh_lifecycle.c` | 3 | 3 | `bvh_destroy()`, `bvh_create()` 수정만 |
| `src/render/trace.c` | 2 | 3 | `intersect_planes()` 추가 |
| `src/lighting/shadow_test.c` | 2 | 3 | `check_plane_shadow()` 추가 |

모든 파일이 5함수/파일 제한 이내.

### PA+PB 통합 시 주의 사항

- T053과 T063은 같은 함수(`is_in_shadow()`)를 수정함 → Phase 8에서 먼저 plane 분리를 적용하고, Phase 9에서 threshold만 제거
- T053 시점에서는 threshold 조건이 아직 존재할 수 있음 → Phase 9에서 최종 정리
- `check_object_shadow()` brute-force fallback은 BVH 비활성화 씬을 위해 유지

### 성능 목표 vs 실측

| 지표 | Round 1 | 보수적 목표 | 실측 결과 | 달성 |
|------|---------|-----------|----------|------|
| perf_all_objects frame time | 25,192ms | < 20,000ms | **6,067ms** | ✅ (75.9% 개선) |
| perf_all_objects shadow tests | 369.6M | < 200M | **83.7M** | ✅ (77.4% 감소) |
| perf_all_objects BVH skip rate | 33.0% | > 50% | **82.0%** | ✅ |
| perf_timing frame time | 241ms | ≤ 250ms | **241ms** | ✅ (동일) |
| perf_spheres_20 frame time | 942ms | ≤ 1,000ms | **1,219ms** | ⚠️ (+29.4%) |
