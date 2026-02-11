# Tasks: HUD / Input / Rendering Optimization

**Input**: Design documents from `/specs/033-hud-input-optimization/`
**Prerequisites**: plan.md, spec.md, research.md, data-model.md, contracts/

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2, US3)
- Include exact file paths in descriptions

---

## Phase 1: Foundational (Header Awareness)

**Purpose**: 기존 프로젝트이므로 별도 초기화 불필요. 모든 변경은 기존 모듈 내에서 수행.

**⚠️ CRITICAL**: US2(데드 코드 제거)가 `render_debounce.h`에서 오래된 필드/enum/defines를 삭제하므로, US1(상태 머신 재설계)보다 **반드시 먼저** 완료해야 한다. US2는 foundational phase 역할을 한다.

**Checkpoint**: Phase 1은 실질 작업 없음 — Phase 2부터 시작.

---

## Phase 2: User Story 2 - 데드 코드 제거 (Priority: P1)

**Goal**: 사용되지 않는 코드(KEY_I 핸들러, RENDER_SHOW_INFO, hud_render_background, cancel_requested, preview_enabled, auto_upgrade, DEBOUNCE_PREVIEW)를 제거하여 유지보수성 향상 및 불필요한 렌더 트리거 방지. `render_debounce.h`/`.c` 정리가 US1의 전제 조건.

**Independent Test**: `make fclean && make`가 `-Wall -Wextra -Werror` 경고 없이 성공하고, `I` 키 입력 시 렌더가 트리거되지 않으며, HUD `H` 키가 정상 동작.

### Implementation for User Story 2

- [ ] T001 [P] [US2] Remove KEY_I handler (`else if (keycode == KEY_I)` block, lines 39-43) from `src/window/window_events.c`; remove `RENDER_SHOW_INFO` define (line 33) from `includes/window.h`. Note: `KEY_I` define in `includes/window_internal.h` is **KEPT** (repurposed for Obj Rotate X- in US4)
- [ ] T002 [P] [US2] Remove `"Display:"` section header and `"I - Info"` text (lines 96-101) from `src/keyguide/keyguide_render.c`
- [ ] T003 [P] [US2] Remove `hud_render_background_row()` (lines 32-53) and `hud_render_background()` (lines 60-70) from `src/hud/hud_text.c`; remove declarations from `includes/hud_text.h` (lines 19-20) and `includes/hud.h` (line 294)
- [ ] T004 [US2] Remove cancel_requested dead code across 5 files: `int cancel_requested` field (line 50) and `debounce_cancel()` declaration (line 63) from `includes/render_debounce.h`; `state->cancel_requested = 1` (line 57) and `cancel_requested = 0` init from `src/render/render_debounce.c`; `debounce_cancel()` function definition (lines 23-26) from `src/render/render_debounce_timer.c`; cancel check block (lines 52-57) from `src/window/window_loop.c`; cancel checks (`if (state->cancel_requested)`) from `src/render/render.c` (~lines 119, 159)
- [ ] T005 [US2] Remove PREVIEW-related dead code: `DEBOUNCE_PREVIEW` from enum (line 30), `DEBOUNCE_DEFAULT_PREVIEW` (line 20) and `DEBOUNCE_DEFAULT_AUTO_UPGRADE` (line 21) defines, `int preview_enabled` (line 48) and `int auto_upgrade` (line 49) fields from `includes/render_debounce.h`; remove `debounce_handle_preview()` static function, `preview_enabled` branch from `debounce_handle_active()`, and field initializations from `debounce_init()` in `src/render/render_debounce.c`
- [ ] T006 [US2] Build verification: `make fclean && make` with zero warnings; `norminette src/render/render_debounce.c src/render/render_debounce_timer.c src/window/window_events.c src/window/window_loop.c src/render/render.c src/hud/hud_text.c src/keyguide/keyguide_render.c includes/render_debounce.h includes/window.h includes/window_internal.h includes/hud.h includes/hud_text.h`

**Checkpoint**: 모든 데드 코드 제거 완료. `render_debounce.h`에 state, timer 필드만 남음. `render_debounce.c`에 4함수(init, on_input, handle_active, update), `render_debounce_timer.c`에 4함수(start, reset, stop, expired). 빌드 성공, I 키 무반응 확인.

---

## Phase 3: User Story 1 - 디바운스 상태 머신 재설계 (Priority: P1) 🎯 MVP

**Goal**: IDLE → ACTIVE → FINAL → COOLDOWN 4-state FSM 구현. 즉시 LQ 프리뷰(스로틀 50ms) + 150ms 디바운스 → FQ → 350ms 쿨다운. 이산 탭 5회 시 FQ 2회 이하.

**Independent Test**: 씬 로드 후 W 키를 300ms 간격으로 5회 탭 → FQ 렌더 1-2회. Key-hold 시 LQ ~20fps + FQ 1회.

### Implementation for User Story 1

- [ ] T007 [US1] Update `includes/render_debounce.h`: add `DEBOUNCE_COOLDOWN` to enum (in place of removed PREVIEW), add `DEBOUNCE_COOLDOWN_MS` (350) and `DEBOUNCE_PREVIEW_MIN_INTERVAL_MS` (50) defines, add `struct timeval last_preview_time` field to `t_debounce_state`, add `debounce_check_preview_throttle()` declaration
- [ ] T008 [US1] Implement `debounce_check_preview_throttle()` in `src/render/render_debounce_timer.c`: calculate elapsed ms from `state->last_preview_time` using `gettimeofday()`, return 1 if `>= DEBOUNCE_PREVIEW_MIN_INTERVAL_MS` (LQ allowed), 0 otherwise (LQ skipped). Fills the function slot freed by `debounce_cancel()` removal (4→5 functions)
- [ ] T009 [US1] Rewrite `debounce_on_input()` and update `debounce_init()` in `src/render/render_debounce.c`: init adds `memset(&state->last_preview_time, 0, sizeof(struct timeval))`; on_input handles 4 state branches — IDLE→ACTIVE (immediate LQ with throttle + timer start), ACTIVE (timer reset + LQ with throttle), FINAL→ACTIVE (LQ + timer reset), COOLDOWN→ACTIVE (LQ + timer reset). LQ trigger: `render_set_flag(render, RENDER_LOW_QUALITY | RENDER_DIRTY)` gated by `debounce_check_preview_throttle()`, update `last_preview_time` via `gettimeofday()` on trigger
- [ ] T010 [US1] Rewrite `debounce_handle_active()` and implement `debounce_handle_cooldown()` as static functions in `src/render/render_debounce.c`: handle_active checks timer expiry → FINAL transition (set RENDER_DIRTY, clear RENDER_LOW_QUALITY); handle_cooldown checks cooldown timer expiry → IDLE transition + timer stop. handle_cooldown replaces the removed handle_preview slot (4→5 functions)
- [ ] T011 [US1] Rewrite `debounce_update()` in `src/render/render_debounce.c`: dispatch ACTIVE to `debounce_handle_active()`, handle FINAL + `!RENDER_DIRTY` → COOLDOWN (start cooldown timer with `DEBOUNCE_COOLDOWN_MS`), dispatch COOLDOWN to `debounce_handle_cooldown()`. Remove PREVIEW dispatch entirely
- [ ] T012 [US1] Norm compliance verification: `norminette src/render/render_debounce.c src/render/render_debounce_timer.c includes/render_debounce.h` — verify each function ≤ 25 lines, ≤ 5 local variables; verify `render_debounce.c` has exactly 5 functions (init, on_input, handle_active, handle_cooldown, update) and `render_debounce_timer.c` has exactly 5 functions (check_preview_throttle, timer_start, timer_reset, timer_stop, timer_expired)
- [ ] T013 [US1] Build and manual test: `make fclean && make`, then test with `./miniRT scenes/valid/perf_all_objects.rt` — verify: single tap (LQ→FQ), rapid taps (<150ms interval, FQ 1회), key-hold (LQ ~20fps + FQ 1회 on release), FQ 직후 탭 (COOLDOWN→ACTIVE 복귀 확인)

**Checkpoint**: 디바운스 상태 머신 완전 재설계 완료. 이산 탭 병합 동작 확인, key-hold LQ 프리뷰 동작 확인. SC-001 (FQ ≤ 2회), SC-002 (LQ ~20fps) 달성.

---

## Phase 4: User Story 3 - pixel_timing 게이트 추가 (Priority: P2)

**Goal**: `RENDER_ENABLE_PIXEL_TIMING` 미설정 시 pixel_timing reset/calculate/print 호출을 생략하여 불필요한 연산과 stdout 노이즈 제거.

**Independent Test**: 기본 실행(`RENDER_ENABLE_PIXEL_TIMING` 미설정) 시 stdout에 pixel timing 출력이 없는지 확인.

### Implementation for User Story 3

- [ ] T014 [US3] Add `RENDER_ENABLE_PIXEL_TIMING` flag guard around `pixel_timing_reset()` (line ~155) and `pixel_timing_calculate_stats()`/`pixel_timing_print_stats()` (lines ~169-170) in `src/render/render.c`. Use `render_has_flag(render, RENDER_ENABLE_PIXEL_TIMING)` conditional per contracts/dead-code.md section 5
- [ ] T015 [US3] Build and verify: `make fclean && make && norminette src/render/render.c`, run `./miniRT scenes/valid/three_spheres.rt` and confirm no pixel_timing output on stdout

**Checkpoint**: pixel_timing 게이트 동작 확인. 플래그 설정 시 기존과 동일 동작 확인. SC-004 달성.

---

## Phase 5: User Story 4 - 키맵 변경 및 키가이드 업데이트 (Priority: P2)

**Goal**: macOS 호환 키맵(Option A)으로 Light 이동 키 교체(`[]/;'/,.`), Resize를 Y,U/N,M 그리드로, Rotate를 I,J/O,K/P,L 그리드로 재배치, 오브젝트 선택을 TAB/SHIFT+TAB으로 변경, HUD 페이지 네비게이션(SHIFT+,/.) 추가. 키가이드에 모든 변경 반영.

**Independent Test**: `[`/`]` Light X, `;`/`'` Light Y, `,`/`.` Light Z, `Y`/`U` Resize X, `I`/`J` Rotate X, `O`/`K` Rotate Y, `P`/`L` Rotate Z, TAB 선택, SHIFT+,/. HUD 페이지 전환 동작 확인. 키가이드 표시 확인.

### Implementation for User Story 4

- [ ] T016 [US4] Update key defines in `includes/window_internal.h`: add `KEY_SEMICOLON` (macOS 41, Linux 59), `KEY_QUOTE` (macOS 39, Linux 39), `KEY_COMMA` (macOS 43, Linux 44), `KEY_PERIOD` (macOS 47, Linux 46), `KEY_L` (macOS 37, Linux 108) for both platform sections; remove `KEY_INSERT`, `KEY_HOME`, `KEY_PGUP`, `KEY_DELETE`, `KEY_END`, `KEY_PGDN`, `KEY_LEFT`, `KEY_RIGHT` from both platform sections. Keep `KEY_I` (Rotate X-), `KEY_J`/`KEY_K` (Rotate X+/Y+), `KEY_Y`/`KEY_U` (Resize X-/X+)
- [ ] T017 [P] [US4] Update `handle_light_move()` and `handle_object_resize()` in `src/window/window_objects.c`: light_move replaces KEY_INSERT/DELETE/HOME/END/PGUP/PGDN with KEY_BRACKET_LEFT/RIGHT, KEY_SEMICOLON/QUOTE, KEY_COMMA/PERIOD for X/Y/Z axis; object_resize replaces KEY_J/KEY_K with KEY_Y/KEY_U for X axis (KEY_N/KEY_M Y axis unchanged)
- [ ] T018 [P] [US4] Update `handle_object_rotate()` in `src/window/window_rotate.c`: replace KEY_U/KEY_O → KEY_I/KEY_J (X axis), KEY_Y/KEY_P → KEY_O/KEY_K (Y axis), KEY_LEFT/KEY_RIGHT → KEY_P/KEY_L (Z axis) in `get_rotation_axis()` or equivalent key-to-axis mapping
- [ ] T019 [P] [US4] Update `handle_transform_keys()` in `src/window/window_key_handlers.c`: replace light move dispatch keys with KEY_BRACKET_LEFT/RIGHT, KEY_SEMICOLON, KEY_QUOTE, KEY_COMMA, KEY_PERIOD; replace resize dispatch keys with KEY_Y, KEY_U, KEY_N, KEY_M; replace rotate dispatch keys with KEY_I, KEY_J, KEY_O, KEY_K, KEY_P, KEY_L. Add HUD page navigation to `handle_hud_keys()`: check `RENDER_SHIFT_HELD` flag + KEY_COMMA → page up, RENDER_SHIFT_HELD + KEY_PERIOD → page down
- [ ] T020 [P] [US4] Update `handle_object_selection()` in `src/window/window_selection.c`: remove KEY_BRACKET_LEFT/KEY_BRACKET_RIGHT selection path (now used for light move); ensure TAB/SHIFT+TAB selection works (already partially supported via `handle_hud_keys`)
- [ ] T021 [US4] Update keyguide text in `src/keyguide/keyguide_render.c` and `src/keyguide/keyguide_render_extra.c`: add Light `[ ] ; ' , .` Move X/Y/Z, Resize `Y U / N M` Size X/Y, Rotate `I J / O K / P L` Rot X/Y/Z, HUD `< >` Page sections; change `"[] - Select"` to `"TAB - Select"` (line 85); review `"TAB - Next"` (line 82) for consolidation with `"TAB - Select"`; update existing Resize/Rotate key labels if present
- [ ] T022 [US4] Build, norminette, and manual test: `make fclean && make && norminette src/window/ src/keyguide/ includes/window_internal.h`, then test all key bindings per quickstart.md section 4 with `./miniRT scenes/valid/perf_all_objects.rt`

**Checkpoint**: 모든 키맵 변경 완료. Light/Resize/Rotate 키 동작 확인, 키가이드에 모든 키 표시 확인. SC-005 달성.

---

## Phase 6: User Story 5 - 키가이드 점진적 리드로 (Priority: P3)

**Goal**: 키가이드 텍스트를 정적(라벨)/동적(값) 섹션으로 분리하여 매 프레임 전체 `mlx_string_put()` 호출을 방지. 정적 라벨은 1회 렌더, 동적 값만 변경 시 업데이트.

**Independent Test**: 키가이드 표시 상태에서 카메라 이동 시 정적 라벨(키 이름)이 재렌더되지 않고 동적 값만 업데이트되는지 확인.

### Implementation for User Story 5

- [ ] T023 [US5] Design static/dynamic split for keyguide in `src/keyguide/keyguide_render.c`: identify static labels (key names, section headers like "Camera:", "Light:", "Rotate:") vs dynamic values (selected object name, mode indicator); add dirty flag or cache mechanism to `t_keyguide_state` in `includes/window.h` to track whether static labels need redraw
- [ ] T024 [US5] Implement conditional rendering in `keyguide_render()` in `src/keyguide/keyguide_render.c`: render static labels only on first draw or visibility toggle (`H` key); render dynamic values on every dirty update. Verify Norm compliance (5 functions/file, 25 lines/function). If file exceeds 5 functions, split helpers into `src/keyguide/keyguide_render_extra.c`
- [ ] T025 [US5] Build, norminette, and manual test: `make fclean && make && norminette src/keyguide/ includes/window.h`, test keyguide toggle (`H` key) and verify reduced `mlx_string_put` calls during active camera/object manipulation

**Checkpoint**: 키가이드 점진적 리드로 동작 확인. 입력 활발 시 체감 렌더링 부하 감소.

---

## Phase 7: Polish & Cross-Cutting Concerns

**Purpose**: 전체 기능 통합 검증, Norm 준수 확인, 회귀 테스트

- [ ] T026 Run full norminette check: `norminette src/ includes/` — zero errors
- [ ] T027 Full build verification: `make fclean && make` with `-Wall -Wextra -Werror` — zero warnings
- [ ] T028 [P] Run quickstart.md validation: execute all 5 manual test scenarios from `specs/033-hud-input-optimization/quickstart.md` (debounce, dead code, pixel_timing, keymap, keyguide)
- [ ] T029 [P] Regression test: run `./miniRT` with all `scenes/valid/*.rt` files per quickstart.md regression script, verify no rendering corruption or crashes
- [ ] T030 Final integration check: verify SC-001 through SC-007 from spec.md — FQ ≤ 2 on 5-tap, LQ ~20fps on key-hold, KEY_I no-op, no pixel_timing stdout, macOS Light move works, norminette pass, identical render output

---

## Dependencies & Execution Order

### Phase Dependencies

- **Phase 1 (Foundational)**: No work needed — existing project
- **Phase 2 (US2 - Dead Code)**: Can start immediately. **BLOCKS Phase 3 (US1)** — `render_debounce.h`/`.c` must be cleaned before adding new fields/functions
- **Phase 3 (US1 - Debounce)**: **Depends on Phase 2 completion** — header and source files must be in clean state
- **Phase 4 (US3 - pixel_timing)**: Independent — can start after Phase 2 or in parallel with Phase 3
- **Phase 5 (US4 - Keymap)**: Independent — can start after Phase 2 (KEY_I handler removed, define kept for Rotate X-) or in parallel with Phase 3/4
- **Phase 6 (US5 - Keyguide Redraw)**: Depends on Phase 5 (US4) — keyguide content must be finalized before optimizing rendering
- **Phase 7 (Polish)**: Depends on all previous phases

### User Story Dependencies

- **US2 (P1)**: No dependencies — start immediately. **BLOCKS US1.**
- **US1 (P1)**: Depends on US2 (header cleanup). Core MVP.
- **US3 (P2)**: No story dependencies — independently implementable after US2.
- **US4 (P2)**: No story dependencies — independently implementable after US2 (KEY_I handler removed, define kept).
- **US5 (P3)**: Depends on US4 (keyguide content finalized).

### Within Each User Story

- Header/define changes before implementation
- Implementation before Norm verification
- Norm verification before build test
- Build test before manual test

### Parallel Opportunities

- **Phase 2 내부**: T001, T002, T003 are all [P] — different files, run in parallel. T004→T005 sequential (same files).
- **Phase 3 이후**: US3 (Phase 4) and US4 (Phase 5) can run in parallel with US1 (Phase 3)
- **Phase 5 내부**: T016 (header) is prerequisite for T017-T021. T017, T018, T019, T020 are all [P] (different files).

---

## Parallel Example: Phase 2 (Dead Code Removal)

```text
# 병렬 실행 가능 (서로 다른 파일):
T001: Remove KEY_I handler from window_events.c + RENDER_SHOW_INFO from window.h
T002: Remove "Display:" section from keyguide_render.c
T003: Remove hud_render_background from hud_text.c, hud_text.h, hud.h

# 순차 실행 (같은 파일 의존):
T004: Remove cancel_requested (render_debounce.h/c, timer.c, window_loop.c, render.c)
T005: Remove PREVIEW-related code (render_debounce.h/c)
T006: Build verification
```

## Parallel Example: Phase 3 이후

```text
# Phase 2 완료 후 병렬 가능:
US1 (Phase 3): Debounce state machine redesign
US3 (Phase 4): pixel_timing gate (독립)
US4 (Phase 5): Keymap changes (독립)

# US4 완료 후:
US5 (Phase 6): Keyguide incremental redraw
```

## Parallel Example: Phase 5 (Keymap) 내부

```text
# T016 (header) 완료 후 병렬 가능 (서로 다른 파일):
T017: Update light_move + object_resize in window_objects.c
T018: Update object_rotate in window_rotate.c
T019: Update transform_keys + hud_keys in window_key_handlers.c
T020: Update object_selection in window_selection.c

# 순차:
T021: Update keyguide text in keyguide_render.c
T022: Build + norminette + manual test
```

---

## Implementation Strategy

### MVP First (US2 + US1)

1. Complete Phase 2: US2 — 데드 코드 제거 (안전한 삭제, 빌드 확인)
2. Complete Phase 3: US1 — 디바운스 상태 머신 재설계 (핵심 기능)
3. **STOP and VALIDATE**: 이산 탭 병합, key-hold LQ 프리뷰, COOLDOWN 동작 확인
4. 이 시점에서 가장 큰 체감 성능 개선 달성

### Incremental Delivery

1. US2 (데드 코드 제거) → 빌드 확인 → 커밋
2. US1 (디바운스 재설계) → 수동 테스트 → 커밋 (MVP!)
3. US3 (pixel_timing 게이트) → 빌드 확인 → 커밋
4. US4 (키맵 변경) → macOS 테스트 → 커밋
5. US5 (키가이드 최적화) → 성능 확인 → 커밋
6. Polish → norminette + 회귀 테스트 → 최종 커밋

---

## Notes

- [P] tasks = different files, no dependencies
- [Story] label maps task to specific user story for traceability
- 42 Norm v4.1: 25줄/함수, 5변수/함수, 5함수/파일, 4인자/함수 — 모든 변경에서 확인 필수
- 커밋은 각 Phase 완료 후 수행 (Phase 내 작은 단위 커밋도 가능)
- US4 키맵: Option A(구두점 키) + 키보드 그리드 Resize(Y,U/N,M) + Rotate(I,J/O,K/P,L) 확정
- KEY_I define은 유지 (RENDER_SHOW_INFO 핸들러만 제거, Rotate X- 로 재활용)
- KEY_BACKSLASH/KEY_SLASH는 추가하지 않음 (Z rotation이 P/L로 대체)
