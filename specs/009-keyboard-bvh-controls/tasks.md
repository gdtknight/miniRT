---

description: "Task list for keyboard controls finalization and BVH enforcement"
---

# Tasks: Keyboard Controls Finalization and BVH Enforcement

**Input**: Design documents from `/specs/009-keyboard-bvh-controls/`
**Prerequisites**: plan.md, spec.md, research.md, data-model.md, contracts/keyboard_layout.h, quickstart.md

**Tests**: Not required - Feature specification does not include TDD requirements.

**Organization**: Tasks are grouped by user story to enable independent implementation and testing of each story.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2, US3)
- Include exact file paths in descriptions

## Path Conventions

- **Single project**: `src/`, `includes/` at repository root
- Project structure: C ray tracer with MLX graphics library

---

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Project initialization and documentation preparation

- [X] T001 Create branch 009-keyboard-bvh-controls from main
- [X] T002 [P] Verify build system compiles without errors using make
- [ ] T003 [P] Verify norminette passes on all existing source files

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Core infrastructure that MUST be complete before ANY user story can be implemented

**⚠️ CRITICAL**: No user story work can begin until this phase is complete

- [X] T004 Add camera initial state fields to t_camera structure in includes/minirt.h
- [X] T005 Store initial camera position and direction in scene parser after camera setup
- [X] T006 [P] Add new key code definitions for macOS in includes/window.h (KEY_T, KEY_G, KEY_V, KEY_E, KEY_C, KEY_X, KEY_Z)
- [X] T007 [P] Add new key code definitions for Linux in includes/window.h (KEY_T, KEY_G, KEY_V, KEY_E, KEY_C, KEY_X, KEY_Z)
- [X] T008 Verify compilation succeeds with new key codes using make

**Checkpoint**: Foundation ready - user story implementation can now begin in parallel

---

## Phase 3: User Story 1 - Object Manipulation with Optimized Key Layout (Priority: P1) 🎯 MVP

**Goal**: Enable users to move selected objects along all three axes using ergonomic R/T/F/G/V/B keyboard layout

**Independent Test**: Select any object and press R/T/F/G/V/B keys to verify movement in all six directions (X-/X+, Y-/Y+, Z-/Z+)

### Implementation for User Story 1

- [X] T009 [US1] Update handle_object_move() function to use KEY_R for X- movement in src/window/window.c
- [X] T010 [US1] Update handle_object_move() function to use KEY_T for X+ movement in src/window/window.c
- [X] T011 [US1] Update handle_object_move() function to use KEY_F for Y- movement in src/window/window.c
- [X] T012 [US1] Update handle_object_move() function to use KEY_G for Y+ movement in src/window/window.c
- [X] T013 [US1] Update handle_object_move() function to use KEY_V for Z- movement in src/window/window.c
- [X] T014 [US1] Update handle_object_move() function to use KEY_B for Z+ movement in src/window/window.c
- [X] T015 [US1] Remove old Numpad key code handling from handle_object_move() in src/window/window.c
- [X] T016 [US1] Update handle_key() dispatcher to route R/T/F/G/V/B keys to handle_object_move() in src/window/window.c
- [X] T017 [US1] Remove old Numpad key routing from handle_key() dispatcher in src/window/window.c
- [ ] T018 [US1] Verify object movement works in all six directions using test scene

**Checkpoint**: At this point, User Story 1 should be fully functional and testable independently

---

## Phase 4: User Story 2 - Maximum Rendering Performance (Priority: P1)

**Goal**: Ensure BVH acceleration is permanently enabled for consistent maximum rendering performance without user intervention

**Independent Test**: Load scenes with 100+ objects and verify rendering maintains consistent frame rate without manual optimization

### Implementation for User Story 2

- [X] T019 [P] [US2] Remove conditional BVH initialization logic from src/main.c
- [X] T020 [P] [US2] Ensure scene_build_bvh() is always called after scene parsing in src/main.c
- [X] T021 [US2] Add error handling for BVH build failure in src/main.c
- [X] T022 [US2] Remove any BVH toggle functionality from user interface if present
- [ ] T023 [US2] Verify BVH is built for all scenes using performance metrics
- [ ] T024 [US2] Test rendering performance with 100+ object scene

**Checkpoint**: At this point, User Stories 1 AND 2 should both work independently

---

## Phase 5: User Story 3 - Camera Navigation (Priority: P2)

**Goal**: Enable users to navigate the 3D scene using intuitive keyboard controls for translation and rotation

**Independent Test**: Load any scene and use W/X/A/D/Q/Z keys for movement and E/C for rotation to verify all camera controls work

### Implementation for User Story 3

- [X] T025 [P] [US3] Add KEY_X handling for backward camera movement in handle_camera_move() in src/window/window.c
- [X] T026 [P] [US3] Add KEY_Q handling for upward world movement in handle_camera_move() in src/window/window.c
- [X] T027 [P] [US3] Add KEY_Z handling for downward world movement in handle_camera_move() in src/window/window.c
- [X] T028 [US3] Update handle_camera_pitch() to use KEY_E instead of KEY_R in src/window/window.c
- [X] T029 [US3] Update handle_camera_pitch() to use KEY_C instead of KEY_F in src/window/window.c
- [X] T030 [US3] Create handle_camera_reset() function for S key in src/window/window.c
- [X] T031 [US3] Add KEY_S handling to call handle_camera_reset() in handle_key() dispatcher in src/window/window.c
- [X] T032 [US3] Update handle_key() dispatcher to include X/Q/Z in camera movement routing in src/window/window.c
- [X] T033 [US3] Update handle_key() dispatcher to route E/C instead of R/F to handle_camera_pitch() in src/window/window.c
- [ ] T034 [US3] Verify all camera translation directions work correctly using test scene
- [ ] T035 [US3] Verify camera pitch up/down works with E/C keys using test scene
- [ ] T036 [US3] Verify camera reset functionality returns to initial position using test scene

**Checkpoint**: All user stories should now be independently functional

---

## Phase 6: Polish & Cross-Cutting Concerns

**Purpose**: Improvements that affect multiple user stories

- [X] T037 [P] Update object movement key labels in keyguide_render.c to show R/T/F/G/V/B
- [X] T038 [P] Update camera control key labels in keyguide_render.c to show W/X/A/D/Q/Z/E/C/S
- [X] T039 [P] Remove Numpad references from keyguide_render.c
- [X] T040 [P] Create or update docs/CONTROLS.md with complete keyboard reference
- [X] T041 [P] Update README.md controls section if present
- [ ] T042 [P] Create Korean translation in docs/specs/009-keyboard-bvh-controls/spec.md
- [ ] T043 Run norminette on all modified files and fix violations
- [ ] T044 Run make to verify clean compilation with no warnings
- [ ] T045 Run valgrind on test scenes to verify zero memory leaks
- [ ] T046 Test all keyboard controls on macOS platform using test scenes
- [ ] T047 Test all keyboard controls on Linux platform using test scenes
- [ ] T048 Run quickstart.md validation checklist for final verification

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies - can start immediately
- **Foundational (Phase 2)**: Depends on Setup completion - BLOCKS all user stories
- **User Stories (Phase 3-5)**: All depend on Foundational phase completion
  - User stories can then proceed in parallel (if staffed)
  - Or sequentially in priority order (P1 → P1 → P2)
- **Polish (Phase 6)**: Depends on all user stories being complete

### User Story Dependencies

- **User Story 1 (P1)**: Can start after Foundational (Phase 2) - No dependencies on other stories
- **User Story 2 (P1)**: Can start after Foundational (Phase 2) - No dependencies on other stories
- **User Story 3 (P2)**: Can start after Foundational (Phase 2) - No dependencies on other stories

### Within Each User Story

- User Story 1: Tasks T009-T017 can be done sequentially or in small batches, then T018 verification
- User Story 2: Tasks T019-T021 are parallelizable, then T022-T024 sequential
- User Story 3: Tasks T025-T027 parallelizable (camera movement), T028-T029 parallelizable (rotation), T030-T033 sequential (reset + dispatcher updates), then T034-T036 verification

### Parallel Opportunities

- All Setup tasks marked [P] can run in parallel
- All Foundational tasks marked [P] can run in parallel (within Phase 2)
- Once Foundational phase completes, all user stories can start in parallel (if team capacity allows)
- Within User Story 2, tasks T019 and T020 can run in parallel
- Within User Story 3, tasks T025-T027 can run in parallel, T028-T029 can run in parallel
- All Polish tasks marked [P] can run in parallel

---

## Parallel Example: User Story 1

```bash
# Launch all object movement key updates together:
Task: "Update handle_object_move() to use KEY_R for X- in src/window/window.c"
Task: "Update handle_object_move() to use KEY_T for X+ in src/window/window.c"
Task: "Update handle_object_move() to use KEY_F for Y- in src/window/window.c"
Task: "Update handle_object_move() to use KEY_G for Y+ in src/window/window.c"
Task: "Update handle_object_move() to use KEY_V for Z- in src/window/window.c"
Task: "Update handle_object_move() to use KEY_B for Z+ in src/window/window.c"
```

---

## Parallel Example: User Story 3

```bash
# Launch all camera movement key additions together:
Task: "Add KEY_X handling for backward camera movement in src/window/window.c"
Task: "Add KEY_Q handling for upward world movement in src/window/window.c"
Task: "Add KEY_Z handling for downward world movement in src/window/window.c"

# Launch camera rotation key updates together:
Task: "Update handle_camera_pitch() to use KEY_E instead of KEY_R"
Task: "Update handle_camera_pitch() to use KEY_C instead of KEY_F"
```

---

## Implementation Strategy

### MVP First (User Stories 1 & 2)

1. Complete Phase 1: Setup
2. Complete Phase 2: Foundational (CRITICAL - blocks all stories)
3. Complete Phase 3: User Story 1 (Object Manipulation)
4. Complete Phase 4: User Story 2 (BVH Enforcement)
5. **STOP and VALIDATE**: Test User Stories 1 & 2 independently
6. Deploy/demo if ready

### Incremental Delivery

1. Complete Setup + Foundational → Foundation ready
2. Add User Story 1 → Test independently → Deploy/Demo (Object controls working!)
3. Add User Story 2 → Test independently → Deploy/Demo (Performance optimized!)
4. Add User Story 3 → Test independently → Deploy/Demo (Full camera control!)
5. Add Polish → Final documentation and cross-platform testing
6. Each story adds value without breaking previous stories

### Parallel Team Strategy

With multiple developers:

1. Team completes Setup + Foundational together
2. Once Foundational is done:
   - Developer A: User Story 1 (Object manipulation)
   - Developer B: User Story 2 (BVH enforcement)
   - Developer C: User Story 3 (Camera navigation)
3. Stories complete and integrate independently

---

## Notes

- [P] tasks = different files, no dependencies
- [Story] label maps task to specific user story for traceability
- Each user story should be independently completable and testable
- Commit after each task or logical group
- Stop at any checkpoint to validate story independently
- Cross-platform testing on both macOS and Linux required before final merge
- Norminette compliance required for all code changes (42 School standards)
- Memory leak testing with valgrind required before merge
- Avoid: vague tasks, same file conflicts, cross-story dependencies that break independence
