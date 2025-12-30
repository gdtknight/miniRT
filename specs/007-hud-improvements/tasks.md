# Tasks: HUD Improvements and Key Guide

**Input**: Design documents from `/specs/007-hud-improvements/`
**Prerequisites**: plan.md, spec.md, research.md, data-model.md, contracts/

**Tests**: Not explicitly requested in specification - implementation only

**Organization**: Tasks are grouped by user story to enable independent implementation and testing of each story.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2, US3, US4)
- Include exact file paths in descriptions

## Path Conventions

- Single project: `src/`, `tests/` at repository root
- Paths shown below are absolute from repository root

---

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Define constants and infrastructure for resolution and transparency changes

- [X] T001 Define window resolution constants (WINDOW_WIDTH=1440, WINDOW_HEIGHT=900, ASPECT_RATIO) in includes/window.h or includes/minirt.h
- [X] T002 [P] Define key guide layout constants (dimensions, margins, colors) in includes/keyguide.h per contracts/keyguide.h
- [X] T003 [P] Define transparency constants (HUD_BG_ALPHA, KEYGUIDE_BG_ALPHA=0.65) in includes/hud_transparency.h per contracts/hud_transparency.h
- [X] T004 Update Makefile to include new source files: src/hud/hud_transparency.c, src/keyguide/*.c

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Core alpha blending utilities that ALL user stories depend on

**⚠️ CRITICAL**: No user story work can begin until this phase is complete

- [X] T005 Implement blend_colors() function in src/hud/hud_transparency.c
- [X] T006 [P] Implement get_red(), get_green(), get_blue() color channel extractors in src/hud/hud_transparency.c
- [X] T007 [P] Implement make_color() RGB combiner in src/hud/hud_transparency.c
- [X] T008 [P] Implement get_pixel() image buffer reader in src/hud/hud_transparency.c
- [X] T009 [P] Implement set_pixel() image buffer writer in src/hud/hud_transparency.c
- [X] T010 Verify alpha blending utilities compile and pass norminette (≤25 lines/function)

**Checkpoint**: Foundation ready - user story implementation can now begin in parallel

---

## Phase 3: User Story 3 - Work at Comfortable Display Resolution (Priority: P2) 🎯 MVP

**Goal**: Update window resolution from 800x600 to 1440x900 for comfortable viewing on modern displays

**Independent Test**: Launch application and verify window opens at 1440x900 resolution with correct aspect ratio

### Implementation for User Story 3

- [X] T011 [P] [US3] Update window initialization to use WINDOW_WIDTH and WINDOW_HEIGHT constants in src/window/window.c
- [X] T012 [P] [US3] Update image creation to use WINDOW_WIDTH and WINDOW_HEIGHT constants in src/window/window.c
- [X] T013 [P] [US3] Update camera aspect ratio calculation to use ASPECT_RATIO constant in src/render/camera.c
- [X] T014 [P] [US3] Update pixel bounds checking to use WINDOW_WIDTH and WINDOW_HEIGHT in src/render/render.c
- [X] T015 [P] [US3] Update all hardcoded 800x600 references in comments and documentation throughout src/ and includes/
- [ ] T016 [US3] Test application launch at 1440x900 resolution with scenes/simple.rt and verify no distortion

**Checkpoint**: Window resolution updated - application now runs at comfortable 1440x900 size

---

## Phase 4: User Story 1 - View Scene Through Transparent HUD (Priority: P1)

**Goal**: Add semi-transparent background to main HUD allowing rendered scene to be partially visible

**Independent Test**: Launch application with any scene, press 'H' to ensure HUD visible, verify scene is visible through HUD background with readable white text

### Implementation for User Story 1

- [X] T017 [US1] Implement hud_render_background() function with alpha blending loop in src/hud/hud_render.c
- [X] T018 [US1] Split hud_render_background() into helper functions if exceeding 25 lines (e.g., hud_blend_row()) per norminette
- [X] T019 [US1] Integrate hud_render_background() into existing HUD render pipeline in src/hud/hud_render.c
- [ ] T020 [US1] Verify HUD text remains readable (minimum 4.5:1 contrast ratio) with transparency enabled
- [ ] T021 [US1] Test HUD transparency with bright scenes (verify text readability) using scenes/bright_test.rt
- [ ] T022 [US1] Test HUD transparency with dark scenes (verify transparency visible) using scenes/dark_test.rt

**Checkpoint**: Main HUD now has semi-transparent background allowing scene visibility

---

## Phase 5: User Story 2 - See Real-Time Object Selection Updates (Priority: P2)

**Goal**: Trigger immediate HUD refresh when object selection changes via bracket keys

**Independent Test**: Open scene with multiple objects, use '[' and ']' keys to cycle through objects, verify HUD immediately highlights newly selected object in green

### Implementation for User Story 2

- [X] T023 [US2] Add hud_mark_dirty() call in handle_object_selection() function after selection update in src/window/window.c
- [X] T024 [US2] Verify hud_select_next() and hud_select_prev() already call hud_mark_dirty() in src/hud/hud_navigation.c (no changes if present)
- [ ] T025 [US2] Test bracket key selection with scenes/multi_objects.rt and verify immediate HUD update (green highlight)
- [ ] T026 [US2] Test rapid bracket key presses and verify HUD updates smoothly without lag (<16ms per update)

**Checkpoint**: HUD now updates immediately when object selection changes

---

## Phase 6: User Story 4 - Learn Controls Without Documentation (Priority: P3)

**Goal**: Display compact key guide in upper-right corner showing essential keyboard controls

**Independent Test**: Launch application and verify key guide appears in upper-right corner with semi-transparent background, toggles with 'H' key

### Implementation for User Story 4

- [X] T027 [P] [US4] Add t_keyguide_state field to t_render structure in includes/render_state.h or appropriate header
- [X] T028 [P] [US4] Implement keyguide_init() function to allocate background image and set position in src/keyguide/keyguide_init.c
- [X] T029 [P] [US4] Implement keyguide_cleanup() function to destroy background image in src/keyguide/keyguide_cleanup.c
- [X] T030 [US4] Implement keyguide_render_background() with alpha blending in src/keyguide/keyguide_render.c
- [X] T031 [US4] Implement keyguide_render_content() to display control text in functional groups in src/keyguide/keyguide_render.c
- [X] T032 [US4] Implement keyguide_render_section() helper for rendering control sections in src/keyguide/keyguide_render.c
- [X] T033 [US4] Implement main keyguide_render() orchestrator function in src/keyguide/keyguide_render.c
- [X] T034 [US4] Integrate keyguide_init() into window initialization function in src/window/window.c
- [X] T035 [US4] Integrate keyguide_cleanup() into close_window() function in src/window/window.c
- [X] T036 [US4] Add keyguide_render() call after hud_render() in render loop in src/render/render.c or src/window/window.c
- [X] T037 [US4] Modify hud_toggle() to sync keyguide visibility with main HUD in src/hud/hud_toggle.c
- [ ] T038 [US4] Verify key guide appears in upper-right corner at correct position (WINDOW_WIDTH - KEYGUIDE_WIDTH - MARGIN_X)
- [ ] T039 [US4] Verify key guide displays all essential controls per FR-009 (ESC, H, WASD, RF, TAB, [], Numpad, BQI)
- [ ] T040 [US4] Test 'H' key toggle and verify both HUDs hide/show together
- [ ] T041 [US4] Verify key guide occupies ≤300x400 pixels and text is readable with transparency

**Checkpoint**: Key guide fully functional with semi-transparent background in upper-right corner

---

## Phase 7: Polish & Cross-Cutting Concerns

**Purpose**: Final validation, documentation, and quality assurance

- [ ] T042 [P] Run norminette on all modified and new files (zero errors/warnings required)
- [X] T043 [P] Build project with make and verify no compilation warnings
- [ ] T044 Run valgrind memory leak check with test scene and verify zero leaks
- [X] T045 [P] Update comments referencing 800x600 resolution to 1440x900 in src/ and includes/
- [X] T046 [P] Add CHANGES.md entry documenting resolution change, HUD transparency, key guide, and real-time selection updates
- [ ] T047 Test on macOS (primary platform) with MacBook M1 13-inch display
- [ ] T048 Test on Linux (X11) if applicable to verify cross-platform compatibility
- [ ] T049 Measure HUD render performance and verify ≤2ms target met (both HUDs combined)
- [ ] T050 Measure frame rate during camera movement and verify 60fps maintained with HUDs visible
- [ ] T051 Run quickstart.md validation checklist from specs/007-hud-improvements/quickstart.md

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies - can start immediately
- **Foundational (Phase 2)**: Depends on Setup completion - BLOCKS all user stories
- **User Story 3 (Phase 3)**: Depends on Foundational (Phase 2) - MVP candidate, independent of other stories
- **User Story 1 (Phase 4)**: Depends on Foundational (Phase 2) - Can run parallel with US3/US2 after Phase 2
- **User Story 2 (Phase 5)**: Depends on Foundational (Phase 2) - Can run parallel with US3/US1 after Phase 2
- **User Story 4 (Phase 6)**: Depends on Foundational (Phase 2) and US1 (transparency utilities) - Can run parallel with US2/US3 after Phase 2
- **Polish (Phase 7)**: Depends on all desired user stories being complete

### User Story Dependencies

- **User Story 3 (P2) - Resolution**: Independent, only depends on Phase 1 constants and Phase 2 completion
- **User Story 1 (P1) - HUD Transparency**: Depends on Phase 2 alpha blending utilities, independent of other stories
- **User Story 2 (P2) - Selection Updates**: Independent, minimal changes to existing handlers
- **User Story 4 (P3) - Key Guide**: Depends on Phase 2 alpha blending utilities, independent of other stories

### Within Each User Story

- **US3**: All tasks can run in parallel (different files: window.c, camera.c, render.c)
- **US1**: T017-T019 sequential (same file), T020-T022 parallel testing
- **US2**: T023-T024 sequential (verification), T025-T026 parallel testing
- **US4**: T027-T029 parallel (different files), T030-T033 sequential (same file), T034-T037 parallel (different files), T038-T041 parallel testing

### Parallel Opportunities

- **Phase 1**: All tasks (T001-T004) can run in parallel - different header files and Makefile
- **Phase 2**: T006-T007, T008-T009 can run in parallel - independent utility functions
- **After Phase 2**: All user stories (US1, US2, US3, US4) can start in parallel if team capacity allows
- **Phase 7**: Most polish tasks (T042-T048) can run in parallel - different validation activities

---

## Parallel Example: After Foundational Phase

```bash
# All user stories can begin in parallel after Phase 2 completes:

# Developer A: User Story 3 (Resolution)
Task: "Update window initialization in src/window/window.c"
Task: "Update camera aspect ratio in src/render/camera.c"
Task: "Update bounds checking in src/render/render.c"

# Developer B: User Story 1 (HUD Transparency)
Task: "Implement hud_render_background() in src/hud/hud_render.c"
Task: "Test transparency with bright/dark scenes"

# Developer C: User Story 2 (Selection Updates)
Task: "Add hud_mark_dirty() in src/window/window.c"
Task: "Test bracket key selection updates"

# Developer D: User Story 4 (Key Guide)
Task: "Implement keyguide_init() in src/keyguide/keyguide_init.c"
Task: "Implement keyguide_render() in src/keyguide/keyguide_render.c"
Task: "Integrate into window lifecycle"
```

---

## Implementation Strategy

### MVP First (User Story 3 + User Story 1)

1. Complete Phase 1: Setup (constants)
2. Complete Phase 2: Foundational (alpha blending utilities - CRITICAL)
3. Complete Phase 3: User Story 3 (resolution update)
4. Complete Phase 4: User Story 1 (HUD transparency)
5. **STOP and VALIDATE**: Test US3 + US1 independently
6. Deploy/demo if ready (comfortable resolution + transparent HUD)

**Rationale**: US3 (P2) and US1 (P1) together provide the most visible user value - comfortable window size with improved HUD visibility. This forms a solid MVP.

### Incremental Delivery

1. Complete Setup + Foundational → Foundation ready
2. Add User Story 3 (Resolution) → Test independently → Deploy/Demo
3. Add User Story 1 (HUD Transparency) → Test independently → Deploy/Demo  
4. Add User Story 2 (Selection Updates) → Test independently → Deploy/Demo
5. Add User Story 4 (Key Guide) → Test independently → Deploy/Demo
6. Each story adds value without breaking previous stories

### Parallel Team Strategy

With multiple developers:

1. Team completes Setup + Foundational together (Phase 1 + Phase 2)
2. Once Foundational is done:
   - Developer A: User Story 3 (Resolution)
   - Developer B: User Story 1 (HUD Transparency)
   - Developer C: User Story 2 (Selection Updates)
   - Developer D: User Story 4 (Key Guide)
3. Stories complete and integrate independently
4. Final integration testing in Phase 7

---

## Notes

- [P] tasks = different files or independent functionality, no dependencies
- [Story] label (US1-US4) maps task to specific user story for traceability
- Each user story should be independently completable and testable
- Commit after each task or logical group
- Stop at any checkpoint to validate story independently
- All code must pass norminette (≤25 lines/function, no forbidden functions)
- Memory leak check required (valgrind) - all mlx_new_image must have paired mlx_destroy_image
- Avoid: vague tasks, same file conflicts, cross-story dependencies that break independence

---

## Summary

**Total Tasks**: 51 tasks across 7 phases
- **Phase 1 (Setup)**: 4 tasks - Infrastructure constants and Makefile
- **Phase 2 (Foundational)**: 6 tasks - Alpha blending utilities (BLOCKING)
- **Phase 3 (US3 - Resolution)**: 6 tasks - Window size and aspect ratio updates
- **Phase 4 (US1 - Transparency)**: 6 tasks - Semi-transparent HUD background
- **Phase 5 (US2 - Selection)**: 4 tasks - Real-time HUD updates on selection change
- **Phase 6 (US4 - Key Guide)**: 15 tasks - Key guide overlay implementation
- **Phase 7 (Polish)**: 10 tasks - Quality assurance and validation

**Task Count per User Story**:
- **US1 (HUD Transparency)**: 6 tasks
- **US2 (Selection Updates)**: 4 tasks
- **US3 (Resolution)**: 6 tasks
- **US4 (Key Guide)**: 15 tasks
- **Infrastructure (Setup + Foundational)**: 10 tasks
- **Polish & QA**: 10 tasks

**Parallel Opportunities Identified**: 
- Phase 1: 3 parallel tasks (T002, T003, T004)
- Phase 2: 6 parallel tasks (T006-T009)
- Phase 3: 4 parallel tasks (T011-T014, T015)
- Phase 4: 2 parallel tasks (T021-T022 testing)
- Phase 6: Multiple parallel opportunities (T027-T029, T034-T037, T038-T041)
- Phase 7: 6 parallel tasks (T042-T043, T045-T046, T047-T048)

**Independent Test Criteria**:
- **US1**: HUD visible with scene showing through semi-transparent background, text readable
- **US2**: Bracket keys immediately update HUD highlighting in green
- **US3**: Window opens at 1440x900 without distortion
- **US4**: Key guide appears in upper-right, toggles with 'H', shows all essential controls

**Suggested MVP Scope**: Phase 1 + Phase 2 + Phase 3 (Resolution) + Phase 4 (HUD Transparency)

**Estimated Timeline**: 6-9 hours for experienced developers, 12-15 hours for developers new to codebase

**42 School Compliance**: All tasks use only allowed functions (mlx_*, malloc, free, math.h), no pthread/fork, norminette compliance enforced
