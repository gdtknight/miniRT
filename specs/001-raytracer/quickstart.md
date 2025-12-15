# miniRT Quickstart Guide

**Version**: 1.0  
**Date**: 2025-12-15  
**Audience**: Developers implementing the miniRT ray tracer

## Prerequisites

- **C Compiler**: GCC or Clang with C99 support
- **Make**: GNU Make for build automation
- **MinilibX**: Installed and accessible (typically in `/usr/local/lib` or project directory)
- **Development Libraries**: X11 development headers (Linux) or XQuartz (macOS)
- **Testing Framework**: Criterion (for unit tests)

### Installing Dependencies

**Linux (Debian/Ubuntu)**:
```bash
sudo apt-get update
sudo apt-get install gcc make libx11-dev libxext-dev libbsd-dev
# Install MinilibX (clone and build from 42 repository)
# Install Criterion for testing
```

**macOS**:
```bash
# Install Xcode Command Line Tools
xcode-select --install
# Install XQuartz from https://www.xquartz.org/
# MinilibX typically provided in project or compile from source
```

---

## Project Setup

### 1. Repository Structure

```bash
miniRT_final/
├── src/                  # Source code
├── includes/             # Header files
├── tests/                # Unit tests
├── docs/                 # Korean documentation
├── specs/                # Feature specifications
│   └── 001-raytracer/    # This feature
├── scenes/               # Example .rt files
├── Makefile              # Build configuration
└── README.md             # Project overview
```

### 2. Clone and Initialize

```bash
git clone <repository-url> miniRT_final
cd miniRT_final
git checkout 001-raytracer  # Feature branch
```

### 3. Initial Build

```bash
make all
```

**Expected Output**:
- Executable `miniRT` created in project root
- No compilation warnings or errors
- Norminette clean (run `norminette src/ includes/`)

---

## Development Workflow

### Phase 1: Vector Math Foundation (Week 1)

**Goal**: Implement core vector operations

**Files to Create**:
- `src/math/vector.c`: Basic vector operations
- `src/math/vector_ops.c`: Advanced operations (cross product, normalize)
- `includes/math.h`: Vector structure and function declarations
- `tests/unit/test_vector.c`: Unit tests

**Implementation Order**:
1. Define `t_vec3` structure in `includes/math.h`
2. Implement: add, subtract, scalar multiply, dot product
3. Implement: cross product, magnitude, normalize
4. Write unit tests for each function
5. Verify: `make test` passes all vector tests

**Key Functions**:
```c
t_vec3  vec3_add(t_vec3 a, t_vec3 b);
t_vec3  vec3_subtract(t_vec3 a, t_vec3 b);
t_vec3  vec3_multiply(t_vec3 v, double scalar);
double  vec3_dot(t_vec3 a, t_vec3 b);
t_vec3  vec3_cross(t_vec3 a, t_vec3 b);
double  vec3_magnitude(t_vec3 v);
t_vec3  vec3_normalize(t_vec3 v);
```

**Validation**:
```bash
make test
./tests/test_vector
# All vector tests should pass
```

---

### Phase 2: Scene File Parser (Week 2)

**Goal**: Parse .rt files into scene structures

**Files to Create**:
- `src/parser/parser.c`: Main parsing logic
- `src/parser/parse_elements.c`: Element-specific parsing
- `src/parser/parse_validation.c`: Validation functions
- `includes/parser.h`: Parser function declarations
- `tests/unit/test_parser.c`: Parser unit tests

**Implementation Order**:
1. Define scene structures (`t_scene`, `t_camera`, etc.) in `includes/minirt.h`
2. Implement file reading and line-by-line parsing
3. Implement element-specific parsers (A, C, L, sp, pl, cy)
4. Implement validation (ranges, normalization)
5. Test with example scene files

**Test Scene** (`scenes/test_basic.rt`):
```
A  0.2  255,255,255
C  0,0,0  0,0,1  90
L  -40,50,0  0.6  255,255,255
sp  0,0,20  20  255,0,0
pl  0,-10,0  0,1,0  200,200,200
```

**Validation**:
```bash
./miniRT scenes/test_basic.rt
# Should parse without errors
# Window should open (even if rendering not implemented yet)
```

---

### Phase 3: Ray-Object Intersections (Week 3)

**Goal**: Implement intersection mathematics

**Files to Create**:
- `src/ray/ray.c`: Ray structure and utilities
- `src/ray/intersections.c`: Intersection algorithms
- `src/objects/sphere.c`: Sphere-specific intersection
- `src/objects/plane.c`: Plane-specific intersection
- `src/objects/cylinder.c`: Cylinder-specific intersection
- `tests/unit/test_intersections.c`: Intersection tests

**Implementation Order**:
1. Define `t_ray` and `t_hit` structures
2. Implement sphere intersection (quadratic equation)
3. Implement plane intersection (dot product method)
4. Implement cylinder intersection (body + caps)
5. Test each intersection type with unit tests

**Key Functions**:
```c
t_hit  intersect_sphere(t_ray ray, t_sphere sphere);
t_hit  intersect_plane(t_ray ray, t_plane plane);
t_hit  intersect_cylinder(t_ray ray, t_cylinder cylinder);
t_hit  find_nearest_intersection(t_ray ray, t_scene *scene);
```

**Validation**:
```bash
make test
./tests/test_intersections
# Verify: ray-sphere, ray-plane, ray-cylinder tests pass
```

---

### Phase 4: Lighting and Rendering (Week 4)

**Goal**: Apply lighting model and render images

**Files to Create**:
- `src/lighting/ambient.c`: Ambient lighting
- `src/lighting/diffuse.c`: Diffuse lighting
- `src/lighting/shadows.c`: Shadow ray casting
- `src/render/render.c`: Main rendering loop
- `src/render/camera.c`: Camera ray generation
- `tests/unit/test_lighting.c`: Lighting tests

**Implementation Order**:
1. Implement camera ray generation
2. Implement ambient lighting
3. Implement diffuse lighting (N·L)
4. Implement shadow ray casting
5. Integrate: render loop that combines all components

**Key Functions**:
```c
t_ray   generate_camera_ray(t_camera *cam, int x, int y, int w, int h);
t_color calculate_ambient(t_hit hit, t_ambient ambient);
t_color calculate_diffuse(t_hit hit, t_light light, t_scene *scene);
bool    is_in_shadow(t_vec3 point, t_light light, t_scene *scene);
void    render_scene(t_render *render, t_scene *scene);
```

**Validation**:
```bash
./miniRT scenes/test_basic.rt
# Should display rendered image with:
# - Red sphere with shading
# - Gray ground plane with shadow
# - Proper ambient + diffuse lighting
```

---

### Phase 5: Window Management (Week 5)

**Goal**: Handle user events and window lifecycle

**Files to Create**:
- `src/window/window.c`: Window initialization
- `src/window/events.c`: Event handlers
- `src/utils/cleanup.c`: Resource cleanup

**Implementation Order**:
1. Initialize MinilibX window and image buffer
2. Implement ESC key handler
3. Implement window close (X button) handler
4. Implement expose event handler (redraw)
5. Implement cleanup function (no leaks)

**Key Functions**:
```c
t_render    *init_window(int width, int height, char *title);
int         handle_keypress(int keycode, t_render *render);
int         handle_close(t_render *render);
void        cleanup_all(t_render *render, t_scene *scene);
```

**Validation**:
```bash
./miniRT scenes/test_basic.rt
# Test: Press ESC → window closes, program exits
# Test: Click X button → window closes, program exits
# Test: Minimize/restore → image remains visible
valgrind --leak-check=full ./miniRT scenes/test_basic.rt
# Should show: "All heap blocks were freed -- no leaks are possible"
```

---

## Building and Testing

### Build Commands

```bash
make all          # Compile miniRT
make clean        # Remove object files
make fclean       # Remove object files and executable
make re           # Rebuild from scratch
make test         # Build and run unit tests
norminette        # Check norminette compliance
```

### Running the Program

```bash
./miniRT <scene_file.rt>
```

**Example**:
```bash
./miniRT scenes/sphere.rt
./miniRT scenes/multiple_objects.rt
./miniRT scenes/cylinder_test.rt
```

### Testing Strategy

**Unit Tests** (run after each implementation phase):
```bash
make test
# Or run specific test:
./tests/test_vector
./tests/test_parser
./tests/test_intersections
./tests/test_lighting
```

**Integration Tests** (visual verification):
1. Render test scenes
2. Verify objects appear correctly
3. Check lighting and shadows
4. Test window interactions

**Memory Tests**:
```bash
valgrind --leak-check=full --show-leak-kinds=all ./miniRT scenes/test.rt
# Must show: no leaks, all memory freed
```

**Norminette Tests**:
```bash
norminette src/
norminette includes/
# Must pass with no errors
```

---

## Common Issues and Solutions

### Issue: MinilibX Not Found

**Symptom**: `mlx.h: No such file or directory`

**Solution**:
```bash
# Update Makefile with correct MinilibX path
INCLUDES = -I./includes -I/usr/local/include -I./mlx
LIBS = -L/usr/local/lib -lmlx -lXext -lX11 -lm
```

### Issue: Window Not Opening

**Symptom**: Program runs but no window appears

**Solution**:
- Check X11/XQuartz is running (macOS: launch XQuartz first)
- Verify `mlx_init()` returns non-NULL
- Check window dimensions are positive

### Issue: Memory Leaks

**Symptom**: Valgrind reports leaks

**Solution**:
- Ensure `cleanup_all()` is called on all exit paths
- Free scene objects arrays before freeing scene
- Destroy MinilibX resources in correct order
- Set pointers to NULL after freeing

### Issue: Norminette Errors

**Symptom**: Functions exceed 25 lines or 4 parameters

**Solution**:
- Break complex functions into smaller helpers
- Use structures to group related parameters
- Extract calculations into separate functions

---

## Performance Tips

**Optimization Checklist**:
- [ ] Normalize vectors only once (cache results)
- [ ] Use epsilon (0.0001) to avoid self-intersection
- [ ] Skip sqrt() when comparing distances (compare squared distances)
- [ ] Render to image buffer, not directly to window
- [ ] Consider early ray termination for distant objects

---

## Documentation

### Code Comments (English)

All functions must have Doxygen-style comments:
```c
/**
 * @brief Calculates intersection between ray and sphere
 * @param ray Ray to test (must have normalized direction)
 * @param sphere Sphere object to test against
 * @return Hit structure with intersection data or no-hit indicator
 */
t_hit intersect_sphere(t_ray ray, t_sphere sphere);
```

### Korean Documentation

Create detailed explanations in `docs/`:
- `docs/architecture.md`: Overall structure
- `docs/math.md`: Vector/math functions (Korean)
- `docs/parser.md`: Parser implementation (Korean)
- `docs/rendering.md`: Rendering algorithm (Korean)

---

## Next Steps

After completing the implementation:

1. **Test Suite**: Verify all unit tests pass
2. **Visual Tests**: Render multiple scenes, verify correctness
3. **Memory Check**: Valgrind clean with no leaks
4. **Norminette**: All files pass norminette checks
5. **Documentation**: Complete Korean docs in `docs/`
6. **README**: Update with implementation notes and examples

**Phase 2 Planning**: After completing Phase 1 (design), proceed to Phase 2 (task breakdown) using the `/speckit.tasks` command.

---

## Resources

- **MinilibX Documentation**: Check 42 intranet or local docs
- **Ray Tracing Tutorial**: "Ray Tracing in One Weekend" by Peter Shirley
- **Mathematics Reference**: `specs/001-raytracer/research.md`
- **Contracts**: `specs/001-raytracer/contracts/`

---

## Support

For questions or issues:
1. Review specification: `specs/001-raytracer/spec.md`
2. Check research notes: `specs/001-raytracer/research.md`
3. Refer to contracts: `specs/001-raytracer/contracts/`
4. Consult Korean docs: `docs/` (after implementation)

**Version**: 1.0 | **Last Updated**: 2025-12-15
