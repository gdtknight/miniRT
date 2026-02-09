# 성능 최우선 개선 계획

**작성일**: 2026-02-09  
**기준**: 10년차 개발자 관점, 성능 최우선  
**원천**: `031-performance-review-report-2026-02-08.md`

---

## 문제 요약

현재 코드베이스의 **치명적 성능 병목 3가지**:

| 순위 | 이슈 | 영향 | 예상 개선 |
|------|------|------|-----------|
| **P0** | BVH `t_max` 미갱신 | BVH pruning 무력화 | **20-40%** |
| **P1** | per-pixel timing 상시 활성 | 측정 오버헤드 | **5-10%** |
| **P2** | metrics 출력 상시 활성 | I/O 병목 | **3-5%** |

**예상 누적 개선**: **28-55%** (S4 기준 25.1s → 11-18s)

---

## Phase 1: BVH Pruning 복원 (P0)

### 현상

```c
// bvh_traverse.c: bvh_node_intersect()
static int	bvh_node_intersect(t_bvh_node *node, t_ray ray,
		t_hit *hit, void *scene)
{
	double	t_min;
	double	t_max;

	// ...
	t_min = 0.001;
	t_max = 1000000.0;  // ❌ 고정값, hit->distance로 갱신 안됨
	if (!aabb_intersect(node->bounds, ray, &t_min, &t_max))
		// ...
}
```

**문제점**:
- Near child에서 hit 발견 → `hit->distance` 갱신
- Far child AABB 테스트 시 여전히 `t_max = 1e6` 사용
- **Pruning 불가** → Far child를 불필요하게 순회

### 해결 방안

```c
static int	bvh_node_intersect(t_bvh_node *node, t_ray ray,
		t_hit *hit, void *scene)
{
	double	t_min;
	double	t_max;

	// ...
	t_min = 0.001;
	t_max = hit->distance;  // ✅ 현재 최근접 hit 거리 사용
	if (!aabb_intersect(node->bounds, ray, &t_min, &t_max))
		// ...
}
```

**효과**:
- Near child hit 후 far child AABB가 hit보다 멀면 즉시 skip
- Child ordering (P6) 효과 극대화
- 깊은 트리에서 효과 누적

### 작업 범위

- **파일**: `src/spatial/bvh_traverse.c`
- **함수**: `bvh_node_intersect()` (1개 변수 변경)
- **리스크**: 매우 낮음 (표준 BVH pruning 기법)

---

## Phase 2: 측정 오버헤드 제거 (P1)

### 현상

```c
// render.c: render_pixel_full_quality()
pixel_start = get_time_ns();
color = trace_primary_ray(...);
pixel_end = get_time_ns();
pixel_timing_add_sample(...);  // 480,000회/프레임
```

**문제점**:
- 모든 픽셀에서 `get_time_ns()` 2회 호출
- S4 기준: 480,000 픽셀 × 2 = **960,000회 시간 측정/프레임**
- 측정이 렌더링 자체보다 비쌀 수 있음

### 해결 방안 (옵션 A: 조건부 컴파일)

```c
// render.c
#ifdef ENABLE_PIXEL_TIMING
	pixel_start = get_time_ns();
#endif
	color = trace_primary_ray(...);
#ifdef ENABLE_PIXEL_TIMING
	pixel_end = get_time_ns();
	pixel_timing_add_sample(...);
#endif
```

**단점**: 42 Norm에서 `#ifdef` 허용 여부 불명확

### 해결 방안 (옵션 B: 런타임 플래그)

```c
// minirt.h: t_scene 또는 render 구조체
typedef struct s_render
{
	// ...
	int	enable_pixel_timing;  // 0 = disabled
}	t_render;

// render.c
if (render->enable_pixel_timing)
{
	pixel_start = get_time_ns();
	color = trace_primary_ray(...);
	pixel_end = get_time_ns();
	pixel_timing_add_sample(...);
}
else
	color = trace_primary_ray(...);
```

**장점**: Norm 준수, 런타임 전환 가능  
**단점**: 분기 추가 (하지만 측정 비용보다 훨씬 저렴)

### 작업 범위

- **파일**: `src/render/render.c`, `includes/render.h` (또는 관련 헤더)
- **함수**: `render_pixel_full_quality()`, `render_create()`
- **리스크**: 낮음 (기능 변경 없음, 측정만 선택적)

---

## Phase 3: Metrics 출력 제거 (P2)

### 현상

```c
// window_loop.c
static void	render_full_and_display(t_render *render)
{
	// ...
	metrics_print_summary(&render->scene->metrics);  // stdout 출력
}
```

**문제점**:
- 매 프레임마다 stdout에 20+ 라인 출력
- I/O 블로킹 발생 가능
- 터미널 스크롤 오버헤드

### 해결 방안

```c
// window_loop.c
static void	render_full_and_display(t_render *render)
{
	// ...
	if (render->enable_metrics_print)  // 기본 0
		metrics_print_summary(&render->scene->metrics);
}
```

**활성화 방법**:
- 키 바인딩 (예: 'M' 키로 토글)
- 또는 `--metrics` CLI 플래그

### 작업 범위

- **파일**: `src/window/window_loop.c`, `includes/window.h` 또는 `render.h`
- **함수**: `render_full_and_display()`, 키 핸들러
- **리스크**: 매우 낮음

---

## 구현 우선순위

| Phase | 작업 | 예상 시간 | 예상 개선 | 의존성 |
|-------|------|-----------|-----------|--------|
| **1** | BVH pruning 복원 | 10분 | **20-40%** | 없음 |
| **2** | Pixel timing 분리 | 30분 | **5-10%** | 없음 |
| **3** | Metrics 출력 분리 | 15분 | **3-5%** | 없음 |

**총 예상 시간**: 55분  
**예상 누적 개선**: **28-55%**

---

## 검증 계획

### Phase 1 검증

**명령**:
```bash
./miniRT scenes/perf/perf_all_objects.rt
```

**측정**:
- Frame time (baseline: 25,121 ms)
- BVH nodes visited (변화 확인)
- BVH skip rate (증가 예상)

**목표**: Frame time ≤ 20,000 ms

### Phase 2+3 검증

**Before**:
```bash
time ./miniRT scenes/perf/perf_all_objects.rt
# stdout에 metrics 출력 포함
```

**After**:
```bash
time ./miniRT scenes/perf/perf_all_objects.rt
# stdout 출력 없음, 측정 오버헤드 제거
```

**목표**: Frame time ≤ 15,000 ms (누적)

---

## 리스크 분석

| Phase | 리스크 | 완화 방안 |
|-------|--------|-----------|
| 1 | `t_max` 초기값 버그 | `hit->distance` 초기화 검증 |
| 2 | Norm 위반 가능성 | 런타임 플래그 사용 (옵션 B) |
| 3 | 디버깅 불편 | 키 바인딩으로 토글 가능 |

---

## Phase 4 이후 (선택)

### 4. Shadow LUT 재생성 (중간 우선순위)

**파일**: `src/lighting/shadow_config.c`  
**이슈**: `set_shadow_samples()` 시 LUT 불일치  
**영향**: 안정성 > 성능

### 5. 컴파일 최적화 플래그 (저우선순위)

**파일**: `Makefile`  
**추가**: `CFLAGS += -O2` (릴리스 빌드)  
**예상 개선**: 10-20% (추가)

---

## 결론

**10년차 개발자 판단**:

1. **Phase 1 (BVH pruning)은 무조건 적용** - ROI 압도적
2. **Phase 2+3은 벤치마크 정확도를 위해 필수** - 측정 왜곡 제거
3. **Phase 4+5는 여유 있을 때** - 안정성/편의성 개선

**예상 최종 성능**:
- S4: **25,121 ms → 11-15,000 ms** (40-60% 개선)
- 컴파일 최적화 추가 시: **9-12,000 ms** (50-70% 개선)

---

*Plan created: 2026-02-09*
