# MiniRT Codebase Baseline Metrics

**Created**: 2026-01-03  
**Feature**: 012-code-cleanup-refactor  
**Purpose**: Document baseline state before refactoring

## Total Lines of Code

**Total LOC**: 5,550 lines

Breakdown by type:
- Source files (src/**/*.c): ~4,850 lines
- Header files (includes/*.h): ~600 lines  
- Main entry point (main.c): ~100 lines

## File Structure

### Source Directories (11 total)

| Directory | File Count | Primary Domain |
|-----------|------------|----------------|
| src/hud/ | 7 | HUD/Performance Display |
| src/keyguide/ | 3 | Interactive Key Guide |
| src/lighting/ | 5 | Lighting & Shadows |
| src/math/ | 2 | Vector Mathematics |
| src/parser/ | 4 | Scene File Parsing |
| src/ray/ | 2 | Ray Tracing Operations |
| src/render/ | 6 | Rendering Engine |
| src/spatial/ | 5 | BVH Spatial Acceleration |
| src/utils/ | 5 | Utility Functions |
| src/window/ | 1 | Window Management |
| src/main.c | 1 | Application Entry |

**Total Source Files**: 41 files

### Header Files (12 total)

| Header | Domain | Lines |
|--------|--------|-------|
| includes/hud.h | HUD Domain | ~50 |
| includes/keyguide.h | KeyGuide Domain | ~30 |
| includes/metrics.h | Metrics Domain | ~40 |
| includes/minirt.h | Core Domain | ~150 |
| includes/objects.h | Object Domain | ~80 |
| includes/parser.h | Parser Domain | ~60 |
| includes/ray.h | Ray Domain | ~50 |
| includes/render_state.h | Rendering Domain | ~70 |
| includes/shadow.h | Lighting Domain | ~40 |
| includes/spatial.h | Spatial Domain | ~60 |
| includes/vec3.h | Math Domain | ~50 |
| includes/window.h | Window Domain | ~20 |

## Current Compliance Status

### Norminette Compliance

**Note**: norminette command not available in environment.
Manual review suggests good compliance based on:
- No #define statements in .c files (verified via grep)
- Modular file organization
- Consistent naming conventions

### 42 School Function Constraints

**Prohibited Functions Check**: 
- pthread functions: 0 occurrences ✓
- fork/pipe functions: 0 occurrences ✓
- External libraries: Only allowed (libc, math, mlx) ✓

### Code Organization

**Current State**:
- ✓ Good modular structure with domain directories
- ✓ Clear header/source separation
- ✓ Logical domain grouping
- ⚠️ Potential unused functions (requires call graph analysis)
- ⚠️ Some domain boundary overlap possible

## Functional Domain Analysis

### Identified Domains

1. **Math Domain** (Tier 0 - Foundation)
   - Header: vec3.h
   - Source: src/math/
   - Dependencies: None

2. **Object Domain** (Tier 1)
   - Header: objects.h
   - Dependencies: Math

3. **Ray Domain** (Tier 1)
   - Header: ray.h
   - Source: src/ray/
   - Dependencies: Math

4. **Spatial Domain** (Tier 2)
   - Header: spatial.h
   - Source: src/spatial/
   - Dependencies: Math, Object, Ray

5. **Lighting Domain** (Tier 2)
   - Header: shadow.h
   - Source: src/lighting/
   - Dependencies: Math, Object, Ray

6. **Parser Domain** (Tier 2)
   - Header: parser.h
   - Source: src/parser/
   - Dependencies: Math, Object

7. **Rendering Domain** (Tier 3)
   - Header: render_state.h
   - Source: src/render/
   - Dependencies: Spatial, Lighting, Ray, Object, Math

8. **Metrics Domain** (Tier 3)
   - Header: metrics.h
   - Source: src/render/metrics*.c
   - Dependencies: Rendering

9. **Window Domain** (Tier 4)
   - Header: window.h
   - Source: src/window/
   - Dependencies: Rendering

10. **HUD Domain** (Tier 4)
    - Header: hud.h
    - Source: src/hud/
    - Dependencies: Window, Metrics

11. **KeyGuide Domain** (Tier 4)
    - Header: keyguide.h
    - Source: src/keyguide/
    - Dependencies: Window

12. **Core Domain** (Tier 5 - Application)
    - Header: minirt.h
    - Source: main.c
    - Dependencies: All domains

## Target Metrics (Success Criteria)

Based on spec.md success criteria:

| Metric | Current | Target | Status |
|--------|---------|--------|--------|
| **SC-001**: Norminette violations | Unknown | 0 | 🔄 To verify |
| **SC-002**: Compilation | Success | Success | ✓ |
| **SC-003**: Test scenes render | Yes | 100% match | ✓ |
| **SC-004**: LOC reduction | 5,550 | ≤4,995 (10%) | 🔄 To achieve |
| **SC-005**: Mixed-domain headers | Unknown | 0 | 🔄 To verify |
| **SC-006**: Files per domain | ~2-3 | ≤2 | 🔄 To verify |
| **SC-007**: Defines in source | 0 | 0 | ✓ Already compliant |
| **SC-008**: Duplicate declarations | Unknown | 0 | 🔄 To verify |
| **SC-011**: Prohibited functions | 0 | 0 | ✓ Already compliant |

## Expected Changes

### Phase 1 Targets
1. Remove unused functions → Reduce LOC by 10-15%
2. Clarify domain boundaries → Achieve single-domain headers
3. Improve organization → Achieve ≤2 files per domain lookup

### Risk Assessment
- **Low Risk**: No #define migration needed (already compliant)
- **Low Risk**: Good existing structure (refinement, not restructuring)
- **Medium Risk**: Function removal may affect callbacks/function pointers

## Notes

- Baseline established before any refactoring work
- All scene files currently render correctly
- Project compiles without errors or warnings
- No prohibited 42 School functions detected
- Good foundation for cleanup and improvement

---

**Next Steps**: Begin Phase 2 (Foundational) to create function usage analysis and establish git checkpoint.

---

## Norminette Compliance Baseline (January 4, 2026)

**Feature**: 013-norminette-compliance  
**Total Errors**: 44 errors

### Error Distribution by Type

| Error Type | Count | Percentage | Priority |
|------------|-------|------------|----------|
| TOO_MANY_FUNCS | 24 | 54.5% | P1 (High) |
| TOO_MANY_LINES | 5 | 11.4% | P2 (Medium) |
| TOO_MANY_ARGS | 6 | 13.6% | P3 (Low) |
| TOO_MANY_TAB | 1 | 2.3% | P2 (Medium) |
| MISALIGNED_FUNC_DECL | 1 | 2.3% | P3 (Low) |
| EMPTY_LINE_EOF | 1 | 2.3% | P3 (Low) |

### Files with Errors (10 files total)

1. **render.c** - 1 error (TOO_MANY_LINES)
2. **render_state.c** - 4 errors (TOO_MANY_FUNCS)
3. **timer.c** - 1 error (EMPTY_LINE_EOF)
4. **window.c** - 9 errors (TOO_MANY_LINES + TOO_MANY_FUNCS)
5. **keyguide_render.c** - 2 errors (TOO_MANY_ARGS + TOO_MANY_LINES)
6. **bvh_build.c** - 6 errors (TOO_MANY_FUNCS)
7. **hud_render.c** - 14 errors (TOO_MANY_ARGS + TOO_MANY_FUNCS)
8. **hud_performance.c** - 2 errors (TOO_MANY_ARGS)
9. **hud.h** - 2 errors (MISALIGNED_FUNC_DECL + TOO_MANY_TAB)
10. **window.h** - 2 errors (TOO_MANY_ARGS)

### Build Status

✅ **SUCCESS** - Build completes without errors after fixing duplicate symbol issue

