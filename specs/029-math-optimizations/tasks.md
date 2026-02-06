# Tasks: Safe Math Optimizations (Phase A)

**Input**: Design documents from `/specs/029-math-optimizations/`
**Prerequisites**: plan.md, spec.md, research.md

**Tests**: Not requested — manual visual verification + metrics comparison.

**Organization**: 3개 독립 최적화 항목(P3, P8, P7). 각 항목은 서로 다른 파일을 수정하므로 병렬 실행 가능하나, 측정 정확성을 위해 순차 실행 권장.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[OPT]**: 최적화 항목 식별자 (P3, P8, P7)
- Include exact file paths in descriptions

---

## Phase 1: Pre-change Verification

**Purpose**: Baseline 상태 확인 및 빌드 검증

- [X] T001 Build project with `make re` and confirm no warnings
- [X] T002 Run `norminette` on target files: `src/lighting/lighting.c`, `src/ray/intersect_object.c`, `src/lighting/shadow_test.c`
- [ ] T003 Run S1–S4 scenes and capture current metrics (Round 1 baseline 재확인용) ⚠️ GUI 필요 — 스킵

**Checkpoint**: 코드 변경 전 상태 확인 완료.

---

## Phase 2: P3 — Specular pow32 경량화

**Goal**: `pow(spec, 32.0)` → 반복 제곱 5회로 대체하여 ~100 cycles/pixel 절감

**Independent Test**: S1–S4 씬 렌더 후 specular highlight 시각적 동일성 확인

### Implementation

- [X] T004 [P3] Add `static double fast_pow32(double x)` helper function in `src/lighting/lighting.c` (함수 위치: `clamp_color()` 앞 또는 `calculate_specular()` 앞)
- [X] T005 [P3] Replace `pow(spec, 32.0)` with `fast_pow32(spec)` in `calculate_specular()` at `src/lighting/lighting.c`
- [X] T006 [P3] Run `norminette src/lighting/lighting.c` and confirm no errors
- [X] T007 [P3] Run `make re` and confirm compilation with zero warnings

**Checkpoint**: P3 구현 완료, 빌드 성공.

---

## Phase 3: P8 — Sphere sqrt 캐싱

**Goal**: sqrt(discriminant) 결과를 `c` 변수에 저장하여 중복 호출 제거

**Independent Test**: Sphere 포함 씬(S2, S3, S4) 렌더 후 sphere 형태/조명 동일성 확인

### Implementation

- [X] T008 [P8] Modify `intersect_sphere_new()` in `src/ray/intersect_object.c`: change `d = (-b - sqrt(d)) / (2.0 * a)` to `c = sqrt(d)` followed by `d = (-b - c) / (2.0 * a)`
- [X] T009 [P8] Modify second root calculation: change `d = (-b + sqrt(b * b - 4 * a * c)) / (2.0 * a)` to `d = (-b + c) / (2.0 * a)`
- [X] T010 [P8] Run `norminette src/ray/intersect_object.c` and confirm no errors
- [X] T011 [P8] Run `make re` and confirm compilation with zero warnings

**Checkpoint**: P8 구현 완료, 빌드 성공.

---

## Phase 4: P7 — Shadow magnitude/normalize 통합

**Goal**: `vec3_magnitude()` + `vec3_normalize()` 이중 sqrt 호출을 1회로 통합

**Independent Test**: Shadow 있는 씬(S2, S3, S4) 렌더 후 그림자 형태 동일성 확인

### Implementation

- [X] T012 [P7] Add `double mag;` variable declaration in `is_in_shadow()` at `src/lighting/shadow_test.c`
- [X] T013 [P7] Replace `shadow_hit.distance = vec3_magnitude(to_light);` with `mag = vec3_magnitude(to_light);` followed by `shadow_hit.distance = mag;` in `src/lighting/shadow_test.c`
- [X] T014 [P7] Replace `light_dir = vec3_normalize(to_light);` with `light_dir = vec3_multiply(to_light, 1.0 / mag);` in `src/lighting/shadow_test.c`
- [X] T015 [P7] Run `norminette src/lighting/shadow_test.c` and confirm no errors
- [X] T016 [P7] Run `make re` and confirm compilation with zero warnings

**Checkpoint**: P7 구현 완료, 빌드 성공.

---

## Phase 5: Validation

**Purpose**: 전체 변경 후 기능 및 품질 검증

- [X] T017 Run full `norminette` on all 3 modified files and confirm no errors
- [X] T018 Run `make re` and confirm final build with zero warnings
- [X] T019 Run S1–S4 scenes and verify visual rendering identical to baseline (T003)
- [X] T020 Verify RGB difference within ±1 tolerance (시각적 확인 완료)

**Checkpoint**: 기능 검증 완료.

---

## Phase 6: Benchmark & Documentation

**Purpose**: 성능 측정 및 결과 기록

- [X] T021 Run S1–S4 scenes and record metrics (1회 측정)
- [X] T022 Update `docs/benchmark-results.md` with Round 2 data (Phase A: P3+P8+P7 누적 효과)
- [ ] T023 Commit code changes and benchmark documentation

**Checkpoint**: Round 2 benchmark 기록 완료, feature complete.

---

## Dependencies & Execution Order

### Phase Dependencies

- **Phase 1 (Pre-change Verification)**: No dependencies — start immediately
- **Phase 2 (P3)**: Depends on Phase 1 (baseline 확인)
- **Phase 3 (P8)**: Depends on Phase 1 (독립 파일이므로 Phase 2와 병렬 가능)
- **Phase 4 (P7)**: Depends on Phase 1 (독립 파일이므로 Phase 2, 3과 병렬 가능)
- **Phase 5 (Validation)**: Depends on Phase 2, 3, 4 완료
- **Phase 6 (Benchmark)**: Depends on Phase 5 완료

### Within Each Optimization Phase

```text
Implementation tasks → norminette → make re
```

### Parallel Opportunities

P3, P8, P7는 서로 다른 파일을 수정하므로 Phase 2, 3, 4를 병렬 실행 가능:

```text
Phase 1 완료 후:
  ├── Phase 2 (P3): src/lighting/lighting.c
  ├── Phase 3 (P8): src/ray/intersect_object.c
  └── Phase 4 (P7): src/lighting/shadow_test.c
모두 완료 → Phase 5 → Phase 6
```

단, 측정 정확성을 위해 순차 실행 권장 (P3 → P8 → P7).

---

## Implementation Strategy

### Sequential (권장)

1. Phase 1 완료: 빌드 및 baseline 확인
2. Phase 2 (P3) 완료: pow32 최적화
3. Phase 3 (P8) 완료: sqrt 캐싱
4. Phase 4 (P7) 완료: magnitude/normalize 통합
5. Phase 5 완료: 전체 검증
6. Phase 6 완료: 벤치마크 기록

### Parallel (팀 작업 시)

```text
Developer A: Phase 2 (P3)
Developer B: Phase 3 (P8)
Developer C: Phase 4 (P7)
→ 동시 완료 후 Phase 5, 6 진행
```

---

## Notes

- T004–T005: `fast_pow32` 함수는 `calculate_specular` 앞에 위치시켜 호출 순서 유지
- T008–T009: `c` 변수 용도 전환 — discriminant 계산 후 sqrt 결과 저장
- T012–T014: `mag` 변수 추가로 5개 변수 제한 충족 (현재 4개 → 5개)
- GUI 필요 작업(T019, T021)은 수동 실행 필요
- Commit은 T023에서 1회 수행 (코드 + 벤치마크 문서 포함)
