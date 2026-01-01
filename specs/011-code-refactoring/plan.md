# Implementation Plan: Code Refactoring for Calculation Optimization

**Branch**: `011-code-refactoring` | **Date**: 2026-01-01 | **Spec**: [spec.md](./spec.md)
**Input**: Feature specification from `/specs/011-code-refactoring/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command. See `.specify/templates/commands/plan.md` for the execution workflow.

## Summary

This feature focuses on identifying and eliminating redundant calculations, duplicate code patterns, and improving code maintainability in the miniRT ray tracing engine. Primary targets include: (1) caching duplicate vector operations (dot products, magnitudes, normalizations) in hot paths, (2) abstracting repetitive intersection loop patterns across object types, (3) precomputing geometric properties during scene parsing, and (4) consolidating similar calculation functions. All optimizations must maintain identical rendering output and comply with 42 School function constraints (no multithreading, external libraries, or prohibited functions).

## Technical Context

**Language/Version**: C (C99 standard), compiled with cc/gcc with flags -Wall -Wextra -Werror  
**Primary Dependencies**: MiniLibX (graphics library), libm (math library), libft (custom C library)  
**Storage**: N/A (scene files parsed at runtime, no persistent storage)  
**Testing**: Manual testing with scene files in scenes/ directory, valgrind for memory leak checks  
**Target Platform**: Linux and macOS (cross-platform via Makefile detection)  
**Project Type**: Single project (ray tracing renderer)  
**Performance Goals**: 5-15% FPS improvement through calculation optimization, maintain 60 FPS target on standard scenes  
**Constraints**: Must comply with 42 School function restrictions (no pthread, fork, external parallelization libs), zero memory leaks, norminette compliance, identical visual output before/after refactoring  
**Scale/Scope**: ~10,000 lines of code, 3 object types (sphere, plane, cylinder), hot paths in ray-object intersection and lighting calculations

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

**Project Structure Standards** (Principle I):
- ✅ No changes to CI scripts in .github/scripts/
- ✅ No changes to user-facing scripts at root level
- ✅ Documentation updates will be in docs/ if needed
- ✅ No scattered automation scripts

**Code Quality Automation** (Principle II):
- ✅ All refactored code MUST pass norminette
- ✅ All refactored code MUST pass valgrind with zero leaks
- ✅ Build MUST succeed without errors
- ✅ Refactoring changes will be tested through existing CI pipeline

**Documentation Synchronization** (Principle III):
- ✅ Code refactoring does not require wiki updates
- ✅ Code comments will be updated to document cached calculations

**Workflow System** (Principle IV):
- ✅ Changes will follow standard PR workflow with quality gates
- ✅ Testing will use existing test scenes in scenes/ directory

**Tools and Environment Standards** (Principle V):
- ✅ No changes to platform support (Linux/macOS)
- ✅ No new dependencies introduced
- ✅ Existing build system and test infrastructure unchanged

**Bilingual Specification Management** (Principle VI):
- ✅ English spec created in specs/011-code-refactoring/
- ⚠️ Korean translation NEEDS to be created in docs/specs/011-code-refactoring/

**42 School Constraints Check** (Principle VII):
- ✅ All functions used MUST be from allowed list (libc, math, mlx_*, libft, get_next_line)
- ✅ pthread functions MUST NOT be used
- ✅ fork/pipe/multiprocessing MUST NOT be used  
- ✅ External parallelization libraries (SIMD, OpenMP, etc.) MUST NOT be used
- ✅ Optimizations MUST use only allowed techniques (algorithmic, caching, math, memory layout)
- ✅ This refactoring uses: caching (local variables), memory layout optimization (precomputed struct fields), mathematical optimization (algebraic simplification)
- ✅ No prohibited optimization techniques will be introduced

**GATE STATUS**: ⚠️ CONDITIONAL PASS - Korean translation must be created alongside English spec

---

### Post-Design Constitution Re-evaluation

**Date**: 2026-01-01 (After Phase 0 Research & Phase 1 Design)

**Project Structure Standards** (Principle I):
- ✅ VERIFIED: No structural changes to CI, scripts, or documentation hierarchy
- ✅ All design artifacts placed in specs/011-code-refactoring/ as required

**Code Quality Automation** (Principle II):
- ✅ VERIFIED: Design includes norminette compliance requirements
- ✅ VERIFIED: Valgrind validation in testing strategy (research.md section 6.3)
- ✅ VERIFIED: No changes bypass quality gates

**Documentation Synchronization** (Principle III):
- ✅ VERIFIED: Code comments will document cached calculations (research.md section 10.1)
- ✅ No wiki updates required for internal refactoring

**Workflow System** (Principle IV):
- ✅ VERIFIED: Standard PR workflow maintained
- ✅ VERIFIED: Phased implementation with testing at each stage (research.md section 9)

**Tools and Environment Standards** (Principle V):
- ✅ VERIFIED: No new dependencies introduced
- ✅ VERIFIED: Cross-platform compatibility maintained
- ✅ VERIFIED: Existing test infrastructure sufficient

**Bilingual Specification Management** (Principle VI):
- ⚠️ STILL PENDING: Korean translation in docs/specs/011-code-refactoring/
- ✅ English spec complete with all required sections

**42 School Constraints Check** (Principle VII):
- ✅ VERIFIED: research.md section 8 documents full compliance
- ✅ VERIFIED: No prohibited functions (pthread, fork, SIMD) in design
- ✅ VERIFIED: All optimizations use allowed techniques:
  - Caching (local variables, precomputed struct fields)
  - Mathematical optimization (cached dot products, precomputed geometry)
  - Memory layout optimization (structure field additions)
- ✅ VERIFIED: Function pointer abstraction uses standard C only
- ✅ VERIFIED: No external libraries or parallelization

**POST-DESIGN GATE STATUS**: ✅ PASS with documentation note
- **Action Required**: Create Korean translation of spec.md in docs/specs/011-code-refactoring/
- **Implementation Ready**: All design artifacts complete and constitution-compliant

## Project Structure

### Documentation (this feature)

```text
specs/[###-feature]/
├── plan.md              # This file (/speckit.plan command output)
├── research.md          # Phase 0 output (/speckit.plan command)
├── data-model.md        # Phase 1 output (/speckit.plan command)
├── quickstart.md        # Phase 1 output (/speckit.plan command)
├── contracts/           # Phase 1 output (/speckit.plan command)
└── tasks.md             # Phase 2 output (/speckit.tasks command - NOT created by /speckit.plan)
```

### Source Code (repository root)

```text
src/
├── ray/                    # PRIMARY TARGET: intersection calculations
│   ├── intersect_cylinder.c   # Duplicate dot products, radius calculations
│   └── intersections.c         # Repetitive intersection loop patterns
├── lighting/               # TARGET: lighting calculations with duplicates
│   ├── lighting.c
│   ├── shadow_calc.c
│   ├── shadow_attenuation.c
│   └── shadow_config.c
├── render/                 # TARGET: screen-to-NDC conversions
│   ├── render.c
│   ├── trace.c
│   ├── camera.c
│   └── render_state.c
├── math/                   # TARGET: vector operation consolidation
│   ├── vector.c
│   └── vector_ops.c
├── parser/                 # TARGET: precompute geometry during parsing
│   ├── parser.c
│   ├── parse_objects.c
│   └── parse_elements.c
└── spatial/                # Existing BVH implementation (no changes needed)

includes/
├── minirt.h               # Core type definitions
├── objects.h              # Object structures (may add cached fields)
├── ray.h                  # Ray structures
└── vec3.h                 # Vector type definitions

tests/
└── (manual testing with scenes/ directory)

scenes/
├── (existing test scenes for validation)
```

**Structure Decision**: Single project structure (Option 1) as miniRT is a standalone ray tracing renderer. Refactoring will focus on src/ray/ for intersection optimizations, src/render/ for coordinate conversion consolidation, src/parser/ for precomputing geometry properties, and src/math/ for vector operation utilities. No structural changes needed—only internal function optimizations and potential additions of cached fields to object structures in includes/objects.h.

## Complexity Tracking

> **Fill ONLY if Constitution Check has violations that must be justified**

No violations. Korean translation requirement noted but not a blocking violation—it's a documentation synchronization requirement that should be fulfilled in parallel or immediately after English spec completion.
