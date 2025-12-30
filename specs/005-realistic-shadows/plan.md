# Implementation Plan: Realistic Shadow Rendering

## Tech Stack
- **Language**: C (C99 standard)
- **Graphics**: MinilibX (42 School library)
- **Build**: Make
- **Testing**: Custom unit test framework
- **Linting**: norminette (42 School standard)
- **Version Control**: Git
- **Platform**: Linux (primary), macOS (secondary)

## Architecture

### Module: Shadow System
Enhanced shadow rendering integrated into existing lighting system.

**Location**: `src/lighting/`

**Components**:
1. `shadow_calc.c` - Shadow calculation functions (NEW)
2. `shadow_config.c` - Shadow configuration management (NEW)
3. `lighting.c` - Modified lighting system (MODIFIED)

### Data Flow
```
Scene Data → Camera Ray → Object Intersection → Hit Point
                                                    ↓
                          Shadow Configuration ← Lighting Calculation
                                                    ↓
                          Multiple Shadow Rays → Shadow Factor
                                                    ↓
                          Attenuation Calculation
                                                    ↓
                          Final Color → Pixel
```

## File Structure

### New Files
```
src/lighting/shadow_calc.c          # Shadow calculation functions
src/lighting/shadow_config.c        # Configuration management
includes/shadow.h                    # Shadow system header
tests/test_shadow_calc.c            # Unit tests for shadow calc
tests/test_shadow_config.c          # Unit tests for config
docs/03-기능/조명/그림자_시스템.md  # Korean documentation
scenes/test_soft_shadows.rt         # Test scene for soft shadows
scenes/test_shadow_attenuation.rt   # Test scene for attenuation
```

### Modified Files
```
src/lighting/lighting.c             # Update apply_lighting()
includes/minirt.h                   # Add shadow config to scene
includes/parser.h                   # Add shadow config parsing (optional)
Makefile                            # Add new source files
```

## Implementation Phases

### Phase 1: Foundation
**Goal**: Set up shadow configuration and adaptive bias

**Files**:
- `includes/shadow.h` (create)
- `src/lighting/shadow_config.c` (create)
- `src/lighting/shadow_calc.c` (create - bias function only)
- `tests/test_shadow_calc.c` (create)

**Functions**:
1. `init_shadow_config()` - Initialize default shadow settings
2. `calculate_shadow_bias()` - Adaptive bias calculation
3. `validate_shadow_config()` - Validate configuration parameters

**Tests**:
- Test default configuration
- Test bias calculation with various angles
- Test edge cases (0°, 90°, etc.)

### Phase 2: Soft Shadows
**Goal**: Implement shadow ray sampling for soft shadows

**Files**:
- `src/lighting/shadow_calc.c` (extend)
- `src/lighting/lighting.c` (modify)
- `tests/test_shadow_calc.c` (extend)

**Functions**:
1. `generate_shadow_sample_offset()` - Generate random offset for shadow ray
2. `calculate_soft_shadow()` - Cast multiple shadow rays
3. `calculate_shadow_factor()` - Refactor from in_shadow(), return float

**Tests**:
- Test shadow factor with 1 sample (hard shadow)
- Test shadow factor with multiple samples
- Test random distribution of samples
- Test shadow factor range [0.0, 1.0]

### Phase 3: Shadow Attenuation
**Goal**: Add distance-based shadow softness

**Files**:
- `src/lighting/shadow_calc.c` (extend)
- `tests/test_shadow_calc.c` (extend)

**Functions**:
1. `calculate_shadow_attenuation()` - Distance-based attenuation
2. `apply_shadow_attenuation()` - Apply attenuation to shadow factor

**Tests**:
- Test attenuation at various distances
- Test attenuation falloff curve
- Test edge cases (zero distance, infinite distance)

### Phase 4: Integration
**Goal**: Integrate shadow system with lighting

**Files**:
- `src/lighting/lighting.c` (modify)
- `includes/minirt.h` (modify)
- `scenes/test_soft_shadows.rt` (create)

**Modifications**:
1. Add `t_shadow_config` to `t_scene`
2. Refactor `apply_lighting()` to use shadow factor
3. Update lighting calculation to support soft shadows

**Tests**:
- Integration test with simple scene
- Visual verification of soft shadows
- Performance benchmark

### Phase 5: Polish & Documentation
**Goal**: Finalize implementation, docs, and testing

**Files**:
- `docs/03-기능/조명/그림자_시스템.md` (create - Korean)
- `scenes/test_shadow_attenuation.rt` (create)
- All source files (add English comments)

**Tasks**:
1. Add comprehensive English Doxygen comments
2. Write Korean documentation
3. Create test scenes
4. Run norminette and fix violations
5. Performance optimization
6. Memory leak check (valgrind)

## Function Signatures

### shadow_calc.c
```c
/* Calculate adaptive shadow bias based on surface normal and light direction */
double  calculate_shadow_bias(t_vec3 normal, t_vec3 light_dir, double base_bias);

/* Generate random offset for shadow ray sampling */
t_vec3  generate_shadow_sample_offset(double radius, int sample_index, int total_samples);

/* Calculate shadow factor using multiple shadow rays (soft shadows) */
double  calculate_shadow_factor(t_scene *scene, t_vec3 point, t_vec3 light_pos, t_shadow_config *config);

/* Calculate distance-based shadow attenuation */
double  calculate_shadow_attenuation(double distance, double max_distance, double softness);

/* Test if point is in shadow (single ray) */
int     is_in_shadow(t_scene *scene, t_vec3 point, t_vec3 light_dir, double light_distance, double bias);
```

### shadow_config.c
```c
/* Initialize shadow configuration with default values */
t_shadow_config init_shadow_config(void);

/* Validate shadow configuration parameters */
int             validate_shadow_config(t_shadow_config *config);

/* Set shadow samples count */
void            set_shadow_samples(t_shadow_config *config, int samples);

/* Set shadow softness factor */
void            set_shadow_softness(t_shadow_config *config, double softness);
```

### lighting.c (modified functions)
```c
/* Apply lighting calculation with soft shadows */
t_color apply_lighting(t_scene *scene, t_hit *hit);
```

## Testing Strategy

### Unit Test Coverage
- All shadow calculation functions
- Configuration management
- Edge cases and error conditions
- Performance benchmarks

### Integration Tests
1. Simple scene with soft shadows
2. Multiple objects with shadow interaction
3. Various light distances
4. Performance with different sample counts

### Visual Tests
1. Side-by-side comparison: hard vs soft shadows
2. Shadow attenuation demonstration
3. Complex scene with all features

## Norminette Compliance Strategy

1. **Function Length**: Keep functions under 25 lines
2. **Line Length**: Max 80 characters
3. **Function Count**: Max 5 functions per file (split if needed)
4. **Variable Names**: Follow norminette conventions
5. **42 Header**: Use proper header in all files
6. **Frequent Checks**: Run norminette after each function

## Performance Targets

- **Hard Shadows (1 sample)**: Same as current performance
- **Soft Shadows (4 samples)**: >20 FPS on 800x600
- **Soft Shadows (16 samples)**: >10 FPS on 800x600
- **Memory Overhead**: <1MB additional

## Dependencies

### Existing Systems
- Vector math (vec3.c)
- Ray system (ray.c)
- Intersection tests (intersections.c)
- Scene data structures (minirt.h)
- Lighting system (lighting.c)

### External Dependencies
- MinilibX (graphics)
- libm (math library)
- libc (standard library)

## Build Integration

### Makefile Updates
```makefile
# Add new source files
SHADOW_SRC = src/lighting/shadow_calc.c \
             src/lighting/shadow_config.c

# Add to SRCS
SRCS += $(SHADOW_SRC)

# Add to OBJS
OBJS += $(SHADOW_SRC:.c=.o)
```

### Compilation Flags
```makefile
CFLAGS = -Wall -Wextra -Werror -O2 -I includes
```

## Risk Mitigation

1. **Performance**: Configurable sample count, optimize hot paths
2. **Norminette**: Small functions, frequent checks
3. **Complexity**: Comprehensive unit tests, visual verification
4. **Integration**: Backward compatibility, feature flags

## Success Metrics

1. ✅ All unit tests pass
2. ✅ Norminette score 100%
3. ✅ Performance targets met
4. ✅ Visual quality improved
5. ✅ No memory leaks
6. ✅ Documentation complete
7. ✅ Backward compatible
