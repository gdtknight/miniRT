# Research: Realistic Shadow Rendering

**Feature**: 005-realistic-shadows
**Date**: 2025-12-19
**Purpose**: Resolve technical unknowns and establish implementation patterns

## Research Tasks

### 1. Shadow Acne Prevention

**Question**: How to prevent shadow acne (self-shadowing artifacts)
on surface-parallel light angles?

**Research Findings**:

**Decision**: Adaptive shadow bias based on surface normal and light
direction angle

**Rationale**:
- Fixed bias (e.g., 0.001) fails at grazing angles where the
  shadow ray origin sits very close to the surface
- Adaptive bias scales with the angle between the surface normal
  and light direction: `bias = base * (1 + (1 - |dot(N, L)|) * scale)`
- At perpendicular incidence (dot=1.0): bias = base * 1.0 (minimal)
- At grazing incidence (dot~0.0): bias = base * 3.0 (maximum)
- This eliminates acne without over-biasing (which causes
  "peter-panning" - shadow detachment from objects)

**Alternatives Considered**:
- Normal offset: Move shadow ray origin along normal instead of
  along light direction. More robust but changes shadow shape
  slightly at edges.
- Dual-sided testing: Cast shadow ray from both sides of surface.
  Doubles shadow ray cost.

---

### 2. Soft Shadow Sampling Strategy

**Question**: How to distribute shadow samples for smooth soft shadows
with minimal sample count?

**Research Findings**:

**Decision**: Stratified sampling on a circular disk around the light
position

**Rationale**:
- Random (Monte Carlo) sampling produces noisy results at low
  sample counts
- Regular grid sampling produces visible banding artifacts
- Stratified sampling divides the sampling domain into strata and
  places one sample per stratum, reducing variance while avoiding
  banding
- Circular distribution matches the physical model of an area light

**Implementation Pattern**:
```c
// Stratified disk sampling
for (i = 0; i < samples; i++)
{
    // Grid position within circular disk
    angle = 2.0 * M_PI * i / samples;
    radius = softness * sqrt((double)i / samples);
    offset.x = cos(angle) * radius;
    offset.y = sin(angle) * radius;
    offset.z = 0;  // Projected onto light plane
}
```

**Sample Count Analysis**:
- 1 sample: Hard shadows (backward compatible)
- 4 samples: Minimal softness, slight noise
- 9 samples: Visible softness, acceptable quality
- 16 samples: Good quality, default choice
- 32+ samples: Diminishing returns, performance cost

**Decision**: Default to 16 samples as balance between quality and
performance.

---

### 3. Shadow Attenuation Model

**Question**: How should shadow softness vary with distance from the
occluder?

**Research Findings**:

**Decision**: Inverse-square attenuation with configurable softness
parameter

**Rationale**:
- Physically, shadow softness increases with distance between
  occluder and receiver (penumbra grows)
- For miniRT, a simpler model suffices: attenuation based on
  distance from light to hit point
- Formula: `atten = 1 / (1 + (d/d_max)^2 * softness)`
- This provides smooth falloff without hard cutoffs

**Physical basis**:
- Real penumbra width = (light_size / distance_to_light) *
  distance_to_receiver
- miniRT uses point lights, so true penumbra is impossible
- The softness parameter simulates area light behavior by
  offsetting shadow ray endpoints

---

### 4. Performance Impact Analysis

**Question**: What is the performance cost of soft shadows?

**Research Findings**:

**Analysis**:
- Hard shadows: 1 shadow ray per pixel per light = N_pixels rays
- Soft shadows (16 samples): 16 shadow rays per pixel per light
- Shadow rays are NOT BVH-accelerated (brute-force O(n))
- For a 800x600 scene with 20 objects and 16 samples:
  - Shadow rays: 800 * 600 * 16 = 7,680,000
  - Intersection tests: 7,680,000 * 20 = 153,600,000
  - This dominates total render time

**Mitigations**:
1. Adaptive quality mode reduces sample count during interaction
2. Sample count is configurable (reduce for complex scenes)
3. Future optimization: BVH-accelerated shadow rays

**Decision**: Accept performance cost with configurable sample count.
Default 16 samples provides good quality/performance balance.

---

### 5. Integration with Existing Lighting

**Question**: How to integrate shadow factor into the existing
`apply_lighting()` pipeline?

**Research Findings**:

**Decision**: Replace binary `in_shadow()` with float-returning
`calculate_shadow_factor()`

**Integration pattern**:
```c
// Before (hard shadows):
if (in_shadow(scene, point, light))
    return (ambient_only);
color = ambient + diffuse + specular;

// After (soft shadows):
shadow = calculate_shadow_factor(scene, point, light, &config);
color = ambient + (1.0 - shadow) * (diffuse + specular);
```

**Key design decisions**:
- Shadow factor 0.0 = fully lit, 1.0 = fully in shadow
- Ambient light is NEVER affected by shadows (per miniRT subject)
- Both diffuse and specular are modulated by shadow factor
- Backward compatible: 1 sample produces same result as binary

---

### 6. Norminette Compliance Strategy

**Question**: How to keep shadow functions within Norminette limits?

**Research Findings**:

**Decision**: Split into 4 source files with helper extraction

**File breakdown**:
- `shadow_config.c`: Configuration init/validation (3-4 functions)
- `shadow_calc.c`: Core calculation + sample offset (4-5 functions)
- `shadow_test.c`: Shadow ray casting (3-4 functions)
- `shadow_attenuation.c`: Distance-based attenuation (2-3 functions)

**Rationale**: Each file stays within the 5-function Norminette limit
while maintaining logical cohesion.

---

## Research Summary

| Question | Decision | Confidence |
|----------|----------|------------|
| Shadow acne | Adaptive bias (angle-based) | High |
| Sampling strategy | Stratified circular disk | High |
| Sample count default | 16 samples | High |
| Attenuation model | Inverse-square with softness | Medium |
| Performance impact | Acceptable with configurability | Medium |
| Lighting integration | Float shadow factor | High |
| Norminette strategy | 4-file split | High |

## Key Risks

1. **Performance**: 16x shadow ray increase is significant; soft
   shadows dominate render time for complex scenes
2. **Shadow ray brute-force**: Not BVH-accelerated; becomes
   bottleneck with many objects
3. **Ambient occlusion**: Deferred (enable_ao flag exists but
   implementation is non-trivial and may not meet performance
   targets)
