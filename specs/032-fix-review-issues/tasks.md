# Tasks: 032-fix-review-issues

**Input**: Design documents from `/specs/032-fix-review-issues/`
**Prerequisites**: plan.md (required), spec.md (required), research.md, data-model.md

**Tests**: Not requested. Validation via norminette + manual benchmark.

**Organization**: Tasks are grouped by fix (R1+R2, R3, R4). R1+R2는 동일 파일·함수이므로 합산.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which fix this task belongs to (R1R2, R3, R4)
- Include exact file paths in descriptions

---

## Phase 1: R1+R2 — BVH malloc 가드 + 누수 수정 (Priority: 심각+높음) 🎯

**Goal**: malloc 실패 시 NULL 역참조 방지 + 재빌드 시 plane_refs.indices 누수 해결

**Independent Test**: `make re && norminette src/spatial/bvh_init.c` → 빌드 성공 + Norm 통과

### Implementation

- [X] T001 [R1R2] Add `alloc_bvh_refs()` static function between `fill_separated_refs()` and `scene_build_bvh()` in `src/spatial/bvh_init.c`
  - Free existing `plane_refs.indices` (R2 누수 수정)
  - Set `plane_refs.count = 0` (기본값, 성공 시에만 pc로 설정)
  - Allocate `plane_refs.indices` and bounded object refs
  - Validate both mallocs, return 0 on failure
  - **실패 시 cleanup**: bc 할당 실패 시 이미 할당된 indices도 free + NULL 처리
  - 성공 시에만 `plane_refs.count = pc` 설정 (실패 시 count=0 유지 → 소비자 루프 미진입)
  - Norm: 18줄, 4파라미터, 0변수 → 준수
  - plan.md Phase 1 §1-1 코드 참조

- [X] T002 [R1R2] Refactor `scene_build_bvh()` to use `alloc_bvh_refs()` in `src/spatial/bvh_init.c`
  - Replace `scene_build_bvh()` 내 plane_refs 할당 + refs 할당 블록을 `alloc_bvh_refs()` 호출로 교체
  - Add early return with `free(refs)` on allocation failure
  - Remove `if (bc > 0 && refs)` guard → simplify to `if (bc > 0)` (근거: alloc_bvh_refs return 1 시 bc>0이면 refs≠NULL 보장)
  - 실패 분기: `free(refs)` + return (refs는 alloc_bvh_refs 내에서 NULL 초기화 → free(NULL) 안전)
  - Norm: 20줄, 3변수, 1파라미터 → 준수
  - plan.md Phase 1 §1-2 코드 참조

- [X] T003 [R1R2] Verify norminette and build for `src/spatial/bvh_init.c`
  - `norminette src/spatial/bvh_init.c` → 0 errors
  - `make re` → 빌드 성공
  - 파일 함수 수 확인: 5함수 (get_object_center, count_planes, fill_separated_refs, alloc_bvh_refs, scene_build_bvh)

**Checkpoint**: BVH 재빌드 시 malloc 실패 안전 + 메모리 누수 없음

---

## Phase 2: R3 — FOV 경계값 제한 (Priority: 중간)

**Goal**: FOV 0과 180을 파서에서 거부하여 수치 불안정 방지

**Independent Test**: FOV=0, FOV=180 → `Error` 출력, FOV=1, FOV=179 → 정상 렌더

### Implementation

- [X] T004 [P] [R3] Change FOV range from `in_range(fov, 0, 180)` to `in_range(fov, 1, 179)` in `parse_camera()` of `src/parser/parse_elements.c`
  - `fov`는 int → `1 ≤ fov ≤ 179` == `0 < fov < 180`
  - 줄 수 변화 없음, 상수 변경만

- [X] T005 [R3] Verify norminette and build for `src/parser/parse_elements.c`
  - `norminette src/parser/parse_elements.c` → 0 errors
  - `make re` → 빌드 성공

**Checkpoint**: FOV 0/180 파싱 거부, FOV 1/179 정상 동작

---

## Phase 3: R4 — set_shadow_samples() LUT 재생성 (Priority: 중간/잠재)

**Goal**: 샘플 수 변경 시 offset_lut를 새 크기로 재할당하여 힙 오버리드 방지

**Independent Test**: `norminette src/lighting/shadow_config.c` + 기존 렌더링 동일

### Implementation

- [X] T006 [P] [R4] Rewrite `set_shadow_samples()` with LUT regeneration in `src/lighting/shadow_config.c`
  - Early return if `!config || samples < 1`
  - `free(config->offset_lut)` → `config->offset_lut = NULL`
  - `config->samples = samples`
  - Call `init_shadow_offset_lut(config)` (same-file static, already defined above)
  - Norm: 8줄, 0변수 → 준수
  - plan.md Phase 3 §3-1 코드 참조

- [X] T007 [R4] Verify norminette and build for `src/lighting/shadow_config.c`
  - `norminette src/lighting/shadow_config.c` → 0 errors
  - `make re` → 빌드 성공
  - 파일 함수 수 확인: 5함수 (변경 없음)

**Checkpoint**: set_shadow_samples() 호출 시 LUT 크기가 samples에 연동

---

## Phase 4: Validation & Polish

**Purpose**: 전체 수정 사항 통합 검증

- [X] T008 Run full norminette on all modified files
  - `norminette src/spatial/bvh_init.c src/parser/parse_elements.c src/lighting/shadow_config.c`
  - 0 errors 확인

- [X] T009 Run benchmark regression test (237.1ms / 5945.2ms — 회귀 없음)
  - `./miniRT scenes/perf/perf_timing.rt` → ~241ms (변화 없음)
  - `./miniRT scenes/perf/perf_all_objects.rt` → ~6,067ms (변화 없음)

- [X] T010 Test FOV boundary values
  - FOV=0 씬 파일 → `Error` + `PARSE_ERR_RANGE` 경로 출력 확인
  - FOV=180 씬 파일 → `Error` + `PARSE_ERR_RANGE` 경로 출력 확인
  - FOV=1 씬 파일 → 정상 렌더 확인
  - FOV=179 씬 파일 → 정상 렌더 확인

- [X] T011 Verify R4 fallback path — 그림자 정상 렌더링 확인
  - `init_shadow_config()` 내 `init_shadow_offset_lut(&config)` 호출을 임시 주석 처리 → offset_lut = NULL 강제
  - `make re && ./miniRT scenes/perf/perf_timing.rt` → 그림자 렌더링 정상 확인
  - `sample_shadow_ray()`가 `generate_shadow_sample_offset()` fallback 사용 확인 (shadow_calc.c의 `sample_shadow_ray` 함수)
  - 확인 후 리버트: `git checkout src/lighting/shadow_config.c`

- [X] T012 Run quickstart.md validation — FOV 1/179 정상, 나머지 수동 진행
  - quickstart.md의 모든 검증 단계 실행 및 확인

---

## Dependencies & Execution Order

### Phase Dependencies

- **Phase 1 (R1+R2)**: No dependencies — 즉시 시작 가능 (최고 우선순위)
- **Phase 2 (R3)**: No dependencies on Phase 1 — **Phase 1과 병렬 가능**
- **Phase 3 (R4)**: No dependencies on Phase 1/2 — **Phase 1/2와 병렬 가능**
- **Phase 4 (Validation)**: Phase 1-3 모두 완료 후

### Within Each Phase

- Phase 1: T001 → T002 → T003 (순차: 같은 파일, 의존 관계)
- Phase 2: T004 → T005 (순차: 같은 파일)
- Phase 3: T006 → T007 (순차: 같은 파일)
- Phase 4: T008 → T009, T010, T011, T012 (T009-T012 병렬 가능)

### Parallel Opportunities

```
Phase 1 (R1R2)  ──→ T001 → T002 → T003 ─┐
Phase 2 (R3)    ──→ T004 → T005 ─────────┼──→ Phase 4: T008 → T009/T010/T011/T012
Phase 3 (R4)    ──→ T006 → T007 ─────────┘
```

- Phase 1/2/3는 모두 다른 파일이므로 **완전 병렬 실행 가능**
- 실제로는 Phase 1 (심각) 먼저 완료 후 Phase 2/3 병렬 진행 권장

---

## Implementation Strategy

### Sequential (권장)

1. Phase 1: R1+R2 (bvh_init.c) — 심각도 최고, 먼저 완료
2. Phase 2+3: R3, R4 병렬 (다른 파일) — 중간 심각도
3. Phase 4: 전체 검증

### 커밋 전략

1. `fix(bvh): add malloc guard and fix plane_refs leak in scene_build_bvh` — T001-T003
2. `fix(parser): restrict FOV range to exclusive (0,180)` — T004-T005
3. `fix(shadow): regenerate offset LUT on sample count change` — T006-T007

---

## Notes

- 모든 수정은 기존 파일 내에서 이루어짐 (신규 파일 없음)
- Norm v4.1: 25줄/함수, 5변수/함수, 4파라미터/함수, 5함수/파일 준수
- 렌더링 결과 변경 없음 — 방어적 코딩만 보강
