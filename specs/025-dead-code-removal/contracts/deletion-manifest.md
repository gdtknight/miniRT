# Deletion Manifest: 025-dead-code-removal

## 파일 삭제 목록 (13개: 헤더 5 + 소스 8)

### 헤더 (5개)
```
includes/overlay.h
includes/format_object_id.h
includes/render_quality.h
includes/render_progressive.h
includes/render_state.h
```

### 소스 (8개)
```
src/spatial/aabb_shapes.c
src/render/render_state.c
src/render/render_quality.c
src/render/render_progressive.c
src/utils/format_object_id.c
src/utils/ft_atof.c
src/parser/parse_validation.c
src/utils/cleanup.c
```

---

## 헤더 부분 수정 목록

### includes/objects.h
```diff
- /* Legacy type aliases for backward compatibility */
- typedef struct s_sphere { ... } t_sphere;
- typedef struct s_plane { ... } t_plane;
- typedef struct s_cylinder { ... } t_cylinder;
```

### includes/parser.h
```diff
- int   parse_vector(char *str, t_vec3 *vec);
- int   parse_color(char *str, t_color *color);
  (선언 + doxygen 주석 포함)
```

### includes/window_internal.h
```diff
- /* Selection helper functions */
- void  cycle_type_forward(t_render *render);
- void  cycle_backward_sphere(t_render *render);
- void  cycle_backward_plane(t_render *render);
- void  cycle_backward_cylinder(t_render *render);
- int   next_type_from_sphere(t_scene *scene);
- int   next_type_from_plane(t_scene *scene);
- int   next_type_from_cylinder(t_scene *scene);
-
- /* Forward declaration for re-rendering */
- void  render_scene_to_buffer(t_scene *scene, t_render *render);
```

### includes/minirt.h
```diff
- /* Cleanup */
- void  cleanup_all(t_scene *scene, void *param);
-
- /* Utility functions */
- double  ft_atof(const char *str);
```

### includes/error.h
```diff
- int   print_error(const char *message);
```

### includes/keyguide.h
```diff
- void  keyguide_render_background(...);
  (선언 제거)
```

---

## 소스 부분 수정 목록

### src/keyguide/keyguide_render.c
```diff
- keyguide_render_background() 함수 전체 (라인 25-52)
```

### src/utils/error.c
```diff
- print_error() 함수 전체 (라인 83-100)
```

### src/parser/parse_validation_strict.c
```diff
+ int  in_range(double value, double min, double max)
+ {
+   return (value >= min && value <= max);
+ }
  (parse_validation.c에서 이동)
```

### src/window/window_lifecycle.c
```diff
- cleanup_all(render->scene, render);
+ t_scene *scene;
+ scene = render->scene;
+ render_destroy(render);
+ scene_destroy(scene);
```

### src/parser/parser.c
```diff
  validate_scene():
- return (print_error("Missing ambient lighting (A)"));
+ return (error_print(ERR_PARSE_MISSING), 0);
  (4곳 동일 패턴)

  parse_scene():
- return (print_error("Invalid file extension (expected .rt)"));
+ return (error_print(ERR_FILE_EXT), 0);
- return (print_error("Cannot open file"));
+ return (error_print(ERR_FILE_OPEN), 0);
- return (print_error("Failed to initialize line reader"));
+ return (error_print(ERR_MALLOC), 0);
```

---

## Makefile SRCS 제거 행 (8개)

```
$(SRC_DIR)/spatial/aabb_shapes.c
$(SRC_DIR)/render/render_state.c
$(SRC_DIR)/render/render_quality.c
$(SRC_DIR)/render/render_progressive.c
$(SRC_DIR)/utils/format_object_id.c
$(SRC_DIR)/utils/ft_atof.c
$(SRC_DIR)/parser/parse_validation.c
$(SRC_DIR)/utils/cleanup.c
```
