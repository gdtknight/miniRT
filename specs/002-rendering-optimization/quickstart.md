# Quickstart Guide: Advanced Rendering Performance Optimization

**Feature**: 002-rendering-optimization
**Date**: 2025-12-19
**Audience**: Developers and testers

## Overview

This guide explains how to use, test, and verify the rendering
optimization features: BVH spatial acceleration, adaptive quality
control, progressive tile rendering, and performance metrics.

---

## BVH Spatial Acceleration

### What It Does

BVH (Bounding Volume Hierarchy) organizes scene objects into a
binary tree of bounding boxes. Instead of testing every ray against
every object (O(n)), rays traverse the tree and skip entire groups
of objects whose bounding boxes are not hit (O(log n)).

### How to Use

BVH is enabled by default after scene loading. Toggle with the
`b` key.

```
Open any .rt scene → BVH builds automatically
Press 'b'         → Toggle BVH on/off
```

### Testing BVH

**Basic verification**:
1. Open a scene with 20+ objects
2. Note render time displayed in HUD
3. Press `b` to disable BVH
4. Note render time increase (expect 10-100x slower)
5. Press `b` again to re-enable

**Visual correctness**:
1. Render scene with BVH enabled, take screenshot
2. Render same scene with BVH disabled
3. Output MUST be pixel-identical

**Edge cases**:
- Scene with 0 objects: should render background without error
- Scene with 1 object: BVH should work (single-node tree)
- Scene with only planes: large bounding boxes should not cause issues

---

## Adaptive Quality Control

### What It Does

Automatically switches between LOW and HIGH quality rendering based
on user interaction. During camera movement or object manipulation,
LOW quality (2x2 pixel blocks) provides faster feedback. After
interaction stops (1 second delay), rendering upgrades to HIGH
quality (per-pixel).

### How to Use

```
Move camera (WASD)  → Quality drops to LOW automatically
Stop moving         → After 1s, re-renders at HIGH quality
Press 'q'           → Toggle adaptive quality mode on/off
```

### Testing Adaptive Quality

1. Open any scene
2. Hold `W` to move camera forward
3. Observe: rendering should be visibly lower quality (blocky)
4. Release `W` and wait 1 second
5. Observe: rendering upgrades to full quality automatically

**Expected behavior in LOW mode**:
- Visible 2x2 pixel blocks
- Faster rendering (~4x fewer rays)
- Frame rate should remain interactive

---

## Performance Metrics

### What It Does

Collects and displays real-time rendering statistics including frame
time, FPS, ray counts, and BVH traversal efficiency.

### How to Use

Metrics are displayed in the HUD overlay when info display is
enabled.

**Displayed metrics**:
- Frame render time (microseconds)
- FPS (rolling average over 60 frames)
- Total rays traced per frame
- BVH nodes visited
- BVH efficiency percentage

### Interpreting Metrics

| Metric | Good | Acceptable | Poor |
|--------|------|------------|------|
| FPS (interactive) | >30 | >10 | <10 |
| FPS (final render) | >1 | >0.5 | <0.5 |
| BVH efficiency | >90% | >70% | <70% |
| Render time (10 obj) | <500ms | <2s | >2s |
| Render time (50 obj) | <3s | <10s | >10s |

---

## Progressive Tile Rendering

### What It Does

Divides the screen into 32x32 pixel tiles and renders them
incrementally, providing visual feedback during rendering.

### Current Status

Infrastructure is in place but not fully integrated into the main
render loop. Tile structures and coordinate generation work
correctly.

---

## Keyboard Controls Summary

| Key | Action |
|-----|--------|
| `b` | Toggle BVH acceleration on/off |
| `q` | Toggle adaptive quality mode |
| `ESC` | Exit program |

---

## Troubleshooting

### Rendering looks blocky
- Adaptive quality is in LOW mode
- Wait 1 second after stopping interaction, or press `q` to
  disable adaptive mode

### Scene renders slowly despite BVH
- Check if BVH is enabled (press `b`)
- Shadow rays are not BVH-accelerated (known limitation)
- Soft shadows with many samples dominate render time

### BVH metrics show low efficiency
- Scenes with few objects (<5) show low efficiency because
  BVH overhead exceeds brute-force for small object counts
- Planes have very large bounding boxes that reduce BVH
  selectivity

### Memory usage seems high
- BVH tree uses approximately 2n nodes for n objects
  (~80 bytes per node)
- For typical scenes (<100 objects), BVH memory is <16KB

---

## Build and Run

```bash
make clean && make
./miniRT scenes/your_scene.rt
```

No special build flags required. BVH and metrics are compiled
by default.

---

## Performance Expectations

| Scene Complexity | Without BVH | With BVH | Speedup |
|-----------------|-------------|----------|---------|
| 5 objects | ~0.5s | ~0.5s | ~1x |
| 20 objects | ~2s | ~0.2s | ~10x |
| 50 objects | ~10s | ~0.3s | ~30x |
| 100 objects | ~40s+ | ~0.5s | ~80x+ |

*Times approximate for 800x600 window on modern hardware,
HIGH quality mode, single light source.*
