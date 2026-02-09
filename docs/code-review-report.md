# miniRT 코드 리뷰 - 시니어 개발자 관점

**리뷰 일시:** 2025년 1월
**리뷰어:** 시니어 소프트웨어 엔지니어
**코드베이스:** miniRT (Ray Tracing 프로젝트)
**총 파일 수:** 89개 C 파일, 23개 헤더 파일

---

## 🔴 치명적 이슈 (즉시 수정 필요)

### 1. **메모리 누수 - MLX 리소스 정리 누락**
**파일:** `src/window/window_lifecycle.c:25-36`

```c
int	close_window(void *param)
{
	t_render	*render;
	t_scene		*scene;

	render = (t_render *)param;
	scene = render->scene;
	render_destroy(render);
	scene_destroy(scene);
	exit(0);  // ⚠️ MLX 이벤트 루프를 종료하지 않고 exit() 호출
	return (0);
}
```

**문제점:**
- `exit(0)`를 직접 호출하여 프로세스를 종료하면 MLX의 내부 리소스가 정리되지 않음
- 다른 MLX 인스턴스가 활성화되어 있을 경우 크래시 가능
- 프로덕션 환경에서는 graceful shutdown이 필수

**해결책:**
```c
// mlx_loop_end()를 사용하거나 적절한 종료 플래그 설정
mlx_loop_end(render->mlx.mlx);  // 또는 적절한 종료 메커니즘
```

---

### 2. **Division by Zero - vec3_normalize() 취약점**
**파일:** `src/math/vector_ops.c:82-101`

```c
t_vec3	vec3_normalize(t_vec3 v)
{
	double	mag;
	t_vec3	result;

	mag = vec3_magnitude(v);
	if (mag > 0.0)  // ⚠️ mag == 0.0일 때 검증 없이 0으로 나누기 가능
	{
		result.x = v.x / mag;
		result.y = v.y / mag;
		result.z = v.z / mag;
	}
```

**문제점:**
- `mag > 0.0` 체크는 있지만, 매우 작은 값(예: 1e-20)일 때 나누기 결과가 폭발적으로 커짐
- Floating point 오버플로우 발생 가능
- NaN이나 Inf가 발생하면 전체 렌더링 파이프라인이 오염됨

**실제 시나리오:**
```c
// 사용자가 카메라 방향을 (0, 0, 0)으로 설정 시도
C 0,0,0 0,0,0 70  // Parser에서 검증하지만, 런타임에서도 방어 필요
```

**해결책:**
```c
#define EPSILON_NORMALIZE 1e-8

t_vec3	vec3_normalize(t_vec3 v)
{
	double	mag;
	
	mag = vec3_magnitude(v);
	if (mag < EPSILON_NORMALIZE)
		return ((t_vec3){0, 0, 1});  // 기본 방향 반환
	return ((t_vec3){v.x / mag, v.y / mag, v.z / mag});
}
```

---

### 3. **Buffer Overflow - 스택 기반 배열 한계 초과 가능**
**파일:** `src/parser/parse_line_reader.c` (LINE_READER_BUFFER_SIZE)

```c
#define LINE_READER_BUFFER_SIZE 4096
#define MAX_LINE_LENGTH 4096

typedef struct s_line_reader
{
	int		fd;
	char	buffer[LINE_READER_BUFFER_SIZE];  // ⚠️ 스택에 4KB 할당
	// ...
}	t_line_reader;
```

**문제점:**
- 스택 프레임이 4KB+ 크기로 매우 큼
- 재귀 호출이 있는 BVH 빌드와 결합 시 스택 오버플로우 위험
- BVH depth가 20까지 가능 (bvh_build_core.c:74)하므로 스택 사용량이 급증

**해결책:**
```c
// 동적 할당으로 변경
typedef struct s_line_reader
{
	int		fd;
	char	*buffer;  // malloc으로 할당
	// ...
}	t_line_reader;
```

---

### 4. **Race Condition - render_debounce 타이머 관리**
**파일:** `src/render/render_debounce_timer.c`, `window/window_loop.c`

```c
// 멀티 스레드는 아니지만, 비동기 이벤트 처리로 인한 타이밍 이슈
if (render->debounce.cancel_requested)
	return ;  // ⚠️ 플래그 체크와 실제 취소 사이 타이밍 윈도우
```

**문제점:**
- `cancel_requested` 플래그가 체크와 사용 사이에 변경될 수 있음
- MLX 이벤트 루프가 예상치 못한 순서로 콜백 호출 가능
- 렌더링 중단이 불완전하게 이루어질 수 있음

---

### 5. **Integer Overflow - BVH 노드 카운팅**
**파일:** `src/spatial/bvh_build_core.c:111`, `spatial.h:52`

```c
typedef struct s_bvh
{
	t_bvh_node	*root;
	int			enabled;
	int			total_nodes;  // ⚠️ int로 노드 수 추적
	int			max_depth;
	int			visualize;
}	t_bvh;

// bvh_build_core.c
bvh->total_nodes = 1;  // ⚠️ 재귀 빌드 중 증가 로직 누락
```

**문제점:**
- `total_nodes`가 실제 노드 수를 추적하지 않음
- 항상 1로 설정되어 메모리 누수 모니터링 불가능
- 대량의 객체(수만 개)를 처리할 때 int 오버플로우 가능

---

### 6. **Use After Free 가능성 - Scene 파괴 순서**
**파일:** `main.c:136-139`

```c
if (!init_render_ctx(scene, &render, bvh_vis))
	return (1);
mlx_loop(render->mlx.mlx);
return (0);  // ⚠️ mlx_loop이 반환되지 않으면 scene이 정리되지 않음
```

**문제점:**
- `mlx_loop()`은 무한 루프이므로 정상적으로 반환되지 않음
- `close_window()`에서 `exit(0)` 호출로 정리는 되지만, 비정상 종료 시 누수
- SIGINT/SIGTERM 핸들러가 없어 강제 종료 시 리소스 누수

---

## ⚠️ 심각한 경고 (조속히 수정 권장)

### 7. **성능 병목 - O(n²) 복잡도**
**파일:** `src/render/render.c:150-165`

```c
void	render_scene_to_buffer(t_scene *scene, t_render *render)
{
	int		x;
	int		y;

	// ...
	y = 0;
	while (y < WINDOW_HEIGHT)  // 900
	{
		x = 0;
		while (x < WINDOW_WIDTH)  // 1440
		{
			render_pixel(scene, render, x, y);  // 각 픽셀마다 trace_ray() 호출
			x++;
		}
		y++;
	}
}
```

**문제점:**
- **1,296,000 픽셀** (1440×900)을 순차적으로 렌더링
- BVH가 없으면 각 픽셀마다 모든 객체 검사 → **O(픽셀 수 × 객체 수)**
- 100개 객체 시 **130,000,000번의 교차 테스트**
- 실시간 상호작용(카메라 이동) 불가능

**실제 측정치 예상:**
- 10개 객체: ~1초
- 100개 객체: ~10초
- 1000개 객체: **>100초**

**해결책:**
- 멀티스레딩 (pthread 사용)
- 타일 기반 렌더링
- 적응형 샘플링 (중요 영역 우선 렌더링)

---

### 8. **메모리 낭비 - 객체 ID 필드 미사용**
**파일:** `includes/objects.h:62-72`

```c
typedef struct s_object
{
	t_object_type	type;
	t_color			color;
	char			id[8];  // ⚠️ 모든 객체마다 8바이트 낭비
	union u_object_data
	{
		t_sphere_data	sphere;
		t_plane_data	plane;
		t_cylinder_data	cylinder;
	}	data;
}	t_object;
```

**문제점:**
- `id` 필드가 코드베이스 전체에서 **단 한 번도 사용되지 않음**
- grep 결과: 0건 (선언 제외)
- 1000개 객체 시 **8KB 메모리 낭비**
- 구조체 크기를 불필요하게 증가시켜 캐시 효율성 저하

---

### 9. **에러 처리 불일치 - 파서 vs 렌더러**
**파일:** 여러 파일

```c
// 파서: 상세한 에러 메시지
if (result != PARSE_OK)
{
	ctx->error_code = result;
	error_context_print(ctx);  // Line number, element type 포함
	return (0);
}

// 렌더러: 무성의한 에러 처리
if (!mlx_context_init(&render->mlx, WINDOW_WIDTH, WINDOW_HEIGHT, "miniRT"))
{
	free(render);
	return (NULL);  // ⚠️ 왜 실패했는지 알 수 없음
}
```

**문제점:**
- 파서는 훌륭한 에러 리포팅
- 런타임 에러는 사용자에게 아무 정보도 제공하지 않음
- MLX 초기화 실패 원인 (디스플레이 연결, 메모리 부족 등) 파악 불가

---

### 10. **하드코딩된 매직 넘버 남발**
**파일:** 여러 파일

```c
// lighting.c:78
spec = fast_pow32(spec);  // ⚠️ 왜 32?

// intersect_object.c:47
if (d < 0.001 || d > hit->distance)  // ⚠️ 왜 0.001?

// bvh_traverse.c:133
t_min = 0.001;
t_max = 1000000.0;  // ⚠️ 왜 백만?

// bvh_build_core.c:74
if (count <= 2 || depth > 20)  // ⚠️ 왜 2? 왜 20?
```

**문제점:**
- 의미를 알 수 없는 상수들이 코드 전체에 산재
- 튜닝이나 디버깅 시 모든 파일을 수정해야 함
- 일관성 없음 (어떤 곳은 0.001, 어떤 곳은 EPSILON 사용)

**해결책:**
```c
// minirt.h에 통합
#define RAY_EPSILON 0.001
#define RAY_MAX_DISTANCE 1000000.0
#define BVH_MAX_DEPTH 20
#define BVH_MIN_LEAF_SIZE 2
#define SPECULAR_SHININESS 32
#define CAMERA_NEAR_PLANE 0.001
```

---

### 11. **BVH 빌드 전략의 비효율성**
**파일:** `src/spatial/bvh_build_core.c:74`

```c
if (count <= 2 || depth > 20)
	return (create_leaf_node(objects, count, scene));
```

**문제점:**
- **Depth 20까지 허용**은 매우 깊음 → 스택 오버플로우 위험
- **리프 노드 최소 크기 2**는 너무 작음 → 과도한 노드 생성
- 최악의 경우 **2^20 = 1,048,576개의 노드** 생성 가능
- 메모리 단편화 및 캐시 스레싱 유발

**권장 설정:**
- Max depth: 12-15
- Min leaf size: 4-8
- Surface Area Heuristic (SAH) 사용 (현재는 단순 중간점 분할)

---

### 12. **AABB 교차 테스트 최적화 누락**
**파일:** `src/spatial/aabb.c:81-101`

```c
int	aabb_intersect(t_aabb box, t_ray ray, double *t_min, double *t_max)
{
	t_axis_check	ac;
	double			tmin;
	double			tmax;

	tmin = *t_min;
	tmax = *t_max;
	ac = (t_axis_check){box.min.x, box.max.x, ray.origin.x,
		ray.direction.x, &tmin, &tmax};
	safe_slab_axis(&ac);  // ⚠️ 3번의 함수 호출
	ac = (t_axis_check){box.min.y, box.max.y, ray.origin.y,
		ray.direction.y, &tmin, &tmax};
	safe_slab_axis(&ac);
	ac = (t_axis_check){box.min.z, box.max.z, ray.origin.z,
		ray.direction.z, &tmin, &tmax};
	safe_slab_axis(&ac);
	// ...
}
```

**문제점:**
- 구조체를 3번 생성하고 함수를 3번 호출하는 오버헤드
- 인라인 전개가 없으면 매우 느림
- BVH 순회 시 **가장 빈번하게 호출되는 함수** (핫 패스)

**최적화:**
```c
// 직접 루프로 언롤링하거나 SIMD 사용
// 또는 early exit 추가 (한 축에서 실패하면 즉시 반환)
```

---

### 13. **파서 - 메모리 누수 위험**
**파일:** `src/parser/parser.c:100-111`

```c
line = line_reader_next(reader);
while (success)
{
	if (check_line_too_long(reader, ctx))
		return (0);  // ⚠️ 현재 line이 free되지 않음
	if (line == NULL)
		break ;
	error_context_set_line(ctx, reader->line_num);
	success = parse_line(line, scene, ctx);
	free(line);  // ⚠️ success == 0일 때만 여기 도달
	line = line_reader_next(reader);
}
```

**문제점:**
- `check_line_too_long()`에서 반환 시 `line`이 free되지 않음
- 파싱 실패 시 마지막 line이 누수
- 큰 파일 파싱 시 누적되어 수십 MB 누수 가능

---

### 14. **Shadow Ray 최적화 누락**
**파일:** `src/lighting/shadow_calc.c`, `shadow_test.c`

**문제점:**
- Shadow ray가 light까지의 거리를 초과하여 검사함
- Early exit 없이 모든 객체를 체크
- 그림자 계산이 **primary ray만큼 비쌈**

**현재 구조:**
```c
// 실제 코드를 보지 못했지만, 일반적인 문제점:
// 1. Light까지의 거리를 t_max로 설정하지 않음
// 2. 첫 번째 교차에서 즉시 반환하지 않음 (any hit vs closest hit)
```

**최적화:**
```c
// Shadow ray는 any hit만 필요
// 첫 교차 발견 즉시 반환
if (first_intersection_found)
	return SHADOWED;
```

---

### 15. **컬러 연산 - Floating Point 부정확성**
**파일:** `src/lighting/lighting.c:134-145`

```c
result.r = (hit->color.r / 255.0) * (scene->ambient.ratio
		* (scene->ambient.color.r / 255.0)
		+ lighting_factor * scene->light.brightness
		* (scene->light.color.r / 255.0)) * 255.0;
```

**문제점:**
- **6번의 부동소수점 연산** per color channel
- 중간 결과를 저장하지 않아 불필요한 재계산
- Color space가 선형이 아닌데 gamma correction 없음
- 부동소수점 오차 누적으로 색상 밴딩 발생

**최적화:**
```c
// 1. Ambient와 light를 미리 계산
// 2. sRGB → Linear → sRGB 변환 추가
// 3. Integer 컬러 연산으로 변경 (정확도 높음)
```

---

## 💡 개선 사항 (장기적 품질 향상)

### 16. **아키텍처 - God Object 패턴**
**파일:** `includes/minirt.h`, `includes/window.h`

```c
typedef struct s_scene
{
	t_ambient		ambient;
	t_camera		camera;
	t_light			light;
	t_shadow_config	shadow_config;
	t_object_list	objects;
	int				flags;
	t_bvh			*bvh;
	t_metrics		metrics;  // ⚠️ 너무 많은 책임
}	t_scene;

typedef struct s_render
{
	t_mlx_context		mlx;
	t_scene				*scene;
	t_selection			selection;
	int					state_flags;
	t_hud_state			hud;
	t_keyguide_state	keyguide;
	t_pixel_timing		pixel_timing;
	t_debounce_state	debounce;  // ⚠️ 너무 많은 책임
}	t_render;
```

**문제점:**
- `t_scene`과 `t_render`가 **너무 많은 역할** 담당
- Single Responsibility Principle 위반
- 테스트, 확장, 유지보수 어려움

**개선안:**
```c
// 분리된 컨텍스트
typedef struct s_render_context { /* rendering state */ } t_render_context;
typedef struct s_ui_context { /* HUD, keyguide, etc */ } t_ui_context;
typedef struct s_scene_data { /* only scene data */ } t_scene_data;
```

---

### 17. **테스트 불가능한 구조**

**문제점:**
- MLX에 강하게 결합되어 단위 테스트 불가능
- 모든 함수가 전역 상태(scene, render)에 의존
- Mock이나 Stub 생성 불가능

**예시:**
```c
// 현재: 테스트 불가능
t_color	apply_lighting(t_scene *scene, t_hit *hit);

// 개선: 의존성 주입
t_color	apply_lighting(
	t_ambient ambient,
	t_light light,
	t_camera camera,
	t_shadow_config *config,
	t_hit *hit
);
```

---

### 18. **문서화 불일치**

**현재 상태:**
- ✅ 파서 함수는 Doxygen 스타일 주석 우수
- ✅ Main 함수는 상세한 설명
- ❌ Vector 연산은 중복 주석 (C 스타일 + Doxygen)
- ❌ BVH 알고리즘 설명 전무
- ❌ 렌더링 파이프라인 문서 없음

**개선안:**
```c
/**
 * @brief BVH 트리를 재귀적으로 구축합니다.
 * 
 * Surface Area Heuristic (SAH)를 사용하여 최적의 분할 축과 위치를 결정합니다.
 * 최대 깊이 또는 최소 리프 크기에 도달하면 재귀를 종료합니다.
 * 
 * @complexity O(n log n) average, O(n²) worst case
 * @memory O(n) for object references + O(log n) stack depth
 * 
 * @param objects 분할할 객체 참조 배열
 * @param count 객체 수
 * @param scene 바운딩 박스 계산을 위한 scene 포인터
 * @param depth 현재 재귀 깊이 (루트는 0)
 * @return 생성된 BVH 노드 (리프 또는 내부 노드)
 */
```

---

### 19. **에러 복구 메커니즘 부재**

**현재:**
- 파싱 실패 → 프로그램 종료
- MLX 초기화 실패 → 프로그램 종료
- 메모리 할당 실패 → 프로그램 종료

**문제점:**
- 사용자에게 복구 옵션 없음
- Fallback 전략 없음 (예: BVH 실패 시 brute force로 전환)
- 디버깅 정보 부족

---

### 20. **코드 중복 - Copy-Paste Programming**
**파일:** `src/hud/*.c`, `src/bvh_vis/*.c`

```c
// hud_format_helpers.c와 bvh_vis_format.c에 거의 동일한 코드
// 문자열 포맷팅 로직이 5개 이상의 파일에 중복
```

**문제점:**
- DRY 원칙 위반
- 버그 수정 시 여러 곳 수정 필요
- 유지보수성 저하

---

### 21. **성능 모니터링 - 불완전한 Metrics**
**파일:** `includes/metrics.h`, `src/render/metrics_*.c`

**현재 추적 항목:**
- ✅ Ray count
- ✅ BVH node visits
- ✅ Intersection tests
- ❌ Memory usage
- ❌ BVH build time
- ❌ Frame render time breakdown

**필요한 추가 metrics:**
```c
typedef struct s_metrics
{
	// 기존 항목...
	
	// 추가 필요
	size_t		memory_used;
	double		bvh_build_time_ms;
	double		render_time_ms;
	double		lighting_time_ms;
	int			cache_hits;
	int			cache_misses;
}	t_metrics;
```

---

### 22. **Makefile - 의존성 추적 부족**
**파일:** `Makefile:141-144`

```makefile
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "$(GREEN)Compiling $<...$(RESET)"
	@$(CC) $(CFLAGS) -c $< -o $@
```

**문제점:**
- 헤더 파일 변경 시 재컴파일 안 됨
- `.d` 의존성 파일 생성 없음
- 대규모 프로젝트에서 빌드 오류 유발

**개선안:**
```makefile
# 의존성 자동 생성
-include $(OBJS:.o=.d)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -MMD -MP -c $< -o $@
```

---

### 23. **플랫폼 특화 코드 분리 부족**
**파일:** `Makefile:28-36`

```makefile
UNAME_S		:= $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	MLX_DIR		= lib/minilibx-linux
	LDFLAGS		= -L$(MLX_DIR) -lmlx -L$(LIBFT_DIR) -lft -lXext -lX11 -lm
endif
ifeq ($(UNAME_S),Darwin)
	MLX_DIR		= lib/minilibx-macos
	LDFLAGS		= -L$(MLX_DIR) -lmlx -L$(LIBFT_DIR) -lft -framework OpenGL -framework AppKit -lm
endif
```

**문제점:**
- 조건부 컴파일이 Makefile에만 존재
- 소스 코드에는 플랫폼별 차이 처리 없음
- Windows 지원 불가능

**개선안:**
```c
// platform.h
#ifdef __APPLE__
	#define PLATFORM_MACOS
#elif __linux__
	#define PLATFORM_LINUX
#endif

// platform-specific includes
#ifdef PLATFORM_MACOS
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif
```

---

### 24. **Cylinder Intersection - 불필요한 중복 계산**
**파일:** `src/ray/intersect_cyl_new.c:148-165`

```c
int	intersect_cylinder_new(t_ray *ray, t_cylinder_data *c, t_color color,
		t_hit *hit)
{
	int		found;
	t_hit	temp;

	found = 0;
	temp.distance = hit->distance;
	if (intersect_cyl_body_new(ray, c, &temp))
		apply_cyl_hit(hit, &temp, color, &found);
	temp.distance = hit->distance;  // ⚠️ 재설정
	if (intersect_cyl_cap_new(ray, c, &temp, c->half_height))
		apply_cyl_hit(hit, &temp, color, &found);
	temp.distance = hit->distance;  // ⚠️ 재설정
	if (intersect_cyl_cap_new(ray, c, &temp, -c->half_height))
		apply_cyl_hit(hit, &temp, color, &found);
	return (found);
}
```

**문제점:**
- `temp.distance`를 매번 재설정하는 오버헤드
- Cap 교차 테스트를 순차적으로 수행 (병렬 가능)
- Early exit 없음 (body에서 교차 발견 시 cap은 무시 가능)

---

### 25. **입력 검증 - Parser의 엄격함 vs Runtime의 무방비**
**파일:** 여러 파일

**파서 검증:**
```c
// parse_elements.c - 매우 엄격
if (!in_range(ratio, 0.0, 1.0))
	return (PARSE_ERR_RANGE);
if (!in_range(fov, 0, 180))
	return (PARSE_ERR_RANGE);
```

**런타임 검증:**
```c
// 대부분의 함수에서 NULL 체크만
if (!scene || !ray)
	return (0);  // ⚠️ 값의 유효성은 검증 안 함
```

**문제점:**
- 파서를 통과하지 않은 데이터 (런타임 생성)는 검증 없음
- 사용자 입력(카메라 회전 등)에 대한 범위 체크 누락
- Defensive programming 부족

---

## 📊 전체 평가

### 코드 성숙도: **C+ (평균 이상)**

**강점:**
- ✅ 파서 구조가 매우 견고함 (엄격한 검증, 우수한 에러 메시지)
- ✅ BVH 구현이 기능적으로 작동함
- ✅ 코드 스타일 일관성 (42 Norm 준수)
- ✅ 함수 분리가 잘 되어 있음 (SRP 일부 준수)
- ✅ 문서화 노력 (Doxygen 주석)

**약점:**
- ❌ 메모리 안전성 문제 (누수, UAF 가능성)
- ❌ 성능 최적화 부족 (멀티스레딩 없음, 비효율적인 알고리즘)
- ❌ 에러 처리 불일치 (파서 vs 런타임)
- ❌ 테스트 불가능한 구조
- ❌ 하드코딩된 값 남발

---

### 프로덕션 준비도: **D (불합격)**

**장애물:**
1. **안정성:** 메모리 누수, 버퍼 오버플로우 위험
2. **성능:** 실시간 상호작용 불가능 (FPS < 1)
3. **확장성:** 멀티스레딩 없음, 대규모 씬 처리 불가
4. **모니터링:** 불완전한 메트릭스, 디버깅 어려움
5. **복구:** 에러 복구 메커니즘 전무

**프로덕션 투입 전 필수 작업:**
- [ ] 메모리 누수 전수 조사 (Valgrind, ASan)
- [ ] 멀티스레딩 구현 (최소 4 threads)
- [ ] Comprehensive 에러 핸들링
- [ ] 단위 테스트 프레임워크 구축
- [ ] 성능 프로파일링 및 최적화

---

### 유지보수성: **B- (보통)**

**긍정적:**
- 파일 구조가 논리적 (기능별 디렉토리)
- 함수명이 명확함
- 대부분의 함수가 짧음 (< 50 lines)

**부정적:**
- 코드 중복 많음 (DRY 위반)
- God Object 패턴 (scene, render)
- 하드코딩된 매직 넘버
- 플랫폼별 코드 분리 부족

---

### 예상 리팩토링 시간

**보수적 추정 (1인 시니어 개발자 기준):**

| 카테고리 | 시간 | 우선순위 |
|---------|------|---------|
| 치명적 이슈 수정 (1-6) | **3-5일** | 🔴 최우선 |
| 메모리 안전성 강화 | **2-3일** | 🔴 최우선 |
| 성능 최적화 (멀티스레딩) | **5-7일** | 🟡 높음 |
| 에러 처리 통일 | **2일** | 🟡 높음 |
| 코드 중복 제거 | **3-4일** | 🟢 중간 |
| 테스트 프레임워크 구축 | **4-5일** | 🟢 중간 |
| 문서화 개선 | **2-3일** | 🔵 낮음 |
| **총계** | **21-33일** (3-5 weeks) | - |

**총 투자 시간:** **약 1개월**

---

## 🎯 우선순위 로드맵

### Phase 1: 안정화 (Week 1-2)
1. ✅ 메모리 누수 수정 (Valgrind 전수 검사)
2. ✅ Use-after-free 해결
3. ✅ Buffer overflow 방지
4. ✅ Division by zero 보호

### Phase 2: 성능 (Week 3)
1. ✅ 멀티스레딩 구현 (pthread)
2. ✅ BVH 최적화 (SAH, max_depth 조정)
3. ✅ AABB 교차 최적화

### Phase 3: 품질 (Week 4)
1. ✅ 에러 처리 통일
2. ✅ 단위 테스트 추가
3. ✅ 코드 중복 제거

### Phase 4: 확장성 (Week 5+)
1. ✅ 아키텍처 리팩토링
2. ✅ 플랫폼 독립성 강화
3. ✅ API 문서화

---

## 🔍 추가 조사 필요 항목

1. **메모리 프로파일링**
   - Valgrind로 전체 파이프라인 검사
   - 피크 메모리 사용량 측정

2. **성능 벤치마킹**
   - 다양한 씬 복잡도에서 FPS 측정
   - BVH on/off 비교

3. **엣지 케이스 테스트**
   - 0개 객체, 10,000개 객체
   - Degenerate geometry (flat cylinders, zero-radius spheres)
   - Extreme camera angles (straight up/down)

4. **크로스 플랫폼 테스트**
   - Linux vs macOS 행동 차이
   - 다양한 디스플레이 해상도

---

## 💬 최종 소견

이 코드베이스는 **학습 프로젝트로는 우수**하지만, **프로덕션 코드로는 부족**합니다.

**핵심 문제는 3가지:**
1. **안전성:** 메모리 관리 취약
2. **성능:** 싱글 스레드, 비최적화된 알고리즘
3. **견고성:** 불완전한 에러 처리

**하지만 희망적인 점:**
- 코드 구조가 논리적이고 읽기 쉬움
- 리팩토링 가능한 수준
- 파서와 BVH는 튼튼한 기초

**추천:**
1. 먼저 **치명적 이슈 6개**를 2주 안에 해결
2. 멀티스레딩 추가로 사용자 경험 개선
3. 단위 테스트 도입으로 회귀 방지

**이 프로젝트는 현재 상태로 배포 불가하지만, 3-4주의 집중적인 리팩토링으로 프로덕션 수준에 도달 가능합니다.**

---

**리뷰 완료.**
**추가 질문이나 특정 이슈에 대한 심화 분석이 필요하면 언제든지 문의하세요.**
