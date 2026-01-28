# Data Model: Realistic Shadow Rendering

**Feature**: 005-realistic-shadows
**Date**: 2025-12-19
**Status**: Implemented

## Entity Overview

The shadow rendering feature introduces three primary entities:

1. **Shadow Configuration** - Runtime shadow behavior settings
2. **Shadow Sample** - Per-calculation context for shadow ray casting
3. **Shadow Factor** - Result of shadow evaluation (float 0.0-1.0)

## Entity Definitions

### 1. Shadow Configuration (`t_shadow_config`)

**Purpose**: Store runtime configuration for shadow rendering
behavior. Embedded in `t_scene` for global access.

**Fields**:
```c
typedef struct s_shadow_config
{
    int     samples;        // Shadow ray count (1=hard, >1=soft)
    double  softness;       // Edge softness factor (0.0-1.0)
    double  bias_scale;     // Shadow bias multiplier
    int     enable_ao;      // Ambient occlusion flag (reserved)
}   t_shadow_config;
```

**Field Descriptions**:
- `samples`: Number of shadow rays cast per pixel per light.
  1 = hard shadows (backward compatible), 16 = default soft.
  Higher values produce smoother shadows at linear performance cost.
- `softness`: Controls the radius of shadow ray offset distribution.
  0.0 = all rays converge to point (hard), 1.0 = maximum spread.
  Default 0.3.
- `bias_scale`: Multiplier applied to adaptive bias calculation.
  Higher values push shadow ray origins further from surfaces.
  Default 2.0.
- `enable_ao`: Reserved for future ambient occlusion. Currently
  always 0 (not implemented).

**Default Values** (from `init_shadow_config()`):
```c
config.samples = 16;
config.softness = 0.3;
config.bias_scale = 2.0;
config.enable_ao = 0;
```

**Validation Rules**:
- `samples >= 1` (cannot be zero)
- `0.0 <= softness <= 1.0`
- `bias_scale > 0.0`

---

### 2. Shadow Sample Context (`t_shadow_sample`)

**Purpose**: Bundle parameters for a single shadow evaluation call.
Used internally to pass context through helper functions while
staying within Norminette's 4-parameter limit.

**Fields**:
```c
typedef struct s_shadow_sample
{
    t_scene         *scene;     // Scene for intersection tests
    t_vec3          point;      // Surface hit point
    t_vec3          light_pos;  // Light source position
    t_shadow_config *config;    // Shadow configuration
    double          bias;       // Computed adaptive bias
}   t_shadow_sample;
```

**Lifecycle**: Stack-allocated per shadow evaluation. No heap
allocation required.

---

## Function Signatures

### shadow_config.c

```c
// Initialize shadow configuration with default values
t_shadow_config init_shadow_config(void);

// Validate configuration parameters
int             validate_shadow_config(t_shadow_config *config);
```

### shadow_calc.c

```c
// Main entry point: compute shadow factor for a surface point
// Returns 0.0 (fully lit) to 1.0 (fully in shadow)
double  calculate_shadow_factor(t_scene *scene, t_vec3 point,
            t_vec3 light_pos, t_shadow_config *config);

// Compute adaptive bias from surface normal and light direction
// Larger bias at grazing angles to prevent shadow acne
double  calculate_shadow_bias(t_vec3 normal, t_vec3 light_dir,
            double base_bias);

// Generate offset vector for stratified shadow sampling
// Returns position on circular disk of given radius
t_vec3  generate_shadow_sample_offset(double radius,
            int sample_index, int total_samples);
```

### shadow_test.c

```c
// Cast single shadow ray and test for occlusion
// Returns 1 if point is in shadow, 0 if lit
int     is_in_shadow(t_scene *scene, t_vec3 point,
            t_vec3 light_pos, double bias);
```

### shadow_attenuation.c

```c
// Calculate distance-based shadow attenuation
// Returns 0.0 (no attenuation) to 1.0 (full attenuation)
double  calculate_shadow_attenuation(double distance,
            double max_distance, double softness);
```

---

## Data Flow

```
Hit Point (from ray-object intersection)
    │
    ├── Normal vector (N)
    ├── Light direction (L)
    │
    ▼
calculate_shadow_bias(N, L, 0.001)
    │
    ▼ bias
    │
calculate_shadow_factor(scene, point, light_pos, config)
    │
    ├── Loop: i = 0..config.samples
    │   ├── generate_shadow_sample_offset(softness, i, samples)
    │   │       → offset vector
    │   ├── offset_light = light_pos + offset
    │   └── is_in_shadow(scene, point, offset_light, bias)
    │           → 0 or 1
    │
    ▼ shadow_count / samples → shadow_factor [0.0, 1.0]
    │
apply_lighting()
    │
    ├── ambient (unaffected by shadow)
    ├── diffuse * (1.0 - shadow_factor)
    └── specular * (1.0 - shadow_factor)
```

## Integration Points

### Scene Structure (`t_scene` in `minirt.h`)

```c
typedef struct s_scene
{
    ...
    t_shadow_config shadow_config;  // Added by this feature
    ...
}   t_scene;
```

Shadow config is initialized during scene setup and remains
constant during rendering (no per-frame allocation).

### Lighting Pipeline (`lighting.c`)

The `apply_lighting()` function calls `calculate_shadow_factor()`
instead of the original binary `in_shadow()`, using the returned
float to modulate diffuse and specular contributions.

## Memory Lifecycle

| Entity | Allocation | Deallocation |
|--------|-----------|--------------|
| `t_shadow_config` | Stack (in `t_scene`) | No heap |
| `t_shadow_sample` | Stack (per call) | No heap |
| Shadow rays | Stack (local vars) | No heap |

No heap allocations are introduced by the shadow system. All
data is stack-allocated or embedded in existing structures.
