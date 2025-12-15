---

description: "Task list for miniRT - Ray Tracing 3D Renderer"
---

# Tasks: miniRT - Ray Tracing 3D Renderer

**Input**: Design documents from `/specs/001-raytracer/`
**Prerequisites**: plan.md, spec.md, research.md, data-model.md, contracts/

**Tests**: Unit tests are REQUIRED per constitution (using Criterion framework)

**Organization**: Tasks are grouped by user story to enable independent implementation and testing of each story.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2, US3)
- Include exact file paths in descriptions

## Path Conventions

Single project structure at repository root:
- Source: `src/`
- Headers: `includes/`
- Tests: `tests/`
- Documentation: `docs/`
- Scenes: `scenes/`

---

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Project initialization and basic structure

- [ ] T001 Create project directory structure (src/, includes/, tests/, docs/, scenes/)
- [ ] T002 Create Makefile with all, clean, fclean, re rules
- [ ] T003 Add MinilibX library configuration to Makefile (-lmlx -lXext -lX11 -lm)
- [ ] T004 [P] Create main header includes/minirt.h with project-wide structures
- [ ] T005 [P] Install Criterion testing framework and configure test Makefile
- [ ] T006 [P] Create error handling utilities in src/utils/error.c
- [ ] T007 [P] Add 42 header to all source files (norminette requirement)

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Core infrastructure that MUST be complete before ANY user story can be implemented

**⚠️ CRITICAL**: No user story work can begin until this phase is complete

### Vector Math Foundation

- [ ] T008 Define t_vec3 structure in includes/math.h
- [ ] T009 [P] Implement vec3_add, vec3_subtract, vec3_multiply in src/math/vector.c
- [ ] T010 [P] Implement vec3_dot, vec3_cross in src/math/vector_ops.c
- [ ] T011 [P] Implement vec3_magnitude, vec3_normalize in src/math/vector_ops.c
- [ ] T012 [P] Unit tests for vector operations in tests/unit/test_vector.c

### Core Data Structures

- [ ] T013 Define t_color structure in includes/minirt.h
- [ ] T014 [P] Define t_ambient, t_camera, t_light structures in includes/minirt.h
- [ ] T015 [P] Define t_sphere, t_plane, t_cylinder structures in includes/objects.h
- [ ] T016 [P] Define t_ray, t_hit structures in includes/ray.h
- [ ] T017 Define t_scene structure (container for all elements) in includes/minirt.h
- [ ] T018 Define t_render structure (MinilibX resources) in includes/window.h

### Memory Management

- [ ] T019 Implement cleanup_scene function in src/utils/cleanup.c
- [ ] T020 Implement cleanup_render function in src/utils/cleanup.c
- [ ] T021 Implement centralized cleanup_all function in src/utils/cleanup.c

**Checkpoint**: Foundation ready - user story implementation can now begin in parallel

---

## Phase 3: User Story 2 - Scene File Parsing (Priority: P1) 🎯 MVP Foundation

**Goal**: Parse .rt files into scene structures with comprehensive validation

**Independent Test**: Create test.rt with all elements, run parser, verify no errors and all elements identified

**Note**: Although US1 (Basic Scene Rendering) is the user-facing feature, US2 (parsing) must be implemented first as it's a technical prerequisite for rendering.

### Unit Tests for Parsing (Write First)

> **NOTE: Write these tests FIRST, ensure they FAIL before implementation**

- [ ] T022 [P] [US2] Unit test for tokenization in tests/unit/test_parser.c
- [ ] T023 [P] [US2] Unit test for ambient parsing in tests/unit/test_parser.c
- [ ] T024 [P] [US2] Unit test for camera parsing in tests/unit/test_parser.c
- [ ] T025 [P] [US2] Unit test for light parsing in tests/unit/test_parser.c
- [ ] T026 [P] [US2] Unit test for object parsing in tests/unit/test_parser.c
- [ ] T027 [P] [US2] Unit test for validation functions in tests/unit/test_parser.c

### Parser Implementation

- [ ] T028 [US2] Create parser.h header with function declarations in includes/parser.h
- [ ] T029 [US2] Implement file opening and line reading in src/parser/parser.c
- [ ] T030 [P] [US2] Implement tokenization (split by whitespace) in src/parser/parser.c
- [ ] T031 [P] [US2] Implement parse_ambient function in src/parser/parse_elements.c
- [ ] T032 [P] [US2] Implement parse_camera function in src/parser/parse_elements.c
- [ ] T033 [P] [US2] Implement parse_light function in src/parser/parse_elements.c
- [ ] T034 [P] [US2] Implement parse_sphere function in src/parser/parse_elements.c
- [ ] T035 [P] [US2] Implement parse_plane function in src/parser/parse_elements.c
- [ ] T036 [P] [US2] Implement parse_cylinder function in src/parser/parse_elements.c

### Validation Implementation

- [ ] T037 [P] [US2] Implement validate_ratio function in src/parser/parse_validation.c
- [ ] T038 [P] [US2] Implement validate_rgb function in src/parser/parse_validation.c
- [ ] T039 [P] [US2] Implement validate_normalized_vector in src/parser/parse_validation.c
- [ ] T040 [P] [US2] Implement validate_positive function in src/parser/parse_validation.c
- [ ] T041 [P] [US2] Implement validate_fov function in src/parser/parse_validation.c

### Scene Validation

- [ ] T042 [US2] Implement check for mandatory elements (A, C, L exactly once) in src/parser/parser.c
- [ ] T043 [US2] Implement duplicate detection for singleton elements in src/parser/parser.c
- [ ] T044 [US2] Implement main parse_scene_file function in src/parser/parser.c

### Test Scenes

- [ ] T045 [P] [US2] Create test scene scenes/test_basic.rt with valid elements
- [ ] T046 [P] [US2] Create error test scenes in scenes/error_cases/

**Checkpoint**: Parser complete - can successfully parse valid scenes and reject invalid ones

---

## Phase 4: User Story 4 - Ray-Object Intersection (Priority: P2)

**Goal**: Calculate precise intersection points between rays and geometric objects

**Independent Test**: Generate rays at known angles, verify intersection math returns expected points

**Note**: Implemented before US1 rendering as intersection logic is needed for rendering

### Unit Tests for Intersections (Write First)

- [ ] T047 [P] [US4] Unit test ray-sphere intersection (miss, tangent, two hits) in tests/unit/test_intersections.c
- [ ] T048 [P] [US4] Unit test ray-plane intersection (parallel, perpendicular, oblique) in tests/unit/test_intersections.c
- [ ] T049 [P] [US4] Unit test ray-cylinder body intersection in tests/unit/test_intersections.c
- [ ] T050 [P] [US4] Unit test ray-cylinder cap intersection in tests/unit/test_intersections.c

### Ray Structure and Utilities

- [ ] T051 [US4] Create ray.h header in includes/ray.h
- [ ] T052 [P] [US4] Implement ray creation function in src/ray/ray.c
- [ ] T053 [P] [US4] Implement ray_at function (point at distance t) in src/ray/ray.c

### Sphere Intersection

- [ ] T054 [US4] Implement intersect_sphere using quadratic equation in src/objects/sphere.c
- [ ] T055 [US4] Calculate sphere surface normal at intersection in src/objects/sphere.c

### Plane Intersection

- [ ] T056 [US4] Implement intersect_plane using dot product method in src/objects/plane.c
- [ ] T057 [US4] Handle plane normal orientation in src/objects/plane.c

### Cylinder Intersection

- [ ] T058 [US4] Implement infinite cylinder body intersection in src/objects/cylinder.c
- [ ] T059 [US4] Implement top cap plane intersection with radius check in src/objects/cylinder.c
- [ ] T060 [US4] Implement bottom cap plane intersection with radius check in src/objects/cylinder.c
- [ ] T061 [US4] Validate height bounds for body intersections in src/objects/cylinder.c
- [ ] T062 [US4] Calculate cylinder surface normals (body vs caps) in src/objects/cylinder.c

### Scene-Wide Intersection

- [ ] T063 [US4] Implement find_nearest_intersection (test all objects) in src/ray/intersections.c
- [ ] T064 [US4] Return nearest positive t value with hit data in src/ray/intersections.c

**Checkpoint**: All intersection math complete and tested

---

## Phase 5: User Story 3 - Lighting System (Priority: P2)

**Goal**: Apply ambient and diffuse lighting with hard shadows

**Independent Test**: Render sphere with light at different positions, verify brightness gradient moves with light

### Unit Tests for Lighting (Write First)

- [ ] T065 [P] [US3] Unit test ambient lighting calculation in tests/unit/test_lighting.c
- [ ] T066 [P] [US3] Unit test diffuse lighting calculation in tests/unit/test_lighting.c
- [ ] T067 [P] [US3] Unit test shadow ray detection in tests/unit/test_lighting.c
- [ ] T068 [P] [US3] Unit test color composition and clamping in tests/unit/test_lighting.c

### Lighting Implementation

- [ ] T069 [US3] Create lighting.h header in includes/lighting.h
- [ ] T070 [P] [US3] Implement calculate_ambient function in src/lighting/ambient.c
- [ ] T071 [P] [US3] Implement calculate_diffuse (N·L formula) in src/lighting/diffuse.c
- [ ] T072 [US3] Implement cast_shadow_ray function in src/lighting/shadows.c
- [ ] T073 [US3] Implement is_in_shadow (test all objects) in src/lighting/shadows.c

### Color Utilities

- [ ] T074 [P] [US3] Implement color_to_float conversion in src/render/color.c
- [ ] T075 [P] [US3] Implement color_to_int with clamping in src/render/color.c
- [ ] T076 [P] [US3] Implement color composition function in src/render/color.c

### Lighting Integration

- [ ] T077 [US3] Implement calculate_lighting (ambient + diffuse + shadows) in src/lighting/lighting.c

**Checkpoint**: Lighting system complete - can calculate final colors with shadows

---

## Phase 6: User Story 1 - Basic Scene Rendering (Priority: P1) 🎯 MVP Delivery

**Goal**: Generate and display rendered 3D images in window

**Independent Test**: Run ./miniRT scenes/test_basic.rt and verify rendered sphere appears with shading

**Note**: This is the user-facing MVP - combines all previous technical work

### Camera Ray Generation

- [ ] T078 [US1] Create render.h header in includes/render.h
- [ ] T079 [US1] Calculate camera viewport from FOV in src/render/camera.c
- [ ] T080 [US1] Calculate camera coordinate system (right, up vectors) in src/render/camera.c
- [ ] T081 [US1] Implement generate_camera_ray function in src/render/camera.c

### Rendering Loop

- [ ] T082 [US1] Implement pixel coordinate to ray direction in src/render/camera.c
- [ ] T083 [US1] Implement main rendering loop (per pixel) in src/render/render.c
- [ ] T084 [US1] Integrate ray generation, intersection, lighting in src/render/render.c
- [ ] T085 [US1] Implement put_pixel function (write to image buffer) in src/render/render.c

### MinilibX Integration

- [ ] T086 [US1] Create window.h header in includes/window.h
- [ ] T087 [US1] Implement init_window (mlx_init, mlx_new_window) in src/window/window.c
- [ ] T088 [US1] Implement init_image (mlx_new_image, mlx_get_data_addr) in src/window/window.c
- [ ] T089 [US1] Display rendered image with mlx_put_image_to_window in src/render/render.c

### Main Program

- [ ] T090 [US1] Implement argument parsing in src/main.c
- [ ] T091 [US1] Validate .rt file extension in src/main.c
- [ ] T092 [US1] Integrate parser → render → display pipeline in src/main.c
- [ ] T093 [US1] Call cleanup on exit in src/main.c

### Integration Test Scenes

- [ ] T094 [P] [US1] Create scenes/sphere.rt (single red sphere)
- [ ] T095 [P] [US1] Create scenes/multiple_objects.rt (sphere + plane + cylinder)
- [ ] T096 [P] [US1] Create scenes/shadow_test.rt (verify hard shadows)

**Checkpoint**: MVP COMPLETE - Program renders scenes to window

---

## Phase 7: User Story 5 - Window Management (Priority: P2)

**Goal**: Handle ESC key, window close, minimize/restore without crashes

**Independent Test**: Launch program, test ESC, X button, minimize - all should work cleanly

### Event Handlers

- [ ] T097 [US5] Create events.h header in includes/events.h
- [ ] T098 [P] [US5] Implement handle_keypress (ESC = exit) in src/window/events.c
- [ ] T099 [P] [US5] Implement handle_close (X button) in src/window/events.c
- [ ] T100 [P] [US5] Implement handle_expose (redraw on restore) in src/window/events.c

### Event Registration

- [ ] T101 [US5] Register mlx_hook for KeyPress event in src/window/window.c
- [ ] T102 [US5] Register mlx_hook for DestroyNotify event in src/window/window.c
- [ ] T103 [US5] Register mlx_hook for Expose event in src/window/window.c

### Event Loop

- [ ] T104 [US5] Start mlx_loop in src/main.c
- [ ] T105 [US5] Ensure cleanup_all called on all exit paths in src/main.c

### Memory Leak Testing

- [ ] T106 [US5] Run valgrind on all test scenes, verify no leaks
- [ ] T107 [US5] Fix any memory leaks found
- [ ] T108 [US5] Test minimize/maximize cycles for stability

**Checkpoint**: Window management complete - program behaves like standard desktop application

---

## Phase 8: User Story 6 - Object Transformations (Priority: P3)

**Goal**: Support arbitrary positions and orientations via coordinates and direction vectors

**Independent Test**: Create scenes with same objects at different positions/orientations, verify correct rendering

**Note**: Much of transformation support already implemented in parser and intersection code - this phase adds validation

### Transformation Validation

- [ ] T109 [P] [US6] Verify camera position transformation in multiple test scenes in scenes/camera_positions/
- [ ] T110 [P] [US6] Verify camera orientation transformation in multiple test scenes in scenes/camera_orientations/
- [ ] T111 [P] [US6] Verify plane orientation via normal vectors in scenes/plane_orientations/
- [ ] T112 [P] [US6] Verify cylinder orientation via axis vectors in scenes/cylinder_orientations/

### Test Scenes for Transformations

- [ ] T113 [P] [US6] Create scenes/transforms/camera_top_view.rt (looking down)
- [ ] T114 [P] [US6] Create scenes/transforms/camera_side_view.rt (looking sideways)
- [ ] T115 [P] [US6] Create scenes/transforms/tilted_plane.rt (45-degree plane)
- [ ] T116 [P] [US6] Create scenes/transforms/horizontal_cylinder.rt (horizontal axis)

**Checkpoint**: Transformations validated - objects can be positioned and oriented freely

---

## Phase 9: User Story 7 - Adjustable Object Properties (Priority: P3)

**Goal**: Support variable object sizes (diameter, height) from scene files

**Independent Test**: Create scenes with same objects at different sizes, verify visual size changes

**Note**: Size properties already implemented in parser and intersection math - this phase validates behavior

### Size Validation Tests

- [ ] T117 [P] [US7] Create scenes/sizes/small_sphere.rt (diameter 5) in scenes/sizes/
- [ ] T118 [P] [US7] Create scenes/sizes/large_sphere.rt (diameter 50) in scenes/sizes/
- [ ] T119 [P] [US7] Create scenes/sizes/short_wide_cylinder.rt (diameter > height) in scenes/sizes/
- [ ] T120 [P] [US7] Create scenes/sizes/tall_thin_cylinder.rt (height > diameter) in scenes/sizes/

### Visual Comparison

- [ ] T121 [US7] Render all size test scenes and verify proportions are correct
- [ ] T122 [US7] Verify sphere radius = diameter / 2 in rendered output
- [ ] T123 [US7] Verify cylinder dimensions match specifications

**Checkpoint**: Object sizing validated - dimensions accurately represented in renders

---

## Phase 10: Polish & Cross-Cutting Concerns

**Purpose**: Improvements that affect multiple user stories

### Documentation

- [ ] T124 [P] Create docs/architecture.md with overall structure (Korean)
- [ ] T125 [P] Create docs/math.md with vector function explanations (Korean)
- [ ] T126 [P] Create docs/parser.md with parsing logic explanation (Korean)
- [ ] T127 [P] Create docs/rendering.md with rendering algorithm explanation (Korean)
- [ ] T128 Update README.md with usage examples and project status

### Code Quality

- [ ] T129 Run norminette on all source files, fix violations
- [ ] T130 Run norminette on all header files, fix violations
- [ ] T131 Add Doxygen comments to all public functions
- [ ] T132 [P] Verify all functions <= 25 lines (norminette requirement)
- [ ] T133 [P] Verify all functions <= 4 parameters (norminette requirement)

### Error Handling

- [ ] T134 Test all error cases from spec.md edge cases section
- [ ] T135 Verify error messages match specified formats
- [ ] T136 Test file not found error handling
- [ ] T137 Test malformed scene file error handling

### Performance

- [ ] T138 Profile rendering performance on complex scenes
- [ ] T139 Optimize: normalize vectors only once where possible
- [ ] T140 Optimize: cache dot products where applicable
- [ ] T141 Verify simple scenes (3-5 objects) render in < 5 seconds
- [ ] T142 Verify complex scenes (20 objects) render in < 30 seconds

### Final Validation

- [ ] T143 Run all unit tests - verify 100% pass
- [ ] T144 Render all test scenes - visual verification
- [ ] T145 Run valgrind on all scenes - verify zero leaks
- [ ] T146 Test all window interactions (ESC, X, minimize)
- [ ] T147 Verify quickstart.md instructions are accurate
- [ ] T148 Create final demo scenes showcasing all features

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies - can start immediately
- **Foundational (Phase 2)**: Depends on Setup completion - BLOCKS all user stories
- **US2 Parsing (Phase 3)**: Depends on Foundational phase
- **US4 Intersections (Phase 4)**: Depends on Foundational phase (independent of US2 but typically done after parsing for testing)
- **US3 Lighting (Phase 5)**: Depends on Foundational phase and US4 (needs intersection results)
- **US1 Rendering (Phase 6)**: Depends on US2, US4, US3 (combines all previous work) - **MVP DELIVERY**
- **US5 Window Management (Phase 7)**: Depends on US1 (needs working render to test events)
- **US6 Transformations (Phase 8)**: Depends on US1 (validation phase)
- **US7 Adjustable Properties (Phase 9)**: Depends on US1 (validation phase)
- **Polish (Phase 10)**: Depends on all desired user stories

### Critical Path to MVP

1. Phase 1: Setup (T001-T007)
2. Phase 2: Foundational (T008-T021) - **BLOCKING**
3. Phase 3: US2 Parsing (T022-T046)
4. Phase 4: US4 Intersections (T047-T064)
5. Phase 5: US3 Lighting (T065-T077)
6. Phase 6: US1 Rendering (T078-T096) - **MVP COMPLETE**

After MVP, US5, US6, US7 can be added incrementally.

### User Story Dependencies

- **US2 Parsing (P1)**: Independent after Foundational
- **US4 Intersections (P2)**: Independent after Foundational
- **US3 Lighting (P2)**: Needs US4 (uses intersection results)
- **US1 Rendering (P1)**: Needs US2, US3, US4 (integrator)
- **US5 Window Management (P2)**: Needs US1 (tests rendered window)
- **US6 Transformations (P3)**: Needs US1 (validation only)
- **US7 Adjustable Properties (P3)**: Needs US1 (validation only)

### Within Each Phase

- **Phase 2 (Foundational)**: Vector math → Data structures → Memory management
- **Phase 3 (Parsing)**: Tests first → Parser implementation → Validation → Integration
- **Phase 4 (Intersections)**: Tests first → Sphere → Plane → Cylinder → Scene-wide
- **Phase 5 (Lighting)**: Tests first → Ambient → Diffuse → Shadows → Integration
- **Phase 6 (Rendering)**: Camera rays → Rendering loop → MinilibX → Main program

### Parallel Opportunities

**Phase 1 (Setup)**:
- T004 (minirt.h), T005 (Criterion), T006 (error.c), T007 (headers) can run in parallel

**Phase 2 (Foundational)**:
- Vector operations (T009, T010, T011) can run in parallel
- Data structure definitions (T014, T015) can run in parallel
- Unit tests (T012) after implementations

**Phase 3 (US2 Parsing)**:
- All unit tests (T022-T027) can run in parallel
- Element parsers (T031-T036) can run in parallel after T030
- Validation functions (T037-T041) can run in parallel
- Test scenes (T045, T046) can run in parallel

**Phase 4 (US4 Intersections)**:
- All unit tests (T047-T050) can run in parallel
- Ray utilities (T052, T053) can run in parallel
- After intersection implementations complete, T063-T064 integrate

**Phase 5 (US3 Lighting)**:
- All unit tests (T065-T068) can run in parallel
- Lighting implementations (T070, T071) can run in parallel
- Color utilities (T074, T075, T076) can run in parallel

**Phase 6 (US1 Rendering)**:
- Test scenes (T094, T095, T096) can run in parallel at end

**Phase 7 (US5 Window Management)**:
- Event handlers (T098, T099, T100) can run in parallel

**Phase 8 (US6 Transformations)**:
- All validation tests (T109-T112) can run in parallel
- All test scenes (T113-T116) can run in parallel

**Phase 9 (US7 Sizes)**:
- All test scenes (T117-T120) can run in parallel

**Phase 10 (Polish)**:
- Documentation files (T124-T127) can run in parallel
- Norminette checks (T129, T130) can run in parallel
- Function checks (T132, T133) can run in parallel

---

## Parallel Example: Phase 2 Foundational

```bash
# After T008 (vector structure defined), launch in parallel:
Task T009: "Implement vec3_add, vec3_subtract, vec3_multiply in src/math/vector.c"
Task T010: "Implement vec3_dot, vec3_cross in src/math/vector_ops.c"
Task T011: "Implement vec3_magnitude, vec3_normalize in src/math/vector_ops.c"

# After vector implementations done:
Task T012: "Unit tests for vector operations in tests/unit/test_vector.c"

# In parallel with vector work, launch:
Task T014: "Define t_ambient, t_camera, t_light structures in includes/minirt.h"
Task T015: "Define t_sphere, t_plane, t_cylinder structures in includes/objects.h"
Task T016: "Define t_ray, t_hit structures in includes/ray.h"
```

---

## Implementation Strategy

### MVP First (Phases 1-6)

1. Complete Phase 1: Setup (7 tasks)
2. Complete Phase 2: Foundational (14 tasks) - **CRITICAL BLOCKER**
3. Complete Phase 3: US2 Parsing (25 tasks)
4. Complete Phase 4: US4 Intersections (18 tasks)
5. Complete Phase 5: US3 Lighting (13 tasks)
6. Complete Phase 6: US1 Rendering (19 tasks)
7. **STOP and VALIDATE**: Run ./miniRT scenes/test_basic.rt
8. **MVP ACHIEVED**: Working ray tracer with all mandatory features

**Total to MVP**: 96 tasks

### Incremental Delivery After MVP

After MVP validation:

9. Add Phase 7: US5 Window Management (12 tasks) - Better UX
10. Add Phase 8: US6 Transformations (8 tasks) - Already working, just validation
11. Add Phase 9: US7 Adjustable Properties (7 tasks) - Already working, just validation
12. Complete Phase 10: Polish (25 tasks) - Documentation, optimization, final validation

**Total tasks**: 148 tasks

### Parallel Team Strategy

With 2 developers after Phase 2:

- Developer A: Phase 3 (US2 Parsing)
- Developer B: Phase 4 (US4 Intersections)

Then both needed for Phase 5 (US3 Lighting) and Phase 6 (US1 Rendering) as these integrate previous work.

With 3+ developers after MVP:

- Developer A: Phase 7 (US5 Window Management)
- Developer B: Phase 8 (US6 Transformations validation)
- Developer C: Phase 9 (US7 Sizes validation)

---

## Task Count Summary

- **Phase 1 (Setup)**: 7 tasks
- **Phase 2 (Foundational)**: 14 tasks [BLOCKING]
- **Phase 3 (US2 Parsing - P1)**: 25 tasks
- **Phase 4 (US4 Intersections - P2)**: 18 tasks
- **Phase 5 (US3 Lighting - P2)**: 13 tasks
- **Phase 6 (US1 Rendering - P1)**: 19 tasks [MVP DELIVERY]
- **Phase 7 (US5 Window Mgmt - P2)**: 12 tasks
- **Phase 8 (US6 Transforms - P3)**: 8 tasks
- **Phase 9 (US7 Sizes - P3)**: 7 tasks
- **Phase 10 (Polish)**: 25 tasks

**Total**: 148 tasks
**MVP Subset**: 96 tasks (Phases 1-6)
**Parallelizable**: ~45% of tasks marked [P]

---

## Testing Strategy

### Unit Tests (Criterion Framework)

**Location**: `tests/unit/`

**Coverage**:
- Vector operations (T012)
- Parser functions (T022-T027)
- Intersection mathematics (T047-T050)
- Lighting calculations (T065-T068)

**Execution**:
```bash
make test
./tests/test_vector
./tests/test_parser
./tests/test_intersections
./tests/test_lighting
```

### Integration Tests (Visual)

**Location**: `scenes/`

**Approach**:
1. Render test scene
2. Visual verification of:
   - Object shapes (spheres circular, cylinders with caps)
   - Lighting (gradients, shadows)
   - Colors (accurate RGB)
   - Transformations (correct positions/orientations)

### Memory Tests

```bash
valgrind --leak-check=full --show-leak-kinds=all ./miniRT scenes/test_basic.rt
# Must show: All heap blocks were freed -- no leaks are possible
```

### Norminette Tests

```bash
norminette src/ includes/
# Must pass with no errors
```

---

## Notes

- **[P] tasks** = different files, no dependencies within phase
- **[Story] label** maps task to specific user story for traceability
- Each user story should be independently completable and testable
- **Constitution compliance**: Unit tests mandatory, norminette mandatory, no memory leaks
- Tests written FIRST (fail), then implementation (pass)
- Commit after each task or logical group
- Stop at any checkpoint to validate independently
- **EPSILON = 0.0001** for floating point comparisons (avoid self-intersection)
- All vectors normalized before use in calculations
- Error messages format: "Error\n<specific message>"

---

## Success Criteria Validation

After completion, verify all success criteria from spec.md:

**Rendering Quality** (SC-001 to SC-005):
- [ ] Spheres appear as perfect circles
- [ ] Cylinders show curved surface + caps
- [ ] Shadows have hard edges
- [ ] Correct proportions with camera changes
- [ ] Correct depth ordering

**Lighting Accuracy** (SC-006 to SC-009):
- [ ] Brightness varies with surface angle
- [ ] Ambient prevents pure black
- [ ] Light brightness ratio works
- [ ] Shadows appear behind objects

**Parsing Robustness** (SC-010 to SC-013):
- [ ] Elements in any order accepted
- [ ] Duplicate A/C/L rejected
- [ ] Out-of-range values rejected
- [ ] Specific error messages provided

**User Experience** (SC-014 to SC-017):
- [ ] ESC and X button both exit cleanly
- [ ] Minimize/restore works smoothly
- [ ] Object sizes adjustable via scene file
- [ ] Camera position/orientation adjustable

**Technical Performance** (SC-018 to SC-020):
- [ ] Zero memory leaks (valgrind clean)
- [ ] Simple scenes < 5 seconds
- [ ] Complex scenes < 30 seconds

**Code Quality** (SC-021 to SC-023):
- [ ] Makefile builds successfully
- [ ] All make rules work (clean, fclean, re)
- [ ] Only permitted functions used

---

**End of Tasks Document**

**Generated**: 2025-12-15
**Feature**: miniRT - Ray Tracing 3D Renderer
**Branch**: 001-raytracer
**Status**: Ready for implementation
