# Tasks: Shadow Offset LUT (P4)

**Input**: Design documents from `/specs/030-shadow-offset-lut/`
**Prerequisites**: plan.md, spec.md, research.md

**Tests**: Not requested — manual visual verification + metrics comparison + leaks check.

**Organization**: 6 phases 순차 실행. magnitude guard → LUT 구현 → cleanup → 검증 → 벤치마크.

## Format: `[ID] [Story] Description`

---

## Phase 1: Pre-change Verification

**Purpose**: Baseline 상태 확인 및 빌드 검증

- [X] T001 Build project with `make re` and confirm no warnings
- [X] T002 Run `norminette` on target files: `includes/shadow.h`, `src/lighting/shadow_config.c`, `src/lighting/shadow_calc.c`, `src/lighting/shadow_test.c`, `src/scene/scene.c`

**Checkpoint**: 코드 변경 전 상태 확인 완료.

---

## Phase 2: Magnitude Guard (선행 안전성 수정)

**Goal**: `is_in_shadow()`에서 mag == 0 시 NaN/INF 방지

**File**: `src/lighting/shadow_test.c`

### Implementation

- [X] T003 In `is_in_shadow()` at `src/lighting/shadow_test.c`, add guard after `mag = vec3_magnitude(to_light);`:
  ```c
  mag = vec3_magnitude(to_light);
  if (mag < 0.0001)
      return (0);
  ```
- [X] T004 Run `norminette src/lighting/shadow_test.c` and confirm no errors
- [X] T005 Run `make re` and confirm compilation with zero warnings

**Checkpoint**: Magnitude guard 구현 완료.

---

## Phase 3: LUT Structure Extension

**Goal**: `t_shadow_config` 구조체에 `offset_lut` 필드 추가

**File**: `includes/shadow.h`

### Implementation

- [X] T006 Add `t_vec3 *offset_lut;` field to `t_shadow_config` struct in `includes/shadow.h` (after `enable_ao` field)
- [X] T007 Run `norminette includes/shadow.h` and confirm no errors

**Checkpoint**: 구조체 확장 완료.

---

## Phase 4: LUT Initialization

**Goal**: LUT 생성 함수 추가 및 초기화 연결

**Files**: `src/lighting/shadow_config.c`

### Implementation

- [X] T008 Add `init_shadow_offset_lut()` function in `src/lighting/shadow_config.c`:
  ```c
  void    init_shadow_offset_lut(t_shadow_config *config)
  {
      int     i;
      int     grid_size;
      double  angle;
      double  r;

      config->offset_lut = malloc(sizeof(t_vec3) * config->samples);
      if (!config->offset_lut)
          return ;
      grid_size = (int)sqrt((double)config->samples);
      if (grid_size < 1)
          grid_size = 1;
      i = 0;
      while (i < config->samples)
      {
          angle = 2.0 * M_PI * (i % grid_size) / (double)grid_size;
          r = (i / (double)grid_size + 0.5) / (double)grid_size;
          config->offset_lut[i].x = r * cos(angle);
          config->offset_lut[i].y = r * sin(angle);
          config->offset_lut[i].z = 0.0;
          i++;
      }
  }
  ```
- [X] T009 Modify `init_shadow_config()` to initialize `offset_lut = NULL` and call `init_shadow_offset_lut(&config)`:
  ```c
  config.offset_lut = NULL;
  init_shadow_offset_lut(&config);
  ```
- [X] T010 Add `#include <stdlib.h>` to `src/lighting/shadow_config.c` if not present (for malloc)
- [X] T011 Run `norminette src/lighting/shadow_config.c` and confirm no errors (5/5 함수 한계 확인)
- [X] T012 Run `make re` and confirm compilation with zero warnings

**Checkpoint**: LUT 초기화 구현 완료.

---

## Phase 5: LUT Usage in Shadow Sampling

**Goal**: `sample_shadow_ray()`에서 LUT 참조로 변경

**File**: `src/lighting/shadow_calc.c`

### Implementation

- [X] T013 Modify `sample_shadow_ray()` in `src/lighting/shadow_calc.c` to use LUT with fallback:
  ```c
  static int  sample_shadow_ray(t_shadow_sample *params, int index)
  {
      t_vec3  offset;
      t_vec3  sample_light_pos;
      double  radius;

      radius = params->config->softness * 2.0;
      if (params->config->offset_lut)
          offset = vec3_multiply(params->config->offset_lut[index], radius);
      else
          offset = generate_shadow_sample_offset(radius, index,
                  params->config->samples);
      sample_light_pos = vec3_add(params->light_pos, offset);
      return (is_in_shadow(params->scene, params->point,
              sample_light_pos, params->bias));
  }
  ```
- [X] T014 Run `norminette src/lighting/shadow_calc.c` and confirm no errors
- [X] T015 Run `make re` and confirm compilation with zero warnings

**Checkpoint**: LUT 사용 구현 완료.

---

## Phase 6: LUT Cleanup

**Goal**: Scene 정리 시 LUT 메모리 해제

**File**: `src/scene/scene.c`

### Implementation

- [X] T016 Add `free_shadow_offset_lut()` static helper in `src/scene/scene.c`:
  ```c
  static void free_shadow_offset_lut(t_shadow_config *config)
  {
      if (config && config->offset_lut)
      {
          free(config->offset_lut);
          config->offset_lut = NULL;
      }
  }
  ```
- [X] T017 Call `free_shadow_offset_lut(&scene->shadow_config)` in `scene_destroy()` before `free(scene)`:
  ```c
  void    scene_destroy(t_scene *scene)
  {
      if (!scene)
          return ;
      object_list_destroy(&scene->objects);
      if (scene->bvh)
          bvh_destroy(scene->bvh);
      free_shadow_offset_lut(&scene->shadow_config);
      free(scene);
  }
  ```
- [X] T018 Run `norminette src/scene/scene.c` and confirm no errors (4/5 함수)
- [X] T019 Run `make re` and confirm compilation with zero warnings

**Checkpoint**: LUT cleanup 구현 완료.

---

## Phase 7: Validation

**Purpose**: 전체 변경 후 기능 및 품질 검증

- [X] T020 Run full `norminette` on all modified files and confirm no errors:
  - `includes/shadow.h`
  - `src/lighting/shadow_config.c`
  - `src/lighting/shadow_calc.c`
  - `src/lighting/shadow_test.c`
  - `src/scene/scene.c`
- [X] T021 Run `make re` and confirm final build with zero warnings
- [X] T022 Run S1–S4 scenes and verify visual rendering identical to baseline:
  - `./miniRT scenes/valid_smoke_simple.rt`
  - `./miniRT scenes/perf/perf_spheres_20.rt`
  - `./miniRT scenes/perf/perf_spheres_50.rt`
  - `./miniRT scenes/perf/perf_all_objects.rt`
- [X] T023 Run memory leak check: `leaks -atExit -- ./miniRT scenes/perf/perf_spheres_20.rt`

**Checkpoint**: 기능 검증 완료.

---

## Phase 8: Benchmark & Documentation

**Purpose**: 성능 측정 및 결과 기록

- [X] T024 Run S1–S4 scenes and record metrics (1회 측정, metrics summary 캡처)
- [X] T025 Update `docs/benchmark-results.md` with Round 3 data (Phase B-1: P4 단독 효과)
- [X] T026 Commit code changes and benchmark documentation

**Checkpoint**: Round 3 benchmark 기록 완료, P4 feature complete.

---

## Dependencies & Execution Order

### Phase Dependencies

- **Phase 1 (Pre-change)**: No dependencies — start immediately
- **Phase 2 (Magnitude Guard)**: Depends on Phase 1
- **Phase 3 (Structure)**: Depends on Phase 1
- **Phase 4 (LUT Init)**: Depends on Phase 3
- **Phase 5 (LUT Usage)**: Depends on Phase 4
- **Phase 6 (Cleanup)**: Depends on Phase 3
- **Phase 7 (Validation)**: Depends on Phase 2, 5, 6 완료
- **Phase 8 (Benchmark)**: Depends on Phase 7 완료

### Execution Flow

```text
Phase 1 (Pre-check)
    ↓
Phase 2 (Magnitude Guard) ─┐
Phase 3 (Structure) ───────┤
    ↓                      │
Phase 4 (LUT Init)         │
    ↓                      │
Phase 5 (LUT Usage) ───────┤
    ↓                      │
Phase 6 (Cleanup) ─────────┤
    ↓                      │
Phase 7 (Validation) ◄─────┘
    ↓
Phase 8 (Benchmark)
```

---

## Notes

- T008: `init_shadow_offset_lut()`는 ~20줄 (Norm 25줄 이하 ✓)
- T011: shadow_config.c 함수 수 확인 필요 (현재 4 → 5)
- T016: `free_shadow_offset_lut`를 scene.c에 static으로 추가 (shadow_config.c 5함수 한계)
- T022: GUI 필요 — 수동 실행
- T023: leaks 명령은 macOS 환경에서 실행
- T026: 커밋 시 코드 + 벤치마크 문서 포함
- **제외**: `set_shadow_samples()` LUT 재생성 — spec Non-Goal (동적 변경 미지원)
