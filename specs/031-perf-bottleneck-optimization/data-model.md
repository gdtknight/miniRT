# Data Model: Performance Bottleneck Optimization

**Feature**: 031-perf-bottleneck-optimization  
**Date**: 2026-02-06

## 구조체 변경 사항

### 1. `t_ray` 확장 (P5)

**파일**: `includes/ray.h`

```c
/* Before */
typedef struct s_ray
{
    t_vec3  origin;
    t_vec3  direction;
}   t_ray;

/* After */
typedef struct s_ray
{
    t_vec3  origin;
    t_vec3  direction;
    t_vec3  inv_dir;    /* 1.0 / direction (IEEE 754 inf 허용) */
}   t_ray;
```

**변경 사유**: AABB slab method에서 나눗셈을 곱셈으로 대체

---

### 2. `t_camera_cache` 신규 + `t_camera` 확장 (P2)

**파일**: `includes/minirt.h`

```c
/* 신규 구조체 */
typedef struct s_camera_cache
{
    t_vec3  right;          /* 카메라 오른쪽 벡터 */
    t_vec3  up;             /* 카메라 위쪽 벡터 */
    double  aspect_ratio;   /* width / height */
    double  fov_scale;      /* tan(fov * 0.5 * PI / 180) */
    int     valid;          /* 캐시 유효 여부 (dirty flag, 0=invalid, 1=valid) */
}   t_camera_cache;

/* 확장된 구조체 */
typedef struct s_camera
{
    t_vec3          position;
    t_vec3          direction;
    t_vec3          initial_position;
    t_vec3          initial_direction;
    double          fov;
    t_camera_cache  cache;  /* 추가 */
}   t_camera;
```

**변경 사유**: 프레임당 basis 계산을 1회로 제한

---

### 3. `t_bvh_node` 확장 (P6)

**파일**: `includes/spatial.h`

```c
/* Before */
typedef struct s_bvh_node
{
    t_aabb              bounds;
    struct s_bvh_node   *left;
    struct s_bvh_node   *right;
    t_object_ref        *objects;
    int                 object_count;
    int                 depth;
}   t_bvh_node;

/* After */
typedef struct s_bvh_node
{
    t_aabb              bounds;
    struct s_bvh_node   *left;
    struct s_bvh_node   *right;
    t_object_ref        *objects;
    int                 object_count;
    int                 depth;
    int                 split_axis; /* 추가: 0=X, 1=Y, 2=Z */
}   t_bvh_node;
```

**변경 사유**: child ordering에서 near/far 결정에 필요

---

## 함수 시그니처

### 신규 함수

```c
/* P1: Shadow ray any-hit */
bool    bvh_intersect_any(t_bvh *bvh, t_ray ray, double max_dist, void *scene);

/* P2: Camera cache 갱신 */
void    update_camera_cache(t_camera *camera);
void    invalidate_camera_cache(t_camera *camera);
```

### 수정 함수

```c
/* P5: inv_dir 인자 활용 (내부 구현 변경) */
int     aabb_intersect(t_aabb box, t_ray ray, double *t_min, double *t_max);
/* → ray.inv_dir 사용으로 내부 나눗셈 제거 */

/* P6: child ordering (내부 구현 변경) */
static int  traverse_children(t_bvh_node *node, t_ray ray, 
                              t_hit_record *hit, void *scene);
/* → split_axis 기반 near/far 결정 */
```

---

## 상태 전이

### Camera Cache 상태

```
[INVALID] ──(render start)──> [compute & validate] ──> [VALID]
                                      ↑
                                      |
[VALID] ──(camera move/rotate)──> [INVALID]
        ──(FOV change)──────────> [INVALID]
```

**트리거**:
- `handle_camera_move()` → `cache.valid = false`
- `handle_camera_pitch()` → `cache.valid = false`
- `handle_key_fov()` → `cache.valid = false`

---

## 메모리 영향

| 구조체 | 추가 필드 | 크기 증가 | 인스턴스 수 | 총 증가량 |
|--------|----------|----------|------------|----------|
| `t_ray` | `inv_dir` | +24 bytes | 임시 (스택) | 무시 가능 |
| `t_camera` | `cache` | +56 bytes | 1개 | +56 bytes |
| `t_bvh_node` | `split_axis` | +4 bytes | ~N개 | +4N bytes |

**총 예상 증가량**: ~수백 바이트 (무시 가능)

---

## Round 2 구조체 변경 사항 (PA+PB) — 설계 단계, 미구현

> **상태**: 아래 변경 사항은 Round 2 **설계 계획**이며 아직 코드에 반영되지 않았습니다. 구현은 tasks.md Phase 8-9에서 진행 예정.

### 4. `t_plane_refs` 신규 (PA)

**파일**: `includes/spatial.h`

```c
/* 신규 구조체: BVH에서 분리된 plane 인덱스 리스트 */
typedef struct s_plane_refs
{
    int    *indices;      /* plane 오브젝트 인덱스 배열 */
    int    count;         /* plane 개수 */
}   t_plane_refs;
```

**변경 사유**: Plane을 BVH에서 제외하고 별도 순회하기 위한 인덱스 관리

---

### 5. `t_bvh` 확장 (PA)

**파일**: `includes/spatial.h`

```c
/* Before */
typedef struct s_bvh
{
    t_bvh_node  *root;
    int         enabled;
    int         total_nodes;
    int         max_depth;
    int         visualize;
}   t_bvh;

/* After */
typedef struct s_bvh
{
    t_bvh_node   *root;
    int          enabled;
    int          total_nodes;
    int          max_depth;
    int          visualize;
    t_plane_refs plane_refs;   /* 추가: BVH에서 분리된 plane 인덱스 */
}   t_bvh;
```

**변경 사유**: BVH 구조체가 plane 정보를 함께 관리하여 순회 시 접근 가능

---

### 6. `SHADOW_BVH_THRESHOLD` 제거 (PB)

**파일**: `includes/shadow.h`

```c
/* Before */
#define SHADOW_BVH_THRESHOLD 20

/* After */
/* 삭제 — BVH가 존재하면 항상 사용 */
```

**변경 사유**: PA 적용 후 BVH 품질 향상으로 소규모 씬에서도 BVH가 효율적

---

## Round 2 함수 시그니처 — 설계 단계, 미구현

### 신규 함수

```c
/* PA: Plane 별도 intersection 테스트 */
int     intersect_planes(t_bvh *bvh, t_ray ray,
            t_hit_record *hit, void *scene);

/* PA: Plane 별도 shadow 테스트 */
int     check_plane_shadow(t_bvh *bvh, t_ray ray,
            double max_dist, void *scene);
```

### 수정 함수

```c
/* PA: BVH 빌드 시 plane 필터링 */
void    scene_build_bvh(t_scene *scene);
/* → plane을 분리하고 bounded 오브젝트만 BVH 구축 */

/* PB: Shadow 테스트에서 threshold 제거 */
int     is_in_shadow(t_scene *scene, t_vec3 point,
            t_vec3 light_pos, double bias);
/* → BVH 항상 사용 + plane 별도 검사 */
```

---

## Round 2 메모리 영향 — 설계 단계, 미구현

| 구조체 | 추가 필드 | 크기 증가 | 인스턴스 수 | 총 증가량 |
|--------|----------|----------|------------|----------|
| `t_bvh` | `plane_refs` | +16 bytes (ptr+int+padding) | 1개 | +16 bytes |
| `t_plane_refs` | (전체) | ~(4*N + 8) bytes | 1개 | ~24 bytes (plane 4개 기준) |

**총 추가량**: ~40 bytes (무시 가능)
**해제 필요**: `bvh_destroy()` 시 `plane_refs.indices` free 추가
