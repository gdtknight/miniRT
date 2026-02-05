# miniRT Baseline Benchmark Report

작성일: 2026-02-05
기준 브랜치: `027-baseline-benchmark` (develop 기반)
목적: 최적화 적용 전 현재 코드의 성능 기준점 기록

---

## 1. 측정 환경

| 항목 | 값 |
|------|-----|
| HW | Apple M1, 8 cores (4P + 4E), 16 GB |
| OS | macOS 26.2 (Build 25C56) |
| 해상도 | 1440 x 900 (1,296,000 pixels) |
| Shadow samples | 16 (기본값, softness 0.3) |
| BVH | ON (기본, median split) |
| 빌드 | `cc -Wall -Wextra -Werror` (최적화 플래그 없음, -O0 implicit) |
| 측정 횟수 | 1회 (첫 full-quality 렌더 완료 기준) |

### Pixel timing 샘플링 제약

`MAX_PIXEL_SAMPLES=480,000` — 전체 1,296,000 픽셀의 37%.
스캔 순서(좌→우, 상→하) 기준으로 앞쪽 픽셀만 수집되므로,
분포 지표(p95/p99)는 전체 화면 대표값이 아닌 부분 샘플 기반이다.

---

## 2. 측정 결과

### 2.1 씬별 요약

| 씬 | Objects | 타입 | Frame time | FPS |
|----|---------|------|-----------|-----|
| perf_timing | 3 | sp 3 | 514 ms | 1.9 |
| perf_spheres_20 | 20 | sp 20 | 1,836 ms | 0.5 |
| perf_spheres_50 | 50 | sp 50 | 3,366 ms | 0.3 |
| perf_all_objects | 18 | sp 7 + cy 7 + pl 3 | 27,174 ms | 0.0 |

### 2.2 Ray / Intersect 지표

| 씬 | Rays | Intersect tests | Tests/ray | BVH nodes | BVH box tests |
|----|------|-----------------|-----------|-----------|---------------|
| perf_timing | 1,296,000 | 4,553,136 | 3.5 | 1,395,384 | 1,395,384 |
| perf_spheres_20 | 1,296,000 | 40,416,610 | 31.2 | 5,223,492 | 5,223,492 |
| perf_spheres_50 | 1,296,000 | 105,172,495 | 81.2 | 4,465,654 | 4,465,654 |
| perf_all_objects | 1,296,000 | 376,326,760 | 290.4 | 19,629,738 | 19,629,738 |

### 2.3 Pixel Timing (나노초 → 마이크로초 변환)

| 씬 | Min (us) | Max (us) | Avg (us) | Median (us) | p95 (us) | p99 (us) |
|----|----------|----------|----------|-------------|----------|----------|
| perf_timing | 0.0 | 83.0 | 0.6 | 0.0 | 1.0 | 1.0 |
| perf_spheres_20 | 0.0 | 56.0 | 3.3 | 1.0 | 2.0 | 12.0 |
| perf_spheres_50 | 1.0 | 138.0 | 6.5 | 1.0 | 3.0 | 24.0 |
| perf_all_objects | 5.0 | 270.0 | 56.1 | 20.0 | 22.0 | 23.0 |

---

## 3. 분석

### 3.1 프레임 타임 스케일링

```
Objects   Frame time   배율 (vs 3 obj)
  3         514 ms      1.0x
 20       1,836 ms      3.6x
 50       3,366 ms      6.5x
 18*     27,174 ms     52.9x   (* 실린더 포함)
```

- sphere-only 씬: 오브젝트 수에 거의 선형 비례 (3→50 = 16.7x objects, 6.5x time)
- perf_all_objects (18 mixed): 오브젝트 수는 perf_spheres_20보다 적지만 14.8x 느림

### 3.2 실린더 교차 비용

perf_all_objects (18 obj)의 Tests/ray가 290.4인 반면, perf_spheres_20 (20 obj)은 31.2.
오브젝트 수는 비슷한데 교차 테스트가 9.3x 많다. 원인:

1. **실린더 교차 계산 자체가 비쌈** — 구는 판별식 1회, 실린더는 측면+캡 복합 계산
2. **shadow ray 폭증** — 실린더/평면이 많은 씬에서 그림자 영역 증가 → shadow 교차 테스트 증가
3. **BVH miss 폴백** — `trace_ray()`에서 BVH가 hit을 못 찾으면 `check_all_objects()` 폴백 실행, 교차 테스트 이중 카운트

### 3.3 BVH 이중 탐색 문제

모든 씬에서 `BVH efficiency = 0.0%`로 표시됨.
이유: `intersect_tests > rays_traced * object_count` (naive보다 많음).

```c
// trace.c — 현재 로직
if (scene->bvh && scene->bvh->enabled)
    if (bvh_intersect(...))  // BVH 경로: intersect_test 카운트
        found = 1;
if (!found)
    check_all_objects(...)   // 폴백 경로: intersect_test 다시 카운트
```

배경 픽셀(ray miss)에서 BVH 탐색 후 `check_all_objects`가 다시 실행됨.
BVH가 전체 오브젝트를 커버하므로 이 폴백은 불필요하다.

**영향 추정**: 1,296,000 rays 중 miss ray 비율 × object_count만큼 불필요 테스트 발생.

### 3.4 Shadow 경로 지배

shadow samples = 16이므로, hit pixel 1개당 최대 16 × object_count 교차 테스트 발생.
perf_all_objects 기준:
- 1,296,000 rays × 290.4 tests/ray = 376M tests
- shadow 기여 추정: (290.4 - 18) / 290.4 ≈ 93.8% (primary 제외 대부분 shadow)

shadow 경로가 전체 비용의 대부분을 차지한다.

### 3.5 pixel timing 분포

perf_all_objects의 p95(22us)와 p99(23us)가 median(20us)과 가까운 반면 max는 270us.
상위 1% 이상에서 극단적 비용 픽셀이 존재하나 대부분 픽셀은 비슷한 비용.
이는 shadow ray가 모든 오브젝트를 스캔하는 brute-force 특성(비용이 균일)과 일치.

---

## 4. 최적화 우선순위 시사점

측정 결과 기반으로 `docs/optimization-research-report.md`의 이론 추정을 재평가:

| 우선순위 | 항목 | 근거 |
|----------|------|------|
| **1** | BVH miss 폴백 제거 | 모든 씬에서 이중 탐색 발생. 제거 시 miss ray의 intersect_test 절반 감소 |
| **2** | Shadow ray BVH 적용 (P1) | shadow가 전체 비용의 ~94% 차지. brute-force → BVH 전환 시 최대 효과 |
| **3** | Shadow offset LUT (P4) | 16 samples × sqrt/cos/sin 제거 |
| **4** | Specular pow32 경량화 (P3) | 모든 hit pixel에 적용, 낮은 위험 |
| **5** | BVH traversal 개선 (P5, P6) | nodes_visited 절대 수가 크므로 per-node 비용 감소 유효 |

기존 이론 추정 대비 변경점:
- **BVH miss 폴백 제거**가 새로운 1순위로 부상 (이론 단계에서 미식별)
- P4 (shadow LUT)와 P1 (shadow BVH)은 shadow 비용 비중이 실측으로 확인되어 우선순위 유지

---

## 5. 다음 단계

1. **BVH miss 폴백 제거** — `trace.c`에서 BVH 활성화 시 `check_all_objects` 폴백 삭제
2. 폴백 제거 후 동일 씬 재측정 → 순수 BVH 성능 확인
3. Shadow BVH 적용 (P1) 구현 및 A/B 비교
4. 각 최적화 후 결과를 `docs/benchmark-results.md`에 누적

---

## Appendix: Raw Output

### A.1 perf_timing.rt (3 spheres)

```
=== Pixel Timing Statistics ===
Total pixels: 480000
Min time:     0.000 µs (0.000000 ms)
Max time:     83.000 µs (0.083000 ms)
Average:      0.601 µs (0.000601 ms)
Median:       0.000 µs (0.000000 ms)
95th %ile:    1.000 µs (0.001000 ms)
99th %ile:    1.000 µs (0.001000 ms)

=== Render Metrics ===
Frame time:        514.1 ms
FPS:               1.9
Rays traced:       1296000
Intersect tests:   4553136
Tests/ray:         3.5
BVH nodes visited: 1395384
BVH box tests:     1395384
BVH efficiency:    0.0%
Objects:           3
```

### A.2 perf_spheres_20.rt (20 spheres)

```
=== Pixel Timing Statistics ===
Total pixels: 480000
Min time:     0.000 µs (0.000000 ms)
Max time:     56.000 µs (0.056000 ms)
Average:      3.332 µs (0.003332 ms)
Median:       1.000 µs (0.001000 ms)
95th %ile:    2.000 µs (0.002000 ms)
99th %ile:    12.000 µs (0.012000 ms)

=== Render Metrics ===
Frame time:        1835.7 ms
FPS:               0.5
Rays traced:       1296000
Intersect tests:   40416610
Tests/ray:         31.2
BVH nodes visited: 5223492
BVH box tests:     5223492
BVH efficiency:    0.0%
Objects:           20
```

### A.3 perf_spheres_50.rt (50 spheres)

```
=== Pixel Timing Statistics ===
Total pixels: 480000
Min time:     1.000 µs (0.001000 ms)
Max time:     138.000 µs (0.138000 ms)
Average:      6.520 µs (0.006520 ms)
Median:       1.000 µs (0.001000 ms)
95th %ile:    3.000 µs (0.003000 ms)
99th %ile:    24.000 µs (0.024000 ms)

=== Render Metrics ===
Frame time:        3366.0 ms
FPS:               0.3
Rays traced:       1296000
Intersect tests:   105172495
Tests/ray:         81.2
BVH nodes visited: 4465654
BVH box tests:     4465654
BVH efficiency:    0.0%
Objects:           50
```

### A.4 perf_all_objects.rt (7sp + 7cy + 3pl)

```
=== Pixel Timing Statistics ===
Total pixels: 480000
Min time:     5.000 µs (0.005000 ms)
Max time:     270.000 µs (0.270000 ms)
Average:      56.119 µs (0.056119 ms)
Median:       20.000 µs (0.020000 ms)
95th %ile:    22.000 µs (0.022000 ms)
99th %ile:    23.000 µs (0.023000 ms)

=== Render Metrics ===
Frame time:        27173.7 ms
FPS:               0.0
Rays traced:       1296000
Intersect tests:   376326760
Tests/ray:         290.4
BVH nodes visited: 19629738
BVH box tests:     19629738
BVH efficiency:    0.0%
Objects:           18
```
