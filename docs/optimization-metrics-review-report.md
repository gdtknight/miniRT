# Optimization Metrics 측정 검토 리포트

작성일: 2026-02-05
대상 코드베이스: miniRT (현 브랜치 기준)
범위: metrics/pixel_timing 측정 코드 + 호출 경로

---

## 1. 목적
최근 추가된 optimization metrics 계측 코드가 **의도한 지표를 정확하게 측정하고 있는지** 점검하고, 문제점과 수정 제안을 정리한다.

---

## 2. 현재 측정 흐름 요약

### 2.1 Frame 측정
- `metrics_start_frame()` → 프레임 시작 시점 및 카운터 초기화
- `render_scene_to_buffer()` → 실제 렌더 수행
- `metrics_end_frame()` → 프레임 종료 시점 및 FPS 계산
- `metrics_print_summary()` → stdout 출력

호출 경로:
- `window_loop.c` → `execute_render_pass()`

### 2.2 Ray/Intersect/BVH 카운터
- `metrics_add_ray()` : primary ray 추적 시 증가
- `metrics_add_intersect_test()` : 교차 테스트마다 증가
- `metrics_add_bvh_node_visit()` : BVH 노드 방문 시 증가
- `metrics_add_bvh_box_test()` : AABB 테스트마다 증가
- `metrics_add_bvh_skip()` : AABB miss 시 증가(현재 미호출)

### 2.3 Pixel Timing
- `render_pixel()`에서 `get_time_ns()` 기반 per-pixel 시간 측정 후 `pixel_timing_add_sample()` 호출
- 프레임 종료 후 `pixel_timing_calculate_stats()` 및 `pixel_timing_print_stats()` 호출

---

## 3. 측정 정확성 검토 결과

### 3.1 [High] Intersect tests/ray 지표 왜곡
**문제:**
- `metrics_add_ray()`는 **primary ray**만 카운트
- `metrics_add_intersect_test()`는 **primary + shadow** 모두 카운트

따라서 다음 값이 의미적으로 왜곡됨:
- `Tests/ray` (`intersect_tests / rays_traced`)
- `BVH efficiency` (`1 - intersect_tests / (rays_traced * object_count)`)

**영향:**
- shadow ray가 많을수록 교차 테스트가 급증하여 **BVH 효율이 과도하게 낮게 출력**됨
- `Tests/ray`가 실제 primary ray 대비 과대평가됨

**근거 위치:**
- primary ray 카운트: `src/render/trace.c:69`
- shadow ray 교차 테스트 카운트: `src/lighting/shadow_test.c:39`
- BVH leaf 교차 테스트 카운트: `src/spatial/bvh_traverse.c:64`
- summary 출력: `src/render/metrics_calc.c:90`

---

### 3.2 [Medium] BVH skip 카운터 미작동
**문제:**
- `metrics_add_bvh_skip()`이 호출되지 않아 `tests_skipped`가 항상 0

**영향:**
- `BVH skip rate` 지표가 항상 0 → BVH rejection 효율 분석 불가

**근거 위치:**
- `metrics_add_bvh_box_test()`만 호출됨: `src/spatial/bvh_traverse.c:129-134`

---

### 3.3 [Medium] Pixel timing 통계가 프레임 단위가 아님
**문제:**
- `pixel_timing_add_sample()`의 count/total/min/max가 **프레임마다 reset되지 않음**
- 결과적으로 출력되는 통계는 **누적 통계**이며 “이번 프레임”의 성능과 불일치

**영향:**
- 프레임별 성능 변화 추적이 어려움
- 실측 비교(A/B) 시 프레임 간 오염 가능

**근거 위치:**
- `render_scene_to_buffer()`는 종료 시점에만 calculate/print 수행: `src/render/render.c:140-165`
- `pixel_timing_add_sample()`은 누적만 수행: `src/render/pixel_timing.c:62-75`

---

## 4. 수정 제안

### 4.1 Intersect tests 분리 또는 Ray 카운터 보강
**선택지 A: shadow 전용 카운터 분리 (권장)**
- `t_metrics`에 `shadow_intersect_tests` 등 별도 필드 추가
- `metrics_add_intersect_test()`는 primary 전용으로 한정
- shadow 경로에서는 `metrics_add_shadow_intersect_test()` 호출
- summary 출력 시 primary와 shadow를 구분 출력

**선택지 B: rays_traced에 shadow ray 포함**
- `is_in_shadow()` 또는 shadow sample 루프에서 `metrics_add_ray()` 증가
- 단점: primary/secondary 구분이 없어져 지표 해석이 모호해짐

**권장 방향:** 선택지 A

---

### 4.2 BVH skip 카운터 추가
- `aabb_intersect()` 실패 직전에 `metrics_add_bvh_skip()` 호출

예시 위치:
- `src/spatial/bvh_traverse.c`

---

### 4.3 Pixel timing 프레임 단위 통계화
**선택지 A: 프레임 시작 시 reset (권장)**
- `metrics_start_frame()` 또는 `render_scene_to_buffer()` 진입 시
  `pixel_timing_reset()` 호출
- reset 시 count/min/max/total 초기화

**선택지 B: 누적 통계 유지 + 출력 시 프레임 범위 표기**
- 누적 통계를 명시하고, 프레임별 통계를 별도로 수집
- 출력 포맷에 "누적"임을 명시

**권장 방향:** 선택지 A

---

## 5. 추가 고려사항
- **BVH efficiency 계산**은 “primary only intersect_tests” 기준이 가장 해석 가능함
- Shadow BVH 적용 이후에는 shadow path도 별도 효율 지표가 필요함
- metrics 출력이 프레임마다 stdout에 남아 성능에 영향을 줄 수 있으므로
  릴리즈 빌드에서는 로그 토글 플래그 고려 필요

---

## 6. 요약
- 현재 metrics 계측은 **카운터 범위 불일치**, **BVH skip 미작동**, **pixel timing 누적** 문제가 있어
  “정확한 성능 분석”에 직접 사용하기에는 부정확하다.
- 위 수정 제안(특히 shadow 분리 + skip 카운터 추가 + timing reset)을 반영하면
  **지표 신뢰도가 크게 개선**될 것이다.

