# Data Structures

miniRT의 핵심 자료구조 정의와 관계를 설명합니다.

---

## 구조체 관계도

```mermaid
flowchart TD
    A[t_scene] --> B[t_object_list]
    A --> C[t_camera]
    A --> D[t_light x MAX_LIGHTS]
    A --> E[t_ambient]
    A --> F[t_shadow_config]
    A --> G[t_bvh]
    A --> H[t_metrics]

    B --> I[t_object]
    I --> J[t_color]
    I --> K[t_sphere_data]
    I --> L[t_plane_data]
    I --> M[t_cylinder_data]
    I --> M2[t_cone_data]

    G --> N[t_bvh_node]
    N --> O[t_aabb]
    N --> P[t_object_ref]

    Q[t_render] --> A
    Q --> R[t_mlx_context]
    Q --> S[t_hud_state]
    Q --> T[t_keyguide_state]
    Q --> U[t_pixel_timing]
    Q --> V[t_debounce_state]

    R --> W[t_mlx_img]
    X[t_ray] --> Y[t_hit]
    Y --> J
    Y --> I
```

---

## 씬 관련

### `t_scene` (includes/minirt.h)

전체 씬 상태를 담는 최상위 구조체.

```c
typedef struct s_scene
{
    t_ambient       ambient;              // 환경광
    t_camera        camera;               // 카메라
    t_light         lights[MAX_LIGHTS];   // 광원 배열 (최대 16)
    int             light_count;          // 현재 광원 수
    int             selected_light;       // 조작 대상 광원 인덱스
    t_shadow_config shadow_config;        // 그림자 설정
    t_object_list   objects;              // 오브젝트 동적 배열
    int             flags;                // 씬 상태 비트 플래그
    t_bvh           *bvh;                 // BVH 트리 포인터
    t_metrics       metrics;              // 성능 메트릭
}   t_scene;
```

플래그: `SCENE_HAS_AMBIENT (0x01)`, `SCENE_HAS_CAMERA (0x02)`, `SCENE_BVH_ENABLED (0x08)`

### `t_camera` (includes/minirt.h)

```c
typedef struct s_camera
{
    t_vec3          position;           // 현재 위치
    t_vec3          direction;          // 시선 방향 (정규화)
    t_vec3          initial_position;   // 리셋용 초기 위치
    t_vec3          initial_direction;  // 리셋용 초기 방향
    double          fov;                // 시야각 (1~179, 정수 파싱)
    double          pitch;              // Euler 피치 (라디안)
    double          yaw;                // Euler 요 (라디안)
    t_camera_cache  cache;              // basis 벡터 캐시
}   t_camera;
```

### `t_camera_cache` (includes/minirt.h)

카메라 basis 벡터를 프레임당 1회만 계산하여 캐싱합니다.

```c
typedef struct s_camera_cache
{
    t_vec3  right;          // 카메라 우측 방향
    t_vec3  up;             // 카메라 상향 방향
    double  aspect_ratio;   // 화면 종횡비
    double  fov_scale;      // FOV 스케일 팩터
    int     valid;          // 캐시 유효성 (dirty flag)
}   t_camera_cache;
```

### `t_color_f` (includes/minirt.h)

조명 계산 시 중간 결과 저장용 부동소수점 색상.

```c
typedef struct s_color_f
{
    double  r;
    double  g;
    double  b;
}   t_color_f;
```

### `t_light` (includes/minirt.h)

```c
typedef struct s_light
{
    t_vec3  position;    // 광원 위치
    double  brightness;  // 밝기 (0.0~1.0)
    t_color color;       // 광원 색상
}   t_light;
```

### `t_ambient` (includes/minirt.h)

```c
typedef struct s_ambient
{
    double  ratio;  // 환경광 강도 (0.0~1.0)
    t_color color;  // 환경광 색상
}   t_ambient;
```

---

## 오브젝트 관련

### `t_object_type` (includes/objects.h)

```c
typedef enum e_object_type
{
    OBJ_NONE = 0,
    OBJ_SPHERE,
    OBJ_PLANE,
    OBJ_CYLINDER,
    OBJ_CONE
}   t_object_type;
```

### `t_object` (includes/objects.h)

통합 오브젝트 구조체. union으로 타입별 데이터를 공유합니다.

```c
typedef struct s_object
{
    t_object_type   type;           // OBJ_SPHERE, OBJ_PLANE, OBJ_CYLINDER, OBJ_CONE
    t_color         color;          // RGB 색상
    t_color         checker_color;  // 체커보드 보조 색상
    int             has_checker;    // 체커보드 활성화 여부
    char            *bump_path;     // 범프맵 XPM 파일 경로
    t_bump_map      *bump_map;      // 범프맵 데이터 (지연 로드)
    char            id[16];         // 식별자 (예: "sp-1", "cy-2")
    union u_object_data
    {
        t_sphere_data   sphere;
        t_plane_data    plane;
        t_cylinder_data cylinder;
        t_cone_data     cone;
    }   data;
}   t_object;
```

### `t_sphere_data`

```c
typedef struct s_sphere_data
{
    t_vec3  center;     // 구 중심
    double  radius;     // 반지름
    double  radius_sq;  // 반지름^2 (캐싱)
}   t_sphere_data;
```

### `t_plane_data`

```c
typedef struct s_plane_data
{
    t_vec3  point;   // 평면 위의 한 점
    t_vec3  normal;  // 법선 벡터 (정규화)
}   t_plane_data;
```

### `t_cylinder_data`

```c
typedef struct s_cylinder_data
{
    t_vec3  center;       // 원기둥 중심
    t_vec3  axis;         // 축 방향 (정규화)
    double  radius;       // 반지름
    double  radius_sq;    // 반지름^2 (캐싱)
    double  half_height;  // 높이/2
}   t_cylinder_data;
```

### `t_cone_data`

```c
typedef struct s_cone_data
{
    t_vec3  center;       // 원뿔 중심 (높이 중앙)
    t_vec3  axis;         // 축 방향 (정규화, center→apex)
    double  radius;       // 밑면 반지름
    double  radius_sq;    // 반지름^2 (캐싱)
    double  half_height;  // 높이/2
}   t_cone_data;
```

### `t_object_list` (includes/minirt.h)

```c
typedef struct s_object_list
{
    t_object    *items;     // 동적 배열
    int         count;      // 현재 개수
    int         capacity;   // 할당 용량 (초기 32)
}   t_object_list;
```

---

## 광선 관련

### `t_ray` (includes/ray.h)

```c
typedef struct s_ray
{
    t_vec3  origin;     // 광선 시작점
    t_vec3  direction;  // 방향 (정규화)
    t_vec3  inv_dir;    // 1/direction (AABB 교차용, 사전 계산)
}   t_ray;
```

### `t_hit` (includes/ray.h)

```c
typedef struct s_hit
{
    double      distance;  // 교차 거리 (t값)
    t_vec3      point;     // 교차점 좌표
    t_vec3      normal;    // 교차점 법선
    t_color     color;     // 오브젝트 색상
    t_object    *obj;      // 교차된 오브젝트 포인터 (체커보드/범프맵 접근용)
}   t_hit;
```

### `t_cyl_calc` (includes/ray.h)

원기둥 교차 계산용 헬퍼 구조체.

```c
typedef struct s_cyl_calc
{
    double  a;
    double  b;
    double  c;
    double  discriminant;
    double  t;
    double  m;
    double  t2;
    double  m2;
}   t_cyl_calc;
```

---

## BVH 관련

### `t_object_ref` (includes/spatial.h)

오브젝트 리스트 내 인덱스 참조.

```c
typedef struct s_object_ref
{
    int  index;
}   t_object_ref;
```

### `t_bvh` (includes/spatial.h)

```c
typedef struct s_bvh
{
    t_bvh_node      *root;         // 트리 루트
    int             enabled;       // 활성화 여부
    int             visualize;     // 시각화 플래그
    t_plane_refs    plane_refs;    // 분리된 plane 인덱스
}   t_bvh;
```

### `t_bvh_node` (includes/spatial.h)

```c
typedef struct s_bvh_node
{
    t_aabb              bounds;        // 바운딩 박스
    struct s_bvh_node   *left;         // 좌측 자식
    struct s_bvh_node   *right;        // 우측 자식
    t_object_ref        *objects;      // 리프 오브젝트 목록
    int                 object_count;  // 리프 오브젝트 수
    int                 depth;         // 노드 깊이
    int                 split_axis;    // 분할 축 (0=x, 1=y, 2=z)
}   t_bvh_node;
```

### `t_aabb` (includes/spatial.h)

```c
typedef struct s_aabb
{
    t_vec3  min;  // 각 축 최솟값으로 구성된 대각선 꼭짓점
    t_vec3  max;  // 각 축 최댓값으로 구성된 대각선 꼭짓점
}   t_aabb;
```

### `t_plane_refs` (includes/spatial.h)

```c
typedef struct s_plane_refs
{
    int     *indices;  // 오브젝트 리스트 내 plane 인덱스 배열
    int     count;     // plane 개수
}   t_plane_refs;
```

### `t_axis_check` (includes/spatial.h)

AABB 교차 검사 시 축별 파라미터.

```c
typedef struct s_axis_check
{
    double  box_min;
    double  box_max;
    double  ray_origin;
    double  inv_dir;
    double  *tmin;
    double  *tmax;
}   t_axis_check;
```

### `t_partition_params` (includes/spatial.h)

BVH 오브젝트 파티션 파라미터.

```c
typedef struct s_partition_params
{
    t_object_ref  *objects;
    int            count;
    int            axis;
    double         split;
    void          *scene;
}   t_partition_params;
```

### `t_split_params` (includes/spatial.h)

BVH 분할 노드 생성 파라미터.

```c
typedef struct s_split_params
{
    t_aabb        bounds;
    t_object_ref  *objects;
    int            mid;
    int            count;
    void          *scene;
    int            depth;
    int            axis;
}   t_split_params;
```

---

## 그림자 관련

### `t_shadow_config` (includes/shadow.h)

```c
typedef struct s_shadow_config
{
    int     samples;      // 그림자 샘플 수 (1=hard, >1=soft)
    double  softness;     // 소프트니스 (0.0~1.0)
    t_vec3  *offset_lut;  // 사전 계산된 offset LUT
}   t_shadow_config;
```

### `t_shadow_query` (includes/shadow.h)

Shadow factor 계산 시 표면 정보 전달용.

```c
typedef struct s_shadow_query
{
    t_vec3  point;   // 표면 위치
    t_vec3  normal;  // 표면 법선 (bias 계산용)
}   t_shadow_query;
```

### `t_shadow_sample` (includes/shadow.h)

Shadow 샘플링 파라미터.

```c
typedef struct s_shadow_sample
{
    t_scene          *scene;
    t_shadow_query   query;      // 표면 위치 + 법선
    t_vec3           light_pos;
    t_shadow_config  *config;
    double           bias;
}   t_shadow_sample;
```

---

## 메트릭 관련

### `t_bvh_metrics` (includes/metrics.h)

```c
typedef struct s_bvh_metrics
{
    long  nodes_visited;   // 방문한 BVH 노드 수
    long  tests_skipped;   // AABB에서 스킵된 테스트 수
}   t_bvh_metrics;
```

### `t_ray_metrics` (includes/metrics.h)

```c
typedef struct s_ray_metrics
{
    long  rays_traced;              // 추적된 전체 레이 수
    long  intersect_tests;          // primary 교차 테스트 수
    long  shadow_intersect_tests;   // shadow 교차 테스트 수
}   t_ray_metrics;
```

### `t_frame_timing` (includes/metrics.h)

```c
typedef struct s_frame_timing
{
    struct timeval  start_time;                        // 프레임 시작 시각
    long            render_time_us;                    // 렌더링 소요 시간 (μs)
    long            frame_times_us[FRAME_HISTORY_SIZE]; // 60-프레임 히스토리
    int             frame_index;                       // 히스토리 인덱스
    long            frame_count;                       // 누적 프레임 수
    double          fps;                               // 초당 프레임
}   t_frame_timing;
```

### `t_metrics` (includes/metrics.h)

```c
typedef struct s_metrics
{
    t_frame_timing  timing;        // 프레임 시간 측정
    t_ray_metrics   ray;           // 레이/교차 카운터
    t_bvh_metrics   bvh;           // BVH 노드 방문/스킵 카운터
}   t_metrics;
```

---

## 렌더 컨텍스트

### `t_mlx_img` (includes/mlx_context.h)

```c
typedef struct s_mlx_img
{
    void  *img;        // MLX 이미지 포인터
    char  *data;       // 픽셀 데이터 버퍼
    int    bpp;        // bits per pixel
    int    size_line;  // 한 줄의 바이트 수
    int    endian;     // 엔디안
    int    width;      // 이미지 너비
    int    height;     // 이미지 높이
}   t_mlx_img;
```

### `t_mlx_context` (includes/mlx_context.h)

```c
typedef struct s_mlx_context
{
    void       *mlx;  // MLX 인스턴스
    void       *win;  // 윈도우 포인터
    t_mlx_img  img;   // 이미지 데이터
}   t_mlx_context;
```

### `t_selection` (includes/render.h)

```c
typedef struct s_selection
{
    t_object_type  type;   // 선택된 오브젝트 타입
    int            index;  // 선택된 오브젝트 인덱스
}   t_selection;
```

### `t_keyguide_state` (includes/render.h)

```c
typedef struct s_keyguide_state
{
    int  visible;  // 표시 여부
    int  x;        // 렌더 위치 X
    int  y;        // 렌더 위치 Y
    int  dirty;    // 재렌더링 필요 여부
}   t_keyguide_state;
```

### `t_pixel_timing` (includes/pixel_timing.h)

```c
typedef struct s_pixel_timing
{
    long    *samples;    // 픽셀 렌더 시간 샘플 배열
    size_t  count;       // 수집된 샘플 수
    size_t  capacity;    // 배열 용량 (MAX_PIXEL_SAMPLES)
    long    min_time;    // 최소 렌더 시간 (ns)
    long    max_time;    // 최대 렌더 시간 (ns)
    long    total_time;  // 총 렌더 시간 (ns)
    int     enabled;     // 활성화 여부
}   t_pixel_timing;
```

### `t_debounce_state_enum` (includes/render_debounce.h)

```c
typedef enum e_debounce_state_enum
{
    DEBOUNCE_IDLE,       // 대기 상태
    DEBOUNCE_ACTIVE,     // 입력 감지 (150ms 대기)
    DEBOUNCE_FINAL,      // Full quality 렌더링
    DEBOUNCE_COOLDOWN    // 쿨다운 (350ms)
}   t_debounce_state_enum;
```

### `t_debounce_timer` (includes/render_debounce.h)

```c
typedef struct s_debounce_timer
{
    struct timeval  last_input_time;  // 마지막 입력 시각
    int             is_active;        // 타이머 활성 여부
    long            delay_ms;         // 대기 시간 (ms)
}   t_debounce_timer;
```

### `t_debounce_state` (includes/render_debounce.h)

```c
typedef struct s_debounce_state
{
    t_debounce_state_enum  state;             // FSM 현재 상태
    t_debounce_timer       timer;             // 입력 지연 타이머
    struct timeval         last_preview_time;  // 마지막 LQ 프리뷰 시각
}   t_debounce_state;
```

### `t_render` (includes/render.h)

```c
typedef struct s_render
{
    t_mlx_context       mlx;            // MiniLibX 핸들
    t_scene             *scene;         // 씬 포인터
    t_selection         selection;      // 선택된 오브젝트
    int                 state_flags;    // 렌더 상태 비트 플래그
    t_hud_state         hud;            // HUD 상태
    t_keyguide_state    keyguide;       // 키가이드 상태
    t_debounce_state    debounce;       // 디바운스 상태
    t_key_binds         key_binds;      // 키 디스패치 테이블
}   t_render;
```

렌더 상태 플래그:

| 플래그 | 값 | 설명 |
|--------|------|------|
| `RENDER_DIRTY` | 0x01 | 재렌더링 필요 |
| `RENDER_RENDERING` | 0x02 | 렌더링 진행 중 |
| `RENDER_LOW_QUALITY` | 0x04 | 저품질 프리뷰 모드 |
| `RENDER_BVH_DIRTY` | 0x20 | BVH 재구축 필요 |
| `RENDER_ENABLE_PIXEL_TIMING` | 0x40 | 픽셀 타이밍 측정 활성화 |
| `RENDER_ENABLE_METRICS_PRINT` | 0x80 | 콘솔 메트릭 출력 활성화 |

---

## 기본 타입

### `t_vec3` (includes/vec3.h)

```c
typedef struct s_vec3
{
    double  x;
    double  y;
    double  z;
}   t_vec3;
```

### `t_color` (includes/objects.h)

```c
typedef struct s_color
{
    int  r;  // 0~255
    int  g;
    int  b;
}   t_color;
```
