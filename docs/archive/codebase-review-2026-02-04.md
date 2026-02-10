# 코드베이스 전체 리뷰 리포트

작성일: 2026-02-04
브랜치: `develop` (fix/linux-mlx-pixel-format 병합 후)
참고: `docs/codebase-cleanup-legacy-report.md`
방법: 전체 src/ + includes/ 정적 분석 (rg 참조 검색 + 파일 읽기)

---

## 0. 요약

| 분류 | 건수 | 심각도 |
|------|------|--------|
| 파일 단위 Dead Code | 9개 (7 .c + 2 .h) | HIGH |
| 부분 Dead Code (함수/선언) | 22개 | HIGH~MEDIUM |
| 의존성 치환 필요 | 3건 | MEDIUM |
| 실질 코드 중복 | 1건 | LOW |
| 구조적 부채 | 2건 | LOW |

예상 삭제 규모: 소스 파일 7개, 헤더 2개, 개별 선언/함수 22개.
예상 수정 규모: 소스 4개, 헤더 6개, Makefile 1개.

---

## 1. Dead Code — 파일 단위 삭제 대상

### 1.1 `includes/overlay.h` — 완전 미사용 헤더

- **심각도**: HIGH
- **include 수**: 0건 (어떤 .c/.h도 include하지 않음)
- **선언된 함수 8개**: 전부 정의 없음, 호출 없음

| 함수 | 선언 위치 | 정의 | 호출 |
|------|-----------|------|------|
| `hud_overlay_init` | overlay.h:59 | 없음 | 없음 |
| `hud_overlay_destroy` | overlay.h:60 | 없음 | 없음 |
| `hud_overlay_toggle` | overlay.h:61 | 없음 | 없음 |
| `hud_overlay_next_page` | overlay.h:62 | 없음 | 없음 |
| `hud_overlay_prev_page` | overlay.h:63 | 없음 | 없음 |
| `keyguide_overlay_init` | overlay.h:65 | 없음 | 없음 |
| `keyguide_overlay_destroy` | overlay.h:66 | 없음 | 없음 |
| `keyguide_overlay_toggle` | overlay.h:67 | 없음 | 없음 |

- **선언된 구조체 4개**: 전부 미사용
  - `t_hud_data` (overlay.h:21-28)
  - `t_keyguide_data` (overlay.h:33-38)
  - `t_hud_overlay` (overlay.h:43-47)
  - `t_keyguide_overlay` (overlay.h:52-56)
- **배경**: `window.h`의 `t_hud_state`/`t_keyguide_state`가 실제 구현체. overlay.h는 미구현 설계 초안으로 추정.
- **판정**: 파일 전체 삭제

---

### 1.2 `src/spatial/aabb_shapes.c` — 3개 함수 전부 미호출

- **심각도**: HIGH
- **함수 3개**: 전부 호출자 0건

| 함수 | 정의 위치 | 헤더 선언 | 호출자 |
|------|-----------|-----------|--------|
| `aabb_for_sphere` | aabb_shapes.c:23 | **없음** (spatial.h에 미선언) | 0건 |
| `aabb_for_cylinder` | aabb_shapes.c:47 | **없음** | 0건 |
| `aabb_for_plane` | aabb_shapes.c:76 | **없음** | 0건 |

- **배경**: 동일 기능을 `bounds.c`가 `t_object` 기반으로 수행. `bounds.c`의 `get_object_bounds()`가 `bvh_build_partition.c:38,42`에서 호출되는 활성 경로.
- **판정**: 파일 전체 삭제 + Makefile 제거

---

### 1.3 `src/render/render_state.c` — 고아 모듈

- **심각도**: HIGH
- **근본 원인**: `t_render_state`가 `t_render` (window.h:72-82)에 **포함되어 있지 않음**. 연결 자체가 불가능한 상태.

| 함수 | 정의 위치 | 호출자 |
|------|-----------|--------|
| `render_state_init` | render_state.c:26 | 0건 |
| `render_state_update` | render_state.c:50 | 0건 |

- **판정**: 파일 전체 삭제 + Makefile 제거 + `render_state.h:66-67` 선언 제거

---

### 1.4 `src/render/render_quality.c` — 고아 모듈 (render_state 종속)

- **심각도**: HIGH
- **근본 원인**: `t_render_state`에 의존하는 함수들이지만, `t_render_state` 자체가 사용되지 않음.

| 함수 | 정의 위치 | 호출자 |
|------|-----------|--------|
| `get_elapsed_us` | render_quality.c:26 | `quality_should_upgrade`만 호출 (내부 static) |
| `quality_set_mode` | render_quality.c:44 | 0건 |
| `quality_should_upgrade` | render_quality.c:59 | `render_state_update`만 호출 (자체도 미사용) |
| `quality_handle_interaction` | render_quality.c:80 | 0건 |

- **부수 효과**: 리포트에서 언급된 "시간 계산 중복 (render_quality.c ↔ timer.c)" 문제가 삭제로 자동 해소됨
- **판정**: 파일 전체 삭제 + Makefile 제거 + `render_state.h:70-72` 선언 제거 + `includes/render_quality.h` 삭제 검토

---

### 1.5 `src/render/render_progressive.c` — 3개 함수 전부 미호출

- **심각도**: HIGH

| 함수 | 정의 위치 | 선언 | 호출자 |
|------|-----------|------|--------|
| `progressive_init` | render_progressive.c:27 | render_state.h:75 | 0건 |
| `progressive_next_tile` | render_progressive.c:54 | render_state.h:77 | 0건 |
| `progressive_reset` | render_progressive.c:85 | render_state.h:78 | 0건 |

- **주의**: FR-004 (024-code-hardening)에서 `progressive_init`에 width/height 저장 로직을 추가했으나, 함수 자체가 호출되지 않아 실행 경로에 도달 불가.
- **판정**: 파일 전체 삭제 + Makefile 제거 + `render_state.h:74-78` 선언 제거

---

### 1.6 `src/utils/format_object_id.c` + `includes/format_object_id.h`

- **심각도**: HIGH

| 함수 | 정의 위치 | 호출자 |
|------|-----------|--------|
| `format_object_id` | format_object_id.c:43 | 0건 |
| `get_object_type_prefix` | format_object_id.c:22 | `format_object_id` 내부만 (자체도 미사용) |

- **배경**: `format_id()` (format_helpers.c:28)는 `parse_objects.c`, `parse_cylinder.c`에서 직접 호출되어 활성 사용 중. `format_object_id`는 `format_id`를 래핑하는 미사용 레이어.
- **판정**: .c 파일 + .h 파일 삭제 + Makefile 제거

---

### 1.7 `src/utils/cleanup.c` — 래퍼 전용 (치환 후 삭제)

- **심각도**: MEDIUM (의존성 치환 필요)
- **함수 3개**: 전부 단순 위임

| 함수 | 정의 위치 | 호출자 | 위임 대상 |
|------|-----------|--------|-----------|
| `cleanup_scene` | cleanup.c:24 | `cleanup_all` 내부만 | `scene_destroy()` |
| `cleanup_render` | cleanup.c:36 | `cleanup_all` 내부만 | `render_destroy()` |
| `cleanup_all` | cleanup.c:49 | `window_lifecycle.c:31` | 위 두 함수 호출 |

- **판정**: `window_lifecycle.c`에서 `cleanup_all` → `render_destroy` + `scene_destroy` 직접 호출로 치환 후 파일 삭제

---

### 1.8 `src/utils/ft_atof.c` — 유일 호출자가 Dead Code

- **심각도**: MEDIUM

| 함수 | 정의 위치 | 선언 | 호출자 |
|------|-----------|------|--------|
| `ft_atof` | ft_atof.c:112 | minirt.h:127 | `parse_validation.c:45,51,57` (parse_vector 내부) |

- **배경**: `parse_vector`가 미사용이므로 `ft_atof`도 실질적 Dead Code. 내부 static 함수 (`ft_isspace`, `ft_isdigit`, `parse_integer_part`, `parse_fractional_part`)도 함께 삭제.
- **판정**: 파일 삭제 + `minirt.h:127` 선언 제거 + Makefile 제거

---

### 1.9 `src/parser/parse_validation.c` — in_range 이동 후 삭제

- **심각도**: MEDIUM (이동 작업 필요)

| 함수 | 정의 위치 | 호출자 | 상태 |
|------|-----------|--------|------|
| `in_range` | parse_validation.c:26 | parse_elements.c:39,103,139 + parse_validation_strict.c:73 | **활성 사용** |
| `parse_vector` | parse_validation.c:40 | 0건 | 미사용 |
| `parse_color` | parse_validation.c:70 | 0건 | 미사용 |

- **판정**: `in_range`를 `parse_validation_strict.c`로 이동 → 파일 삭제 + Makefile 제거

---

## 2. Dead Code — 부분 삭제 대상 (선언/함수 단위)

### 2.1 `includes/objects.h` — 레거시 typedef 3개

| typedef | 위치 | 사용처 |
|---------|------|--------|
| `t_sphere` | objects.h:75-83 | 0건 |
| `t_plane` | objects.h:85-91 | 0건 |
| `t_cylinder` | objects.h:93-104 | 0건 |

- **배경**: 현재 코드는 `t_object` + union(`t_sphere_data`, `t_plane_data`, `t_cylinder_data`)을 사용
- **삭제 범위**: objects.h:74-104 (주석 포함 31줄)

---

### 2.2 `includes/parser.h` — 레거시 함수 선언 2개

| 선언 | 위치 |
|------|------|
| `parse_vector` + 주석 | parser.h:189-195 |
| `parse_color` + 주석 | parser.h:197-203 |

---

### 2.3 `includes/window_internal.h` — 미구현 선언 7개 + 중복 선언 1개

| 선언 | 위치 | 정의 | 호출 |
|------|------|------|------|
| `cycle_type_forward` | window_internal.h:130 | 없음 | 없음 |
| `cycle_backward_sphere` | window_internal.h:131 | 없음 | 없음 |
| `cycle_backward_plane` | window_internal.h:132 | 없음 | 없음 |
| `cycle_backward_cylinder` | window_internal.h:133 | 없음 | 없음 |
| `next_type_from_sphere` | window_internal.h:134 | 없음 | 없음 |
| `next_type_from_plane` | window_internal.h:135 | 없음 | 없음 |
| `next_type_from_cylinder` | window_internal.h:136 | 없음 | 없음 |
| `render_scene_to_buffer` | window_internal.h:139 | render.c:140 | window_loop.c:50 |

- 마지막 항목은 `minirt.h:130`에 이미 선언되어 있으므로 **중복 forward declaration** → 삭제
- **삭제 범위**: window_internal.h:129-139 (주석 포함)

---

### 2.4 `includes/render_state.h` — 고아 선언 정리

| 선언 | 위치 | 상태 |
|------|------|------|
| `render_state_init` | render_state.h:66 | 정의 미호출 |
| `render_state_update` | render_state.h:67 | 정의 미호출 |
| `quality_set_mode` | render_state.h:70 | 정의 미호출 |
| `quality_should_upgrade` | render_state.h:71 | 정의 미호출 |
| `quality_handle_interaction` | render_state.h:72 | 정의 미호출 |
| `progressive_init` | render_state.h:75-76 | 정의 미호출 |
| `progressive_next_tile` | render_state.h:77 | 정의 미호출 |
| `progressive_reset` | render_state.h:78 | 정의 미호출 |

- **삭제 범위**: render_state.h:65-78 (선언부 전체)
- **유지 대상**: 타입 정의 (`t_quality_mode`, `t_interaction_state`, `t_progressive_state`, `t_tile_rect`, `t_render_state`)는 다른 곳에서 참조 가능하므로 별도 확인 후 결정

---

### 2.5 `includes/error.h` — `print_error` 선언

| 선언 | 위치 |
|------|------|
| `print_error` | error.h:43 |

- 전제: `src/utils/error.c:83-100`의 함수 정의 삭제 + 7곳 호출 치환 완료 후

---

### 2.6 `includes/minirt.h` — 삭제 대상 선언 2개

| 선언 | 위치 |
|------|------|
| `cleanup_all` + 주석 | minirt.h:123-124 |
| `ft_atof` | minirt.h:127 |

---

### 2.7 `src/utils/error.c` — `print_error` 함수

- **위치**: error.c:83-100 (주석 포함 18줄)
- **전제**: 7곳 호출 치환 완료 후 삭제

---

### 2.8 `src/keyguide/keyguide_render.c` — `keyguide_render_background` 함수

- **위치**: keyguide_render.c:25-52 (주석 포함 28줄)
- **선언**: keyguide.h:43
- **호출자**: 0건
- **배경**: keyguide_render()는 keyguide_render_content()/keyguide_render_extra()를 호출. background 함수는 호출 경로 없음.

---

## 3. 의존성 치환 필요 항목

### 3.1 `cleanup_all` → 직접 호출 치환

**파일**: `src/window/window_lifecycle.c`

**현재** (line 26-34):
```c
int	close_window(void *param)
{
	t_render	*render;

	render = (t_render *)param;
	cleanup_all(render->scene, render);
	exit(0);
	return (0);
}
```

**변경 후**:
```c
int	close_window(void *param)
{
	t_render	*render;
	t_scene		*scene;

	render = (t_render *)param;
	scene = render->scene;
	render_destroy(render);
	scene_destroy(scene);
	exit(0);
	return (0);
}
```

- **주의**: `render_destroy(render)`가 render를 free하므로, `render->scene`을 먼저 지역 변수에 저장해야 함
- **변수 추가**: `t_scene *scene` (Norm 5변수 제한 내: render, scene = 2개)

---

### 3.2 `print_error` → `error_print` 전환 (7곳)

**파일**: `src/parser/parser.c`

#### 3.2.1 `validate_scene` 함수 (4곳)

**현재** (line 53-64):
```c
int	validate_scene(t_scene *scene)
{
	if (!scene_has_ambient(scene))
		return (print_error("Missing ambient lighting (A)"));
	if (!scene_has_camera(scene))
		return (print_error("Missing camera (C)"));
	if (!scene_has_light(scene))
		return (print_error("Missing light (L)"));
	if (scene->objects.count == 0)
		return (print_error("No objects in scene"));
	return (1);
}
```

**에러 코드 매핑**:

| 현재 메시지 | 대응 코드 | error_get_message 출력 |
|-------------|-----------|----------------------|
| "Missing ambient lighting (A)" | `ERR_PARSE_MISSING` | "Missing required element in scene file" |
| "Missing camera (C)" | `ERR_PARSE_MISSING` | "Missing required element in scene file" |
| "Missing light (L)" | `ERR_PARSE_MISSING` | "Missing required element in scene file" |
| "No objects in scene" | `ERR_PARSE_MISSING` | "Missing required element in scene file" |

**트레이드오프**: 4개의 구체적 메시지가 1개의 일반 메시지로 통합됨. 사용자가 어떤 요소가 빠졌는지 알 수 없게 됨.

**대안 A — 일반 코드 사용 (구체성 손실, 최소 변경)**:
```c
int	validate_scene(t_scene *scene)
{
	if (!scene_has_ambient(scene))
		return (error_print(ERR_PARSE_MISSING), 0);
	if (!scene_has_camera(scene))
		return (error_print(ERR_PARSE_MISSING), 0);
	if (!scene_has_light(scene))
		return (error_print(ERR_PARSE_MISSING), 0);
	if (scene->objects.count == 0)
		return (error_print(ERR_PARSE_MISSING), 0);
	return (1);
}
```

**대안 B — error_context 활용 (구체성 유지)**:
```c
int	validate_scene(t_scene *scene)
{
	t_error_context	ctx;

	error_context_init(&ctx);
	ctx.error_code = PARSE_ERR_MISSING_ELEMENT;
	if (!scene_has_ambient(scene))
		return (error_context_set_element(&ctx, "A"),
			error_context_print(&ctx), 0);
	// ... (Norm 25줄 제한 검토 필요)
}
```

**대안 C — 전용 에러 코드 추가 (가장 정확)**:
- `error.h`에 `ERR_MISSING_AMBIENT`, `ERR_MISSING_CAMERA`, `ERR_MISSING_LIGHT`, `ERR_MISSING_OBJECTS` 추가
- `error.c`의 `error_messages[]`에 대응 메시지 추가
- 가장 구체적이나 enum 확장 필요

#### 3.2.2 `parse_scene` 함수 (3곳)

**현재** (line 138-148):
```c
if (!validate_extension(filename))
	return (print_error("Invalid file extension (expected .rt)"));
fd = open(filename, O_RDONLY);
if (fd < 0)
	return (print_error("Cannot open file"));
...
if (!line_reader_init(&reader, fd))
{
	close(fd);
	return (print_error("Failed to initialize line reader"));
}
```

**에러 코드 매핑**:

| 현재 메시지 | 대응 코드 | 정확도 |
|-------------|-----------|--------|
| "Invalid file extension (expected .rt)" | `ERR_FILE_EXT` | 정확 — "Invalid file extension (expected .rt)" |
| "Cannot open file" | `ERR_FILE_OPEN` | 정확 — "Failed to open file" |
| "Failed to initialize line reader" | `ERR_MALLOC` | 근사 — "Memory allocation failed" |

**변경 후**:
```c
if (!validate_extension(filename))
	return (error_print(ERR_FILE_EXT), 0);
fd = open(filename, O_RDONLY);
if (fd < 0)
	return (error_print(ERR_FILE_OPEN), 0);
...
if (!line_reader_init(&reader, fd))
{
	close(fd);
	return (error_print(ERR_MALLOC), 0);
}
```

- **반환값 주의**: `print_error`는 0 반환, `error_print`는 1 반환. comma operator `(error_print(code), 0)` 패턴으로 0 반환 유지.

---

### 3.3 `in_range` 함수 이동

**소스**: `src/parser/parse_validation.c:26-29`
```c
int	in_range(double value, double min, double max)
{
	return (value >= min && value <= max);
}
```

**이동 대상**: `src/parser/parse_validation_strict.c` 상단

**호출자** (4곳, 전부 parser 모듈):
- `parse_elements.c:39` — ambient ratio 범위 검증 (0.0-1.0)
- `parse_elements.c:103` — camera FOV 범위 검증 (0-180)
- `parse_elements.c:139` — light brightness 범위 검증 (0.0-1.0)
- `parse_validation_strict.c:73` — RGB 컴포넌트 범위 검증 (0-255)

**헤더 변경**: 불필요 (`parser.h:212`에 이미 선언 존재)

---

## 4. 코드 중복

### 4.1 타입 카운트 중복 — 통합 권장 (LOW)

**중복 함수 2개**:

`src/parser/parse_objects.c:25-39` (static):
```c
static int	get_type_count(t_scene *scene, t_object_type type)
{
	int	i;
	int	count;

	count = 0;
	i = 0;
	while (i < scene->objects.count)
	{
		if (scene->objects.items[i].type == type)
			count++;
		i++;
	}
	return (count);
}
```

`src/parser/parse_cylinder.c:24-38` (static):
```c
static int	get_cylinder_count(t_scene *scene)
{
	int	i;
	int	count;

	count = 0;
	i = 0;
	while (i < scene->objects.count)
	{
		if (scene->objects.items[i].type == OBJ_CYLINDER)
			count++;
		i++;
	}
	return (count);
}
```

- **동일 로직**: 타입 비교 조건만 다름 (매개변수 vs 하드코딩)
- **호출자**:
  - `get_type_count`: parse_objects.c:93 (sphere), parse_objects.c:149 (plane)
  - `get_cylinder_count`: parse_cylinder.c:127
- **통합 방안**: `get_type_count`를 공유 함수로 승격하여 `parse_cylinder.c`에서도 사용
- **우선순위**: LOW (static 함수 간 중복, 기능 정상 작동)

---

### 4.2 해소된 중복 항목

리포트에서 지적한 나머지 3건은 삭제로 자동 해소됨:

| 리포트 항목 | 원인 | 해소 방법 |
|-------------|------|-----------|
| AABB 계산 중복 (aabb_shapes.c ↔ bounds.c) | `aabb_shapes.c`가 Dead Code | 파일 삭제 (1.2) |
| 숫자 파싱 중복 (ft_atof.c ↔ parse_number_utils.c) | `ft_atof`가 Dead Code | 파일 삭제 (1.8) |
| 시간 계산 중복 (render_quality.c ↔ timer.c) | `render_quality.c`가 Dead Code | 파일 삭제 (1.4) |

---

## 5. 구조적 부채

### 5.1 `window.h` 전이 include 과다 (LOW)

**의존 체인**:
```
window.h
├── minirt.h
│   ├── <unistd.h>
│   ├── <fcntl.h>
│   ├── <stdbool.h>
│   ├── libft.h
│   ├── error.h
│   ├── vec3.h
│   ├── objects.h
│   ├── ray.h
│   ├── shadow.h
│   └── metrics.h
├── mlx.h
├── mlx_context.h
├── pixel_timing.h
└── render_debounce.h
```

- `window.h`를 include하면 15개 이상의 헤더가 전이 포함됨
- UI 레이어가 raytracing 내부(`ray.h`, `shadow.h`)에 의존하는 구조
- **우선순위**: LOW (기능에 영향 없으나 장기적 유지보수 부채)

### 5.2 `render_state.h` 활성/비활성 혼재 (LOW)

`render_state.h`에 선언된 항목 중:
- **활성 타입**: `t_progressive_state`, `t_tile_rect` (구조체 정의는 다른 곳에서 참조 가능)
- **비활성 타입**: `t_render_state`, `t_quality_mode`, `t_interaction_state`
- **비활성 함수**: 8개 전부 (위 2.4 참고)

render_state/quality/progressive 모듈 삭제 시 `render_state.h` 내 타입이 다른 헤더에서 참조되는지 확인 필요. `t_render`가 `t_render_state`를 포함하지 않으므로, 타입 참조도 없을 가능성이 높으나 검증 필요.

---

## 6. 리포트(`codebase-cleanup-legacy-report.md`) 대비 차이

### 6.1 리포트 항목 검증 결과

| 리포트 항목 | 검증 결과 |
|-------------|-----------|
| 2.1 즉시 삭제 가능 (parse_vector 등) | **전부 확인됨** |
| 2.2 의존성 정리 후 삭제 (cleanup, print_error) | **확인됨** + 구체적 매핑 분석 완료 |
| 2.3 유지 대상 (in_range, format_id 등) | **확인됨** |
| 3-A AABB 중복 | **정정**: 중복이 아닌 Dead Code |
| 3-A 숫자 파싱 중복 | **정정**: 중복이 아닌 Dead Code |
| 3-A 타입 카운트 중복 | **확인됨**: 실질 중복 |
| 3-A 시간 계산 중복 | **정정**: 중복이 아닌 Dead Code |
| 3-B overlay.h | **확인됨** + 심각도 상향 (완전 미사용) |
| 3-B window_internal.h selection helpers | **확인됨** + 7개 전부 특정 |
| 3-B render_state 미연결 | **확인됨** + 근본 원인 특정 (t_render에 미포함) |

### 6.2 추가 발견 (리포트 미포함)

| 항목 | 심각도 | 설명 |
|------|--------|------|
| `aabb_shapes.c` 전체 Dead Code | HIGH | 리포트는 "중복"으로 분류했으나 실제로는 호출자 0건+헤더 선언 없음 |
| `render_progressive.c` 3개 함수 미호출 | HIGH | 리포트 미언급. FR-004에서 수정했으나 호출 경로 부재 |
| `render_quality.c` 전체 Dead Code | HIGH | 리포트 미언급. render_state 고아 모듈의 일부 |
| `keyguide_render_background` 미호출 | MEDIUM | 리포트 "일부 미호출"로 모호하게 언급 |
| `render_scene_to_buffer` 중복 선언 | LOW | window_internal.h:139 + minirt.h:130 |
| `ft_atof` Dead Code | MEDIUM | 리포트 미언급. parse_vector 삭제 시 함께 삭제 가능 |

---

## 7. 권장 실행 순서

### Phase 1: 무참조 Dead Code 삭제 (안전, 의존성 없음)

| 순번 | 작업 | 대상 |
|------|------|------|
| 1-1 | 파일 삭제 | `includes/overlay.h` |
| 1-2 | 파일 삭제 + Makefile | `src/spatial/aabb_shapes.c` |
| 1-3 | 파일 삭제 + Makefile | `src/render/render_state.c` |
| 1-4 | 파일 삭제 + Makefile | `src/render/render_quality.c` |
| 1-5 | 파일 삭제 + Makefile | `src/render/render_progressive.c` |
| 1-6 | 파일 삭제 + Makefile, 헤더 삭제 | `src/utils/format_object_id.c`, `includes/format_object_id.h` |
| 1-7 | typedef 삭제 | `includes/objects.h:74-104` |
| 1-8 | 선언 삭제 | `includes/parser.h:189-203` (parse_vector, parse_color) |
| 1-9 | 선언 삭제 | `includes/window_internal.h:129-139` (selection helpers + 중복 선언) |
| 1-10 | 선언 삭제 | `includes/render_state.h:65-78` (전 함수 선언) |
| 1-11 | 함수 삭제 | `src/keyguide/keyguide_render.c:25-52` + `includes/keyguide.h` 선언 |
| 1-12 | 헤더 삭제 검토 | `includes/render_quality.h` (render_quality.c 삭제 후 필요 여부) |

### Phase 2: 의존성 분리 (이동 작업)

| 순번 | 작업 | 대상 |
|------|------|------|
| 2-1 | `in_range` 이동 | parse_validation.c → parse_validation_strict.c |
| 2-2 | 파일 삭제 + Makefile | `src/parser/parse_validation.c` |
| 2-3 | 파일 삭제 + Makefile, 선언 삭제 | `src/utils/ft_atof.c`, `includes/minirt.h:127` |

### Phase 3: 래퍼/에러 경로 치환

| 순번 | 작업 | 대상 |
|------|------|------|
| 3-1 | callsite 치환 | `window_lifecycle.c`: cleanup_all → render_destroy + scene_destroy |
| 3-2 | 파일 삭제 + Makefile, 선언 삭제 | `src/utils/cleanup.c`, `includes/minirt.h:123-124` |
| 3-3 | callsite 치환 (7곳) | `parser.c`: print_error → error_print (대안 A/B/C 선택 필요) |
| 3-4 | 함수/선언 삭제 | `src/utils/error.c:83-100`, `includes/error.h:43` |

### Phase 4: 검증

| 순번 | 작업 |
|------|------|
| 4-1 | `make re` (빌드 성공 확인) |
| 4-2 | `norminette src/ includes/` |
| 4-3 | 정상 씬 렌더링 회귀 테스트 |
| 4-4 | 비정상 씬 18개 에러 출력 회귀 테스트 |

---

## 8. 변경 범위 총괄

### 삭제 파일 (9개)

| 파일 | Phase |
|------|-------|
| `includes/overlay.h` | 1 |
| `includes/format_object_id.h` | 1 |
| `src/spatial/aabb_shapes.c` | 1 |
| `src/render/render_state.c` | 1 |
| `src/render/render_quality.c` | 1 |
| `src/render/render_progressive.c` | 1 |
| `src/utils/format_object_id.c` | 1 |
| `src/parser/parse_validation.c` | 2 |
| `src/utils/ft_atof.c` | 2 |
| `src/utils/cleanup.c` | 3 |

### 수정 파일 (10개)

| 파일 | 변경 내용 | Phase |
|------|-----------|-------|
| `includes/objects.h` | typedef 3개 삭제 | 1 |
| `includes/parser.h` | 선언 2개 삭제 | 1 |
| `includes/window_internal.h` | 선언 8개 삭제 | 1 |
| `includes/render_state.h` | 선언 8개 삭제 | 1 |
| `includes/keyguide.h` | 선언 1개 삭제 | 1 |
| `src/keyguide/keyguide_render.c` | 함수 1개 삭제 | 1 |
| `src/parser/parse_validation_strict.c` | in_range 수신 | 2 |
| `includes/minirt.h` | 선언 2개 삭제 | 2-3 |
| `src/window/window_lifecycle.c` | cleanup_all 치환 | 3 |
| `src/parser/parser.c` | print_error 7곳 치환 | 3 |
| `includes/error.h` | 선언 1개 삭제 | 3 |
| `src/utils/error.c` | 함수 1개 삭제 | 3 |
| `Makefile` | 소스 7개 행 제거 | 1-3 |

### 검토 필요 (삭제 여부 미확정)

| 파일 | 사유 |
|------|------|
| `includes/render_quality.h` | render_quality.c 삭제 후 다른 include 여부 확인 |
| `includes/render_state.h` 타입 정의부 | 타입 참조 여부 확인 후 결정 |

---

## 9. 미결 사항 (결정 필요)

### 9.1 `print_error` 전환 대안 선택

| 대안 | 장점 | 단점 |
|------|------|------|
| A: `error_print(ERR_PARSE_MISSING)` 일괄 | 최소 변경 | 구체적 메시지 손실 ("Missing ambient lighting (A)" → 일반) |
| B: `error_context_print` 활용 | 구체성 유지 | 함수 시그니처 변경 필요, Norm 줄 수 검토 필요 |
| C: 전용 에러 코드 추가 | 가장 정확 | enum/메시지 배열 확장 필요 (4개 코드 추가) |

### 9.2 `render_state.h` 타입 존속 여부

`t_render_state`, `t_quality_mode`, `t_interaction_state`, `t_progressive_state`, `t_tile_rect`이 render_state/quality/progressive 파일 삭제 후에도 다른 곳에서 참조되는지 확인 필요. 참조 없으면 타입 정의도 삭제 가능.

### 9.3 `render_progressive.c` 삭제 vs 보존

FR-004에서 width/height 지원을 추가했으나 호출 경로가 없음. 향후 progressive 렌더링 기능 구현 예정이면 보존, 아니면 삭제.
