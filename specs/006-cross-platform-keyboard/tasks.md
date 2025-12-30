---

description: "Task list for cross-platform keyboard support implementation"
---

# Tasks: Cross-Platform Keyboard Support

**Input**: Design documents from `/specs/006-cross-platform-keyboard/`
**Prerequisites**: plan.md, spec.md, research.md, data-model.md, contracts/keycodes.h, quickstart.md

**Tests**: No test tasks included - feature focuses on refactoring existing keycode definitions

**Organization**: Tasks are organized by user story to enable validation of each functional group independently

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2, US3, US4, US5)
- Include exact file paths in descriptions

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Verify build environment and baseline functionality

- [X] T001 Verify project compiles successfully on current platform (make fclean && make)
- [X] T002 Backup current window.c implementation to src/window/window.c.backup
- [X] T003 Run norminette on src/window/window.c to verify baseline compliance
- [X] T004 Test all keyboard controls on Linux/Ubuntu to document baseline behavior

**Checkpoint**: Baseline established - ready to implement cross-platform changes

---

## Phase 2: Foundational (Platform-Specific Keycode Definitions)

**Purpose**: Core keycode definition changes that ALL user stories depend on

**⚠️ CRITICAL**: This phase MUST be complete before ANY user story validation can begin

- [X] T005 Replace keycode definitions (lines 37-71) in src/window/window.c with platform-specific conditional compilation blocks
- [X] T006 Add macOS keycode definitions using __APPLE__ preprocessor directive in src/window/window.c
- [X] T007 Wrap existing Linux keycode definitions with __linux__ preprocessor directive in src/window/window.c
- [X] T008 Add platform detection error directive for unsupported platforms in src/window/window.c
- [X] T009 Verify norminette compliance on modified src/window/window.c
- [X] T010 Compile on macOS and verify no errors or warnings (make fclean && make)
- [ ] T011 Compile on Linux and verify no errors or warnings (make fclean && make)

**Checkpoint**: Platform-specific keycodes defined - user story validation can now begin

---

## Phase 3: User Story 1 - Basic Camera Navigation (Priority: P1) 🎯 MVP

**Goal**: Enable camera navigation (WASD movement, RF pitch) to work identically on both macOS and Ubuntu

**Independent Test**: Launch miniRT on macOS, press W/A/S/D/R/F keys, verify camera moves correctly. Repeat on Ubuntu and verify identical behavior.

### Implementation for User Story 1

- [X] T012 [US1] Verify KEY_W, KEY_A, KEY_S, KEY_D macros defined correctly for both platforms in src/window/window.c
- [X] T013 [US1] Verify KEY_R, KEY_F macros defined correctly for both platforms in src/window/window.c
- [ ] T014 [US1] Test forward camera movement (W key) on macOS with test scene
- [ ] T015 [US1] Test left camera movement (A key) on macOS with test scene
- [ ] T016 [US1] Test backward camera movement (S key) on macOS with test scene
- [ ] T017 [US1] Test right camera movement (D key) on macOS with test scene
- [ ] T018 [US1] Test pitch up rotation (R key) on macOS with test scene
- [ ] T019 [US1] Test pitch down rotation (F key) on macOS with test scene
- [ ] T020 [US1] Verify all camera controls on Linux/Ubuntu remain unchanged from baseline

**Checkpoint**: Camera navigation works identically on both platforms - core interaction mechanism functional

---

## Phase 4: User Story 4 - Application Control Keys (Priority: P1)

**Goal**: Enable application controls (ESC exit, H HUD toggle, TAB navigation, Shift modifier, arrow keys) to work on both platforms

**Independent Test**: Launch miniRT on macOS, press ESC to exit cleanly, H to toggle HUD, TAB to navigate, Shift+TAB for reverse, arrows to scroll. Verify on Ubuntu.

### Implementation for User Story 4

- [X] T021 [US4] Verify KEY_ESC, KEY_H, KEY_TAB macros defined correctly for both platforms in src/window/window.c
- [X] T022 [US4] Verify KEY_SHIFT_L, KEY_SHIFT_R, KEY_UP, KEY_DOWN macros defined correctly for both platforms in src/window/window.c
- [ ] T023 [US4] Test ESC key exits application cleanly on macOS
- [ ] T024 [US4] Test H key toggles HUD visibility on macOS
- [ ] T025 [US4] Test TAB key navigates to next HUD section on macOS
- [ ] T026 [US4] Test Shift+TAB navigates to previous HUD section on macOS
- [ ] T027 [US4] Test UP arrow scrolls HUD content up on macOS
- [ ] T028 [US4] Test DOWN arrow scrolls HUD content down on macOS
- [ ] T029 [US4] Verify all application controls on Linux/Ubuntu remain unchanged from baseline

**Checkpoint**: Essential application controls work on both platforms - user can navigate UI and exit

---

## Phase 5: User Story 2 - Object Manipulation Controls (Priority: P2)

**Goal**: Enable object selection and movement (bracket keys, numpad keys) to work on both platforms

**Independent Test**: Load scene with multiple objects on macOS, use bracket keys to select, numpad keys to move. Verify on Ubuntu.

### Implementation for User Story 2

- [X] T030 [US2] Verify KEY_BRACKET_LEFT, KEY_BRACKET_RIGHT macros defined correctly for both platforms in src/window/window.c
- [X] T031 [US2] Verify KEY_KP_1, KEY_KP_2, KEY_KP_3, KEY_KP_4, KEY_KP_6, KEY_KP_8 macros defined correctly for both platforms in src/window/window.c
- [ ] T032 [US2] Test bracket right (]) selects next object on macOS with multi-object scene
- [ ] T033 [US2] Test bracket left ([) selects previous object on macOS with multi-object scene
- [ ] T034 [US2] Test numpad 4 moves selected object in -X direction on macOS
- [ ] T035 [US2] Test numpad 6 moves selected object in +X direction on macOS
- [ ] T036 [US2] Test numpad 2 moves selected object in -Y direction on macOS
- [ ] T037 [US2] Test numpad 8 moves selected object in +Y direction on macOS
- [ ] T038 [US2] Test numpad 1 moves selected object in -Z direction on macOS
- [ ] T039 [US2] Test numpad 3 moves selected object in +Z direction on macOS
- [ ] T040 [US2] Verify all object manipulation controls on Linux/Ubuntu remain unchanged from baseline

**Checkpoint**: Object manipulation works on both platforms - users can edit scenes effectively

---

## Phase 6: User Story 3 - Lighting Controls (Priority: P3)

**Goal**: Enable lighting position adjustment (Insert/Delete/Home/End/PageUp/PageDown) to work on both platforms

**Independent Test**: Load any scene on macOS, use lighting control keys to move light source in all axes. Verify on Ubuntu.

### Implementation for User Story 3

- [X] T041 [US3] Verify KEY_INSERT, KEY_DELETE, KEY_HOME, KEY_END, KEY_PGUP, KEY_PGDN macros defined correctly for both platforms in src/window/window.c
- [ ] T042 [US3] Test Insert key moves light in +X direction on macOS
- [ ] T043 [US3] Test Delete key moves light in -X direction on macOS
- [ ] T044 [US3] Test Home key moves light in +Y direction on macOS
- [ ] T045 [US3] Test End key moves light in -Y direction on macOS
- [ ] T046 [US3] Test Page Up key moves light in +Z direction on macOS
- [ ] T047 [US3] Test Page Down key moves light in -Z direction on macOS
- [ ] T048 [US3] Verify all lighting controls on Linux/Ubuntu remain unchanged from baseline

**Checkpoint**: Lighting adjustment works on both platforms - users can refine scene quality

---

## Phase 7: User Story 5 - Rendering Option Controls (Priority: P3)

**Goal**: Enable rendering option toggles (BVH, adaptive sampling, info display) to work on both platforms

**Independent Test**: Launch miniRT on macOS, press B/Q/I keys to toggle features. Verify on Ubuntu.

### Implementation for User Story 5

- [X] T049 [US5] Verify KEY_B, KEY_Q, KEY_I macros defined correctly for both platforms in src/window/window.c
- [ ] T050 [US5] Test B key toggles BVH acceleration on macOS
- [ ] T051 [US5] Test Q key toggles adaptive sampling on macOS
- [ ] T052 [US5] Test I key toggles info display on macOS
- [ ] T053 [US5] Verify all rendering option controls on Linux/Ubuntu remain unchanged from baseline

**Checkpoint**: All rendering options accessible on both platforms - power users can optimize performance

---

## Phase 8: Polish & Cross-Cutting Concerns

**Purpose**: Comprehensive validation and edge case testing

- [ ] T054 Run complete validation checklist from quickstart.md on macOS
- [ ] T055 Run complete validation checklist from quickstart.md on Linux/Ubuntu
- [ ] T056 Test rapid key presses (W key multiple times quickly) on both platforms
- [ ] T057 Test key combinations (Shift+TAB) on both platforms
- [ ] T058 Test simultaneous keys (W+D diagonal movement) on both platforms
- [ ] T059 Test undefined keys (Z, X, C) produce no crashes on both platforms
- [ ] T060 Verify cross-platform consistency test from quickstart.md (identical key sequences produce identical results)
- [ ] T061 Verify zero memory leaks with valgrind on Linux
- [ ] T062 Document any platform-specific quirks discovered during testing
- [ ] T063 Verify all success criteria from spec.md are met
- [ ] T064 Remove backup file src/window/window.c.backup
- [ ] T065 Final norminette check on src/window/window.c

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies - can start immediately
- **Foundational (Phase 2)**: Depends on Setup completion - BLOCKS all user stories
- **User Stories (Phase 3-7)**: All depend on Foundational phase completion
  - User Story 1 (P1 - Camera): Can start after Foundational - MVP critical
  - User Story 4 (P1 - Application): Can start after Foundational - MVP critical
  - User Story 2 (P2 - Objects): Can start after Foundational
  - User Story 3 (P3 - Lighting): Can start after Foundational
  - User Story 5 (P3 - Rendering): Can start after Foundational
- **Polish (Phase 8)**: Depends on all user stories being complete

### User Story Dependencies

- **User Story 1 (P1 - Camera)**: No dependencies on other stories - independently testable
- **User Story 4 (P1 - Application)**: No dependencies on other stories - independently testable
- **User Story 2 (P2 - Objects)**: No dependencies on other stories - independently testable
- **User Story 3 (P3 - Lighting)**: No dependencies on other stories - independently testable
- **User Story 5 (P3 - Rendering)**: No dependencies on other stories - independently testable

### Within Each User Story

- Verify keycode macros are defined correctly (first tasks)
- Test each individual key on macOS (middle tasks)
- Verify Linux behavior unchanged (last task in each story)

### Parallel Opportunities

**Phase 1 - Setup**: Tasks T001-T004 can run sequentially (each depends on previous)

**Phase 2 - Foundational**: 
- T005-T008 (code changes) must be sequential
- T009 (norminette) depends on T005-T008
- T010 (macOS build) can run parallel with T011 (Linux build) if both platforms available

**Phase 3-7 - User Stories**: Once Foundational (Phase 2) completes:
- All user stories (Phase 3, 4, 5, 6, 7) can run in parallel if team capacity allows
- Within each user story: verification tasks (T012-T013, T021-T022, etc.) can run in parallel
- Testing tasks within each story must be sequential (each requires launching and testing app)

**Phase 8 - Polish**: All tasks must run sequentially as they depend on complete implementation

---

## Parallel Example: After Foundational Phase

```bash
# All user stories can start validation simultaneously:

# Developer 1: User Story 1 (Camera Navigation)
Task: "Verify KEY_W, KEY_A, KEY_S, KEY_D macros defined correctly (T012)"
Task: "Verify KEY_R, KEY_F macros defined correctly (T013)"
# Then test each key: T014-T020

# Developer 2: User Story 4 (Application Controls)
Task: "Verify KEY_ESC, KEY_H, KEY_TAB macros defined correctly (T021)"
Task: "Verify KEY_SHIFT_L, KEY_SHIFT_R, KEY_UP, KEY_DOWN macros defined correctly (T022)"
# Then test each key: T023-T029

# Developer 3: User Story 2 (Object Manipulation)
Task: "Verify KEY_BRACKET_LEFT, KEY_BRACKET_RIGHT macros defined correctly (T030)"
Task: "Verify KEY_KP_* macros defined correctly (T031)"
# Then test each key: T032-T040
```

---

## Implementation Strategy

### MVP First (User Stories 1 & 4 Only - P1)

1. Complete Phase 1: Setup (T001-T004)
2. Complete Phase 2: Foundational (T005-T011) - **CRITICAL**
3. Complete Phase 3: User Story 1 - Camera Navigation (T012-T020)
4. Complete Phase 4: User Story 4 - Application Controls (T021-T029)
5. **STOP and VALIDATE**: Test camera and application controls independently on both platforms
6. If successful, this provides usable cross-platform navigation (MVP!)

### Incremental Delivery

1. **Foundation** (Phase 1+2): Platform-specific keycodes implemented
2. **MVP** (Phase 3+4): Camera + Application controls working → Deploy/Demo (essential functionality)
3. **Enhanced** (Phase 5): Object manipulation working → Deploy/Demo (scene editing enabled)
4. **Complete** (Phase 6+7): Lighting + Rendering options working → Deploy/Demo (full feature set)
5. **Polish** (Phase 8): All edge cases tested → Final release

### Single Developer Strategy

Work sequentially in priority order:
1. Setup → Foundational (mandatory)
2. User Story 1 (P1 - Camera) → Test independently
3. User Story 4 (P1 - Application) → Test independently
4. User Story 2 (P2 - Objects) → Test independently
5. User Story 3 (P3 - Lighting) → Test independently
6. User Story 5 (P3 - Rendering) → Test independently
7. Polish → Final validation

---

## Notes

- **No [P] markers on tasks**: All tasks are sequential within their phase due to testing requirements on same file
- **[Story] labels**: Map tasks to user stories from spec.md for traceability
- **Each user story is independently testable**: Can validate camera controls without object manipulation working
- **Platform testing pattern**: Test on macOS first (new functionality), then verify Linux unchanged (regression check)
- **Success criteria**: All 30+ keyboard controls respond identically on both Ubuntu and macOS
- **Rollback**: If issues arise, restore from src/window/window.c.backup
- **Commit strategy**: Commit after Phase 2 completion, then after each user story validation
- **Key constraint**: Changes limited to keycode definitions only (lines 37-71 of window.c) - handler functions unchanged
