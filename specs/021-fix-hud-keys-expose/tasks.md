# Tasks: Fix HUD Key Guide & Expose Restore

**Input**: Design documents from `/specs/021-fix-hud-keys-expose/`
**Prerequisites**: plan.md, spec.md, research.md, data-model.md, quickstart.md

**Organization**: Tasks are grouped by user story to enable independent implementation and testing.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2)
- Include exact file paths in descriptions

---

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: KEYGUIDE_HEIGHT 상수 변경 및 Makefile 준비 (양쪽 US 공통)

- [x] T001 Update KEYGUIDE_HEIGHT from 400 to 500 in includes/keyguide.h
- [x] T002 Add keyguide_render_extra.c to source list in Makefile

**Checkpoint**: 빌드 환경 준비 완료. 기존 기능 변경 없이 컴파일 가능해야 함.

---

## Phase 2: User Story 2 - Display New Keys in Key Guide (Priority: P1)

**Goal**: 키 가이드에 Resize(J/K, N/M) 및 Rotation(U/O, Y/P, ←/→) 섹션을 추가하여 신규 키 라벨을 표시한다.

**Independent Test**: 프로그램 실행 → H키 → 키 가이드에 Resize/Rotation 섹션 및 10개 키 라벨이 표시되는지 확인.

### Implementation for User Story 2

- [x] T003 [US2] Create keyguide_render_extra.c with 42 header, keyguide_render_resize(), keyguide_render_rotation(), keyguide_render_extra() in src/keyguide/keyguide_render_extra.c
- [x] T004 [US2] Add keyguide_render_extra() prototype to includes/keyguide.h
- [x] T005 [US2] Call keyguide_render_extra() from keyguide_render() in src/keyguide/keyguide_render.c
- [x] T006 [US2] Run norminette on src/keyguide/keyguide_render_extra.c, src/keyguide/keyguide_render.c, includes/keyguide.h and fix errors
- [x] T007 [US2] Build and visually verify Resize/Rotation labels display correctly without overlap (SC-002, SC-004)

**Checkpoint**: 키 가이드에 Resize/Rotation 섹션이 헤더와 함께 표시됨. 기존 섹션 레이아웃 영향 없음.

---

## Phase 3: User Story 1 - HUD Not Restored After Window Expose (Priority: P1)

**Goal**: expose 이벤트 시 HUD와 키 가이드가 이전 visible 상태에 따라 재출력되도록 수정한다.

**Independent Test**: HUD 표시 상태에서 다른 창으로 가림 → 복원 → HUD/키가이드 그대로 표시 확인.

### Implementation for User Story 1

- [x] T008 [US1] Modify handle_expose() to call hud_render() and keyguide_render() when hud.visible is true in src/window/window_events.c
- [x] T009 [US1] Add required includes (hud.h, keyguide.h) to src/window/window_events.c if not present
- [x] T010 [US1] Run norminette on src/window/window_events.c and fix errors
- [x] T011 [US1] 수동 검증: expose restore: HUD ON → cover → restore → HUD visible (SC-001)
- [x] T012 [US1] 수동 검증: expose restore: HUD OFF → cover → restore → HUD not visible (SC-001)
- [x] T013 [US1] 수동 검증: SC-003: HUD Performance 섹션의 "Frame:" 값을 expose 전후로 비교하여 프레임 카운터가 증가하지 않음을 확인

**Checkpoint**: expose 복원 후 HUD/키가이드 상태가 100% 유지됨. 추가 렌더링 없음.

---

## Phase 4: Polish & Cross-Cutting Concerns

**Purpose**: Norminette 최종 검증, 불필요 코드 제거, 전체 빌드 확인

- [x] T014 Run norminette on all changed files: includes/keyguide.h, src/keyguide/keyguide_render.c, src/keyguide/keyguide_render_extra.c, src/window/window_events.c
- [x] T015 Remove dead code: (1) make re로 -Wall -Werror 미사용 static 함수/변수 자동 검출·제거, (2) non-static 심볼은 grep -rn으로 src/ includes/ 참조 횟수 확인하여 정의만 존재(0% 사용)하면 제거, (3) make re 빌드 성공 + 런타임 정상 동작 검증
- [x] T016 Full build verification with make re and -Wall -Wextra -Werror clean compile
- [x] T017 Run quickstart.md validation: all SC-001 through SC-004 pass

---

## Dependencies & Execution Order

### Phase Dependencies

- **Phase 1 (Setup)**: No dependencies — start immediately
- **Phase 2 (US2 - Key Guide)**: Depends on T001, T002 (Setup)
- **Phase 3 (US1 - Expose)**: Depends on Phase 2 completion (expose 재출력 시 신규 키도 포함되어야 함)
- **Phase 4 (Polish)**: Depends on Phase 2 + Phase 3 completion

### User Story Dependencies

- **US2 먼저 (권장)**: keyguide 신규 섹션이 추가되어야 expose 복원 시에도 올바르게 표시됨
- **US1 다음**: expose 핸들러 수정은 US2 완료 후 진행하여 신규 섹션 포함 상태에서 검증
- **참고**: expose 복원 로직 자체는 신규 keyguide 섹션과 독립적이므로 US1을 병렬 구현 후 US2 완료 시점에 통합 검증하는 것도 가능

### Parallel Opportunities

- T001과 T002는 병렬 실행 가능 (서로 다른 파일)
- T003과 T004는 병렬 실행 가능 (서로 다른 파일)
- US1과 US2는 순차 실행 권장 (US2 → US1)

---

## Implementation Strategy

### MVP First (User Story 2)

1. Phase 1: Setup (T001, T002)
2. Phase 2: US2 - Key Guide 라벨 추가 (T003~T007)
3. **STOP and VALIDATE**: 키 가이드에 10개 신규 키 표시 확인

### Full Delivery

1. Setup → US2 → US1 → Polish
2. 각 Phase 완료 후 checkpoint 검증
3. Phase 4에서 전체 norminette + 빌드 + SC 검증

---

## Summary

| Metric | Value |
|--------|-------|
| Total tasks | 17 |
| US1 tasks | 6 (T008~T013) |
| US2 tasks | 5 (T003~T007) |
| Setup tasks | 2 (T001~T002) |
| Polish tasks | 4 (T014~T017) |
| Parallel opportunities | T001∥T002, T003∥T004 |
| Execution order | Setup → US2 → US1 → Polish |
| MVP scope | Phase 1 + Phase 2 (US2: keyguide 라벨 추가) |
