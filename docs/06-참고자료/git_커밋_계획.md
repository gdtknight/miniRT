# Git Commit Strategy for miniRT

## Branch Structure
- main: Production-ready code
- develop: Integration branch
- feature/phase-{1-7}: Feature branches for each phase

## Commit Plan

### Phase 1: Project Setup (feature/phase-1-setup)
1. Add Makefile and build configuration
2. Add project structure (includes/, src/ directories)
3. Add main.c entry point
4. Add documentation (README.md)

### Phase 2: Data Structures (feature/phase-2-datastructures)
1. Add vec3.h - Vector structure definitions
2. Add objects.h - Scene object structures
3. Add ray.h - Ray tracing structures
4. Add minirt.h - Main header file
5. Add parser.h - Parser function declarations
6. Add window.h - Window management declarations

### Phase 3: Vector Math (feature/phase-3-math)
1. Add vector.c - Basic vector operations
2. Add vector_ops.c - Advanced vector operations (dot, cross, normalize)

### Phase 4: Parser (feature/phase-4-parser)
1. Add parser.c - Main parsing logic and file reading
2. Add parse_elements.c - Parse A, C, L elements
3. Add parse_objects.c - Parse sp, pl, cy objects
4. Add parse_validation.c - Scene validation functions

### Phase 5: Ray Intersections (feature/phase-5-intersections)
1. Add intersections.c - Ray-sphere intersection
2. Update intersections.c - Ray-plane intersection
3. Update intersections.c - Ray-cylinder intersection

### Phase 6: Lighting (feature/phase-6-lighting)
1. Add lighting.c - Ambient lighting
2. Update lighting.c - Diffuse lighting
3. Update lighting.c - Shadow calculation

### Phase 7: Rendering (feature/phase-7-rendering)
1. Add render.c - Camera ray generation
2. Update render.c - Ray tracing loop
3. Update render.c - Main rendering function

### Phase 8: Window Management (feature/phase-8-window)
1. Add window.c - Window initialization
2. Update window.c - Event handlers (ESC, X button)

### Phase 9: Utilities (feature/phase-9-utils)
1. Add error.c - Error handling functions
2. Add cleanup.c - Memory cleanup functions

### Phase 10: Test Scenes (feature/phase-10-scenes)
1. Add user story scenes (US-01 to US-07)
2. Add error test scenes
3. Add comprehensive test scenes

### Phase 11: Documentation (feature/phase-11-docs)
1. Add Korean documentation files
2. Add test scene documentation
3. Add implementation guides

### Phase 12: Cleanup (feature/phase-12-cleanup)
1. Remove empty stub files
2. Fix norminette issues
3. Update Makefile (remove deleted files)
4. Add cleanup report
