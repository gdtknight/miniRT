# main.c 함수 호출 관계 (프로젝트 전체 기준)

이 문서는 `src/main.c`를 기준으로, 프로젝트 전체에서 이어지는 함수 호출 관계를
초기화, 파싱, BVH, 렌더링, 입력 처리, 정리 흐름으로 나눠 정리한 것이다.
MLX 콜백은 `mlx_loop()` 이후에 호출되는 흐름으로 별도 섹션에 포함한다.

## 1) 프로그램 시작 (main.c)

```
main(argc, argv)
|- printf()  [사용법 안내]
|- parse_args(argc, argv, &filename, &bvh_vis)
|  |- strcmp()
|  `- printf()  [옵션 오류/중복 파일 안내]
|- initialize_scene_and_render(filename, &scene, &render, bvh_vis)
|  |- init_scene()
|  |  |- malloc()
|  |  |- ft_memset()
|  |  |- init_shadow_config()
|  |  `- render_state_init()
|  |     |- timer_start()
|  |     `- metrics_init()
|  |- print_error()  [씬 초기화 실패 시]
|  |- parse_scene(filename, scene)
|  |  |- open()
|  |  |- read_line()
|  |  |- parse_line()
|  |  |  |- parse_ambient()
|  |  |  |  |- ft_atof()
|  |  |  |  |- in_range()
|  |  |  |  `- parse_color()
|  |  |  |     |- ft_atoi()
|  |  |  |     `- in_range()
|  |  |  |- parse_camera()
|  |  |  |  |- parse_vector()
|  |  |  |  |  `- ft_atof()
|  |  |  |  |- vec3_normalize()
|  |  |  |  `- ft_atof()
|  |  |  |- parse_light()
|  |  |  |  |- parse_vector()
|  |  |  |  |- ft_atof()
|  |  |  |  |- in_range()
|  |  |  |  `- parse_color()
|  |  |  |- parse_sphere()
|  |  |  |  |- parse_vector()
|  |  |  |  |- ft_atof()
|  |  |  |  |- snprintf()
|  |  |  |  `- parse_color()
|  |  |  |- parse_plane()
|  |  |  |  |- parse_vector()
|  |  |  |  |- vec3_normalize()
|  |  |  |  |- snprintf()
|  |  |  |  `- parse_color()
|  |  |  `- parse_cylinder()
|  |  |     |- parse_vector()
|  |  |     |- vec3_normalize()
|  |  |     |- snprintf()
|  |  |     |- parse_cylinder_params()
|  |  |     |  `- ft_atof()
|  |  |     `- parse_color()
|  |  |- validate_scene()
|  |  |  `- print_error()  [필수 요소 누락]
|  |  |- scene_build_bvh()  [파싱 성공 시]
|  |  `- close()
|  |- cleanup_scene()  [파싱 실패 시]
|  |  |- render_state_cleanup()
|  |  |  `- bvh_destroy()
|  |  `- free()
|  |- scene_build_bvh()
|  |- bvh_visualize(bvh, NULL, scene)  [--bvh-vis]
|  |- init_window(scene)
|  |  |- malloc()
|  |  |- init_mlx()
|  |  |  |- mlx_init()
|  |  |  |- mlx_new_window()
|  |  |  |- mlx_new_image()
|  |  |  `- mlx_get_data_addr()
|  |  |- init_render_state()
|  |  |  `- pixel_timing_init()
|  |  |- init_ui_components()
|  |  |  |- hud_init()
|  |  |  |- keyguide_init()
|  |  |  `- hud_calculate_total_pages()
|  |  `- register_hooks()
|  |     |- mlx_hook()       [close_window, handle_key, handle_key_release]
|  |     `- mlx_loop_hook()  [render_loop]
|  |- cleanup_scene()  [윈도우 실패 시]
|  `- print_error()  [윈도우 실패 시]
`- mlx_loop(render->mlx)  [이벤트 루프 진입]
```

## 2) BVH 빌드 경로 (scene_build_bvh)

```
scene_build_bvh(scene)
|- count_objects()
|- fill_object_refs()
|  |- fill_spheres()
|  `- fill_planes_cylinders()
|- bvh_create()
`- bvh_build(bvh, refs, total, scene)
   |- bvh_node_destroy()  [기존 트리 제거]
   `- bvh_build_recursive(objects, count, scene, depth)
      |- compute_bounds()
      |  |- get_object_bounds()
      |  |  |- aabb_for_sphere()
      |  |  |- aabb_for_cylinder()
      |  |  `- aabb_for_plane()
      |  `- aabb_merge()
      |- choose_split_axis()
      |- calculate_split_position()
      |- partition_objects()
      |  `- get_object_center()
      |- create_split_node()
      |  `- bvh_build_recursive()  [left/right]
      `- create_leaf_node()
         |- compute_bounds()
         |- malloc()
         `- free()  [실패 시]
```

## 3) BVH 시각화 경로 (옵션)

```
bvh_visualize(bvh, config, scene)
|- bvh_vis_default_config()
|  `- bvh_vis_get_terminal_width()
|     `- ioctl()
|- check_edge_cases()
|  `- print_warning_message()
|- prefix_init()
|- bvh_visualize_tree()
|  |- format_node_info()
|  |- format_object_list()
|  |- print_node_line()
|  |- prefix_push() / prefix_pop()
|  `- bvh_collect_statistics()
|     `- print_statistics_summary()
`- prefix_destroy()
```

## 4) 이벤트 루프 이후 (MLX 콜백)

```
mlx_loop()
`- render_loop()  [mlx_loop_hook 등록]
   |- metrics_start_frame()
   |  `- timer_start()
   |- render_scene_to_buffer()
   |  |- render_low_quality()  [저품질 모드]
   |  |  |- create_camera_ray()
   |  |  |- trace_ray()
   |  |  `- draw_pixel_block()
   |  |     `- put_pixel_to_buffer()
   |  `- render_pixel()  [고품질 모드]
   |     |- create_camera_ray()
   |     |- get_time_ns()
   |     |- trace_ray()
   |     |- get_time_ns()
   |     |- pixel_timing_add_sample()
   |     `- put_pixel_to_buffer()
   |  |- pixel_timing_calculate_stats()
   |  `- pixel_timing_print_stats()
   |- mlx_put_image_to_window()
   |- metrics_end_frame()
   |  `- timer_elapsed_us()
   |- hud_render()
   |  |- hud_render_background()
   |  |- hud_render_objects()
   |  |  `- render_object_by_index()
   |  |     |- render_sphere_obj()
   |  |     |- render_plane_obj()
   |  |     `- render_cylinder_obj()
   |  `- hud_render_controls()
   `- keyguide_render()
```

## 5) 레이 트레이싱/조명 경로

```
trace_ray(scene, ray)
|- metrics_add_ray()
|- 교차 검사
|  |- bvh_intersect()  [BVH 활성화 시]
|  |  `- bvh_node_intersect()
|  |     |- aabb_intersect()
|  |     `- bvh_leaf_intersect()
|  |        `- intersect_object()
|  |           |- intersect_sphere()
|  |           |- intersect_plane()
|  |           `- intersect_cylinder()
|  `- check_*_intersections()  [BVH 미사용 시]
|     |- check_sphere_intersections()
|     |- check_plane_intersections()
|     `- check_cylinder_intersections()
|- apply_lighting()  [hit_found == 1]
|  |- vec3_subtract()
|  |- vec3_normalize()
|  |- calculate_shadow_factor()
|  |  |- calculate_shadow_bias()
|  |  |- generate_shadow_sample_offset()
|  |  `- is_in_shadow()
|  |     |- intersect_sphere()
|  |     |- intersect_plane()
|  |     `- intersect_cylinder()
|  `- clamp_color()
`- return (0,0,0)  [hit_found == 0]
```

## 6) 입력 처리 경로 (키 이벤트)

```
handle_key(keycode, render)
|- close_window()  [ESC]
|  |- pixel_timing_cleanup()
|  |- keyguide_cleanup()
|  |- hud_cleanup()
|  `- cleanup_all()
|     |- cleanup_render()
|     |  `- mlx_destroy_image()
|     `- cleanup_scene()
|- handle_hud_keys()
|  |- hud_toggle()
|  |- hud_select_next() / hud_select_prev()
|  `- hud_change_page()
|- handle_object_selection()
|  |- select_next_object()
|  |  `- cycle_type_forward()
|  |     `- next_type_from_*()
|  `- select_prev_object()
|     `- cycle_backward_sphere()/cycle_backward_plane()/cycle_backward_cylinder()
|- handle_camera_keys()
|  |- handle_camera_move()
|  |- handle_camera_pitch()
|  `- handle_camera_reset()
`- handle_transform_keys()
   |- handle_object_move()
   `- handle_light_move()

handle_key_release(keycode, render)
`- is_movement_key()
```

## 7) 정리/종료 경로

```
cleanup_all(scene, render)
|- cleanup_render()
|  |- mlx_destroy_image()
|  `- free()
`- cleanup_scene()
   |- render_state_cleanup()
   |  `- bvh_destroy()
   `- free()
```

## 보조 유틸리티 (주요 호출 집합)

- 벡터 연산: `vec3_add`, `vec3_subtract`, `vec3_multiply`, `vec3_dot`,
  `vec3_cross`, `vec3_magnitude`, `vec3_normalize`
- 파서 유틸: `ft_atof`, `ft_atoi`, `in_range`, `parse_vector`, `parse_color`
- 타이머: `get_time_ns`, `timer_start`, `timer_elapsed_us`
