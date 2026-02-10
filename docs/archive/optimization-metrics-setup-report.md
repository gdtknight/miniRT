# miniRT Optimization Metrics Setup Report

작성일: 2026-02-04
기준 브랜치: `develop` (026-resource-cleanup 병합 포함)

---

## 1. 현재 계측 인프라 현황

### 1.1 사용 중인 계측

| 함수 | 위치 | 호출처 | 측정 항목 |
|------|------|--------|-----------|
| `metrics_start_frame()` | metrics_frame.c:63 | window_loop.c:49 | 프레임 시작 타이머 |
| `metrics_end_frame()` | metrics_frame.c:112 | window_loop.c:58 | 프레임 종료, FPS 계산 |
| `metrics_add_ray()` | metrics_counters.c:20 | trace.c:68 | ray 총 수 카운트 |
| `pixel_timing_add_sample()` | pixel_timing.c:62 | render.c:65 | 픽셀 렌더 시간(ns) (주1) |
| `pixel_timing_print_stats()` | pixel_timing_print.c:61 | render.c:164 | min/max/avg/median/p95/p99 출력 |
| `calculate_fps()` | metrics_calc.c:21 | hud_performance.c | HUD FPS 표시 |

> **(주1) pixel_timing 샘플링 제약**: `MAX_PIXEL_SAMPLES=480,000`으로 전체 픽셀
> (1,296,000)의 37%만 수집 가능. 스캔 순서(좌→우, 상→하) 기준으로 앞쪽 ~37%
> 픽셀만 기록되므로 화면 하단/우측 픽셀은 통계에서 제외됨. 따라서 p95/p99 등
> 분포 지표는 전체 화면 대표값이 아닌 부분 샘플 기반임에 유의.

### 1.2 정의되었으나 미사용 함수 (호출처 0곳)

| 함수 | 위치 | 의도된 용도 |
|------|------|-------------|
| `metrics_add_intersect_test()` | metrics_counters.c:31 | 교차 테스트 수 |
| `metrics_add_bvh_node_visit()` | metrics_counters.c:42 | BVH 노드 방문 수 |
| `metrics_add_bvh_skip()` | metrics_counters.c:53 | BVH early rejection 수 |
| `metrics_add_bvh_box_test()` | metrics_counters.c:64 | AABB 테스트 수 |
| `metrics_reset_bvh()` | metrics_calc.c:70 | BVH 카운터 리셋 |
| `calculate_bvh_efficiency()` | metrics_calc.c:36 | BVH 효율 퍼센트 |
| `calculate_avg_tests_per_ray()` | metrics_calc.c:58 | ray당 평균 테스트 수 |

### 1.3 현재 데이터 구조

```c
// metrics.h
typedef struct s_bvh_metrics       // 모든 필드 미사용
{
    long  nodes_visited;
    long  tests_skipped;
    long  box_tests;
} t_bvh_metrics;

typedef struct s_ray_metrics
{
    long  rays_traced;             // 사용 중
    long  intersect_tests;         // 미사용
} t_ray_metrics;

typedef struct s_frame_timing      // 사용 중
{
    struct timeval  start_time;
    struct timeval  end_time;
    long            render_time_us;
    long            frame_times_us[60];
    int             frame_index;
    long            frame_count;
    double          fps;
} t_frame_timing;

// pixel_timing.h
typedef struct s_pixel_timing      // 사용 중
{
    long    *samples;              // 480,000개 버퍼 (전체 1,296,000 픽셀의 37%)
    size_t  count;                 // count가 capacity에 도달하면 이후 샘플 무시
    size_t  capacity;              // MAX_PIXEL_SAMPLES = 480,000
    long    min_time;
    long    max_time;
    long    total_time;
    int     enabled;
} t_pixel_timing;
```

---

## 2. 계측 확장 계획

### 2.1 Phase 1: 기존 미사용 카운터 연결

이미 정의된 함수와 구조체를 실제 핫패스에 연결하는 작업.
구조체/헤더 변경 없음, 호출 삽입만 필요.

#### 2.1.1 BVH 노드 방문 카운터

**삽입 위치**: `src/spatial/bvh_traverse.c`

```
bvh_node_intersect() 진입 시:
  metrics_add_bvh_node_visit()     [bvh_traverse.c:126 이후]

aabb_intersect() 호출 시:
  metrics_add_bvh_box_test()       [bvh_traverse.c:129 이전]

aabb_intersect() 실패 시:
  metrics_add_bvh_skip()           [bvh_traverse.c:130, return(0) 직전]
```

**문제점**: `bvh_node_intersect()`는 `t_scene*`이 아닌 `void *scene`을 받음.
metrics는 `scene->metrics`에 저장됨.

**해결 방안**:
- `void *scene`을 `t_scene *`으로 캐스트 (`(t_scene *)scene`)
- 또는 metrics 포인터를 traversal 함수에 전달

**42 Norm 고려**: 함수 인자 4개 제한 — 현재 `bvh_node_intersect(node, ray, hit, scene)` 이미 4개. metrics 추가 불가.
- **권장**: scene 캐스트 방식 사용 (`((t_scene *)scene)->metrics`)

#### 2.1.2 교차 테스트 카운터

교차 테스트 카운터는 `intersect_object_new()`를 호출하는 3곳에 삽입한다.
(`intersect_object_new()` 자체는 ray, obj, hit만 받아 scene/metrics 접근 불가.)

**삽입 위치** (호출자 3곳):

```
check_all_objects() 루프 내:
  metrics_add_intersect_test()     [trace.c:43]

check_object_shadow() 루프 내:
  metrics_add_intersect_test()     [shadow_test.c:38]

bvh_leaf_intersect() 루프 내:
  metrics_add_intersect_test()     [bvh_traverse.c:63]
```

#### 2.1.3 수정 파일 목록

| 파일 | 변경 내용 |
|------|-----------|
| `src/spatial/bvh_traverse.c` | node_visit, box_test, skip 카운터 3개 + bvh_leaf_intersect 내 intersect_test 카운터 |
| `src/render/trace.c` | check_all_objects 내 intersect_test 카운터 |
| `src/lighting/shadow_test.c` | check_object_shadow 내 intersect_test 카운터 |

---

### 2.2 Phase 2: Shadow 전용 계측 추가

최적화 효과의 대부분이 shadow 경로에서 발생하므로 별도 계측 필요.

#### 2.2.1 신규 구조체

```c
// shadow.h 또는 metrics.h에 추가
typedef struct s_shadow_metrics
{
    long  shadow_rays_cast;        // shadow ray 총 수
    long  shadow_hits;             // 그림자 판정 횟수
    long  shadow_intersect_tests;  // shadow ray 교차 테스트 수
    long  shadow_bvh_skips;        // shadow BVH early rejection
} t_shadow_metrics;
```

#### 2.2.2 삽입 위치

| 카운터 | 삽입 위치 | 파일 |
|--------|-----------|------|
| shadow_rays_cast | `is_in_shadow()` 진입 | shadow_test.c:58 |
| shadow_hits | `check_object_shadow()` return(1) | shadow_test.c:39 |
| shadow_intersect_tests | `check_object_shadow()` 루프 내 | shadow_test.c:38 |
| shadow_bvh_skips | (P1 구현 후) BVH any-hit 경로 | shadow_test.c |

#### 2.2.3 핵심 파생 지표

- **Shadow tests/pixel**: `shadow_intersect_tests / pixels_rendered`
- **Shadow hit rate**: `shadow_hits / shadow_rays_cast`
- **Shadow BVH efficiency**: `1 - (shadow_intersect_tests / (shadow_rays_cast * object_count))`

---

### 2.3 Phase 3: 구간 타이밍 분할

현재 `pixel_timing`은 전체 픽셀 시간만 측정. 내부 구간 분리로 병목 정밀 분석.

#### 2.3.1 구간 정의

```c
typedef struct s_phase_timing
{
    long  camera_ray_ns;      // create_camera_ray 소요
    long  bvh_traverse_ns;    // bvh_intersect 소요
    long  lighting_ns;        // apply_lighting 소요 (shadow 포함)
    long  shadow_ns;          // calculate_shadow_factor 소요
} t_phase_timing;
```

#### 2.3.2 삽입 위치

```
render_pixel()에서:
  t0 = get_time_ns();
  ray = create_camera_ray(...);
  t1 = get_time_ns();              // camera_ray_ns = t1 - t0

trace_ray()에서:
  t2 = get_time_ns();
  bvh_intersect(...);
  t3 = get_time_ns();              // bvh_traverse_ns = t3 - t2

  t4 = get_time_ns();
  apply_lighting(...);
  t5 = get_time_ns();              // lighting_ns = t5 - t4

calc_lighting_factor()에서:
  t6 = get_time_ns();
  calculate_shadow_factor(...);
  t7 = get_time_ns();              // shadow_ns = t7 - t6
```

#### 2.3.3 구현 고려사항

- **성능 오버헤드**: `get_time_ns()` 호출 자체 ~100-200ns
- 1,296,000 pixels x 4 계측점 = ~5.2M 타이머 호출
- **전략**: 샘플링 기반 — 매 100번째 픽셀만 계측 (오버헤드 1% 이하)
- `if (pixel_index % 100 == 0)` 조건부 계측

#### 2.3.4 42 Norm 제약

- 타이밍 코드 추가 시 함수 본문이 25줄 제한에 걸릴 수 있으므로 헬퍼 함수 분리 검토
- 변수 5개 제한 → timing 변수를 배열로 통합하는 등 변수 수 절감 필요
- 구현 시점에 대상 함수의 줄 수를 확인하고 분리 여부 판단

---

### 2.4 Phase 4: HUD 표시 확장

현재 HUD에 FPS, frame time, BVH status만 표시. 추가 지표 표시.

#### 2.4.1 추가 표시 항목

```
=== Performance ===
FPS: 1.2
Frame: 833.3ms
BVH: ON
--- Details ---              ← 신규
Rays: 1,296,000
Intersects/ray: 12.3
Shadow tests: 20,736,000
BVH nodes/ray: 8.2
BVH skip rate: 45.2%
```

#### 2.4.2 토글 방식

- 기존 `RENDER_SHOW_INFO` 플래그 (`0x10`) 활용
- 단축키(예: 'I')로 상세 지표 토글
- 기본: 간략 모드 (FPS/Frame/BVH만)
- 상세: 전체 지표 표시

#### 2.4.3 수정 파일

| 파일 | 변경 내용 |
|------|-----------|
| `src/hud/hud_performance.c` | 상세 지표 렌더링 함수 추가 |
| `src/hud/hud_format_simple.c` | 정수/퍼센트 포맷 함수 추가 |
| `src/window/window_key_handlers.c` | 'I' 키 핸들러 추가 |

---

## 3. 측정 프로토콜

### 3.1 기준선 (Baseline) 측정

최적화 적용 전 현재 코드의 정확한 성능 수치 기록.

#### 측정 절차

1. 기존 씬 활용 (별도 생성 불필요):
   - `scenes/valid/valid_smoke_simple.rt` — 소형 (objects < 10)
   - `scenes/perf/perf_spheres_20.rt` — 중형 (objects 20)
   - `scenes/perf/perf_spheres_50.rt` — 대형 (objects 50)
   - `scenes/perf/perf_all_objects.rt` — 혼합 오브젝트 타입

2. 각 씬별 5회 렌더 실행, 결과 수집:
   ```
   Frame time avg / p95 / p99
   FPS (60-frame rolling average)
   Rays traced
   Pixel timing: min / max / avg / median / p95 / p99
   ```

3. 결과를 `docs/benchmark-baseline.md`에 기록 (Step 2 실행 시 신규 생성)

#### 측정 환경 통제

- 동일 하드웨어
- 동일 해상도 (1440x900)
- 동일 shadow samples (16)
- BVH ON/OFF 양쪽 측정
- 5회 측정 후 중앙값 사용

### 3.2 최적화별 A/B 비교

각 최적화 항목 적용 시:

1. 변경 전 (baseline) 수치 확인
2. 변경 적용
3. 동일 씬/조건으로 5회 측정
4. 지표별 변화율 계산:
   ```
   개선율 = (baseline - optimized) / baseline x 100%
   ```

### 3.3 핵심 KPI 정의

| KPI | 단위 | 산출 방법 | 목적 |
|-----|------|-----------|------|
| Frame time | ms | `metrics.timing.render_time_us / 1000` | 전체 렌더 속도 |
| FPS | fps | `1,000,000 / render_time_us` | 사용자 체감 |
| Pixel p95 | us | `pixel_timing` 95th percentile | worst-case 픽셀 |
| Intersect tests/ray | count | `intersect_tests / rays_traced` | BVH 효율 |
| BVH nodes/ray | count | `nodes_visited / rays_traced` | traversal 효율 |
| BVH skip rate | % | `tests_skipped / box_tests x 100` | AABB rejection 효율 |
| Shadow tests/pixel | count | `shadow_intersect_tests / pixel_count` | shadow 비용 |
| Shadow time ratio | % | `shadow_ns / total_pixel_ns x 100` | shadow 비중 |

---

## 4. 구현 순서

### Step 1: 기존 카운터 연결 (Phase 1)

- bvh_traverse.c에 node_visit/box_test/skip 카운터 3개 + bvh_leaf_intersect 내 intersect_test
- trace.c, shadow_test.c에 intersect_test 카운터 각 1개
- intersect_test 카운터 삽입 총 3곳: check_all_objects, check_object_shadow, bvh_leaf_intersect
- **추가 함수**: 0개 (기존 함수 호출만 삽입)
- **검증**: `pixel_timing_print_stats()` 출력에 카운터 값 추가

### Step 2: Baseline 측정

- 기존 scenes/valid, scenes/perf 씬 활용 (4종)
- 5회 측정, 결과 기록
- 카운터 값 포함한 baseline 문서화

### Step 3: Shadow 계측 (Phase 2)

- `t_shadow_metrics` 구조체 추가
- shadow_test.c에 카운터 삽입
- HUD 또는 stdout 출력으로 shadow 지표 노출

### Step 4: 최적화 적용 및 비교

- optimization-research-report.md의 Phase A부터 순차 적용
- 각 최적화마다 A/B 비교 실행
- 결과를 `docs/benchmark-results.md`에 누적 기록 (Step 4 실행 시 신규 생성)

> **참고**: `docs/benchmark-baseline.md`와 `docs/benchmark-results.md`는 현 시점에서
> 존재하지 않으며, 각각 Step 2와 Step 4 실행 시 실측 데이터로 생성할 계획 문서임.

### Step 5: 구간 타이밍 (Phase 3, 선택적)

- 샘플링 기반 구간 분할 계측
- shadow vs lighting vs traversal 비중 확인
- 추가 최적화 우선순위 재조정

---

## 5. 제약 사항 및 주의점

### 5.1 계측 오버헤드

- 카운터 증분 (`long++`): ~1 cycle — 무시 가능
- `get_time_ns()` 호출: ~100-200ns — 전체 계측 시 ~1-2% 오버헤드
- 샘플링으로 오버헤드 제어 가능

### 5.2 42 Norm 호환성

| 제약 | 영향 | 대응 |
|------|------|------|
| 함수 25줄 | 타이밍 코드 추가 시 초과 가능 | 헬퍼 함수 분리 |
| 변수 5개 | 타이밍 변수 추가 시 초과 가능 | 배열/구조체로 통합 |
| 인자 4개 | metrics 전달 불가 | scene 캐스트 사용 |
| 파일 5함수 | 계측 함수 추가 시 초과 가능 | 파일 분리 |

### 5.3 릴리즈 빌드 고려

계측 코드의 프로덕션 영향을 최소화하기 위한 방안:

- 카운터 증분: 비용 무시 가능 → 항상 활성
- 구간 타이밍: 조건부 활성 (`pixel_timing.enabled` 플래그 활용)
- HUD 상세 모드: 토글 방식 (기본 비활성)
