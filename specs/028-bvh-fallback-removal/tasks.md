# Tasks: BVH Miss Fallback Removal (P0)

**Input**: Design documents from `/specs/028-bvh-fallback-removal/`
**Prerequisites**: plan.md, spec.md, research.md

**Tests**: Not requested — manual visual verification + metrics comparison.

**Organization**: Single user story (US1: fallback removal). Benchmark measurement as polish phase.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1)
- Include exact file paths in descriptions

---

## Phase 1: Pre-change Verification

**Purpose**: Confirm BVH covers all objects and capture baseline metrics before code change.

- [X] T001 Verify `scene_build_bvh()` passes `scene->objects.count` to `bvh_build()` in `src/spatial/bvh_init.c`
- [X] T002 Build project with `make re` and confirm no warnings
- [X] T003 Run S1–S4 scenes and capture baseline metrics output (pre-change reference)

**Checkpoint**: BVH coverage confirmed, baseline captured.

---

## Phase 2: User Story 1 — Fallback Removal

**Goal**: Remove redundant `check_all_objects()` fallback from `trace_ray()` when BVH is valid, while preserving fallback for BVH-invalid/disabled states.

**Independent Test**: Run S1–S4 scenes, verify identical rendering and reduced primary `intersect_tests`.

### Implementation

- [X] T004 [US1] Modify `trace_ray()` in `src/render/trace.c`: change BVH-on condition from `scene->bvh && scene->bvh->enabled` to `scene->bvh && scene->bvh->enabled && scene->bvh->root`
- [X] T005 [US1] Modify `trace_ray()` in `src/render/trace.c`: restructure to if/else — BVH valid branch returns directly from `bvh_intersect()` result, else branch uses `check_all_objects()`
- [X] T006 [US1] Remove `found` variable from `trace_ray()` in `src/render/trace.c` (no longer needed with early-return structure)
- [X] T007 [US1] Update function comment/doxygen for `trace_ray()` in `src/render/trace.c` to reflect new BVH-valid/invalid branching logic

### Validation

- [X] T008 [US1] Run `norminette src/render/trace.c` and confirm no errors
- [X] T009 [US1] Run `make re` and confirm compilation with zero warnings under `-Wall -Wextra -Werror`
- [X] T010 [US1] Run S1–S4 scenes and verify visual rendering is identical to baseline (T003) ⚠️ GUI 필요
- [X] T011 [US1] Run S1–S4 scenes and verify primary `intersect_tests` decreased compared to baseline (T003) — S2–S4 중 최소 1개 씬에서 10% 이상 감소 확인 ⚠️ GUI 필요
- [X] T012 [US1] BVH root null 엣지 케이스 검증: `src/render/trace.c`의 `trace_ray()` 함수 내 BVH 조건 직전에 `scene->bvh->root = NULL;` 1줄을 임시 삽입 → `make re` → S1 씬 실행하여 `check_all_objects()` 경로로 정상 렌더 확인 → 임시 줄 삭제 후 `make re`로 원복

**Checkpoint**: Fallback removed, rendering correct, metrics improved, edge case verified.

---

## Phase 3: Polish — Benchmark Documentation

**Purpose**: Record measurement results per `docs/optimization-plan.md` Phase 0 measurement gate.

- [X] T013 Run S1–S4 scenes 5 times each, record median metrics per `docs/optimization-metrics-collection-plan.md`
- [X] T014 Create `docs/benchmark-results.md` Round 1 using template from `docs/benchmark-baseline.md` Appendix B, filling baseline vs optimized data
- [ ] T015 Commit code change (`src/render/trace.c`) and benchmark documentation (`docs/benchmark-results.md`)

**Checkpoint**: Round 1 benchmark recorded, feature complete.

---

## Dependencies & Execution Order

### Phase Dependencies

- **Phase 1 (Pre-change Verification)**: No dependencies — start immediately
- **Phase 2 (US1 — Fallback Removal)**: Depends on Phase 1 (baseline capture)
  - T004–T006 are sequential (same function in same file)
  - T007 depends on T004–T006 (comment reflects final code)
  - T008–T011 depend on T004–T007 (validation after implementation)
- **Phase 3 (Polish)**: Depends on Phase 2 completion (code must be correct before measurement)

### Within Phase 2

```text
T004 → T005 → T006 → T007 → T008, T009 (parallel) → T010, T011 (parallel) → T012
```

- T004–T007: Sequential (all modify same function in same file)
- T008, T009: Parallel (norminette and make are independent)
- T010, T011: Parallel (visual check and metrics check are independent verifications)
- T012: Edge case verification (depends on T004–T007 implementation)

---

## Implementation Strategy

### Single-pass delivery

1. Complete Phase 1: Confirm preconditions and capture baseline
2. Complete Phase 2: Apply code change + validate
3. Complete Phase 3: Measure and document
4. **DONE**: Feature complete, ready for merge to develop

---

## Notes

- T004–T006 are logically a single edit to `trace_ray()` but separated for traceability
- Implementation can treat T004–T006 as one atomic change if preferred
- No parallel user stories — this is a single focused change
- Commit은 T015에서 1회 수행 (코드 + 벤치마크 문서 포함)
