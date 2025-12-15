# Implementation Plan: miniRT - Ray Tracing 3D Renderer

**Branch**: `001-raytracer` | **Date**: 2025-12-15 | **Spec**: [spec.md](./spec.md)
**Input**: Feature specification from `/specs/001-raytracer/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command. See `.specify/templates/commands/plan.md` for the execution workflow.

## Summary

miniRT is a ray tracing-based 3D image generation program written in C that renders geometric objects (spheres, planes, cylinders) from scene description files. The program uses the MinilibX library for windowing, implements ray-object intersection mathematics, applies ambient and diffuse lighting with hard shadows, and provides comprehensive scene file parsing with error validation.

## Technical Context

**Language/Version**: C (C99 or later with -std=c99 or compatible)  
**Primary Dependencies**: MinilibX (X11/XQuartz graphics library), libm (math library), optional libft  
**Storage**: File-based (.rt scene description files in ASCII text format)  
**Testing**: Unit tests required per constitution (testing framework NEEDS CLARIFICATION)  
**Target Platform**: Linux/macOS with X11/XQuartz display server
**Project Type**: Single project (graphics application with window rendering)  
**Performance Goals**: Simple scenes (1-5 objects) render in <5 seconds, up to 20 objects in <30 seconds  
**Constraints**: No memory leaks (Valgrind clean), norminette compliance (NON-NEGOTIABLE), single-threaded execution  
**Scale/Scope**: Educational project - handles 1-20 objects per scene, 3 geometric primitives, basic lighting model

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

### ✅ I. 42 Norminette Compliance (NON-NEGOTIABLE)
- **Status**: COMPLIANT
- **Action**: All C source and header files will comply with 42 norminette
- **Verification**: Run `norminette` on all *.c and *.h files before commits

### ✅ II. Readability-First Code
- **Status**: COMPLIANT
- **Action**: Use descriptive names, break complex ray-tracing math into helper functions, define constants for magic numbers

### ✅ III. Mandatory Unit Testing (NON-NEGOTIABLE)
- **Status**: COMPLIANT - Criterion framework selected
- **Action**: Each function (vector math, intersections, parsing) will have unit tests after implementation
- **Framework**: Criterion testing framework (see research.md for rationale)

### ✅ IV. Bilingual Documentation
- **Status**: COMPLIANT
- **Action**: English Doxygen comments in code, Korean explanations in `docs/` directory

### ✅ V. Build Verification Workflow (NON-NEGOTIABLE)
- **Status**: COMPLIANT
- **Action**: After each change: compile → run → verify output → run tests → check norminette

## Project Structure

### Documentation (this feature)

```text
specs/001-raytracer/
├── plan.md              # This file (/speckit.plan command output)
├── research.md          # Phase 0 output (/speckit.plan command)
├── data-model.md        # Phase 1 output (/speckit.plan command)
├── quickstart.md        # Phase 1 output (/speckit.plan command)
├── contracts/           # Phase 1 output (/speckit.plan command)
│   ├── scene-file-format.md    # .rt file format specification
│   └── rendering-pipeline.md   # Ray tracing algorithm contracts
└── tasks.md             # Phase 2 output (/speckit.tasks command - NOT created by /speckit.plan)
```

### Source Code (repository root)

```text
src/
├── main.c               # Entry point, argument parsing
├── parser/              # Scene file parsing
│   ├── parser.c
│   ├── parse_elements.c
│   └── parse_validation.c
├── math/                # Vector and matrix operations
│   ├── vector.c
│   ├── vector_ops.c
│   └── matrix.c
├── ray/                 # Ray tracing core
│   ├── ray.c
│   └── intersections.c
├── objects/             # Geometric primitives
│   ├── sphere.c
│   ├── plane.c
│   └── cylinder.c
├── lighting/            # Lighting calculations
│   ├── ambient.c
│   ├── diffuse.c
│   └── shadows.c
├── render/              # Rendering engine
│   ├── render.c
│   ├── camera.c
│   └── color.c
├── window/              # MinilibX window management
│   ├── window.c
│   └── events.c
└── utils/               # Error handling, memory management
    ├── error.c
    └── cleanup.c

includes/
├── minirt.h             # Main header with structures
├── parser.h
├── math.h
├── ray.h
├── objects.h
├── lighting.h
├── render.h
└── window.h

tests/
├── unit/                # Unit tests for each function
│   ├── test_vector.c
│   ├── test_intersections.c
│   ├── test_parser.c
│   └── test_lighting.c
└── scenes/              # Test scene files
    ├── basic_sphere.rt
    ├── multiple_objects.rt
    └── error_cases/

docs/                    # Korean documentation
├── architecture.md      # 전체 구조 설명
├── math.md              # 수학 함수 설명
├── parser.md            # 파서 설명
└── rendering.md         # 렌더링 엔진 설명

Makefile                 # Build system (all, clean, fclean, re)
README.md                # Project overview and changelog
```

**Structure Decision**: Single project structure is appropriate for this graphics application. The organization separates concerns (parsing, math, ray tracing, rendering, windowing) while maintaining clear dependencies: parser → objects/scene → ray tracing → lighting → rendering → window display.

## Complexity Tracking

> **No constitution violations requiring justification**

All aspects of the project comply with constitution principles:
- Single project structure (no unnecessary complexity)
- Norminette compliance is mandatory and achievable
- Unit testing will be implemented for all functions
- Bilingual documentation planned
- Build verification workflow is straightforward for this project type

## Phase Summary

### Phase 0: Outline & Research ✅ COMPLETE

**Status**: All NEEDS CLARIFICATION items resolved

**Deliverables**:
- ✅ `research.md` created with 10 research topics
- ✅ Testing framework selected: Criterion
- ✅ Ray-object intersection mathematics documented
- ✅ Lighting model strategy defined
- ✅ Scene parsing approach established
- ✅ MinilibX patterns identified
- ✅ Memory management strategy defined
- ✅ Norminette compliance patterns documented
- ✅ Color handling approach defined

**Key Decisions**:
- Unit testing: Criterion framework (modern, lightweight, 42-compatible)
- Intersection math: Quadratic equation (sphere), dot product (plane), combined approach (cylinder)
- Lighting: Phong model limited to ambient + diffuse components
- Parsing: Line-by-line tokenization with element-specific validation
- Window management: Event-driven architecture with MinilibX hooks
- Memory: Centralized cleanup with resource tracking
- Colors: Integer RGB [0-255] with float intermediate calculations

---

### Phase 1: Design & Contracts ✅ COMPLETE

**Status**: Data model and contracts defined

**Deliverables**:
- ✅ `data-model.md` created with 12 core entities
- ✅ `contracts/scene-file-format.md` created (scene file specification)
- ✅ `contracts/rendering-pipeline.md` created (rendering algorithm)
- ✅ `quickstart.md` created (developer guide with 5-week implementation plan)
- ✅ Agent context updated (Copilot instructions file)

**Key Entities Defined**:
1. Vector (t_vec3) - 3D coordinates and directions
2. Color (t_color) - RGB representation
3. Ambient Light (t_ambient) - Global illumination
4. Camera (t_camera) - Viewpoint and projection
5. Point Light (t_light) - Directional light source
6. Sphere (t_sphere) - Spherical geometry
7. Plane (t_plane) - Infinite flat surface
8. Cylinder (t_cylinder) - Finite cylindrical object
9. Ray (t_ray) - Ray tracing construct
10. Intersection (t_hit) - Ray-object collision result
11. Scene (t_scene) - Scene container
12. Render Context (t_render) - Runtime state

**Contracts Defined**:
- Scene file format with EBNF grammar
- Validation rules for all parameters
- Error message specifications
- Rendering pipeline stages (6 stages)
- Ray-object intersection algorithms
- Lighting calculation formulas
- Shadow ray casting protocol

---

### Phase 2: Task Breakdown ⏳ PENDING

**Status**: Not started - requires `/speckit.tasks` command

**Expected Deliverables**:
- `tasks.md` - Detailed implementation tasks with dependencies
- Task breakdown by module (parser, math, intersections, lighting, window)
- Testing milestones for each module
- Integration checkpoints

**Note**: This phase is NOT part of the `/speckit.plan` command scope. Run `/speckit.tasks` separately to generate task breakdown.

---

## Implementation Readiness

### ✅ Prerequisites Complete
- [x] Feature specification reviewed and understood
- [x] Constitution compliance verified
- [x] All technical unknowns researched and resolved
- [x] Data model defined with 12 entities
- [x] Scene file format contract specified
- [x] Rendering pipeline contract defined
- [x] Developer quickstart guide created
- [x] Agent context updated with project technologies

### 📋 Ready for Implementation
- Project structure defined and documented
- Module boundaries clear (parser, math, ray, objects, lighting, render, window, utils)
- Testing strategy established (Criterion framework)
- Build system requirements specified (Makefile with all, clean, fclean, re)
- Memory management strategy defined (centralized cleanup)
- Error handling patterns documented

### 🎯 Next Steps
1. **Create Git branch**: `git checkout -b 001-raytracer`
2. **Run task breakdown**: Use `/speckit.tasks` command to generate `tasks.md`
3. **Begin implementation**: Follow 5-week plan in `quickstart.md`
4. **Verify at each phase**: Build, test, check norminette
5. **Document in Korean**: Create `docs/` content as implementation progresses

---

## Success Metrics

### Technical Validation
- [ ] All source files pass norminette without errors
- [ ] Unit tests created and passing for all functions
- [ ] Valgrind shows zero memory leaks
- [ ] Program builds with `make all` without warnings
- [ ] ESC and window close buttons work correctly

### Functional Validation
- [ ] Renders spheres as perfect circles when viewed head-on
- [ ] Cylinders show both curved surface and flat caps
- [ ] Shadows have hard, well-defined edges
- [ ] Ambient lighting prevents completely black areas
- [ ] Diffuse lighting creates proper brightness gradients
- [ ] Objects maintain correct proportions with different camera angles

### Performance Validation
- [ ] Simple scenes (1-5 objects) render in < 5 seconds
- [ ] Complex scenes (up to 20 objects) render in < 30 seconds
- [ ] No noticeable lag in window event handling

---

## Artifacts Generated

### Planning Documents
| File | Status | Purpose |
|------|--------|---------|
| `plan.md` | ✅ Complete | This file - overall implementation plan |
| `research.md` | ✅ Complete | Technical research and decisions (10 topics) |
| `data-model.md` | ✅ Complete | Entity definitions and relationships (12 entities) |
| `quickstart.md` | ✅ Complete | Developer guide with 5-week implementation plan |
| `contracts/scene-file-format.md` | ✅ Complete | Scene file specification with EBNF grammar |
| `contracts/rendering-pipeline.md` | ✅ Complete | Rendering algorithm specification |
| `tasks.md` | ⏳ Pending | Task breakdown (requires `/speckit.tasks` command) |

### Configuration Files
| File | Status | Purpose |
|------|--------|---------|
| `.github/agents/copilot-instructions.md` | ✅ Updated | Copilot agent context with C, MinilibX, testing info |

---

## Command Execution Report

**Command**: `/speckit.plan` for feature `001-raytracer`  
**Branch**: `001-raytracer`  
**Execution Date**: 2025-12-15  
**Status**: ✅ SUCCEEDED

**Phases Completed**:
1. ✅ Setup: Parsed configuration from `.specify/scripts/bash/setup-plan.sh`
2. ✅ Context Loading: Read spec, constitution, and plan template
3. ✅ Technical Context: Filled with C, MinilibX, testing framework details
4. ✅ Constitution Check: Verified compliance with all 5 principles
5. ✅ Phase 0 Research: Generated `research.md` with 10 resolved topics
6. ✅ Phase 1 Design: Created data model (12 entities), contracts (2 specs), quickstart guide
7. ✅ Agent Context Update: Updated Copilot instructions file

**Files Created**: 6 files
**Files Modified**: 2 files
**Total Lines Added**: ~2,800 lines of documentation

**Command terminated after Phase 1 as specified in workflow.**

---

**End of Implementation Plan**  
**Next Command**: `/speckit.tasks` to generate task breakdown for Phase 2
