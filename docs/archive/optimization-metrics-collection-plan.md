# Optimization Metrics 수집 계획

작성일: 2026-02-05
대상 코드베이스: miniRT (현재 브랜치 기준)

---

## 1. 목적
optimization 관련 변경 효과를 **정량적으로 비교**할 수 있도록, 일관된 지표와 측정 절차를 정의한다.

---

## 2. 수집 대상 지표

### 2.1 Primary path
- `rays_traced`
- `intersect_tests`
- `primary_tests_per_ray = intersect_tests / rays_traced`

### 2.2 Shadow path
- `shadow_intersect_tests`
- `shadow_tests_per_pixel = shadow_intersect_tests / (W * H)`

### 2.3 BVH
- `nodes_visited`
- `tests_skipped`
- `skip_rate = tests_skipped / nodes_visited`

### 2.4 Timing
- Frame time (ms)
- FPS
- Pixel timing: min / avg / median / p95 / p99

---

## 3. 측정 범위 및 조건

### 3.1 범위
- full-quality 렌더링만 측정
- low-quality 경로는 비교 대상에서 제외

### 3.2 조건 고정
- 해상도: 1440x900
- shadow samples: 16 (기본값)
- 동일 하드웨어, 동일 실행 조건

---

## 4. 베이스라인 시나리오

| 씬 | 용도 | 특징 |
|----|------|------|
| `scenes/valid/valid_smoke_simple.rt` | 소형 | 객체 수 적음 |
| `scenes/perf/perf_spheres_20.rt` | 중형 | 객체 20 |
| `scenes/perf/perf_spheres_50.rt` | 대형 | 객체 50 |
| `scenes/perf/perf_all_objects.rt` | 혼합 | 다양한 타입 포함 |

- 각 씬별 **5회 실행**
- 중앙값 기준으로 기록

---

## 5. A/B 비교 절차

1. 변경 전 baseline 측정
2. 변경 적용 후 동일 조건 측정
3. 개선율 계산:
   ```
   개선율 = (baseline - optimized) / baseline x 100%
   ```

---

## 6. 기록 파일 (권장)

- `docs/benchmark-baseline.md`
- `docs/benchmark-results.md`

기록 항목 예시:
- Frame time avg / p95 / p99
- FPS (rolling)
- Primary tests/ray
- Shadow tests/pixel
- BVH skip rate
- Pixel timing 통계

---

## 7. 해석 주의사항

- `primary_tests_per_ray`는 **shadow 경로 제외** 기준
- BVH `skip_rate`는 **nodes_visited 대비 skip 비율**
- pixel timing은 **프레임 단위 통계**이며 full-quality에만 적용

---

## 8. 추후 확장 (선택)

- shadow ray count 분리 (shadow rays cast)
- BVH box test 카운터 추가
- HUD 또는 stdout 토글 옵션 추가

