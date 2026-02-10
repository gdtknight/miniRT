# miniRT 함수 호출 관계도

## 프로그램 진입점: main()
`src/main.c`

```
main()
├── parse_args()                      # 명령행 인자 파싱
│   └── strcmp()                      # --bvh-vis 옵션 체크
│
├── initialize_scene_and_render()     # 씬과 렌더러 초기화
│   ├── init_scene()                  # 씬 구조체 초기화
│   │   ├── malloc()
│   │   ├── ft_memset()              # src/utils/memory.c
│   │   ├── init_shadow_config()     # src/lighting/shadow_config.c
│   │   └── render_state_init()      # src/render/render_state.c
│   │
│   ├── parse_scene()                 # src/parser/parser.c - 씬 파일 파싱
│   │   ├── open()
│   │   ├── read_line()              # 한 줄씩 읽기
│   │   ├── parse_line()             # 라인별 파싱
│   │   │   ├── parse_ambient()      # src/parser/parse_elements.c - A 요소
│   │   │   │   ├── ft_atof()
│   │   │   │   ├── in_range()
│   │   │   │   └── parse_color()
│   │   │   ├── parse_camera()       # src/parser/parse_elements.c - C 요소
│   │   │   │   ├── parse_vector()
│   │   │   │   ├── vec3_normalize() # src/math/vector_ops.c
│   │   │   │   └── ft_atof()
│   │   │   ├── parse_light()        # src/parser/parse_elements.c - L 요소
│   │   │   │   ├── parse_vector()
│   │   │   │   └── parse_color()
│   │   │   ├── parse_sphere()       # src/parser/parse_objects.c - sp 요소
│   │   │   │   ├── parse_vector()
│   │   │   │   └── parse_color()
│   │   │   ├── parse_plane()        # src/parser/parse_objects.c - pl 요소
│   │   │   │   ├── parse_vector()
│   │   │   │   └── parse_color()
│   │   │   └── parse_cylinder()     # src/parser/parse_objects.c - cy 요소
│   │   │       ├── parse_vector()
│   │   │       └── parse_color()
│   │   ├── validate_scene()         # src/parser/parser.c - 씬 검증
│   │   ├── scene_build_bvh()        # src/spatial/bvh_init.c - BVH 트리 구축
│   │   │   ├── count_objects()
│   │   │   ├── fill_object_refs()
│   │   │   ├── bvh_create()         # src/spatial/bvh_lifecycle.c
│   │   │   └── bvh_build()          # src/spatial/bvh_build_core.c
│   │   │       ├── compute_bounds() # src/spatial/bounds.c
│   │   │       ├── bvh_build_recursive()
│   │   │       │   ├── bvh_create_leaf() # src/spatial/bvh_build_core.c
│   │   │       │   ├── find_best_split() # src/spatial/bvh_build_split.c
│   │   │       │   └── partition_objects() # src/spatial/bvh_build_partition.c
│   │   │       └── aabb_expand()    # src/spatial/aabb_basic.c
│   │   └── close()
│   │
│   ├── cleanup_scene()               # src/utils/cleanup.c - 실패시 정리
│   │   ├── free()
│   │   └── bvh_destroy()            # src/spatial/bvh_lifecycle.c
│   │
│   ├── bvh_visualize()               # src/bvh_vis/bvh_vis_init.c - BVH 시각화 (옵션)
│   │   ├── bvh_visualize_tree()     # src/bvh_vis/bvh_vis_tree.c
│   │   │   ├── print_node_info()
│   │   │   ├── traverse_recursive()
│   │   │   └── print_statistics_summary()
│   │   └── bvh_collect_statistics() # src/bvh_vis/bvh_vis_stats.c
│   │
│   └── init_window()                 # src/window/window_init.c - 윈도우 초기화
│       ├── malloc()
│       ├── init_mlx()               # MLX 초기화
│       │   ├── mlx_init()           # MLX 라이브러리
│       │   ├── mlx_new_window()     # 800x600 윈도우 생성
│       │   ├── mlx_new_image()      # 이미지 버퍼 생성
│       │   └── mlx_get_data_addr()  # 이미지 데이터 포인터
│       ├── init_render_state()      # 렌더 상태 초기화
│       │   └── pixel_timing_init()  # src/render/pixel_timing.c
│       ├── init_ui_components()     # UI 컴포넌트 초기화
│       │   ├── hud_init()           # src/hud/hud_init.c
│       │   ├── keyguide_init()      # src/keyguide/keyguide.c
│       │   └── hud_calculate_total_pages()
│       └── register_hooks()         # 이벤트 핸들러 등록
│           ├── mlx_hook()           # 키보드/윈도우 이벤트
│           │   ├── close_window()   # src/window/window_lifecycle.c
│           │   ├── handle_key()     # src/window/window_events.c
│           │   └── handle_key_release() # src/window/window_events.c
│           └── mlx_loop_hook()      # 렌더링 루프 등록
│               └── render_loop()    # src/window/window_loop.c
│
└── mlx_loop()                        # MLX 이벤트 루프 시작 (메인 루프)
```

---

## 렌더링 루프: render_loop()
`src/window/window_loop.c`

```
render_loop()
├── metrics_start_frame()             # src/render/metrics_frame.c
│
├── render_scene_to_buffer()          # src/render/render.c - 씬 렌더링
│   ├── render_low_quality()         # 저품질 모드 (빠른 프리뷰)
│   │   ├── create_camera_ray()      # src/render/camera.c
│   │   ├── trace_ray()              # src/render/trace.c - 레이 트레이싱
│   │   │   ├── trace_bvh()          # src/spatial/bvh_traverse.c - BVH 사용
│   │   │   │   ├── aabb_intersect() # src/spatial/aabb.c
│   │   │   │   ├── intersect_sphere() # src/ray/intersect_sphere.c
│   │   │   │   ├── intersect_plane()  # src/ray/intersect_plane.c
│   │   │   │   └── intersect_cylinder() # src/ray/intersect_cylinder.c
│   │   │   └── apply_lighting()     # src/lighting/lighting.c
│   │   │       ├── vec3_normalize() # src/math/vector_ops.c
│   │   │       ├── vec3_dot()       # src/math/vector_ops.c
│   │   │       ├── calculate_specular() # Phong 반사
│   │   │       ├── calculate_shadow_factor() # src/lighting/shadow_calc.c
│   │   │       │   ├── calculate_shadow_bias()
│   │   │       │   ├── generate_shadow_sample_offset()
│   │   │       │   └── is_in_shadow() # src/lighting/shadow_test.c
│   │   │       │       ├── check_sphere_shadow()
│   │   │       │       ├── check_plane_shadow()
│   │   │       │       └── check_cylinder_shadow()
│   │   │       └── clamp_color()
│   │   └── draw_pixel_block()       # 2x2 블록 렌더링
│   │       └── put_pixel_to_buffer()
│   │
│   └── render_pixel()               # 풀 품질 모드 (픽셀별)
│       ├── create_camera_ray()      # src/render/camera.c
│       ├── get_time_ns()            # src/render/pixel_timing.c
│       ├── trace_ray()              # src/render/trace.c
│       ├── pixel_timing_add_sample() # src/render/pixel_timing.c
│       └── put_pixel_to_buffer()
│
├── mlx_put_image_to_window()         # 화면에 이미지 출력
│
├── hud_render()                      # src/hud/hud_render.c - HUD 렌더링
│   ├── hud_render_background()
│   ├── hud_render_stats()
│   ├── hud_render_objects()
│   │   └── render_object_by_index()
│   │       ├── render_sphere_obj()
│   │       ├── render_plane_obj()
│   │       └── render_cylinder_obj()
│   └── hud_render_controls()
│
├── keyguide_render()                 # src/keyguide/keyguide.c - 키가이드
│
└── metrics_end_frame()               # src/render/metrics_frame.c
    ├── metrics_calculate_fps()      # src/render/metrics_calc.c
    └── metrics_update_averages()    # src/render/metrics_calc.c
```

---

## 키보드 이벤트: handle_key()
`src/window/window_events.c`

```
handle_key()
├── Shift 키 처리
│   └── render->shift_pressed = 1
│
├── ESC (53) - 종료
│   └── close_window()               # src/window/window_lifecycle.c
│       └── cleanup_all()            # src/window/window_lifecycle.c
│           ├── cleanup_render()
│           │   ├── mlx_destroy_image()
│           │   ├── hud_cleanup()
│           │   └── free()
│           └── cleanup_scene()      # src/utils/cleanup.c
│
├── R (15) - 카메라 리셋
│   └── handle_camera_reset()        # src/window/window_camera.c
│       └── hud_mark_dirty()
│
├── H (4) - HUD 토글
│   └── hud_toggle()                 # src/hud/hud_render.c
│
├── TAB (48) - 객체 선택
│   └── handle_object_selection()    # src/window/window_key_handlers.c
│       ├── select_next_object()     # src/window/window_selection.c
│       ├── select_prev_object()     # src/window/window_selection.c
│       └── hud_mark_dirty()
│
├── WASD (13,0,1,2) - 카메라/라이트/객체 이동
│   ├── handle_camera_move()         # src/window/window_camera.c
│   ├── handle_light_move()          # src/window/window_key_handlers.c
│   └── handle_object_move()         # src/window/window_objects.c
│       ├── scene_build_bvh()        # BVH 재구축
│       └── hud_mark_dirty()
│
├── 방향키 (123,124,125,126) - 카메라 회전
│   └── handle_camera_pitch()        # src/window/window_camera.c
│       └── hud_mark_dirty()
│
├── Q/E - HUD 페이지 전환
│   └── hud_change_page()            # src/hud/hud_render.c
│
└── Z/X/C - 객체 타입 순환
    ├── cycle_type_forward()         # src/window/window_select_cycle.c
    ├── cycle_type_backward()
    ├── cycle_backward_sphere()
    ├── cycle_backward_plane()
    └── cycle_backward_cylinder()
```

---

## 레이 트레이싱: trace_ray()
`src/render/trace.c`

```
trace_ray()
├── BVH 활성화 시
│   └── trace_bvh()                  # src/spatial/bvh_traverse.c
│       ├── bvh_intersect()          # BVH 트리 순회
│       │   ├── aabb_intersect()     # src/spatial/aabb.c - 바운딩 박스 검사
│       │   │   ├── aabb_intersect_ray()
│       │   │   └── aabb_contains()
│       │   ├── intersect_sphere()   # src/ray/intersect_sphere.c
│       │   ├── intersect_plane()    # src/ray/intersect_plane.c
│       │   └── intersect_cylinder() # src/ray/intersect_cylinder.c
│       │       ├── intersect_cylinder_caps()
│       │       └── intersect_cylinder_body()
│       └── apply_lighting()         # src/lighting/lighting.c
│
└── BVH 비활성화 시 (직접 순회)
    ├── intersect_sphere()           # 모든 구체 검사
    ├── intersect_plane()            # 모든 평면 검사
    ├── intersect_cylinder()         # 모든 실린더 검사
    └── apply_lighting()             # 조명 계산
        ├── vec3_normalize()         # src/math/vector_ops.c
        ├── vec3_subtract()          # src/math/vector.c
        ├── vec3_dot()               # src/math/vector_ops.c
        ├── calculate_specular()     # Phong 스페큘러
        │   ├── vec3_dot()
        │   └── pow()
        ├── calculate_shadow_factor() # src/lighting/shadow_calc.c
        │   ├── calculate_shadow_bias() # src/lighting/shadow_calc.c
        │   ├── generate_shadow_sample_offset() # 소프트 쉐도우 샘플링
        │   └── is_in_shadow()       # src/lighting/shadow_test.c
        │       ├── vec3_magnitude() # src/math/vector_ops.c
        │       ├── check_sphere_shadow()
        │       ├── check_plane_shadow()
        │       └── check_cylinder_shadow()
        └── clamp_color()            # RGB 값 제한 [0-255]
```

---

## 유틸리티 함수들

### 벡터 연산 (src/math/)
- `vec3_add()` - 벡터 덧셈
- `vec3_subtract()` - 벡터 뺄셈
- `vec3_multiply()` - 스칼라 곱셈
- `vec3_dot()` - 내적
- `vec3_cross()` - 외적
- `vec3_magnitude()` - 크기 계산
- `vec3_normalize()` - 정규화

### 메모리 관리 (src/utils/)
- `ft_memset()` - 메모리 초기화
- `ft_memcpy()` - 메모리 복사
- `cleanup_scene()` - 씬 정리
- `cleanup_render()` - 렌더러 정리

### 에러 처리 (src/utils/)
- `print_error()` - 에러 메시지 출력
- `print_warning()` - 경고 메시지 출력

### 파싱 유틸리티 (src/parser/)
- `ft_atof()` - 문자열을 실수로 변환
- `parse_vector()` - 벡터 파싱
- `parse_color()` - 색상 파싱 (RGB)
- `in_range()` - 범위 검증

---

## 주요 데이터 구조

### t_scene
씬의 모든 데이터를 담는 최상위 구조체
- 환경광(ambient), 카메라(camera), 조명(light)
- 오브젝트 배열 (spheres, planes, cylinders)
- BVH 트리 (render_state.bvh)
- 쉐도우 설정 (shadow_config)

### t_render
렌더링 상태와 MLX 관련 데이터
- MLX 포인터, 윈도우, 이미지 버퍼
- 씬 포인터
- 선택된 객체 정보 (selection)
- HUD, 키가이드
- 픽셀 타이밍 통계

### t_bvh
BVH (Bounding Volume Hierarchy) 트리
- root: 루트 노드
- visualize: 시각화 플래그

### t_bvh_node
BVH 트리의 노드
- bounds: AABB 바운딩 박스
- left, right: 자식 노드
- objects: 리프 노드의 오브젝트 배열
- is_leaf: 리프 노드 여부

---

## 성능 최적화 기법

1. **BVH (Bounding Volume Hierarchy)**
   - 공간 분할 자료구조로 레이-오브젝트 교차 검사 최적화
   - O(n) → O(log n) 복잡도 개선

2. **Low Quality Mode**
   - 2x2 픽셀 블록 렌더링으로 4배 속도 향상
   - 실시간 상호작용 시 사용

3. **Direct Buffer Writing**
   - mlx_pixel_put 대신 직접 메모리 버퍼에 쓰기
   - 렌더링 속도 대폭 향상

4. **Soft Shadow Sampling**
   - 다중 샘플링으로 부드러운 그림자 생성
   - 샘플 수 조절 가능 (성능 vs 품질)

5. **Pixel Timing Statistics**
   - 픽셀당 렌더링 시간 측정
   - 성능 분석 및 최적화 지표

---

## 빌드 및 실행

```bash
# 빌드
make

# 실행
./miniRT scenes/scene.rt

# BVH 트리 시각화
./miniRT scenes/scene.rt --bvh-vis
```

## 주요 키 바인딩

- `ESC`: 프로그램 종료
- `H`: HUD 토글
- `R`: 카메라 리셋
- `WASD`: 카메라/라이트/객체 이동
- `방향키`: 카메라 회전
- `TAB`: 객체 선택
- `Q/E`: HUD 페이지 전환
- `Z/X/C`: 객체 타입 순환
