# Quickstart: Code Hardening

**Feature**: 024-code-hardening
**Date**: 2026-01-31

## Overview

이 문서는 024-code-hardening 기능의 빠른 구현 가이드입니다. 9개의 결함 수정을 우선순위 순으로 나열합니다.

---

## Prerequisites

- 023-parsing-robustness 기능이 병합된 develop 브랜치
- 42 Norm v4.1 준수 컴파일 환경
- 테스트 장면 파일 (scenes/valid/*, scenes/invalid/*)

---

## Implementation Order

### Phase 1: High Priority (렌더링 정확성)

#### 1.1 BVH Distance 초기화 (FR-001)

**파일**: `src/spatial/bvh_traverse.c`

```c
// bvh_leaf_intersect() 함수 시작 부분
static int  bvh_leaf_intersect(t_bvh_node *node, t_ray ray, t_hit_record *hit,
        void *scene)
{
    int             i;
    int             hit_anything;
    t_hit_record    temp_hit;

    temp_hit.distance = hit->distance;  // ADD THIS LINE
    hit_anything = 0;
    // ...
}
```

**테스트**: 10+ 오브젝트 장면을 100회 렌더링하여 일관성 확인

#### 1.2 AABB Div-by-Zero 방지 (FR-002)

**파일**: `src/spatial/aabb.c`

```c
// 새 헬퍼 함수 추가 (update_bounds 위)
static void safe_slab_axis(double origin, double dir, double box_min,
        double box_max, double *tmin, double *tmax)
{
    double  inv_d;
    double  t0;
    double  t1;

    if (fabs(dir) < 1e-8)
    {
        if (origin < box_min || origin > box_max)
        {
            *tmin = 1e30;
            *tmax = -1e30;
        }
        return ;
    }
    inv_d = 1.0 / dir;
    t0 = (box_min - origin) * inv_d;
    t1 = (box_max - origin) * inv_d;
    update_bounds(tmin, tmax, t0, t1);
}

// aabb_intersect() 수정
int aabb_intersect(t_aabb box, t_ray ray, double *t_min, double *t_max)
{
    double  tmin;
    double  tmax;

    tmin = *t_min;
    tmax = *t_max;
    safe_slab_axis(ray.origin.x, ray.direction.x, box.min.x, box.max.x,
        &tmin, &tmax);
    safe_slab_axis(ray.origin.y, ray.direction.y, box.min.y, box.max.y,
        &tmin, &tmax);
    safe_slab_axis(ray.origin.z, ray.direction.z, box.min.z, box.max.z,
        &tmin, &tmax);
    *t_min = tmin;
    *t_max = tmax;
    return (tmax >= tmin && tmax > 0);
}
```

**테스트**: 카메라 방향 (0,0,1), (1,0,0), (0,0,-1)로 렌더링

---

### Phase 2: Medium Priority (특정 조건 오동작)

#### 2.1 카메라 Gimbal Lock 방지 (FR-003)

**파일**: `src/render/camera.c`

```c
static void init_camera_calc(t_camera *camera, t_cam_calc *calc)
{
    t_vec3  world_up;
    double  dot;

    calc->aspect_ratio = ASPECT_RATIO_NUM / ASPECT_RATIO_DEN;
    calc->fov_scale = tan(camera->fov * 0.5 * M_PI / 180.0);
    world_up = (t_vec3){0, 1, 0};
    dot = fabs(vec3_dot(camera->direction, world_up));
    if (dot > 0.999)
        world_up = (t_vec3){0, 0, 1};
    calc->right = vec3_normalize(vec3_cross(camera->direction, world_up));
    calc->up = vec3_normalize(vec3_cross(calc->right, camera->direction));
}
```

**테스트**: 카메라 방향 (0,1,0), (0,-1,0)으로 렌더링

#### 2.2 프로그레시브 타일 너비 (FR-004)

**파일**: `includes/render_progressive.h`

```c
typedef struct s_progressive_state
{
    int     tile_size;
    int     total_tiles;
    int     current_tile;
    int     tiles_completed;
    int     enabled;
    int     width;
    int     height;
}   t_progressive_state;
```

**파일**: `src/render/render_progressive.c`

```c
void progressive_init(t_progressive_state *prog, int width, int height,
        int tile_size)
{
    // ... existing code ...
    prog->width = width;
    prog->height = height;
}

int progressive_next_tile(t_progressive_state *prog, t_tile_rect *rect)
{
    int tiles_x;
    // ...
    tiles_x = (prog->width + prog->tile_size - 1) / prog->tile_size;
    // ... rest unchanged
}
```

**테스트**: WINDOW_WIDTH=1024, 640으로 빌드 후 렌더링

#### 2.3 그림자 표면 노멀 (FR-005)

**파일**: `src/lighting/shadow_calc.c`

```c
// 함수 시그니처 변경
static double calc_shadow_samples(t_scene *scene, t_vec3 point,
        t_vec3 light_pos, t_shadow_config *config, t_vec3 surface_normal)
{
    // ...
    params.bias = calculate_shadow_bias(surface_normal, light_dir, 0.001);
    // ...
}

double calculate_shadow_factor(t_scene *scene, t_vec3 point,
        t_vec3 light_pos, t_shadow_config *config, t_vec3 surface_normal)
{
    double shadow_count;
    shadow_count = calc_shadow_samples(scene, point, light_pos, config,
            surface_normal);
    return (shadow_count / (double)config->samples);
}
```

**호출 지점 업데이트**: lighting.c 등에서 `hit.normal` 전달

#### 2.4 I/O 오류 플래그 (FR-006)

**파일**: `includes/parser.h`

```c
typedef struct s_line_reader
{
    // ... existing fields ...
    int     io_error;
}   t_line_reader;

typedef enum e_parse_result
{
    // ... existing codes ...
    PARSE_ERR_IO,
    PARSE_ERR_COUNT
}   t_parse_result;
```

**파일**: `src/parser/parse_line_reader.c`

```c
static int refill_buffer(t_line_reader *reader)
{
    reader->buf_len = read(reader->fd, reader->buffer,
            LINE_READER_BUFFER_SIZE);
    reader->buf_pos = 0;
    if (reader->buf_len < 0)
    {
        reader->io_error = 1;
        reader->buf_len = 0;
    }
    return (reader->buf_len);
}
```

**파일**: `src/parser/parser_utils.c` (line_reader_init)

```c
reader->io_error = 0;
```

---

### Phase 3: Low Priority (방어적 코딩)

#### 3.1 정수 오버플로 검사 (FR-007)

**파일**: `src/parser/parse_number.c`

```c
#include <limits.h>

static int parse_int_digits(const char **str, int *result, int *has_digits)
{
    int digit;

    while (parse_is_digit(**str))
    {
        digit = **str - '0';
        if (*result > (INT_MAX - digit) / 10)
            return (0);
        *result = *result * 10 + digit;
        (*str)++;
        *has_digits = 1;
    }
    return (1);
}
```

#### 3.2 Capacity 오버플로 검사 (FR-008)

**파일**: `src/scene/object_list.c`

```c
#include <limits.h>

int object_list_grow(t_object_list *list)
{
    // ...
    if (list->capacity > INT_MAX / 2)
        return (0);
    new_capacity = list->capacity * 2;
    // ...
}
```

#### 3.3 픽셀 범위 검사 (FR-009)

**파일**: `src/window/mlx_pixel.c`

```c
void mlx_img_put_pixel(t_mlx_img *img, int x, int y, int color)
{
    char    *dst;

    if (!img || !img->data)
        return ;
    if (x < 0 || x >= img->width || y < 0 || y >= img->height)
        return ;
    dst = img->data + (y * img->size_line + x * (img->bpp / 8));
    *(unsigned int *)dst = color;
}

int mlx_img_get_pixel(t_mlx_img *img, int x, int y)
{
    char    *src;

    if (!img || !img->data)
        return (0);
    if (x < 0 || x >= img->width || y < 0 || y >= img->height)
        return (0);
    src = img->data + (y * img->size_line + x * (img->bpp / 8));
    return (*(unsigned int *)src);
}
```

**참고**: `t_mlx_img`에 width/height 필드 추가 필요할 수 있음.

---

## Verification

### 1. 빌드 확인

```bash
make re
norminette src/ includes/
```

### 2. 회귀 테스트

```bash
for scene in scenes/valid/*.rt; do
    ./miniRT "$scene" &
    sleep 2
    kill $! 2>/dev/null
done
```

### 3. 엣지 케이스 테스트

```bash
# 축 정렬 카메라
./miniRT scenes/valid/valid_camera_angle.rt

# 수직 카메라 (테스트 장면 생성 필요)
./miniRT scenes/test_vertical_camera.rt

# 오버플로 입력
./miniRT scenes/invalid/invalid_number_format.rt
```

---

## Common Issues

1. **Norm 위반**: 함수 25줄 초과 시 분리 필요
2. **t_mlx_img width/height**: 구조체에 필드 없으면 추가
3. **그림자 호출 지점**: calculate_shadow_factor 호출 모두 업데이트 필요
