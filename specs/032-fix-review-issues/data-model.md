# Data Model: 032-fix-review-issues

**Date**: 2026-02-10

## 구조체 변경

### 변경 없음

이번 피처는 기존 구조체를 수정하지 않습니다.
모든 수정은 함수 로직 레벨에서 이루어집니다.

### 참조 구조체 (변경 없음, 맥락 참고)

```c
/* includes/spatial.h */
typedef struct s_plane_refs
{
    int     *indices;   /* R1: NULL 가드 대상, R2: free 누수 대상 */
    int     count;
}   t_plane_refs;

typedef struct s_bvh
{
    t_bvh_node      *root;
    int             enabled;
    int             total_nodes;
    int             max_depth;
    int             visualize;
    t_plane_refs    plane_refs;
}   t_bvh;
```

```c
/* includes/shadow.h */
typedef struct s_shadow_config
{
    int     samples;        /* R4: 변경 시 LUT 재생성 필요 */
    double  softness;
    double  bias_scale;
    int     enable_ao;
    t_vec3  *offset_lut;    /* R4: samples 크기에 연동 */
}   t_shadow_config;
```

## 함수 시그니처 변경

### 신규 함수

```c
/* src/spatial/bvh_init.c (static) */
static int  alloc_bvh_refs(t_scene *scene, t_object_ref **refs, int pc, int bc);
```

### 수정 함수 (시그니처 변경 없음)

| 함수 | 파일 | 변경 내용 |
|------|------|-----------|
| `scene_build_bvh()` | bvh_init.c | alloc_bvh_refs 호출로 리팩터 |
| `set_shadow_samples()` | shadow_config.c | LUT free + 재생성 추가 |
| `parse_camera()` | parse_elements.c | in_range 상수 변경 |
