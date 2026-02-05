# Feature Spec: BVH Miss Fallback Removal (P0)

## Overview

`trace_ray()` currently falls back to brute-force `check_all_objects()` when `bvh_intersect()` returns no hit. Since the BVH is built from `scene->objects.count` (all objects), this fallback is redundant — it doubles intersection tests on miss rays and distorts BVH skip rate metrics.

## Problem

```c
// trace.c — current logic
if (scene->bvh && scene->bvh->enabled)
{
    if (bvh_intersect(scene->bvh, *ray, &hit, scene))
        found = 1;
}
if (!found)
{
    if (check_all_objects(scene, ray, &hit))  // redundant
        found = 1;
}
```

When BVH is enabled and a ray misses all objects, both `bvh_intersect()` and `check_all_objects()` execute, resulting in:
- 2x intersection tests per miss ray
- `intersect_tests` counter inflation
- BVH skip rate becoming uninterpretable (fallback adds tests outside BVH path)

## Requirements

### Functional
1. When BVH is enabled **and valid** (`bvh->root != NULL`), `trace_ray()` must use only `bvh_intersect()` — no brute-force fallback
2. When BVH is disabled, null, or enabled but invalid (`root == NULL`), `check_all_objects()` must still work as before
3. Rendering output must be pixel-identical to current behavior (BVH covers all objects)

### Edge Case: BVH enabled but root null
`bvh_create()` sets `enabled = 1` immediately. If `bvh_build_recursive()` fails (e.g. malloc failure), `root` remains NULL while `enabled` stays 1. The BVH-on condition must check `scene->bvh->root` in addition to `enabled` to avoid all-black rendering in this edge case.

### Verification
1. `bvh_build()` receives `scene->objects.count` — confirm in `scene_build_bvh()` 함수 내부에서 `total = scene->objects.count` 할당 확인
2. All 4 test scenarios (S1–S4) must render identically before/after the change
3. S2–S4 각 씬에서 primary `intersect_tests`가 baseline 대비 감소해야 함. 이중 탐색 제거이므로 miss ray 비율에 비례하여 감소가 관측되어야 하며, S2–S4 중 최소 1개 씬에서 10% 이상 감소를 기대. Note: `intersect_tests`는 오브젝트 교차 테스트만 카운트 (AABB 박스 테스트는 `nodes_visited`로 별도 추적)

### Constraints
- 42 Norm v4.1 compliance (25 lines/function, 5 vars, etc.)
- Code change: `src/render/trace.c` only
- Benchmark measurement: `docs/benchmark-results.md` (documentation, not code)
- `check_all_objects()` becomes BVH-invalid/off-only path

## Scope

| Item | In Scope | Notes |
|------|----------|-------|
| Remove fallback in trace_ray() | Yes | Core code change |
| Restructure if/else logic | Yes | BVH valid → bvh_intersect only; otherwise → check_all_objects |
| Benchmark re-measurement | Yes | Documentation (`docs/benchmark-results.md`), not code |
| Shadow path changes | No | Separate optimization (P1) |
| BVH algorithm changes | No | Separate optimizations (P5, P6) |

## Success Criteria

- `make re` compiles without warnings
- `norminette src/render/trace.c` passes
- S1–S4 scenes render correctly
- S2–S4 중 최소 1개 씬에서 primary `intersect_tests`가 baseline 대비 10% 이상 감소
- BVH enabled + root null 상태에서 check_all_objects() 폴백이 동작
