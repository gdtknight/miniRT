# Feature Specification: Realistic Shadow Rendering

## Overview
Enhance shadow realism in miniRT raytracer by implementing soft shadows, shadow attenuation, and improved shadow interaction while maintaining 42 norminette compliance.

## GitHub Issue
- **Issue**: [#4](https://github.com/gdtknight/miniRT_final/issues/4)
- **Branch**: 002-realistic-shadows

## Current State Analysis

### Existing Implementation (src/lighting/lighting.c)
- **Hard shadows**: Binary shadow detection (in shadow or not)
- **Single point light**: Only one light source supported
- **Basic shadow bias**: Fixed 0.001 offset to prevent self-intersection
- **Simple shadow ray**: Single ray cast to light source
- **No attenuation**: Shadows are either full or absent

### Limitations
1. Unrealistic sharp shadow edges
2. No penumbra (soft shadow region)
3. No distance-based shadow softness
4. Shadow acne still possible in edge cases
5. No ambient occlusion effects

## Requirements

### Functional Requirements
1. **Soft Shadows**
   - Implement shadow ray sampling for smoother edges
   - Configurable shadow softness parameter
   - Maintain performance within acceptable limits

2. **Shadow Attenuation**
   - Distance-based shadow softness
   - Gradual transition from umbra to penumbra
   - Physically-based falloff model

3. **Improved Shadow Bias**
   - Adaptive bias based on surface angle
   - Prevent shadow acne on grazing angles
   - Maintain shadow accuracy

4. **Multiple Light Source Support** (if time permits)
   - Shadow contribution from each light
   - Combined shadow effects
   - Proper light interaction

5. **Ambient Occlusion** (optional, if feasible)
   - Simple AO approximation
   - Enhance depth perception
   - Must be fast enough for real-time

### Non-Functional Requirements
1. **42 Norminette Compliance**: All code must pass norminette
2. **Unit Testing**: All functions must have unit tests
3. **Documentation**: English comments + Korean docs
4. **Performance**: Maintain interactive frame rates
5. **Backward Compatibility**: Existing scene files must work

## Technical Design

### New Data Structures
```c
typedef struct s_shadow_config
{
    int     samples;        // Number of shadow samples (1 = hard shadows)
    double  softness;       // Shadow softness factor (0.0 - 1.0)
    double  bias_scale;     // Shadow bias multiplier
    int     enable_ao;      // Enable ambient occlusion
}   t_shadow_config;
```

### New Functions

1. **calculate_soft_shadow()**
   - Cast multiple shadow rays with slight offsets
   - Return shadow intensity (0.0 = no shadow, 1.0 = full shadow)
   - Support configurable sample count

2. **calculate_shadow_bias()**
   - Compute adaptive bias based on normal and light direction
   - Prevent shadow acne
   - Return appropriate offset value

3. **calculate_shadow_attenuation()**
   - Compute distance-based shadow softness
   - Return attenuation factor
   - Use physically-based falloff

4. **calculate_ambient_occlusion()** (optional)
   - Cast rays in hemisphere around hit point
   - Approximate occlusion from nearby geometry
   - Return AO factor

### Modified Functions

1. **in_shadow()** → **calculate_shadow_factor()**
   - Return float instead of binary int
   - Support soft shadows
   - Use adaptive bias

2. **apply_lighting()**
   - Use shadow factor instead of binary shadow
   - Support shadow attenuation
   - Integrate AO if enabled

## Implementation Plan

### Phase 1: Foundation (Priority: HIGH)
1. Add shadow configuration structure
2. Implement adaptive shadow bias
3. Refactor in_shadow() to return shadow factor
4. Update apply_lighting() to use shadow factor
5. Unit tests for bias calculation

### Phase 2: Soft Shadows (Priority: HIGH)
1. Implement shadow ray sampling
2. Add soft shadow calculation
3. Add configuration parameter to scene
4. Unit tests for soft shadows
5. Test with various sample counts

### Phase 3: Shadow Attenuation (Priority: MEDIUM)
1. Implement distance-based attenuation
2. Add falloff calculation
3. Integrate with shadow factor
4. Unit tests for attenuation
5. Visual verification

### Phase 4: Ambient Occlusion (Priority: LOW, Optional)
1. Implement simple AO algorithm
2. Add AO configuration
3. Integrate with lighting
4. Performance testing
5. Unit tests

### Phase 5: Polish (Priority: MEDIUM)
1. Create test scenes
2. Performance optimization
3. Documentation (English + Korean)
4. Norminette verification
5. Final testing

## Testing Strategy

### Unit Tests
1. Adaptive bias calculation
2. Shadow factor computation
3. Soft shadow sampling
4. Attenuation calculation
5. Edge cases (grazing angles, etc.)

### Integration Tests
1. Shadow rendering with various scene configurations
2. Performance benchmarks
3. Comparison with reference images
4. Memory leak checks (valgrind)

### Test Scenes
1. Simple sphere with soft shadows
2. Multiple objects with shadow interaction
3. Various light distances
4. Grazing angle shadows
5. Complex scene with all features

## Acceptance Criteria

1. ✅ All code passes norminette
2. ✅ All unit tests pass
3. ✅ Soft shadows visible in test scenes
4. ✅ No shadow acne artifacts
5. ✅ Performance: >10 FPS on standard scenes
6. ✅ Documentation complete (English + Korean)
7. ✅ Build-run-verify cycle passes
8. ✅ Backward compatible with existing scenes

## Risks and Mitigations

| Risk | Impact | Mitigation |
|------|--------|-----------|
| Performance degradation | HIGH | Configurable sample count, optimize ray casting |
| Norminette violations | HIGH | Frequent norminette checks, small functions |
| Complex math causing errors | MEDIUM | Unit tests, visual verification |
| Shadow acne persists | MEDIUM | Adaptive bias, thorough testing |
| AO too slow | LOW | Make it optional, simple algorithm |

## Timeline Estimate
- Phase 1: 2-3 hours
- Phase 2: 3-4 hours
- Phase 3: 2-3 hours
- Phase 4: 3-4 hours (optional)
- Phase 5: 2-3 hours
- **Total**: 12-17 hours (without AO: 9-13 hours)

## References
- Current implementation: src/lighting/lighting.c
- Constitution v1.1.0
- Ray Tracing in One Weekend (Peter Shirley)
- PBRT (Physically Based Rendering Techniques)
