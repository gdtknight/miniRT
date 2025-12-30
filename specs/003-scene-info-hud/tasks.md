# Tasks: Scene Information HUD

**Feature Branch**: `001-scene-info-hud`  
**Input**: Design documents from `/home/yoshin/work/miniRT/specs/001-scene-info-hud/`  
**Prerequisites**: plan.md ✅, spec.md ✅, data-model.md ✅, contracts/hud_api.h ✅, quickstart.md ✅

**Project Type**: Single C project (miniRT)  
**Language**: C (C99), 42 Norm compliance required  
**Library**: MiniLibX  
**Tests**: Manual testing with RT scene files (no automated test framework)

## Format: `- [ ] [ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2, US3)
- All tasks include exact file paths

---

## Phase 1: Setup (Project Integration)

**Purpose**: Create HUD module structure and integrate with existing build system

- [ ] T001 Create HUD module directory structure: `src/hud/` and update `includes/hud.h`
- [ ] T002 Copy API contract header from `specs/001-scene-info-hud/contracts/hud_api.h` to `includes/hud.h`
- [ ] T003 Update `Makefile` to include HUD source files in compilation
- [ ] T004 Update `includes/window.h` to add `t_hud_state` and `shift_pressed` fields to `t_render` structure

**Checkpoint**: Build system ready to compile HUD module

---

## Phase 2: Foundational (HUD Core Infrastructure)

**Purpose**: Core HUD lifecycle and data structures that ALL user stories depend on

**⚠️ CRITICAL**: No user story work can begin until this phase is complete

- [ ] T005 [P] Implement HUD initialization in `src/hud/hud_init.c`: `hud_init()` function with background image creation
- [ ] T006 [P] Implement HUD cleanup in `src/hud/hud_init.c`: `hud_cleanup()` function with image destruction
- [ ] T007 [P] Implement background creation helper in `src/hud/hud_init.c`: `hud_create_background()` with alpha-blended pixels
- [ ] T008 [P] Implement page calculation helper in `src/hud/hud_init.c`: `hud_calculate_total_pages()` function
- [ ] T009 [P] Implement float formatting utility in `src/hud/hud_format.c`: `hud_format_float()` function (2 decimal places)
- [ ] T010 [P] Implement vec3 formatting utility in `src/hud/hud_format.c`: `hud_format_vec3()` function
- [ ] T011 [P] Implement color formatting utility in `src/hud/hud_format.c`: `hud_format_color()` function
- [ ] T012 Integrate HUD initialization into `src/window/window.c`: Call `hud_init()` in `init_window()`
- [ ] T013 Integrate HUD cleanup into `src/window/window.c`: Call `hud_cleanup()` in `close_window()`
- [ ] T014 Initialize shift_pressed flag in `src/window/window.c`: Set to 0 in `init_window()`

**Checkpoint**: HUD lifecycle complete, formatting utilities ready, ready for rendering implementation

---

## Phase 3: User Story 1 - View Scene Configuration (Priority: P1) 🎯 MVP

**Goal**: Display camera, ambient light, and light source information on screen to verify RT file parsing

**Independent Test**: Load any RT file (e.g., `scenes/basic_sphere.rt`), verify HUD shows camera position/FOV, ambient light ratio/color, and light source position/brightness/color in top-left corner with readable contrast

### Implementation for User Story 1

- [ ] T015 [P] [US1] Implement background rendering in `src/hud/hud_render.c`: `hud_render_background()` function using `mlx_put_image_to_window()`
- [ ] T016 [P] [US1] Implement camera info rendering in `src/hud/hud_render.c`: `hud_render_camera()` function with position, direction, FOV
- [ ] T017 [P] [US1] Implement ambient light rendering in `src/hud/hud_render.c`: `hud_render_ambient()` function with ratio and color
- [ ] T018 [P] [US1] Implement light source rendering in `src/hud/hud_render.c`: `hud_render_light()` function with position, brightness, color
- [ ] T019 [US1] Implement main content rendering in `src/hud/hud_render.c`: `hud_render_content()` function that calls camera/ambient/light renderers
- [ ] T020 [US1] Implement main HUD render in `src/hud/hud_render.c`: `hud_render()` function with visibility check and dirty flag handling
- [ ] T021 [US1] Integrate HUD rendering into `src/window/window.c`: Call `hud_render()` after scene rendering in render loop
- [ ] T022 [US1] Add HUD logging in `src/hud/hud_init.c`: Log initialization at INFO level (OBS-001)
- [ ] T023 [US1] Add render time logging in `src/hud/hud_render.c`: Log render metrics at DEBUG level (OBS-003)

**Test User Story 1**:
- [ ] T024 [US1] Manual test with `scenes/basic_sphere.rt`: Verify camera, ambient, light display correctly
- [ ] T025 [US1] Manual test with `scenes/multi_objects.rt`: Verify HUD appears with readable contrast
- [ ] T026 [US1] Run valgrind: Verify no memory leaks in HUD init/cleanup
- [ ] T027 [US1] Run norminette on all HUD files: Verify 42 Norm compliance

**Checkpoint**: Basic HUD visible with scene configuration, User Story 1 complete and testable

---

## Phase 4: User Story 2 - View Object Summary (Priority: P2)

**Goal**: Display object counts and properties (spheres, planes, cylinders) with pagination for large scenes

**Independent Test**: Load RT file with 20+ objects (e.g., create `tests/manual/hud_test_scenes/large_scene.rt`), verify HUD shows correct counts, displays 8 objects per page, shows pagination controls

### Implementation for User Story 2

- [ ] T028 [P] [US2] Implement sphere rendering helper in `src/hud/hud_render.c`: Function to render sphere properties (position, radius, color)
- [ ] T029 [P] [US2] Implement plane rendering helper in `src/hud/hud_render.c`: Function to render plane properties (position, normal, color)
- [ ] T030 [P] [US2] Implement cylinder rendering helper in `src/hud/hud_render.c`: Function to render cylinder properties (position, direction, diameter, height, color)
- [ ] T031 [US2] Implement object list rendering in `src/hud/hud_render.c`: `hud_render_objects()` function with pagination logic
- [ ] T032 [US2] Update `hud_render_content()` in `src/hud/hud_render.c`: Add call to `hud_render_objects()` after light rendering
- [ ] T033 [US2] Implement page navigation in `src/hud/hud_navigation.c`: `hud_page_down()` function with wraparound
- [ ] T034 [US2] Implement page navigation in `src/hud/hud_navigation.c`: `hud_page_up()` function with wraparound
- [ ] T035 [US2] Integrate page keys into `src/window/window.c`: Handle Up/Down arrow keys in `handle_key()` to call page navigation functions
- [ ] T036 [US2] Add page navigation logging in `src/hud/hud_navigation.c`: Log at DEBUG level when page changes

**Test User Story 2**:
- [ ] T037 [US2] Create test scene: `tests/manual/hud_test_scenes/large_scene.rt` with 20 spheres, 5 planes, 3 cylinders
- [ ] T038 [US2] Manual test pagination: Verify objects 1-8 on page 1, objects 9-16 on page 2, etc.
- [ ] T039 [US2] Manual test wraparound: Verify pressing Down on last page wraps to page 1
- [ ] T040 [US2] Run norminette on `src/hud/hud_navigation.c`: Verify 42 Norm compliance

**Checkpoint**: Object list with pagination complete, User Stories 1 AND 2 both functional

---

## Phase 5: User Story 3 - Real-time Updates (Priority: P2)

**Goal**: Update HUD display immediately when camera or object positions change during interaction

**Independent Test**: Load any scene, move camera using existing controls (if available), verify HUD camera position updates in real-time

### Implementation for User Story 3

- [ ] T041 [US3] Implement dirty flag helper in `src/hud/hud_render.c`: `hud_mark_dirty()` function to set `render->hud.dirty = 1`
- [ ] T042 [US3] Add camera update integration in `src/window/window.c`: Call `hud_mark_dirty()` when camera moves (if camera controls exist)
- [ ] T043 [US3] Add object update integration in `src/window/window.c`: Call `hud_mark_dirty()` when objects move (if object controls exist)
- [ ] T044 [US3] Update render loop in `src/window/window.c`: Check `hud.dirty` flag to conditionally render HUD
- [ ] T045 [US3] Optimize HUD rendering in `src/hud/hud_render.c`: Clear dirty flag after render, skip if clean and no visibility change

**Test User Story 3**:
- [ ] T046 [US3] Manual test camera movement: Move camera, verify position in HUD updates within 16ms (one frame)
- [ ] T047 [US3] Manual test with static scene: Verify HUD doesn't re-render every frame when nothing changes (dirty flag optimization)
- [ ] T048 [US3] Performance test: Measure HUD render time with `gettimeofday()`, verify ≤2ms overhead

**Checkpoint**: Real-time updates working, User Stories 1, 2, AND 3 all functional

---

## Phase 6: User Story 4 - Toggle HUD Visibility (Priority: P3)

**Goal**: Allow user to hide/show HUD with keyboard shortcut for unobstructed scene viewing

**Independent Test**: Load any scene, press 'H' key, verify HUD disappears, press 'H' again, verify HUD reappears with same content

### Implementation for User Story 4

- [ ] T049 [US4] Implement toggle function in `src/hud/hud_toggle.c`: `hud_toggle()` function to flip `visible` flag
- [ ] T050 [US4] Add toggle logging in `src/hud/hud_toggle.c`: Log toggle events at INFO level with new state (OBS-002)
- [ ] T051 [US4] Integrate toggle key into `src/window/window.c`: Handle 'H' key press in `handle_key()` to call `hud_toggle()`
- [ ] T052 [US4] Update render condition in `src/hud/hud_render.c`: Return early if `hud.visible == 0` in `hud_render()`

**Test User Story 4**:
- [ ] T053 [US4] Manual test toggle: Press 'H', verify HUD disappears completely
- [ ] T054 [US4] Manual test restore: Press 'H' again, verify HUD reappears with current scene data
- [ ] T055 [US4] Check logs: Verify toggle events logged at INFO level with "visible=0" and "visible=1"

**Checkpoint**: Toggle functionality complete, User Stories 1-4 all functional

---

## Phase 7: User Story 5 - Highlight Selected Object (Priority: P3)

**Goal**: Highlight selected object in HUD list using Tab/Shift+Tab navigation for easy identification during manipulation

**Independent Test**: Load scene with multiple objects, press Tab repeatedly, verify each object highlights in sequence (spheres → planes → cylinders → wrap), Shift+Tab navigates backward

### Implementation for User Story 5

- [ ] T056 [P] [US5] Implement global index helper in `src/hud/hud_navigation.c`: `hud_get_global_index()` function to convert (type, index) to linear index
- [ ] T057 [P] [US5] Implement selection converter in `src/hud/hud_navigation.c`: `hud_get_selection_from_global()` function to convert linear index to (type, index)
- [ ] T058 [US5] Implement next selection in `src/hud/hud_navigation.c`: `hud_select_next()` function with wraparound logic
- [ ] T059 [US5] Implement previous selection in `src/hud/hud_navigation.c`: `hud_select_prev()` function with wraparound logic
- [ ] T060 [US5] Add Shift key tracking in `src/window/window.c`: Handle Shift key press/release events to set `render->shift_pressed` flag
- [ ] T061 [US5] Add Tab key handling in `src/window/window.c`: Check `shift_pressed` flag, call `hud_select_next()` or `hud_select_prev()` accordingly
- [ ] T062 [US5] Update object rendering in `src/hud/hud_render.c`: Check if current object matches `render->selection`, use `HUD_COLOR_HIGHLIGHT` if match
- [ ] T063 [US5] Add selection logging in `src/hud/hud_navigation.c`: Log selection changes at DEBUG level

**Test User Story 5**:
- [ ] T064 [US5] Create test scene: `tests/manual/hud_test_scenes/selection_test.rt` with 3 spheres, 2 planes, 2 cylinders
- [ ] T065 [US5] Manual test Tab forward: Press Tab 7 times, verify selection cycles through all 7 objects and wraps to first
- [ ] T066 [US5] Manual test Shift+Tab backward: Press Shift+Tab, verify selection moves backward through list
- [ ] T067 [US5] Manual test highlight visibility: Verify selected object uses green color (0x00FF00) vs white (0xFFFFFF)
- [ ] T068 [US5] Run norminette on updated files: Verify all navigation functions comply with 42 Norm

**Checkpoint**: Object selection and highlighting complete, ALL user stories (1-5) functional

---

## Phase 8: Polish & Cross-Cutting Concerns

**Purpose**: Final quality improvements, documentation, and validation

- [ ] T069 [P] Create additional test scenes in `tests/manual/hud_test_scenes/`: edge_cases.rt (0 objects), single_object.rt, max_objects.rt (100 objects)
- [ ] T070 [P] Update project README.md: Add HUD feature description and keyboard shortcuts
- [ ] T071 [P] Create Korean translation at `docs/specs/001-scene-info-hud/spec.md` (use translation tool if available)
- [ ] T072 Comprehensive memory leak test: Run valgrind with toggle, navigation, pagination operations
- [ ] T073 Performance validation: Measure HUD render time across all test scenes, verify ≤2ms for all cases
- [ ] T074 Code review cleanup: Check all functions ≤25 lines, proper naming, comments where needed
- [ ] T075 Run full norminette check: Verify all HUD module files pass 42 Norm
- [ ] T076 Validate quickstart.md: Follow all examples in `specs/001-scene-info-hud/quickstart.md`, verify instructions work
- [ ] T077 Final integration test: Load 5 different RT scenes, test all HUD features (display, pagination, selection, toggle)

**Checkpoint**: Feature complete, validated, documented, ready for merge

---

## Dependencies & Execution Order

### Phase Dependencies

- **Phase 1 (Setup)**: No dependencies - start immediately
- **Phase 2 (Foundational)**: Depends on Phase 1 completion - BLOCKS all user stories
- **Phase 3 (US1)**: Depends on Phase 2 completion
- **Phase 4 (US2)**: Depends on Phase 2 completion (can start in parallel with US1 if multiple developers)
- **Phase 5 (US3)**: Depends on Phase 3 completion (needs basic rendering from US1)
- **Phase 6 (US4)**: Depends on Phase 3 completion (needs rendering to toggle)
- **Phase 7 (US5)**: Depends on Phase 4 completion (needs object list from US2)
- **Phase 8 (Polish)**: Depends on completion of all desired user stories

### User Story Dependencies

```
Phase 1 (Setup)
    ↓
Phase 2 (Foundational) ← CRITICAL BLOCKING PHASE
    ↓
    ├─→ US1 (View Scene Config) ← MVP MINIMUM
    │       ↓
    │   ├─→ US3 (Real-time Updates)
    │   └─→ US4 (Toggle Visibility)
    │
    └─→ US2 (View Objects)
            ↓
        US5 (Highlight Selection)
```

### Within Each User Story

- **US1**: T015-T018 (rendering functions) can run in parallel → T019 (content coordinator) → T020 (main render) → T021 (integration) → T022-T023 (logging) → T024-T027 (tests)
- **US2**: T028-T030 (object renderers) can run in parallel → T031 (object list) → T032 (integration) → T033-T034 (pagination) can run in parallel → T035 (key integration) → T036 (logging) → T037-T040 (tests)
- **US3**: T041 (dirty flag) → T042-T043 (integrations) can run in parallel → T044-T045 (optimizations) → T046-T048 (tests)
- **US4**: T049 (toggle) → T050 (logging) → T051-T052 (integrations) → T053-T055 (tests)
- **US5**: T056-T057 (helpers) can run in parallel → T058-T059 (navigation) can run in parallel → T060-T062 (integrations) → T063 (logging) → T064-T068 (tests)

### Parallel Opportunities

**Within Phase 2 (Foundational)**:
- T005-T008: All hud_init.c functions can be written in parallel
- T009-T011: All hud_format.c functions can be written in parallel
- Then: T012-T014 (integrations) sequentially

**Within US1**:
```bash
# Launch in parallel:
T015: src/hud/hud_render.c - hud_render_background()
T016: src/hud/hud_render.c - hud_render_camera()
T017: src/hud/hud_render.c - hud_render_ambient()
T018: src/hud/hud_render.c - hud_render_light()
```

**Within US2**:
```bash
# Launch in parallel:
T028: src/hud/hud_render.c - sphere renderer
T029: src/hud/hud_render.c - plane renderer
T030: src/hud/hud_render.c - cylinder renderer

# Then launch in parallel:
T033: src/hud/hud_navigation.c - hud_page_down()
T034: src/hud/hud_navigation.c - hud_page_up()
```

**Cross-Story Parallelism** (if multiple developers):
- After Phase 2: Developer A → US1, Developer B → US2 (both start simultaneously)
- After US1 complete: Developer A → US3/US4, Developer B continues US2
- After US2 complete: Developer B → US5, Developer A continues US3/US4

**Polish Phase**:
```bash
# Launch in parallel:
T069: Create test scenes
T070: Update README
T071: Create Korean translation
```

---

## Implementation Strategy

### MVP First (Minimum Viable Product)

**Goal**: Get core functionality working as fast as possible

1. ✅ Complete Phase 1: Setup (T001-T004)
2. ✅ Complete Phase 2: Foundational (T005-T014) - CRITICAL
3. ✅ Complete Phase 3: User Story 1 (T015-T027)
4. **STOP and VALIDATE**: 
   - Load `scenes/basic_sphere.rt`
   - Verify camera, ambient, light info displayed
   - Check memory leaks with valgrind
   - Verify 42 Norm compliance
5. **MVP READY**: Basic HUD showing scene configuration

**MVP Scope**: Just User Story 1 provides immediate value for RT file verification

### Incremental Delivery (Recommended)

Each phase adds value independently:

1. **Iteration 1**: Setup + Foundational + US1 → Basic scene info HUD ✅ DEPLOY
2. **Iteration 2**: + US2 → Add object list with pagination ✅ DEPLOY
3. **Iteration 3**: + US3 → Add real-time updates ✅ DEPLOY
4. **Iteration 4**: + US4 → Add toggle visibility ✅ DEPLOY
5. **Iteration 5**: + US5 → Add object highlighting ✅ DEPLOY
6. **Iteration 6**: + Polish → Final quality pass ✅ DEPLOY

Each iteration is independently testable and deliverable.

### Parallel Team Strategy

If you have 2 developers:

**Phase 1-2**: Both work together (setup + foundation)

**Phase 3-4** (after foundation complete):
- Developer A: User Story 1 (T015-T027)
- Developer B: User Story 2 (T028-T040)
- No conflicts (different logical features, minimal file overlap)

**Phase 5-7**:
- Developer A: US3 + US4 (T041-T055)
- Developer B: US5 (T056-T068)

**Phase 8**: Both work on polish together

---

## File-Level Task Mapping

### New Files Created

**includes/hud.h** (API contract):
- T002: Copy from contracts/hud_api.h

**src/hud/hud_init.c** (lifecycle):
- T005: hud_init()
- T006: hud_cleanup()
- T007: hud_create_background()
- T008: hud_calculate_total_pages()
- T022: Initialization logging

**src/hud/hud_format.c** (text formatting):
- T009: hud_format_float()
- T010: hud_format_vec3()
- T011: hud_format_color()

**src/hud/hud_render.c** (rendering):
- T015: hud_render_background()
- T016: hud_render_camera()
- T017: hud_render_ambient()
- T018: hud_render_light()
- T019: hud_render_content()
- T020: hud_render()
- T023: Render time logging
- T028: Sphere rendering helper
- T029: Plane rendering helper
- T030: Cylinder rendering helper
- T031: hud_render_objects()
- T041: hud_mark_dirty()
- T045: Dirty flag optimization
- T062: Selection highlighting

**src/hud/hud_navigation.c** (user interaction):
- T033: hud_page_down()
- T034: hud_page_up()
- T036: Page navigation logging
- T056: hud_get_global_index()
- T057: hud_get_selection_from_global()
- T058: hud_select_next()
- T059: hud_select_prev()
- T063: Selection logging

**src/hud/hud_toggle.c** (visibility control):
- T049: hud_toggle()
- T050: Toggle logging

### Files Modified

**Makefile**:
- T003: Add HUD source files to build

**includes/window.h**:
- T004: Add t_hud_state and shift_pressed to t_render

**src/window/window.c** (integration points):
- T012: Call hud_init() in init_window()
- T013: Call hud_cleanup() in close_window()
- T014: Initialize shift_pressed flag
- T021: Call hud_render() in render loop
- T035: Handle Up/Down arrow keys for pagination
- T042: Call hud_mark_dirty() on camera movement
- T043: Call hud_mark_dirty() on object movement
- T044: Check hud.dirty flag in render loop
- T051: Handle 'H' key for toggle
- T060: Handle Shift key press/release
- T061: Handle Tab key with shift detection

**README.md** (documentation):
- T070: Add HUD feature description

### Test Files Created

**tests/manual/hud_test_scenes/large_scene.rt**:
- T037: Scene with 20+ objects for pagination testing

**tests/manual/hud_test_scenes/selection_test.rt**:
- T064: Scene with multiple object types for selection testing

**tests/manual/hud_test_scenes/** (edge cases):
- T069: edge_cases.rt, single_object.rt, max_objects.rt

---

## Success Criteria Validation

Each user story must pass its independent test criteria:

**US1 - View Scene Configuration**:
- ✅ Camera position, direction, FOV displayed
- ✅ Ambient light ratio and color displayed
- ✅ Light source position, brightness, color displayed
- ✅ Text readable with good contrast (semi-transparent background)
- ✅ Verified with `scenes/basic_sphere.rt`

**US2 - View Object Summary**:
- ✅ Object counts displayed for spheres, planes, cylinders
- ✅ Object properties displayed (position, size, color)
- ✅ Pagination active when >8 objects
- ✅ Page navigation works with Up/Down arrows
- ✅ Verified with `tests/manual/hud_test_scenes/large_scene.rt`

**US3 - Real-time Updates**:
- ✅ Camera position updates when camera moves
- ✅ Direction vector updates when camera rotates
- ✅ Updates occur within 16ms (one frame)
- ✅ Verified by moving camera and observing HUD

**US4 - Toggle HUD Visibility**:
- ✅ 'H' key hides HUD completely
- ✅ 'H' key shows HUD again
- ✅ Scene continues rendering when HUD hidden
- ✅ Toggle events logged at INFO level

**US5 - Highlight Selected Object**:
- ✅ Tab cycles through objects forward
- ✅ Shift+Tab cycles backward
- ✅ Selected object highlighted in green
- ✅ Highlight wraps around (last → first)
- ✅ Verified with `tests/manual/hud_test_scenes/selection_test.rt`

**Performance Goals**:
- ✅ HUD rendering ≤2ms per frame (measured with gettimeofday)
- ✅ No memory leaks (verified with valgrind)
- ✅ 42 Norm compliance (verified with norminette)

---

## Notes

- All tasks include exact file paths for clarity
- [P] marker indicates parallelizable tasks (different files, no dependencies)
- [Story] label maps task to user story for traceability
- Manual testing approach (no automated test framework in miniRT)
- Each user story independently completable and testable
- 42 Norm compliance checked at multiple points
- Memory management validated with valgrind
- Performance validated with time measurements

**Total Tasks**: 77 tasks across 8 phases  
**Task Breakdown by Phase**:
- Phase 1 (Setup): 4 tasks
- Phase 2 (Foundational): 10 tasks
- Phase 3 (US1): 13 tasks
- Phase 4 (US2): 13 tasks
- Phase 5 (US3): 8 tasks
- Phase 6 (US4): 7 tasks
- Phase 7 (US5): 13 tasks
- Phase 8 (Polish): 9 tasks

**Estimated Effort**: ~15-20 hours for experienced C developer (including testing and validation)

**Ready for Implementation**: All tasks are specific, actionable, and include file paths. Each task can be completed independently within the dependency constraints.
