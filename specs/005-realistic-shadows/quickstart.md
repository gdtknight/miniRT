# Quickstart Guide: Realistic Shadow Rendering

**Feature**: 005-realistic-shadows
**Date**: 2025-12-19
**Audience**: Developers and testers

## Overview

This guide explains how to use, test, and verify the realistic
shadow rendering features: soft shadows with stratified sampling,
adaptive shadow bias, and distance-based shadow attenuation.

---

## Soft Shadows

### What It Does

Instead of casting a single shadow ray (producing hard, sharp shadow
edges), the system casts 16 shadow rays with slightly offset target
positions. The ratio of blocked rays determines shadow intensity,
producing smooth, realistic shadow edges with penumbra-like
transitions.

### How It Works

```
Hard Shadow (1 sample):        Soft Shadow (16 samples):
    Light •                        Light •
          |                         /|\
          |                        / | \  (16 rays spread)
          ▼                       ▼  ▼  ▼
   ████████████                ▓▓████████▓▓
   Sharp edge                 Smooth gradient
```

### Visual Verification

1. Open any scene with objects casting shadows on a plane
2. Observe shadow edges: they should show a smooth gradient
   from dark (umbra) to light (penumbra)
3. Compare with hard shadows: shadows should be visibly softer

**Good indicators**:
- Smooth shadow edges without visible stepping
- Shadow intensity varies from fully dark to fully lit
- No shadow acne (dark speckles on lit surfaces)

**Bad indicators**:
- Noisy, speckled shadow edges (too few samples)
- Shadow acne (dark dots on surfaces facing light)
- Shadows detached from objects ("peter-panning")

---

## Shadow Configuration

### Default Settings

| Parameter | Value | Effect |
|-----------|-------|--------|
| `samples` | 16 | Number of shadow rays per pixel |
| `softness` | 0.3 | Shadow edge spread radius |
| `bias_scale` | 2.0 | Adaptive bias multiplier |
| `enable_ao` | 0 | Ambient occlusion (not implemented) |

### Adjusting Shadow Quality

Shadow configuration is set in `init_shadow_config()` in
`src/lighting/shadow_config.c`. To modify:

**Faster rendering (fewer samples)**:
```c
config.samples = 4;     // 4x faster shadow rendering
config.softness = 0.2;  // Tighter shadow edges
```

**Higher quality (more samples)**:
```c
config.samples = 32;    // Smoother edges, 2x slower
config.softness = 0.5;  // Wider, softer shadows
```

---

## Adaptive Shadow Bias

### What It Does

Prevents shadow acne by adjusting the shadow ray origin offset
based on the angle between the surface normal and light direction.
At grazing angles (where acne is most likely), the bias is
automatically increased.

### How to Test

1. Create a scene with a large plane and a light at a low angle
2. Without adaptive bias: dark speckles appear on the plane surface
3. With adaptive bias: plane surface is clean

**Test scene**:
```
A  0.1                    255,255,255
C  0,5,30    0,-0.1,-1    70
L  100,50,0               0.8          255,255,255
pl 0,0,0     0,1,0                     200,200,200
sp 0,3,0                  6            255,0,0
```

The plane should show a clean shadow from the sphere with no
acne artifacts, even at the far edges where light hits at
grazing angles.

---

## Shadow Attenuation

### What It Does

Reduces shadow intensity based on distance from the light source.
Objects closer to the light cast stronger shadows; distant objects
cast softer, more diffuse shadows.

### Formula

```
attenuation = 1 / (1 + (distance/max_distance)^2 * softness)
```

### Visual Effect

- Near light: Sharp, dark shadows
- Far from light: Softer, lighter shadows
- Very far: Shadows nearly invisible

---

## Performance Impact

### Rendering Time Comparison

| Mode | Shadow Rays/Pixel | Relative Cost |
|------|------------------|---------------|
| Hard shadows (1 sample) | 1 | 1x (baseline) |
| Soft shadows (4 samples) | 4 | ~4x |
| Soft shadows (16 samples) | 16 | ~16x |
| Soft shadows (32 samples) | 32 | ~32x |

**Note**: Shadow rays use brute-force intersection (not BVH).
For scenes with many objects, shadow calculation dominates
total render time.

### Performance Tips

- Use fewer samples (4-9) for scenes with many objects (>20)
- Reduce softness for tighter, cheaper shadows
- During interactive mode, adaptive quality may reduce sample count
- BVH accelerates primary rays but NOT shadow rays

---

## File Structure

```
src/lighting/
├── shadow_config.c        # Configuration initialization
├── shadow_calc.c          # Soft shadow calculation core
├── shadow_test.c          # Shadow ray casting
├── shadow_attenuation.c   # Distance-based attenuation
└── lighting.c             # Main lighting (uses shadow_factor)

includes/
└── shadow.h               # Shadow types and function prototypes
```

---

## Testing Checklist

### Visual Tests

- [ ] Sphere on plane: soft shadow edges visible
- [ ] Multiple objects: overlapping shadows blend correctly
- [ ] Grazing angle light: no shadow acne on plane
- [ ] Camera behind object: shadow visible on far surface
- [ ] No objects between light and surface: fully lit (no false shadows)

### Regression Tests

- [ ] Hard shadow mode (samples=1): identical to original implementation
- [ ] All existing .rt scenes render without errors
- [ ] No memory leaks (check with `leaks` or valgrind)
- [ ] No crashes on empty scenes or degenerate geometry
- [ ] Norminette compliance on all shadow files

### Performance Tests

- [ ] 16-sample soft shadows: render completes in reasonable time
- [ ] Frame time displayed correctly in HUD metrics
- [ ] No performance regression in non-shadow code paths

---

## Build and Run

```bash
make clean && make
./miniRT scenes/your_scene.rt
```

No special flags needed. Soft shadows are enabled by default with
16 samples. The shadow system is compiled as part of the standard
build.

---

## Troubleshooting

### Shadow acne (dark speckles on surfaces)
- Check `bias_scale` value in shadow_config (default 2.0)
- Increase `bias_scale` if acne persists at extreme angles
- Verify adaptive bias function is called (not hardcoded bias)

### Shadows look too noisy/grainy
- Increase `samples` count (try 32 or 64)
- Reduce `softness` to concentrate samples

### Shadows are too soft/blurry
- Reduce `softness` value (try 0.1)
- Reduce `samples` if individual ray noise is visible

### Performance is too slow
- Reduce `samples` to 4 or 9
- Check if BVH is enabled for primary rays
- Note: shadow rays are always brute-force (known limitation)

### Shadows missing entirely
- Verify light source is positioned correctly in .rt file
- Check ambient lighting ratio (too high ambient washes out shadows)
- Verify objects are between light and receiving surface
