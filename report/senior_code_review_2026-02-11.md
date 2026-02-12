# miniRT Senior Code Review Report (v5)

- Date: 2026-02-11 (v5 — exhaustive full-codebase review)
- Branch: `034-full-features`
- Scope: 99 source files, 24 headers, 11,281 LOC (full codebase)
- Reviewer perspective: 10-year senior C engineer
- Method: Line-by-line analysis across 4 domains + automated 4-agent parallel review with manual verification
- Note: v2/v3/v4 findings re-verified. New findings marked with [v5]. False positives documented in Appendix A.

---

## Table of Contents

1. [Actual Bugs](#1-actual-bugs)
2. [Numerical Stability](#2-numerical-stability)
3. [Defensive Programming Gaps](#3-defensive-programming-gaps)
4. [Memory Safety](#4-memory-safety)
5. [Architecture & Design](#5-architecture--design)
6. [Intersection & Rendering Correctness](#6-intersection--rendering-correctness)
7. [Build & Tooling](#7-build--tooling)
8. [Positive Assessment](#8-positive-assessment)
9. [Fix Priority Matrix](#9-fix-priority-matrix)
10. [Overall Verdict](#10-overall-verdict)
A. [False Positives Dismissed](#appendix-a-false-positives-dismissed-v5)

---

## 1. Actual Bugs

### 1.1 [CRITICAL] `asin()` domain violation in UV computation

**Files:**
- `src/texture/checkerboard.c:42`
- `src/texture/bump_map_perturb.c:37`

**Code:**
```c
local = vec3_normalize(vec3_subtract(hit->point, obj->data.sphere.center));
v = 0.5 - asin(local.y) / M_PI;
```

**Problem:**
After `vec3_normalize()`, `local.y` should theoretically be in [-1, 1].
However, floating-point rounding errors can produce values like `1.0000000001`.
Per C11 standard, `asin(x)` for `|x| > 1` is **undefined behavior** (most implementations return NaN).

**Impact:**
NaN propagates through `floor()` into `pattern` variable, producing unpredictable
checkerboard artifacts or black pixels on spheres near poles.

**Reproduction:**
Sphere with center at origin, hit point at exact north/south pole.

**Fix:**
```c
v = 0.5 - asin(fmax(-1.0, fmin(1.0, local.y))) / M_PI;
```

---

### 1.2 [HIGH] `clamp_color()` missing lower bound

**File:** `src/lighting/lighting_utils.c:22-30`

**Code:**
```c
void    clamp_color(t_color *result)
{
    if (result->r > 255)
        result->r = 255;
    if (result->g > 255)
        result->g = 255;
    if (result->b > 255)
        result->b = 255;
}
```

**Problem:**
`t_color` members are `int` (`objects.h:31-34`). Only upper bound (255) is clamped.
Negative values pass through unchecked.

**Current risk assessment:**
The lighting pipeline currently produces non-negative values because:
- diffuse is clamped to >= 0 (`lighting.c:48-49`)
- specular is clamped to >= 0 (`lighting.c:31-32`)
- shadow_factor is in [0, 1]
- ambient ratio is in [0, 1]
- all color components are in [0, 255]

So this is NOT triggerable with the current code path. However, any future
modification to the lighting model (e.g., subtractive lighting, color correction)
could expose this. A senior engineer would consider this a **latent defect**.

**[v2] Compounding concern:** `render.c:35` performs `(color.r << 16) | (color.g << 8) | color.b`.
Per C11 6.5.7/4, left-shifting a negative `int` is **undefined behavior**. If
`clamp_color()` allows a negative value through, the pixel write operation becomes UB,
not just a visual glitch. This elevates the latent risk.

**Fix:**
```c
if (result->r < 0)
    result->r = 0;
// ... repeat for g, b
```

---

### 1.3 [HIGH] `mlx_get_data_addr()` return value unchecked

**File:** `src/texture/bump_map_load.c:33-35`

**Code:**
```c
bmap->data = mlx_get_data_addr(bmap->img, &bmap->bpp,
        &bmap->size_line, &bmap->endian);
return (bmap);
```

**Problem:**
`mlx_get_data_addr()` can return NULL in certain MLX failure states.
If `bmap->data` is NULL, subsequent `sample_height()` (`bump_map_perturb.c:23`)
dereferences `bmap->data + offset`, causing segfault.

**Fix:**
```c
bmap->data = mlx_get_data_addr(bmap->img, &bmap->bpp, &bmap->size_line, &bmap->endian);
if (!bmap->data)
{
    mlx_destroy_image(mlx, bmap->img);
    free(bmap);
    return (NULL);
}
```

---

### 1.4 [MEDIUM] Camera gimbal lock when direction is near vertical

**Files:**
- `src/window/window_camera.c:44`
- `src/window/window_camera.c:85`

**Code:**
```c
right = vec3_normalize(vec3_cross(render->scene->camera.direction,
            (t_vec3){0, 1, 0}));
```

**Problem:**
When camera direction approaches `(0, +/-1, 0)`, the cross product with `(0, 1, 0)`
approaches zero, producing an undefined or degenerate `right` vector.
`vec3_normalize()` on a near-zero vector yields garbage.

**Impact:**
Camera strafing/pitch becomes unstable or non-responsive when looking straight up/down.
Not a crash, but poor UX and mathematical incorrectness.

**[v2] Asymmetry note:** `camera.c:36` already handles this case for ray generation:
```c
if (fabs(vec3_dot(camera->direction, world_up)) > 0.999)
    world_up = (t_vec3){0, 0, 1};
```
The same guard is missing in `window_camera.c` for movement/pitch, creating
inconsistent behavior: ray generation handles vertical cameras correctly, but
keyboard movement breaks when looking straight up/down.

**Fix:**
Apply the same guard pattern used in `camera.c:36`:
```c
if (fabs(vec3_dot(direction, (t_vec3){0, 1, 0})) > 0.99)
    right = vec3_normalize(vec3_cross(direction, (t_vec3){0, 0, 1}));
else
    right = vec3_normalize(vec3_cross(direction, (t_vec3){0, 1, 0}));
```

---

### 1.5 [HIGH] Cone apex zero normal [v3]

**File:** `src/ray/intersect_cone_body.c:61-65`

**Code:**
```c
apex = vec3_add(c->center, vec3_multiply(c->axis, c->half_height));
normal = vec3_subtract(hit->point, apex);
k = c->radius / (2.0 * c->half_height);
normal = vec3_subtract(normal, vec3_multiply(c->axis, calc.m * (1.0 + k * k)));
hit->normal = vec3_normalize(normal);
```

**Problem:**
When `calc.m == 0` (ray passes exactly through the cone apex), `hit->point == apex`,
so `normal = {0,0,0}` before the `vec3_multiply` subtraction. After subtraction,
the remaining term `calc.m * (1.0 + k*k) = 0`, so `normal` stays `{0,0,0}`.
`vec3_normalize({0,0,0})` returns `{0,0,0}` (by design in `vector_ops.c:94-98`).

The bounds check at line 56 allows `calc.m` in range `[-2*half_height, 0]`,
so `calc.m == 0` is a valid hit.

**Impact:**
Zero normal causes `diffuse = vec3_dot({0,0,0}, light_dir) = 0`, producing a
black spot at the cone apex. Not a crash, but visually incorrect.

**Related:** `vec3_normalize` returning zero vector is by design (`vector_ops.c:82-101`),
but downstream code (lighting, shadow) doesn't guard against zero normals.

**Fix:**
Clamp `calc.m` away from zero or add a small epsilon shift:
```c
if (calc.m > -0.001)
    calc.m = -0.001;
```

---

### 1.6 [MEDIUM] Cone `half_height == 0` division by zero [v3]

**File:** `src/ray/intersect_cone_body.c:25`

**Code:**
```c
k2 = c->radius / (2.0 * c->half_height);
```

**Problem:**
If `half_height == 0` (degenerate cone), this is division by zero, producing
`+/-INF`. The subsequent quadratic formula (`calc->a`, `calc->b`, `calc->c`)
will contain INF terms, and `calc->discriminant` becomes NaN.

**Current safety:**
The parser likely validates `half_height > 0` from the scene file dimension field,
but the intersection function itself has no guard.

**Fix:**
```c
if (c->half_height < EPSILON)
    return (0);
```

---

### 1.7 [MEDIUM] Camera yaw uses incomplete Rodrigues formula [v4]

**File:** `src/window/window_camera.c:18-26`

**Code:**
```c
static t_vec3    rotate_dir(t_vec3 dir, t_vec3 axis, double cos_a, double sin_a)
{
    t_vec3    result;

    result.x = dir.x * cos_a + (axis.y * dir.z - axis.z * dir.y) * sin_a;
    result.y = dir.y * cos_a + (axis.z * dir.x - axis.x * dir.z) * sin_a;
    result.z = dir.z * cos_a + (axis.x * dir.y - axis.y * dir.x) * sin_a;
    return (result);
}
```

**Problem:**
This implements only two of three Rodrigues terms:
`v_rot = v * cos(a) + (k x v) * sin(a)`
Missing the third term: `+ k * (k . v) * (1 - cos(a))`

Compare with the correct implementation in `window_rotate.c:28-44`:
```c
result = vec3_add(vec3_multiply(v, cos_a),
        vec3_add(vec3_multiply(cross, sin_a),
            vec3_multiply(k, dot * (1.0 - cos_a))));
```

**Impact:**
- **Pitch** (`handle_camera_pitch`): axis = `right` = `cross(dir, up)`.
  Since `dot(right, dir) == 0` by construction, third term is zero. **Pitch is correct.**
- **Yaw** (`handle_camera_yaw`): axis = `(0, 1, 0)`.
  If camera has non-zero Y direction (looking up/down), `dot(axis, dir) != 0`,
  and the Y component is gradually attenuated by each rotation. **Yaw is incorrect.**

The camera direction slowly flattens to the XZ plane during repeated yaw rotations
when looking up or down. The direction drifts.

**Fix:**
Either add the missing third term (requires 5th variable, may violate Norm),
or call `rodrigues_rotate` from `window_rotate.c` (currently `static`).

---

### 1.8 [HIGH] Cone body misses valid far-root intersection [v5]

**File:** `src/ray/intersect_cone_body.c:56-57`

**Code:**
```c
if (calc.m < -2.0 * c->half_height || calc.m > 0.0)
    return (0);  // far root never tested
```

**Problem:**
The quadratic equation for cone-ray intersection produces two roots (near and far).
`calc_cyl_intersect()` selects the near root first. If the near root's height `m`
fails the range check, the function returns 0 without testing the far root.

For cones, the cap at the base (`intersect_cone_cap.c`) only covers the base disk
at `m = -half_height`. There is **no cap at the apex** (`m = 0`). So when a ray
enters through the apex side (near root at `m > 0`) and exits through the body
(far root in valid range), the near root fails the height check and the far root
is never tested. The base cap cannot compensate because the valid far-root hit
is on the cone body, not on a cap.

**Impact:**
Missing cone body pixels when viewing from certain angles. The cone appears to have
holes or invisible patches near the apex. This is a **real visual bug**.

**Contrast with cylinder (1.9):** Cylinders have TWO caps (top and bottom), so a
missed body far-root is always caught by a cap intersection that is closer.

**Fix:**
```c
if (calc.m < -2.0 * c->half_height || calc.m > 0.0)
{
    calc.t = calc.t2;
    calc.m = calc.m2;
    if (calc.t < 0.001 || calc.m < -2.0 * c->half_height || calc.m > 0.0)
        return (0);
}
```

---

### 1.9 [MEDIUM] Cylinder body misses valid far-root intersection [v5]

**File:** `src/ray/intersect_cyl_new.c:109`

**Code:**
```c
if (calc.m < -c->half_height || calc.m > c->half_height)
    return (0);  // far root never tested
```

**Problem:**
Same near-root-only pattern as cone (1.8). When the near root's height `m` is
outside `[-half_height, +half_height]`, the function returns 0 without trying the
far root.

**Mitigating factor:**
Cylinders have TWO cap disks (top and bottom caps in `intersect_cyl_caps`).
When a ray enters from above/below the cylinder bounds, the near body root fails
the height check, but the cap intersection always produces a closer (or equal)
hit than the far body root. This is because the ray must cross the cap plane
before reaching the far side of the infinite cylinder surface.

**Residual risk:**
Due to floating-point precision, edge cases at the exact cap-body boundary
could theoretically produce a missed pixel. In practice, this has not been
observed.

**Severity rationale:** Downgraded from HIGH to MEDIUM because caps compensate
in all practical cases. The cone variant (1.8) is HIGH because it has no apex cap.

**Fix:**
Same pattern as 1.8 — try far root when near root fails height check.

---

## 2. Numerical Stability

### 2.1 [MEDIUM] `0 * INFINITY = NaN` in AABB slab test

**Files:**
- `src/render/camera.c:71-73` (ray inv_dir computation)
- `src/lighting/shadow_test.c:108-110` (shadow ray inv_dir)
- `src/spatial/aabb.c:52` (slab test)
- `src/spatial/aabb_basic.c:22-41` (min/max helpers)

**Code chain:**
```c
// camera.c:71
ray.inv_dir.x = 1.0 / ray.direction.x;   // direction.x == 0 -> inv_dir.x = +/-INF

// aabb.c:52
t0 = (ac->box_min - ac->ray_origin) * ac->inv_dir;  // origin == box_min -> 0 * INF = NaN
```

**Problem:**
When a ray has zero direction component AND the ray origin lies exactly on an AABB
face boundary, `(box_min - origin) = 0`, and `0 * INFINITY = NaN` per IEEE 754.

**[v2] NaN propagation mechanism (verified):**
`min_double()` and `max_double()` in `aabb_basic.c:22-41` use `a < b` / `a > b`:
```c
double  min_double(double a, double b)
{
    if (a < b)       // NaN < anything = false
        return (a);
    return (b);      // returns b (which may also be NaN)
}
```
NaN comparisons always return false, so NaN propagates through the slab test
and causes incorrect BVH traversal (missed intersections or false positives).

**Occurrence probability:**
Extremely rare in primary rays. More likely in **shadow rays** where:
- Light is directly above an object (axis-aligned `light_dir`)
- Shadow ray origin is on BVH node boundary (object surface)

**Standard solution (Williams et al., 2005):**
Perturb zero direction components:
```c
if (ray.direction.x == 0.0) ray.direction.x = 1e-20;
```

---

### 2.2 [LOW] Epsilon inconsistency across codebase

| Location | Epsilon | Usage |
|----------|---------|-------|
| `minirt.h` | `EPSILON = 0.0001` | Defined constant |
| Sphere intersection (`intersect_object.c:47`) | `0.001` (hardcoded) | t-value near-zero check |
| Plane intersection (`intersect_object.c:78`) | `0.0001` (hardcoded) | Denom near-zero check |
| Plane intersection (`intersect_object.c:82`) | `0.001` (hardcoded) | t-value near-zero check |
| BVH traversal (`bvh_traverse.c:126`) | `0.001` (hardcoded) | t_min for AABB test |
| Shadow test (`shadow_test.c:102`) | `0.0001` (hardcoded) | Magnitude check |

**Problem:**
Two different epsilon values (0.001 and 0.0001) are used interchangeably
without documented rationale. Inconsistent epsilon selection can cause:
- Self-intersection artifacts on thin geometry (epsilon too small)
- Missing intersections (epsilon too large)

**Recommendation:**
Define named constants by purpose:
```c
#define RAY_T_MIN     0.001    // Minimum valid t for ray-object intersection
#define COEFF_EPSILON 0.0001   // Near-zero check for quadratic coefficients
```

---

### 2.3 [MEDIUM] Bump map UV integer truncation for negative coordinates [v5]

**File:** `src/texture/bump_map_perturb.c:52-53`

**Code:**
```c
px = (int)(uv[0] * bmap->width) % bmap->width;
py = (int)(uv[1] * bmap->height) % bmap->height;
if (px < 0)
    px += bmap->width;
if (py < 0)
    py += bmap->height;
```

**Problem:**
C99/C11 `(int)` cast truncates toward zero. For small negative values like
`uv[0] = -0.001` with `width = 512`: `(int)(-0.512) = 0`, not `-1`.
Then `0 % 512 = 0`, and the `if (px < 0)` guard never fires.

The correct pixel should be `511` (wrapping around), but the code maps it to `0`.
This creates a discontinuity at the UV=0 boundary.

**Affected objects:**
- **Sphere:** Safe. `get_surface_uv()` produces `uv[0]` in `[0, 1]` (atan2 + asin mapping).
- **Non-sphere (plane, cylinder, cone):** Unsafe. UV is computed via `vec3_dot(hit->point, tangent)`,
  which can produce any real value. Negative UVs near zero are incorrectly mapped.

**Impact:**
Visible texture seam on bump-mapped non-sphere objects at the UV=0 boundary.
Bump normals jump discontinuously, producing a thin line artifact.

**Fix:**
```c
px = (int)floor(uv[0] * bmap->width) % bmap->width;
```
`floor()` correctly rounds toward negative infinity, making the `if (px < 0)` guard work.

---

## 3. Defensive Programming Gaps

### 3.1 [MEDIUM] `handle_light_move()` lacks bounds check

**File:** `src/window/window_objects.c:108-111`

**Code:**
```c
render->scene->lights[render->scene->selected_light].position
    = vec3_add(
        render->scene->lights[render->scene->selected_light].position,
        move);
```

**[v2] Guard chain analysis (verified):**
1. Parser enforces at least 1 L (light) element per scene
2. `selected_light` starts at 0 (scene init)
3. Only `KEY_EQUAL` handler modifies it (`window_key_handlers.c:131-134`):
   ```c
   if (keycode == KEY_EQUAL && render->scene->light_count > 0)
       render->scene->selected_light = (render->scene->selected_light + 1)
           % render->scene->light_count;
   ```
4. `handle_light_move()` is called from `handle_transform_keys()` with NO light_count guard

**Gap:**
The function relies entirely on caller guarantees. If any future code path
calls `handle_light_move()` without ensuring valid `selected_light`, out-of-bounds
array access occurs. Compare with `move_selected_object()` (`window_objects.c:31-33`)
which has an explicit bounds check.

**Recommendation:**
Add entry guard matching `move_selected_object` pattern:
```c
if (render->scene->selected_light < 0
    || render->scene->selected_light >= render->scene->light_count)
    return ;
```

---

### 3.2 [MEDIUM] `scene_build_bvh()` return value unchecked

**File:** `src/main.c:80`

**Code:**
```c
scene_build_bvh(*scene);  // Return is void, failure is silent
return (1);
```

**Problem:**
`scene_build_bvh()` returns void. If BVH construction fails internally
(malloc failure in `alloc_bvh_refs` at `bvh_init.c:147`, or `bvh_create` at
`bvh_init.c:142`), the scene continues without BVH. The fallback brute-force
path handles this gracefully (`trace.c:107` checks `bvh->root != NULL`), so
it's not a crash. However, silent fallback to O(n) intersection is a
performance cliff that should at least be logged.

**[v2] Verified:** `bvh_init.c:149` `free(refs)` is safe because `alloc_bvh_refs`
sets `*refs = NULL` at line 110 before any failure path. No crash risk.

---

### 3.3 [MEDIUM] Bump map load failure message lacks context

**File:** `src/texture/bump_map_load.c:63`

**Code:**
```c
write(2, "Error\n", 6);
return (0);
```

**Problem:**
When bump map loading fails, only "Error\n" is printed.
No indication of which file failed. With multiple bump-mapped objects,
this is useless for debugging.

**Recommendation:**
Print the failing filepath (within Norm constraints):
```c
error_write_str("Error: bump map load: ");
error_write_str(obj->bump_path);
error_write_str("\n");
```

---

### 3.4 [LOW] `ft_bzero` initialization inconsistency

| Parser | Uses ft_bzero | Relies on parse_bonus_options init |
|--------|--------------|-----------------------------------|
| parse_sphere | No | Yes |
| parse_plane | No | Yes |
| parse_cylinder | No | Yes |
| parse_cone | Yes (`parse_cone.c:83`) | Yes (redundant) |

**Problem:**
Only cone parser uses `ft_bzero()`. Others rely on `parse_bonus_options()` to
initialize `has_checker`, `bump_path`, `bump_map`. If `t_object` gains new
fields in the future, sphere/plane/cylinder parsers will have uninitialized
members while cone won't.

**Recommendation:**
Apply `ft_bzero(&obj, sizeof(t_object))` consistently in all object parsers,
or remove it from parse_cone for consistency.

---

### 3.5 [LOW] `error_write_int()` does not handle negative numbers

**File:** `src/parser/parse_error_msg.c:35-43`

**Code:**
```c
void    error_write_int(int n)
{
    char    c;

    if (n >= 10)
        error_write_int(n / 10);
    c = '0' + (n % 10);
    write(2, &c, 1);
}
```

**Problem:**
If `n` is negative, `n % 10` produces negative remainder, yielding
characters before '0' in ASCII (e.g., '0' + (-5) = '+').
Currently `n` is only used for line numbers (always positive), so this
doesn't manifest. But the function interface promises `int`, not `unsigned int`.

---

### 3.6 [LOW] Duplicate bonus options silently accepted

**File:** `src/parser/parse_bonus_options.c:68-82`

**Code:**
```c
while (!at_line_end(*token))
{
    result = PARSE_ERR_TRAILING_TOKEN;
    if (starts_with(*token, "checker:"))
        result = parse_checker_opt(*token, obj, token);
    else if (starts_with(*token, "bump:"))
        result = parse_bump_opt(*token, obj, token);
    // ...
}
```

**Problem:**
Input like `checker:255,0,0 checker:0,255,0` silently overwrites.
For `bump:a.xpm bump:b.xpm`, the first `bump_path` leaks (`ft_substr`
allocates at `parse_bonus_options.c:53`, then second call overwrites pointer
without freeing first).

**Impact:**
Memory leak on duplicate `bump:` options. Inconsistent behavior for
duplicate `checker:` (overwrites without error).

---

### 3.7 [MEDIUM] `hud_text.c` unbounded label copy [v3]

**File:** `src/hud/hud_text.c:43-44`

**Code:**
```c
char    buf[128];
int     i;

i = 0;
while (i < 8)
    buf[i++] = ' ';
while (*label)
    buf[i++] = *label++;     // no bounds check
buf[i] = '\0';
hud_format_vec3(buf + i, vec);  // appends ~30 chars
```

**Problem:**
The label copy loop at line 43-44 has no bounds check against the 128-byte buffer.
After 8 spaces + label + '\0' + vec3 format (~30 chars), the effective label
budget is ~90 chars.

**Current safety:**
All callers are internal and use short labels ("Pos: ", "Dir: ", "Amb: ", etc.),
all under 10 chars. So overflow is NOT triggerable with current code.

**Risk:**
If a future caller passes a long label, stack buffer overflow occurs.

---

### 3.8 [LOW] `bounds_for_cone` doxygen comment says "plane" [v3]

**File:** `src/spatial/bounds.c:57`

**Code:**
```c
/**
 * @brief Compute a large AABB approximation for a plane.  // <-- wrong
 */
static t_aabb    bounds_for_cone(t_cone_data *c)
```

**Problem:**
Copy-paste error: the `@brief` for `bounds_for_cone()` says "plane"
instead of "cone".

---

### 3.9 [LOW] `shadow_calc.c` hardcoded pi constant [v3]

**File:** `src/lighting/shadow_calc.c:71`

**Code:**
```c
angle = 2.0 * 3.14159265358979323846 * (sample_index % grid_size)
```

**Problem:**
Uses a literal pi constant instead of `M_PI` (from `<math.h>`).
Other files in the same codebase (shadow_config.c, window_camera.c, etc.)
use `M_PI`. Inconsistency reduces maintainability.

### 3.10 [MEDIUM] `pixel_timing` total_time/count mismatch [v4]

**File:** `src/render/pixel_timing.c:66-75`

**Code:**
```c
if (timing->count < timing->capacity)
{
    timing->samples[timing->count] = time_ns;
    timing->count++;
}
...
timing->total_time += time_ns;
```

**Problem:**
When `count >= capacity` (MAX_PIXEL_SAMPLES = 480,000), samples stop being stored
but `total_time` continues accumulating. At full resolution (1440x900 = 1,296,000 pixels),
the average at `pixel_timing_print.c:90` is computed as:
```c
avg_ns = (double)timing->total_time / (double)timing->count;
```
Where `total_time` includes all 1.3M pixels but `count` is capped at 480K.
This makes the average ~2.7x too large. Percentiles are computed from only the first
37% of pixels.

**Impact:**
Misleading diagnostic output. Not a crash.

---

### 3.11 [LOW] `prefix_destroy` NULL deref outside guard [v4]

**File:** `src/bvh_vis/bvh_vis_prefix.c:121-123`

**Code:**
```c
if (state && state->buffer)
{
    free(state->buffer);
    state->buffer = NULL;
}
state->capacity = 0;   // outside the guard
state->length = 0;
state->level = 0;
```

**Problem:**
Lines 121-123 are outside the `if (state && ...)` guard. If `state == NULL`,
dereferences crash.

**Current safety:**
Only called from `bvh_visualize()` with stack-allocated `t_prefix_state`.
Never NULL in current code.

---

### 3.12 [LOW] `prefix_push` UTF-8 truncation [v4]

**File:** `src/bvh_vis/bvh_vis_prefix.c:63,75`

**Code:**
```c
prefix = "│   ";   // 6 bytes: 3 (UTF-8 │) + 3 spaces
...
ft_memcpy(state->buffer + state->length, prefix, 4);  // copies only 4 bytes
```

**Problem:**
`"│   "` (U+2502 + 3 spaces) is 6 bytes in UTF-8, but only 4 bytes are copied.
Result: `│ ` (3-byte char + 1 space) instead of `│   ` (3-byte char + 3 spaces).
Tree indentation is visually wrong for non-last nodes.

Not a buffer overflow (length tracking is consistent at +4).

---

### 3.13 [LOW] `format_object_list` size parameter mismatch [v4]

**File:** `src/bvh_vis/bvh_vis_format.c:34`

**Code:**
```c
ft_strlcpy(buffer, "Objects: [", 256);
ft_strlcat(buffer, id, 256);
```

**Problem:**
Uses size=256, but `t_node_info.objects` is `char[64]` (`bvh_vis.h:52`).
If concatenated string exceeds 64 bytes, stack buffer overflow occurs.

**Current safety:**
Leaf nodes contain at most 2 objects (threshold in `bvh_build_core.c:75`),
so maximum output is ~27 bytes (`"Objects: [sp_001, cy_001]"`). Safe in practice.

Only theoretically triggerable with depth > 20 (forces large leaf nodes).

---

### 3.14 [LOW] `float_to_str` rounding carry not handled [v4]

**File:** `src/utils/format_helpers.c:124-125`

**Code:**
```c
int_part = (int)value;
frac_part = (int)((value - int_part) * mult + 0.5);
```

**Problem:**
For `value = 9.999`, `precision = 2`: `frac_part = (int)(99.9 + 0.5) = 100`.
Output becomes `"9.100"` instead of `"10.00"`. The rounding carry from
fractional to integer part is not propagated.

**Impact:**
HUD display occasionally shows wrong numbers for values near decimal boundaries.

---

### 3.15 [MEDIUM] `hud_format_float` `(int)` cast undefined behavior [v5]

**File:** `src/hud/hud_format.c:37-38`

**Code:**
```c
int_part = (int)value;
frac_part = (int)((value - int_part) * mult + 0.5);
```

**Problem:**
Per C11 6.3.1.4, converting a floating-point value to `int` when the value
is outside `[INT_MIN, INT_MAX]` is **undefined behavior**. For `value > 2147483647.0`
or `value < -2147483648.0`, `(int)value` is UB (not just truncation).

**Current safety:**
Object coordinates, colors, and other HUD-displayed values are typically in small
ranges. But the function interface accepts `double` without range constraint.

**Impact:**
If any object has very large coordinates (e.g., a distant light), the HUD
format function invokes UB. Could manifest as garbage display values or crash
depending on platform.

**Recommendation:**
Clamp `value` before cast:
```c
if (value > 999999.0)
    value = 999999.0;
if (value < -999999.0)
    value = -999999.0;
```

---

### 3.16 [LOW] `handle_expose` callback type mismatch [v5]

**File:** `src/window/window_init.c:74`, `src/window/window_events.c:51`

**Code:**
```c
// Declaration (window_internal.h:123)
int    handle_expose(t_render *render);

// Registration
mlx_hook(render->mlx.win, 12, 1L << 15, handle_expose, render);
```

**Problem:**
MiniLibX expose callback should have signature `int (*)(void *)`. `handle_expose`
takes `t_render *` instead of `void *`. Every other MLX callback in the codebase
correctly uses `void *param`:
- `close_window(void *param)` — correct
- `handle_key(int keycode, void *param)` — correct
- `handle_key_release(int keycode, void *param)` — correct
- `render_loop(void *param)` — correct

Since MLX declares `int (*funct)()` (K&R style without prototype), the compiler
does not catch this. Works on all mainstream platforms (x86_64, ARM64) because
`void *` and `t_render *` have identical representation, but is technically UB.

**Fix:**
```c
int    handle_expose(void *param)
{
    t_render *render;
    render = (t_render *)param;
    // ...
}
```

---

### 3.17 [LOW] `pixel_timing_init` malloc unchecked [v5]

**File:** `src/render/pixel_timing.c:27`

**Code:**
```c
void    pixel_timing_init(t_pixel_timing *timing)
{
    timing->capacity = MAX_PIXEL_SAMPLES;
    timing->samples = malloc(sizeof(long) * timing->capacity);
    timing->count = 0;
    timing->min_time = LONG_MAX;
    timing->max_time = 0;
    timing->total_time = 0;
    timing->enabled = 1;    // set regardless of malloc result
}
```

**Problem:**
`malloc()` return value is not checked. If allocation fails (480K * 8 = 3.84MB),
`timing->samples` is NULL but `timing->enabled` is set to 1.

**Current safety:**
`pixel_timing_add_sample()` at line 64 checks `!timing->samples` and returns early.
So NULL samples never causes a crash — timing is simply silently disabled.

**Impact:**
Silent degradation, not a crash. The user sees no timing statistics without
knowing why. A warning message would be more helpful.

---

## 4. Memory Safety

### 4.1 Lifecycle path analysis

| Path | Verdict | Detail |
|------|---------|--------|
| Normal exit (`close_window`) | **Safe** | bump cleanup -> render destroy -> scene destroy |
| Bump load failure (`main.c:109-114`) | **Safe** | cleanup_all_bump_maps() -> render_destroy() -> scene_destroy() |
| Parse failure (`main.c:75-78`) | **Safe** | scene_destroy() -> object_list_destroy() frees bump_path per object |
| Bonus option parse error (`parse_bonus_options.c:75-79`) | **Safe** | free(bump_path) + NULL before return (fixed in current codebase) |
| object_list_grow failure | **Leak** | `bump_path` not freed before return (section 4.2) [v4] |
| MLX init failure | **Safe** | render_create failure -> scene_destroy |
| BVH alloc failure (`bvh_init.c:147-150`) | **Safe** | alloc_bvh_refs sets *refs=NULL (line 110), free(refs) is safe |
| BVH deep split malloc failure | **Silent loss** | `create_split_node` NULL child drops objects (section 4.4) [v5] |
| Parser line malloc failure | **Silent truncation** | Scene parsed partially, returns success (section 4.5) [v5] |

### 4.2 [MEDIUM] `bump_path` leak on `object_list_add` failure [v4]

**Files:**
- `src/parser/parse_objects.c:95-96` (sphere, plane)
- `src/parser/parse_cylinder.c:132-133`
- `src/parser/parse_cone.c:101-102`

**Code (sphere example):**
```c
result = parse_bonus_options(&token, &obj);    // allocates obj.bump_path
if (result != PARSE_OK)
    return (result);
format_id(obj.id, 8, "sp-", get_type_count(scene, OBJ_SPHERE) + 1);
if (!object_list_add(&scene->objects, &obj))   // fails here
    return (PARSE_ERR_FORMAT);                 // obj.bump_path leaked
```

**Problem:**
`parse_bonus_options()` allocates `obj.bump_path` via `ft_substr()`.
If `object_list_add()` fails (capacity grow failure), the function returns
`PARSE_ERR_FORMAT` without freeing `obj.bump_path`. Since `obj` is a stack
variable, the pointer is lost = **memory leak**.

**Affected parsers:** All 4 object parsers (sphere, plane, cylinder, cone)
have the same pattern.

**Note:** `object_list_add` success path is safe — struct copy (`list->items[count] = *obj`)
transfers `bump_path` ownership to the list. `object_list_destroy()` frees all
`bump_path` pointers (line 53).

**Fix:**
```c
if (!object_list_add(&scene->objects, &obj))
{
    free(obj.bump_path);
    return (PARSE_ERR_FORMAT);
}
```

### 4.3 Remaining concern: bump_path leak on duplicate `bump:` option

**File:** `src/parser/parse_bonus_options.c:53,73`

As noted in 3.6, if `bump:a.xpm bump:b.xpm` appears on one line, `parse_bump_opt`
is called twice. The first call sets `obj->bump_path = ft_substr(...)`. The second
call overwrites `obj->bump_path` without freeing the first allocation.

**Practical impact:** Low (malformed input, program exits shortly after).
**Valgrind impact:** Will show as "definitely lost" block.

### 4.4 [MEDIUM] `create_split_node` silent NULL child on malloc failure [v5]

**File:** `src/spatial/bvh_build_split.c:74-77`

**Code:**
```c
node->left = bvh_build_recursive(sp->objects, sp->mid,
        sp->scene, sp->depth + 1);
node->right = bvh_build_recursive(sp->objects + sp->mid,
        sp->count - sp->mid, sp->scene, sp->depth + 1);
```

**Problem:**
`bvh_build_recursive` can return NULL on internal `malloc` failure.
If either child is NULL, the BVH tree has a missing subtree. Objects
assigned to that subtree are silently lost from the BVH.

**Impact:**
Under memory pressure, objects become invisible — their subtree is never
traversed. Not a crash (BVH traversal checks for NULL nodes), but objects
disappear without any error message.

**Trigger condition:**
Requires `malloc` failure during deep BVH construction. Unlikely in normal
operation, but possible with very large scenes on constrained systems.

**Fix:**
```c
node->left = bvh_build_recursive(...);
node->right = bvh_build_recursive(...);
if (!node->left || !node->right)
{
    bvh_node_destroy(node->left);
    bvh_node_destroy(node->right);
    free(node);
    return (NULL);  // propagate failure
}
```

---

### 4.5 [MEDIUM] Parser silently truncates scene on malloc failure [v5]

**Files:**
- `src/parser/parse_line_reader.c:100-102` (`finalize_line`)
- `src/parser/parser.c:105-113` (`process_lines`)

**Code chain:**
```c
// parse_line_reader.c:100 — finalize_line
result = malloc(reader->line_len + 1);
if (!result)
    return (NULL);

// parser.c:105-113 — process_lines
line = line_reader_next(&reader);
if (line == NULL)
    break ;           // treats malloc failure as EOF
// ...
if (reader.io_error)
{
    ctx->error_code = PARSE_ERR_IO;
    error_context_print(ctx);
    return (0);
}
return (success);     // returns 1 even though lines were skipped
```

**Problem:**
When `finalize_line` fails to `malloc` the line buffer, it returns NULL.
`line_reader_next` propagates the NULL to `process_lines`, which treats
NULL as end-of-file and breaks out of the parsing loop. The `io_error`
check only covers `read()` failures, not `malloc` failures.

Result: the parser returns `success = 1` even though remaining lines
were never parsed. Scene is silently truncated.

**Mitigating factor:**
`validate_scene()` (called after parsing) checks for required elements
(camera, ambient, light). If any are in the truncated portion, validation
fails. However, if required elements were already parsed before the malloc
failure, validation passes and the scene renders with missing objects.

**Impact:**
Under memory pressure, scene silently loses objects. No error message.

---

## 5. Architecture & Design

### 5.1 `adjust_samples()` global state mutation pattern

**File:** `src/lighting/lighting.c:76-86`

```c
static int    adjust_samples(t_scene *scene)
{
    int    orig;

    orig = scene->shadow_config.samples;
    if (scene->light_count > 1)
        scene->shadow_config.samples = orig / scene->light_count;
    if (scene->shadow_config.samples < 1)
        scene->shadow_config.samples = 1;
    return (orig);
}
```

**Usage in `apply_lighting()` (lighting.c:102-104):**
```c
orig_samples = adjust_samples(scene);
accumulate_lights(scene, hit, view_dir, &acc);
scene->shadow_config.samples = orig_samples;  // restore
```

**Concern:**
Temporarily mutates shared state and restores afterward. This works in
single-threaded code but:
- Violates principle of least surprise (caller doesn't expect scene mutation)
- Fragile against exception paths (if anything between adjust/restore crashes, state is corrupted)
- Blocks any future multi-threaded rendering

**Accepted reason:** Norm 4-parameter limit prevents passing `effective_samples`
as an additional argument to `accumulate_lights()` -> `calc_lighting_factor()` chain.

---

### 5.2 `void *scene` in BVH functions

**File:** `includes/spatial.h:122-133`

```c
void bvh_build(t_bvh *bvh, t_object_ref *objects, int count, void *scene);
int  bvh_intersect(t_bvh *bvh, t_ray ray, t_hit_record *hit, void *scene);
int  bvh_node_intersect(t_bvh_node *node, t_ray ray, t_hit_record *hit, void *scene);
```

**Rationale:** Avoids `spatial.h <-> minirt.h` circular dependency.
**Trade-off:** Sacrifices type safety for compilation order independence.
**Verdict:** Pragmatic solution under 42 Norm header constraints. Forward
declaration (`typedef struct s_scene t_scene`) would be cleaner but requires
header dependency restructuring.

---

### 5.3 Intersection function signature inconsistency

**File:** `src/ray/intersect_object.c:103-120`

| Function | Signature pattern |
|----------|-------------------|
| `intersect_sphere_new` (line 109) | `(ray, &data.sphere, color, hit)` |
| `intersect_plane_new` (line 111) | `(ray, &data.plane, color, hit)` |
| `intersect_cylinder_new` (line 113) | `(ray, &data.cylinder, color, hit)` |
| `intersect_cone_new` (line 116) | `(ray, obj, hit)` — different |

Cone receives full `t_object*` while others receive data-specific pointers + color.
The `hit->obj = obj` assignment at line 118 handles all types uniformly, so the
cone doesn't actually need full object access.

**Impact:** No functional issue. Code clarity concern only.

---

## 6. Intersection & Rendering Correctness

### 6.1 Intersection algorithms

| Object | Verdict | Notes |
|--------|---------|-------|
| Sphere (`intersect_object.c:29-56`) | **Correct** | Standard quadratic formula, near/far root selection, epsilon guard (0.001) |
| Plane (`intersect_object.c:70-91`) | **Correct** | Normal auto-correction (dot > 0 -> flip), denom guard (0.0001) |
| Cylinder body | **Correct*** | Axis projection, height range validation, half_height symmetric bounds. *Exception: missed far-root (section 1.9), compensated by caps |
| Cylinder caps | **Correct** | Disk intersection, squared radius comparison (avoids sqrt) |
| Cone body | **Defect** | Apex-based quadratic, k^2 parameterization, proper normal derivation. **Missed far-root (1.8)**, zero normal at apex (1.5) |
| Cone cap | **Correct** | Base disk intersection (base only, no apex cap), -axis normal direction, backface correction |

### 6.2 AABB slab test

**File:** `src/spatial/aabb.c:69-89`

```c
return (tmax >= tmin && tmax > 0);
```

**Verdict:** Correct standard implementation.
- Three-axis slab intersection with swap for negative inv_dir (`update_bounds`)
- Properly handles rays starting inside box (tmin < 0, tmax > 0)
- IEEE 754 infinity arithmetic works correctly for axis-aligned rays
- **Exception:** NaN case documented in section 2.1

### 6.3 BVH traversal

**File:** `src/spatial/bvh_traverse.c`

- Near/far child ordering based on ray direction sign (`traverse_children:86-92`): **Correct**
- Distance propagation between children (`traverse_children:93-103`): **Correct**
- `th.distance` properly reset to `hit->distance` before far child test (line 97)
- Leaf intersection with distance-monotonic update (`bvh_leaf_intersect:47-64`): **Correct**
- Plane separation from BVH (`trace.c:107-116`): **Correct** design choice

### 6.4 BVH construction

- Midpoint split on longest axis: **Correct**, simple but effective
- Degenerate partition fallback (all left/all right -> force midpoint): **Correct**
- Depth limit (20): **Adequate** (supports ~1M objects in balanced tree)
- Leaf threshold (count <= 2): **Reasonable**

### 6.5 Lighting model

**File:** `src/lighting/lighting.c`

- Phong specular with fast_pow32 (x^32 via repeated squaring, `lighting_utils.c:40-48`): **Correct**
- Diffuse clamped to [0, inf) (line 48-49), specular input clamped to [0, 1] (line 31-32): **Correct**
- Multi-light accumulation with per-light shadow factor (lines 57-74): **Correct**
- Ambient + accumulated diffuse/specular blend (lines 105-110): **Correct**
- Color clamp to [0, 255]: **Partial** (upper only, see section 1.2)

### 6.6 Shadow system

**File:** `src/lighting/shadow_test.c`

- Shadow ray bias via surface normal offset (line 106): **Correct**
- Distance limit set to light distance (line 104): **Correct**
- BVH any-hit early termination (line 114) + separate plane test (line 116): **Correct**
- Brute-force fallback for low object count (line 118): **Correct**

---

## 7. Build & Tooling

### 7.1 [MEDIUM] Makefile lacks header dependency tracking

**File:** `Makefile:150-153`

```makefile
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
    @mkdir -p $(dir $@)
    @$(CC) $(CFLAGS) -c $< -o $@
```

**Problem:**
Object targets depend only on their `.c` file. If a header changes,
dependent `.o` files are NOT rebuilt. This causes stale object files
in incremental builds.

**Fix:**
```makefile
CFLAGS += -MMD -MP
-include $(OBJS:.o=.d)
```

### 7.2 [LOW] Pixel codec bpp assumption

**File:** `src/window/mlx_pixel_codec.c`

The `else` branch in `decode_rgb()` assumes `bpp >= 32` but never validates.
If MLX reports `bpp == 16`, `src[3]` is out-of-bounds access.

Current MLX implementations on macOS/Linux always use 24 or 32 bpp,
so this is a portability concern rather than an active bug.

### 7.3 [LOW] `window_selection.c` dead code — file not in Makefile [v5]

**File:** `src/window/window_selection.c` (exists on disk, NOT in `Makefile` SRCS)

**Code:**
```c
void    handle_object_selection(t_render *render, int keycode)
{
    if (render->scene->objects.count == 0)
        return ;
    if (keycode == KEY_BRACKET_RIGHT)
        hud_select_next(render);
    else if (keycode == KEY_BRACKET_LEFT)
        hud_select_prev(render);
    hud_mark_dirty(render);
}
```

**Problem:**
This file exists in the repository but is NOT listed in `Makefile` SRCS.
It is never compiled or linked. The function `handle_object_selection` is
NOT declared in `window_internal.h`.

**History:**
The functionality has been superseded:
- `KEY_BRACKET_LEFT/RIGHT` now mapped to `handle_light_move()` (`window_key_handlers.c:100-106`)
- `KEY_TAB` directly calls `hud_select_next()` (`window_key_handlers.c:126`)

**Impact:**
Dead code artifact. No runtime effect.

**Recommendation:**
Delete the file or add a comment explaining why it's kept.

---

### 7.4 [LOW] `hud_select_prev()` unreachable code [v5]

**File:** `src/hud/hud_navigation.c` (contains `hud_select_prev`)

**Problem:**
`hud_select_prev()` is only called from `handle_object_selection()` in
`window_selection.c` (7.3), which is dead code (not compiled).
No other call site exists in the codebase.

**Impact:**
The function is compiled and linked (it IS in Makefile via `hud_navigation.c`),
but never executed. Dead code in the binary.

---

### 7.5 [LOW] Duplicate function declarations across headers [v5]

**Files:** Multiple headers

**Problem:**
The following functions are declared in two separate headers:

| Function | Header 1 | Header 2 |
|----------|----------|----------|
| `hud_render_objects` | `hud.h` | `hud_objects.h` |
| `hud_mark_dirty` | `hud.h` | `hud_text.h` |
| `hud_render_camera` | `hud.h` | `hud_scene.h` |
| `hud_render_ambient` | `hud.h` | `hud_scene.h` |
| `hud_render_light` | `hud.h` | `hud_scene.h` |
| `get_time_ns` | `metrics.h` | `pixel_timing.h` |

**Impact:**
Valid C (compatible duplicate declarations), but maintenance risk. If one
declaration is updated without the other, silent ABI mismatch occurs.

---

### 7.6 [LOW] `copy_str_to_buf` missing `const` on `src` parameter [v5]

**File:** `includes/hud_text.h:21`

**Code:**
```c
int    copy_str_to_buf(char *dst, char *src);
```

**Problem:**
Called with string literals (`copy_str_to_buf(buf, "Objects (Page ")`),
which are `const char *`. The function never writes to `src`.
Compiling with `-Wwrite-strings` produces warnings.

---

## 8. Positive Assessment

### 8.1 Architecture

| Aspect | Grade | Notes |
|--------|-------|-------|
| Module separation | **A** | 13 directories, clear concern boundaries (parser/ray/lighting/texture/spatial/window/hud) |
| BVH acceleration | **A** | Plane separation + bounded-object BVH + brute-force fallback |
| Debounce FSM | **A** | IDLE -> ACTIVE -> FINAL -> COOLDOWN, low-quality preview on input |
| Multi-light design | **A** | Per-light shadow factor, adaptive sample distribution |
| Bump map deferred loading | **A** | Parse path stores filepath only, MLX init required for XPM decode |
| Cleanup ordering | **A** | bump cleanup (needs mlx) -> render destroy (destroys mlx) -> scene destroy |
| Camera cache | **A** | Lazy recompute with `valid` flag, gimbal lock guard in ray generation |

### 8.2 Code quality

| Aspect | Grade | Notes |
|--------|-------|-------|
| 42 Norm compliance | **A+** | 100% pass, 97 files. Complex raytracer within 5func/file, 25lines/func |
| Memory management | **A** | Consistent create/destroy pattern, NULL guards, post-free NULL assignment |
| Error propagation | **A** | `t_parse_result` enum -> `error_context` -> `get_error_message()` chain (13 error codes) |
| Input validation | **A** | Range checks, overflow prevention (`INT_MAX/2` guard), direction normalization, FOV [1,179] |
| Resource cleanup | **A-** | All paths verified safe except BVH build silent failure |
| avg functions/file | **3.72/5** | Well-distributed, not artificially cramped |

### 8.3 Notable engineering decisions

1. **`object_list_grow()` with `INT_MAX/2` overflow guard** (`object_list.c`)
   -- Prevents capacity*2 integer overflow. Professional-grade defensive check.

2. **BVH partition degeneration fallback** (`bvh_build_partition.c`)
   -- Prevents infinite recursion when all objects fall to one side.

3. **Separate plane refs in BVH** (`bvh_init.c`)
   -- Infinite planes can't be bounded in AABB. Separating them from BVH
   and testing independently is the correct architectural choice.

4. **`create/destroy` lifecycle symmetry** across all modules
   -- `scene_create/scene_destroy`, `render_create/render_destroy`,
   `bvh_create/bvh_destroy`, `bump_map_load/bump_map_destroy`.
   Consistent mental model for resource ownership.

5. **Camera cache with gimbal lock guard** (`camera.c:36`)
   -- Switches world_up reference vector when direction approaches vertical.
   Prevents degenerate basis vectors in ray generation.

6. **`alloc_bvh_refs` defensive cleanup** (`bvh_init.c:103-123`)
   -- Sets `*refs = NULL` before potential failure, cleans up partial allocs,
   frees previous `plane_refs.indices` before reallocating (preventing leak on rebuild).

---

## 9. Fix Priority Matrix

| Priority | Issue | Effort | Section |
|----------|-------|--------|---------|
| **P0** | `asin()` input clamping | 1 line x 2 files | 1.1 |
| **P0** | `clamp_color()` lower bound | 3 lines | 1.2 |
| **P0** | `mlx_get_data_addr` NULL check | 5 lines | 1.3 |
| **P0** | Cone body missed far-root intersection | 8 lines | 1.8 |
| **P1** | Cone apex zero normal | 2 lines | 1.5 |
| **P1** | Cone `half_height == 0` guard | 2 lines | 1.6 |
| **P1** | Camera gimbal lock in movement/pitch | 3 lines x 2 locations | 1.4 |
| **P1** | Camera yaw incomplete Rodrigues formula | Function rewrite | 1.7 |
| **P1** | Cylinder body missed far-root intersection | 8 lines | 1.9 |
| **P1** | `handle_light_move()` bounds check | 2 lines | 3.1 |
| **P1** | Bump map error message with filepath | 1 line | 3.3 |
| **P1** | `bump_path` leak on `object_list_add` failure | 2 lines x 4 parsers | 4.2 |
| **P2** | Bump map UV integer truncation | 2 lines | 2.3 |
| **P2** | `hud_text.c` unbounded label copy | 2 lines | 3.7 |
| **P2** | Epsilon constant unification | Global rename | 2.2 |
| **P2** | `ft_bzero` consistency | 3 files | 3.4 |
| **P2** | Makefile header dependency | 2 lines | 7.1 |
| **P2** | Duplicate bump: option leak | 3 lines | 3.6 |
| **P2** | `pixel_timing` total_time/count mismatch | 3 lines | 3.10 |
| **P2** | `hud_format_float` `(int)` cast UB | 3 lines | 3.15 |
| **P2** | BVH `create_split_node` silent NULL child | 6 lines | 4.4 |
| **P2** | Parser silent truncation on malloc failure | 5 lines | 4.5 |
| **P3** | NaN-safe AABB min/max | Function rewrite | 2.1 |
| **P3** | `scene_build_bvh` error logging | 1 line | 3.2 |
| **P3** | `error_write_int` negative handling | 3 lines | 3.5 |
| **P3** | Pixel codec bpp validation | 3 lines | 7.2 |
| **P3** | `shadow_calc.c` hardcoded pi | 1 line | 3.9 |
| **P3** | `bounds_for_cone` wrong doxygen | 1 line | 3.8 |
| **P3** | `prefix_destroy` NULL deref outside guard | 1 line | 3.11 |
| **P3** | `prefix_push` UTF-8 truncation | 2 lines | 3.12 |
| **P3** | `format_object_list` size 256 vs 64 | 1 line | 3.13 |
| **P3** | `float_to_str` rounding carry | 5 lines | 3.14 |
| **P3** | `handle_expose` callback type mismatch | 3 lines | 3.16 |
| **P3** | `pixel_timing_init` malloc unchecked | 3 lines | 3.17 |
| **P3** | `window_selection.c` dead code | Delete file | 7.3 |
| **P3** | `hud_select_prev()` unreachable | N/A | 7.4 |
| **P3** | Duplicate function declarations | Refactor headers | 7.5 |
| **P3** | `copy_str_to_buf` missing `const` | 1 line | 7.6 |

---

## 10. Overall Verdict

**Production quality: HIGH**

The codebase demonstrates strong fundamentals in systems programming, 3D graphics
mathematics, and memory management -- all under the extreme constraint of 42 Norm
(25 lines/function, 5 variables/function, 5 functions/file, 4 parameters/function).

**Finding summary:** 38 findings across 7 categories.

| Severity | Count | Key items |
|----------|-------|-----------|
| CRITICAL | 1 | `asin()` domain violation (1.1) |
| HIGH | 4 | `clamp_color()` lower bound (1.2), `mlx_get_data_addr` NULL (1.3), cone apex zero normal (1.5), cone missed far-root (1.8) |
| MEDIUM | 16 | Camera gimbal (1.4), cone half_height=0 (1.6), camera yaw Rodrigues (1.7), cylinder missed far-root (1.9), AABB NaN (2.1), bump UV truncation (2.3), light_move bounds (3.1), BVH void return (3.2), bump error msg (3.3), hud_text unbounded (3.7), pixel_timing mismatch (3.10), hud_format UB (3.15), bump_path leak (4.2), BVH NULL child (4.4), parser truncation (4.5), Makefile no header deps (7.1) |
| LOW | 17 | Remaining defensive, build, documentation issues |

**Actual bugs (runtime-triggerable):** 8 items.
- P0 (4): `asin()` domain violation, `clamp_color()` missing lower bound, `mlx_get_data_addr()` NULL unchecked, **cone body missed far-root intersection**
- P1 (4): Cone apex zero normal, cone `half_height == 0` division by zero, camera yaw incomplete Rodrigues, cylinder body missed far-root

The asin issue, cone missed far-root, cone apex zero normal, and camera yaw drift
are the most likely to manifest during normal usage. The cone missed far-root (1.8)
is a **newly discovered visual bug** that produces visible holes in cone geometry.

**Defensive programming gaps:** Several functions rely on caller guarantees instead
of self-validating inputs. This is a common trade-off under Norm constraints but
should be documented with comments indicating preconditions. v5 adds `hud_format_float`
`(int)` cast UB (3.15) and `handle_expose` type mismatch (3.16) as notable gaps.

**Memory safety:** Very good. All lifecycle paths (normal exit, parse failure,
bump load failure, MLX init failure, BVH alloc failure) have been verified as
leak-free, with exceptions:
- Duplicate `bump:` options in malformed input (3.6/4.3)
- `bump_path` leak on `object_list_add` failure (4.2)
- BVH `create_split_node` silent NULL child drops objects under memory pressure (4.4)
- Parser silently truncates scene on `malloc` failure (4.5)

**Mathematical correctness:** Intersection algorithms are mostly correct, with two
newly identified root-selection issues:
- **Cone body** (1.8): missed far-root when near-root fails height check. No apex
  cap to compensate. **Real visual bug** producing invisible cone patches.
- **Cylinder body** (1.9): same pattern, but caps compensate in practice. **Latent defect.**
- Bump map UV computation (2.3) has integer truncation for negative coordinates,
  causing texture seams on non-sphere objects.

**Dead code:** `window_selection.c` exists on disk but is not compiled (7.3).
Its only consumer `hud_select_prev()` is unreachable (7.4). Both are artifacts
of a superseded object selection mechanism.

**False positive rate:** The exhaustive v5 review with 4-agent parallel analysis produced
12 false positives that were dismissed through manual source code verification
(see Appendix A). This highlights the importance of human review over automated
analysis for Norm-constrained codebases where safety invariants are maintained
through non-obvious call chain guarantees.

**Under 42 Norm constraints, this is well-engineered code.**
The primary recommendation is to address the P0 items (4 items, ~17 lines total),
which eliminates all known runtime-triggerable bugs. P1 items (8 items, ~35 lines)
address edge cases, mathematical correctness, defensive guards, and memory leak paths.

---

## Appendix A. False Positives Dismissed [v5]

The following findings were raised by automated 4-agent parallel review but
dismissed after manual source code verification:

| # | Finding | Dismissal Reason |
|---|---------|------------------|
| FP-1 | Parser uninitialized `bump_path` in sphere/plane/cylinder | `parse_bonus_options()` is ALWAYS called before `object_list_add()` — initializes `has_checker=0`, `bump_path=NULL`, `bump_map=NULL` (`parse_objects.c:91`) |
| FP-2 | BVH memory leak on partial tree build | Partial tree is correctly freed by `bvh_node_destroy()` recursive cleanup. `alloc_bvh_refs` sets `*refs=NULL` (line 110) before any failure path |
| FP-3 | `close_window` NULL scene dereference | MLX hooks only registered after full init succeeds. `close_window` is never called with NULL scene |
| FP-4 | Race condition on `RENDER_DIRTY` flag | MLX uses single-threaded event loop. All flag reads/writes occur in same thread |
| FP-5 | `shadow_calc.c` light at surface point (division by zero) | `is_in_shadow()` checks `mag < 0.0001` (`shadow_test.c:102`) and returns early, preventing downstream division |
| FP-6 | Negative modulo in `bump_map_perturb.c:52-57` | C99/C11 guarantees `%` result sign matches dividend. The `if (px < 0) px += width` guard at lines 54-57 correctly handles this. **Note:** FP-6 dismisses the modulo sign concern, but the `(int)` truncation-toward-zero issue is a REAL bug — see section 2.3. |
| FP-7 | Buffer overflow in `finalize_line` (parser) | `len` is bounded by `copy_line_content` loop which stops at newline/EOF. Cannot exceed buffer |
| FP-8 | HUD stale `total_pages` after object add/remove | Objects are never added or removed at runtime — scene is immutable after parse |
| FP-9 | `hud_render.c` NULL dereference on uninitialized scene | HUD render callback only registered after full initialization. Scene is always valid when HUD renders |
| FP-10 | `pixel_timing_print.c` P95/P99 array out-of-bounds [v5] | `(size_t)(count * 0.95)` and `(size_t)(count * 0.99)` always produce values `< count`. For `count=480000`: P95 index=456000, P99 index=475200, both valid. |
| FP-11 | `adjust_samples` division by zero when `light_count=0` [v5] | `if (scene->shadow_config.samples < 1) samples = 1` guard prevents zero. Also, parser requires at least 1 light element. |
| FP-12 | `camera.c:71` inv_dir infinite values causing BVH failures [v5] | Standard IEEE 754 ray tracing technique. `1.0/0.0 = ±INF` is well-defined. INF arithmetic in slab test is correct (Williams et al., 2005). The REAL issue is the `0*INF=NaN` case (section 2.1), not INF itself. |
