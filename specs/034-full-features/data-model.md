# Data Model: 034-full-features

## Entity Changes

### 1. Cone Data (신규)

```c
typedef struct s_cone_data
{
    t_vec3  center;
    t_vec3  axis;
    double  radius;       /* 밑면 반지름 (diameter / 2) */
    double  radius_sq;    /* radius * radius (캐시) */
    double  half_height;  /* height / 2 */
}   t_cone_data;
```

관계: `t_object.data.cone` (union 멤버 추가)

### 2. Object Type Enum (확장)

```c
typedef enum e_object_type
{
    OBJ_NONE = 0,
    OBJ_SPHERE,
    OBJ_PLANE,
    OBJ_CYLINDER,
    OBJ_CONE          /* 신규 */
}   t_object_type;
```

### 3. Object Struct (확장)

```c
union u_object_data
{
    t_sphere_data   sphere;
    t_plane_data    plane;
    t_cylinder_data cylinder;
    t_cone_data     cone;      /* 신규 */
};
```

체커보드/범프맵 필드 추가:
```c
typedef struct s_object
{
    t_object_type       type;
    t_color             color;
    t_color             checker_color;   /* 신규: 체커보드 보조 색상 */
    int                 has_checker;     /* 신규: 체커보드 활성화 여부 */
    char                *bump_path;      /* 신규: 범프맵 파일 경로 (파싱 시 저장, NULL이면 미적용) */
    t_bump_map          *bump_map;       /* 신규: 범프맵 데이터 (mlx_init 후 지연 로드) */
    char                id[8];
    union u_object_data data;
}   t_object;
```

### 4. Bump Map (신규)

```c
typedef struct s_bump_map
{
    void    *img;         /* mlx_xpm_file_to_image 반환값 */
    char    *data;        /* 이미지 데이터 포인터 */
    int     width;
    int     height;
    int     bpp;
    int     size_line;
    int     endian;
}   t_bump_map;
```

### 5. Light (다중 광원 확장)

```c
# define MAX_LIGHTS 16

typedef struct s_scene
{
    t_ambient       ambient;
    t_camera        camera;
    t_light         lights[MAX_LIGHTS];   /* 변경: light → lights[] */
    int             light_count;          /* 신규: 광원 수 */
    int             selected_light;       /* 신규: 현재 선택된 광원 인덱스 */
    t_shadow_config shadow_config;
    t_object_list   objects;
    int             flags;
    t_bvh           *bvh;
    t_metrics       metrics;
}   t_scene;
```

### 6. Hit Record (obj 포인터 추가)

```c
typedef struct s_hit
{
    bool        hit;
    double      distance;
    t_vec3      point;
    t_vec3      normal;
    t_color     color;
    t_object    *obj;       /* 신규: 교차한 오브젝트 포인터 (체커보드/범프맵 접근용) */
}   t_hit;
```

교차 함수에서 hit 기록 시 `hit->obj = obj` 설정 필요. `apply_lighting()`에서 `hit->obj->has_checker`, `hit->obj->bump_map` 접근.

### 7. BVH (total_nodes 삭제)

```c
typedef struct s_bvh
{
    t_bvh_node  *root;
    /* total_nodes 필드 삭제 */
}   t_bvh;
```

## 씬 파일 형식 확장

### 원뿔 (co)
```
co  50.0,0.0,20.6  0.0,0.0,1.0  14.2  21.42  10,0,255
```
형식: `co [center] [axis] [diameter] [height] [R,G,B]`

### 다중 광원 (L 복수 허용)
```
L  -40.0,50.0,0.0   0.6  255,255,255
L   20.0,30.0,-10.0  0.4  255,0,0
```

### 체커보드 옵션 (선택적 접미사)
```
sp  0,0,20  12.6  255,0,0  checker:0,255,0
pl  0,0,0   0,1,0  200,200,200  checker:50,50,50
```

### 범프맵 옵션 (선택적 접미사)
```
sp  0,0,20  12.6  255,0,0  bump:earth.xpm
sp  0,0,20  12.6  255,0,0  checker:0,255,0  bump:earth.xpm
bump:earth.xpm  checker:0,255,0    # 순서 자유 (FR-009a)
```

### 보너스 옵션 파싱 규칙
- `checker:`와 `bump:` 순서 무관 (FR-009a)
- 미인식 토큰(예: `foo:bar`)은 파싱 에러 (FR-009b)

## Validation Rules

| 필드 | 검증 규칙 |
|------|-----------|
| cone center | 실수 벡터, 범위 제한 없음 |
| cone axis | 정규화 벡터, 각 성분 [-1, 1] |
| cone diameter | 양수 (> 0) |
| cone height | 양수 (> 0) |
| checker color | RGB 각 [0, 255] |
| bump file | .xpm 확장자, 파일 존재 확인 |
| light count | [1, MAX_LIGHTS] (0개이면 파싱 에러) |
| light brightness | [0.0, 1.0] |
| light color | RGB 각 [0, 255] |
