# Data Model: 025-dead-code-removal

이 피처는 새로운 데이터 모델을 도입하지 않습니다. 기존 모델에서 미사용 항목을 제거합니다.

## 삭제 대상 타입

### includes/objects.h에서 제거

| 타입 | 사유 |
|------|------|
| `t_sphere` (struct s_sphere) | 미사용 레거시. 현재 t_object + t_sphere_data 사용 |
| `t_plane` (struct s_plane) | 미사용 레거시. 현재 t_object + t_plane_data 사용 |
| `t_cylinder` (struct s_cylinder) | 미사용 레거시. 현재 t_object + t_cylinder_data 사용 |

### includes/render_state.h 전체 제거

| 타입 | 사유 |
|------|------|
| `t_quality_mode` (enum) | t_render에 미포함, 외부 참조 0건 |
| `t_interaction_state` (struct) | t_render에 미포함, 외부 참조 0건 |
| `t_progressive_state` (struct) | t_render에 미포함, 외부 참조 0건 |
| `t_tile_rect` (struct) | 외부 참조 0건 |
| `t_render_state` (struct) | t_render에 미포함, 외부 참조 0건 |

### includes/overlay.h 전체 제거

| 타입 | 사유 |
|------|------|
| `t_hud_data` (struct) | 0 include, 미사용 |
| `t_keyguide_data` (struct) | 0 include, 미사용 |
| `t_hud_overlay` (struct) | 0 include, 미사용 |
| `t_keyguide_overlay` (struct) | 0 include, 미사용 |

## 유지 대상 타입 (변경 없음)

| 타입 | 위치 | 사유 |
|------|------|------|
| `t_object` + union | objects.h | 활성 사용 |
| `t_sphere_data` | objects.h | 활성 사용 |
| `t_plane_data` | objects.h | 활성 사용 |
| `t_cylinder_data` | objects.h | 활성 사용 |
| `t_hud_state` | window.h | 활성 사용 (overlay.h의 t_hud_data와 별개) |
| `t_keyguide_state` | window.h | 활성 사용 |
| `t_render` | window.h | 활성 사용 |

## 삭제 대상 함수

### 파일 단위 삭제 (파일 전체 제거)

| 파일 | 함수 |
|------|------|
| aabb_shapes.c | `aabb_for_sphere`, `aabb_for_cylinder`, `aabb_for_plane` |
| render_state.c | `render_state_init`, `render_state_update` |
| render_quality.c | `get_elapsed_us`, `quality_set_mode`, `quality_should_upgrade`, `quality_handle_interaction` |
| render_progressive.c | `progressive_init`, `progressive_next_tile`, `progressive_reset` |
| format_object_id.c | `format_object_id`, `get_object_type_prefix` |
| ft_atof.c | `ft_atof`, `ft_isspace`, `ft_isdigit`, `parse_integer_part`, `parse_fractional_part` |
| parse_validation.c | `parse_vector`, `parse_color` (in_range는 이동) |
| cleanup.c | `cleanup_scene`, `cleanup_render`, `cleanup_all` |

### 부분 삭제 (함수만 제거)

| 파일 | 함수 | 사유 |
|------|------|------|
| keyguide_render.c | `keyguide_render_background` | 호출자 0건 |
| error.c | `print_error` | 전환 후 미사용 |

### 이동

| 함수 | 소스 | 대상 |
|------|------|------|
| `in_range` | parse_validation.c | parse_validation_strict.c |
