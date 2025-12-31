# Quickstart: BVH Performance Verification and Debugging Tools

**Feature**: BVH Performance Diagnostics  
**Branch**: `008-bvh-performance-diagnostics`  
**Audience**: Developers and testers validating BVH effectiveness

---

## Overview

This feature adds real-time performance monitoring to miniRT, allowing you to:
- See immediate FPS and frame time feedback
- Verify BVH acceleration is working correctly
- Compare BVH-enabled vs disabled rendering performance
- Diagnose spatial culling effectiveness
- Validate BVH tree quality

**Time to First Diagnostic**: < 30 seconds from launch

---

## Quick Start (3 Steps)

### 1. Launch miniRT with a Test Scene

```bash
cd /path/to/miniRT
./miniRT scenes/complex_scene.rt
```

**What you'll see**: Application window opens with rendered scene and HUD on the left side.

---

### 2. View Performance Metrics

Performance section appears **automatically** at the bottom of the HUD:

```
=== Performance ===
FPS: 58.3
Frame: 16.8ms
BVH: ON
```

**Interpretation**:
- **FPS: 58.3** - Rendering at ~58 frames per second (near 60 FPS target)
- **Frame: 16.8ms** - Each frame takes 16.8 milliseconds (under 16ms budget)
- **BVH: ON** - Spatial acceleration is active

---

### 3. Toggle BVH to Compare Performance

Press **`B`** key to disable BVH:

```
=== Performance ===
FPS: 5.2
Frame: 192.3ms
BVH: OFF
```

**Result**: Without BVH, FPS drops dramatically (58.3 → 5.2 FPS) confirming BVH provides ~11x speedup.

Press **`B`** again to re-enable BVH and verify FPS returns to ~60.

**✅ Success**: You've validated BVH is working correctly!

---

## Key Bindings

| Key | Action | Description |
|-----|--------|-------------|
| **H** | Toggle HUD | Show/hide entire HUD (including performance section) |
| **I** | Toggle Info | Show/hide detailed BVH statistics |
| **B** | Toggle BVH | Enable/disable BVH acceleration |
| **ESC** | Quit | Exit application |

---

## Display Modes

### Basic Mode (Default)

Shows essential performance metrics only:

```
=== Performance ===
FPS: 58.3              ← Frames per second (smoothed over 1 second)
Frame: 16.8ms          ← Frame rendering time in milliseconds
BVH: ON                ← BVH acceleration status
```

**When to use**: Normal operation, quick performance check

---

### Detailed Mode (Press `I`)

Shows comprehensive BVH diagnostics:

```
=== Performance ===
FPS: 58.3
Frame: 16.8ms
BVH: ON

[I] BVH Details:
  Ray-Box: 45,231      ← Ray-AABB intersection tests
  Ray-Obj: 12,960      ← Ray-object intersection tests
  Nodes Visit: 8,542   ← BVH nodes traversed
  Nodes Skip: 36,689   ← BVH branches culled (spatial pruning)
  Avg Tests/Ray: 10.0  ← Average object tests per ray
  Efficiency: 90.0%    ← Percentage reduction in tests (vs naive)

Tree Stats:            ← BVH structure quality
  Nodes: 127           ← Total BVH nodes (internal + leaf)
  Depth: 7             ← Maximum tree depth
  Avg Objs/Leaf: 2.3   ← Average objects per leaf node
```

**When to use**: Diagnosing BVH effectiveness, optimization tuning

---

## Interpreting Metrics

### FPS (Frames Per Second)

**What it measures**: How many frames rendered per second (smoothed over 60 frames)

**Target values**:
- **60 FPS**: Ideal (hitting frame rate cap)
- **30-60 FPS**: Acceptable (interactive)
- **<30 FPS**: Poor (sluggish experience)

**What affects it**:
- Scene complexity (object count)
- BVH status (ON = faster, OFF = slower)
- Render quality settings

**Example**:
```
Complex scene (100 objects):
  BVH ON:  58.3 FPS  ✓ Good
  BVH OFF:  5.2 FPS  ✗ Bad
```

---

### Frame Time (Milliseconds)

**What it measures**: How long one frame takes to render

**Target values**:
- **<16ms**: Hitting 60 FPS target
- **16-33ms**: 30-60 FPS range
- **>33ms**: Below 30 FPS

**What affects it**:
- Inversely related to FPS (frame_time = 1000ms / FPS)
- Scene complexity
- BVH effectiveness

**Example**:
```
Frame: 16.8ms → FPS = 1000 / 16.8 = 59.5 FPS
```

---

### BVH Status (ON/OFF)

**What it shows**: Whether BVH spatial acceleration is active

**Expected behavior**:
- **ON**: Fast rendering (BVH traversal active)
- **OFF**: Slow rendering (naive intersection, all objects tested)

**How to toggle**: Press `B` key

**Use case**: Compare performance with/without BVH to validate effectiveness

---

### Ray-Box Tests (Detailed Mode)

**What it measures**: Number of ray-AABB intersection tests performed this frame

**Typical values**:
- Simple scenes: ~10,000 - 50,000
- Complex scenes: ~50,000 - 500,000

**Interpretation**: Higher values indicate more BVH traversal work (expected for complex scenes)

---

### Ray-Obj Tests (Detailed Mode)

**What it measures**: Number of ray-object intersection tests performed this frame

**Expected behavior**:
- **With BVH**: Significantly lower than naive approach (rays × objects)
- **Without BVH**: Equals rays × objects (every ray tests every object)

**Example**:
```
Scene: 100 objects, 1,296,000 rays (1440×900 resolution)

Naive approach (BVH OFF):
  Ray-Obj: 129,600,000  (1,296,000 × 100)

BVH approach (BVH ON):
  Ray-Obj: 12,960,000   (10x reduction!)
```

---

### Nodes Visited/Skipped (Detailed Mode)

**What it measures**:
- **Nodes Visit**: BVH nodes traversed during ray tracing
- **Nodes Skip**: BVH branches culled (spatial pruning)

**Interpretation**:
- Higher "Nodes Skip" = Better spatial culling
- Ratio "Nodes Skip / (Nodes Visit + Nodes Skip)" should be >80%

**Example**:
```
Nodes Visit: 8,542
Nodes Skip: 36,689

Culling ratio = 36,689 / (8,542 + 36,689) = 81.1% ✓ Good
```

---

### Avg Tests/Ray (Detailed Mode)

**What it measures**: Average number of object intersection tests per ray

**Expected values**:
- **Without BVH**: Equals object count (e.g., 100 tests/ray for 100 objects)
- **With BVH**: Much lower (e.g., 10-20 tests/ray for 100 objects)

**Interpretation**: Lower is better (more effective spatial culling)

**Example**:
```
Scene: 100 objects

BVH OFF: Avg Tests/Ray = 100.0  (tests every object)
BVH ON:  Avg Tests/Ray = 10.0   (90% reduction!)
```

---

### Efficiency Percentage (Detailed Mode)

**What it measures**: Percentage reduction in intersection tests compared to naive approach

**Formula**: `Efficiency = (1 - actual_tests / naive_tests) × 100%`

**Expected values**:
- **Simple scenes (5-10 objects)**: 30-50% (BVH overhead may hurt)
- **Medium scenes (10-50 objects)**: 60-80%
- **Complex scenes (50+ objects)**: 80-95% (BVH shines)

**Interpretation**:
- **>80%**: Excellent (BVH is highly effective)
- **50-80%**: Good (BVH provides benefit)
- **<50%**: Poor (consider disabling BVH for this scene)

**Example**:
```
Efficiency: 90.0%  →  BVH reduced tests by 90% (10x speedup)
```

---

### Tree Stats (Detailed Mode)

**BVH Tree Quality Indicators**:

#### Nodes
- Total count of BVH nodes (internal + leaf)
- **Expected**: Roughly 2× object count for balanced tree
- **Example**: 100 objects → ~200 nodes

#### Depth
- Maximum tree depth (root = 0)
- **Expected**: Roughly log₂(object_count) for balanced tree
- **Example**: 100 objects → depth ~7 (2⁷ = 128)
- **Warning**: Depth >15 indicates unbalanced tree (poor partitioning)

#### Avg Objs/Leaf
- Average number of objects per leaf node
- **Expected**: 1-4 objects per leaf (good granularity)
- **Warning**: >8 objects/leaf → poor spatial subdivision

---

## Common Scenarios

### Scenario 1: Validating BVH Works

**Goal**: Confirm BVH improves performance

**Steps**:
1. Launch miniRT with complex scene (50+ objects)
2. Note FPS with BVH ON (should be ~60 FPS)
3. Press `B` to disable BVH
4. Observe FPS drop dramatically (5-10 FPS)
5. Press `B` to re-enable BVH
6. Observe FPS return to ~60 FPS

**Expected Result**: 5-10x speedup with BVH enabled

---

### Scenario 2: Diagnosing Poor Performance

**Goal**: Identify why rendering is slow

**Steps**:
1. Press `I` to enable detailed mode
2. Check **Efficiency** percentage:
   - If <50%: BVH may not be helping this scene
   - If >80%: BVH is working, issue is elsewhere
3. Check **Avg Tests/Ray**:
   - If close to object count: BVH not culling effectively
   - If much lower than object count: BVH is working
4. Check **Tree Stats → Depth**:
   - If >15: Tree is unbalanced, consider rebuilding

**Action**: If BVH efficiency is low, try pressing `B` to disable BVH (simple scenes may be faster without it)

---

### Scenario 3: Comparing Scene Complexity

**Goal**: Understand how object count affects performance

**Test scenes**:
- `scenes/simple_scene.rt` (5 objects)
- `scenes/medium_scene.rt` (25 objects)
- `scenes/complex_scene.rt` (100 objects)

**Observations**:
```
Simple (5 objects):
  BVH OFF: 60.0 FPS, Avg Tests/Ray: 5.0
  BVH ON:  45.0 FPS, Avg Tests/Ray: 8.0  ← BVH overhead hurts!

Medium (25 objects):
  BVH OFF: 30.0 FPS, Avg Tests/Ray: 25.0
  BVH ON:  58.0 FPS, Avg Tests/Ray: 12.0  ← BVH helps (2x speedup)

Complex (100 objects):
  BVH OFF:  5.2 FPS, Avg Tests/Ray: 100.0
  BVH ON:  58.3 FPS, Avg Tests/Ray: 10.0   ← BVH critical (11x speedup)
```

**Conclusion**: BVH is essential for scenes with 25+ objects, but may hurt performance on very simple scenes.

---

## Terminal Output

### BVH Build Statistics

When scene loads, terminal displays BVH construction info:

```
BVH Build Complete:
  Total Nodes: 127
  Max Depth: 7
  Avg Objects/Leaf: 2.3
  Build Time: 12.45ms
```

**Interpretation**:
- **Total Nodes**: How large the BVH tree is (memory footprint)
- **Max Depth**: Tree balance quality (lower is better, aim for log₂(object_count))
- **Avg Objects/Leaf**: Leaf node granularity (1-4 is ideal)
- **Build Time**: One-time construction cost (amortized over all frames)

---

### Performance Warnings (Future)

If BVH causes performance degradation:

```
WARNING: BVH overhead detected in scene with 5 objects.
Consider disabling BVH ('B' key) for better performance.
```

**When this appears**: Simple scenes where BVH traversal overhead exceeds benefits

**Action**: Press `B` to disable BVH and verify FPS improves

---

## Troubleshooting

### Issue: HUD Not Visible

**Symptoms**: Performance metrics not showing

**Solution**: Press `H` key to toggle HUD visibility

---

### Issue: Detailed Stats Not Showing

**Symptoms**: Only FPS, Frame, BVH status visible

**Solution**: Press `I` key to toggle detailed info mode

---

### Issue: FPS Shows 0.0

**Symptoms**: FPS displays "0.0"

**Causes**:
- First few frames (buffer not filled yet) - Wait ~1 second
- Extremely slow rendering (>1000ms per frame) - Check scene complexity

**Solution**: Wait a few frames for rolling average to stabilize

---

### Issue: Metrics Don't Update

**Symptoms**: Performance numbers stay static

**Causes**:
- Application paused
- Rendering frozen

**Solution**: Move camera or interact with scene to trigger re-render

---

### Issue: BVH Toggle Doesn't Work

**Symptoms**: Pressing `B` doesn't change BVH status

**Solution**: Check terminal for error messages, verify BVH was built successfully

---

## Performance Expectations

### Simple Scenes (5-10 objects)

**BVH OFF**:
- FPS: 60+ (hitting frame cap)
- Frame: <16ms
- Avg Tests/Ray: 5-10

**BVH ON**:
- FPS: 45-60 (may be slower due to overhead)
- Frame: 16-22ms
- Avg Tests/Ray: 8-15 (overhead from traversal)

**Recommendation**: Disable BVH (`B` key) for best performance

---

### Medium Scenes (10-50 objects)

**BVH OFF**:
- FPS: 20-40
- Frame: 25-50ms
- Avg Tests/Ray: 10-50

**BVH ON**:
- FPS: 50-60
- Frame: 16-20ms
- Avg Tests/Ray: 10-20
- Efficiency: 60-80%

**Recommendation**: Enable BVH for 2-3x speedup

---

### Complex Scenes (50+ objects)

**BVH OFF**:
- FPS: 2-10 (unusable)
- Frame: 100-500ms
- Avg Tests/Ray: 50-100+

**BVH ON**:
- FPS: 50-60
- Frame: 16-20ms
- Avg Tests/Ray: 10-20
- Efficiency: 80-95%

**Recommendation**: BVH is essential (5-20x speedup)

---

## Next Steps

### For Developers

1. **Read `data-model.md`**: Understand metrics structures and counters
2. **Read `contracts/metrics-api.md`**: Learn metrics collection API
3. **Read `contracts/hud-api.md`**: Learn performance display implementation
4. **Review `research.md`**: Deep dive into design decisions

### For Testers

1. **Run integration tests**: `./test_metrics_overhead.sh`
2. **Validate BVH effectiveness**: `./test_bvh_effectiveness.sh`
3. **Test across platforms**: Verify on both macOS and Linux
4. **Report issues**: Document unexpected performance patterns

### For Users

1. **Monitor FPS**: Keep an eye on frame rate during interaction
2. **Toggle BVH**: Experiment with `B` key to find optimal setting for your scenes
3. **Use detailed mode**: Press `I` when diagnosing performance issues
4. **Share feedback**: Report scenes where BVH doesn't help (or hurts)

---

## Summary

✅ **Launch** miniRT → See performance metrics automatically  
✅ **Press `B`** → Compare BVH ON vs OFF  
✅ **Press `I`** → View detailed BVH statistics  
✅ **Interpret** Efficiency >80% = BVH working well  
✅ **Troubleshoot** Press `H` to show/hide HUD  

**Time invested**: 5 minutes to understand, 30 seconds to validate BVH is working.

**Key insight**: BVH should provide 5-20x speedup on complex scenes (50+ objects).
