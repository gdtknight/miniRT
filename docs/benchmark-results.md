# miniRT Benchmark Results

측정 기준: `docs/optimization-metrics-collection-plan.md`
Baseline: `docs/benchmark-baseline.md`
개선율 = `(baseline - optimized) / baseline × 100%`

---

## Round 1: P0 — BVH miss 폴백 제거

브랜치: `028-bvh-fallback-removal`
변경 요약: `trace_ray()`에서 BVH 유효 시 `check_all_objects()` 폴백 삭제. BVH enabled + root null 엣지 케이스에서는 brute-force 경로 유지.

> **측정 조건**: 1회 측정 (5회 중앙값 미적용). Baseline은 metrics 분리 이전(primary+shadow 합산)이므로 일부 지표는 직접 비교 불가.

### Timing

| 씬 | Baseline (ms) | Optimized (ms) | 개선율 (%) |
|----|--------------|----------------|-----------|
| S1 | — | 465.4 | — |
| S2 | 1,835.7 | 1,142.4 | 37.8% |
| S3 | 3,366.0 | 1,775.8 | 47.2% |
| S4 | 27,173.7 | 26,784.4 | 1.4% |

### Primary path

| 씬 | Baseline tests/ray | Optimized tests/ray | 비고 |
|----|-------------------|--------------------|------|
| S1 | — | 0.1 | — |
| S2 | 31.2 (합산) | 0.3 | Baseline은 primary+shadow 합산 |
| S3 | 81.2 (합산) | 0.3 | Baseline은 primary+shadow 합산 |
| S4 | 290.4 (합산) | 5.2 | Baseline은 primary+shadow 합산 |

### Shadow path

| 씬 | Shadow tests | Shadow tests/pixel | 비고 |
|----|-------------|-------------------|------|
| S1 | 864,976 | 1.8 | 신규 측정 (baseline 분리 없음) |
| S2 | 15,364,205 | 32.0 | 신규 측정 |
| S3 | 43,150,396 | 89.9 | 신규 측정 |
| S4 | 369,636,978 | 770.1 | 신규 측정 |

### Combined intersect tests

| 씬 | Baseline (합산) | Optimized (합산) | 개선율 (%) |
|----|----------------|-----------------|-----------|
| S1 | — | 968,017 | — |
| S2 | 40,416,610 | 15,718,110 | 61.1% |
| S3 | 105,172,495 | 43,508,295 | 58.6% |
| S4 | 376,326,760 | 376,326,760 | 0.0% |

### BVH

| 씬 | Baseline skip rate (%) | Optimized skip rate (%) | 비고 |
|----|----------------------|--------------------------|------|
| S1 | — | 92.0% | Baseline 미측정 |
| S2 | 0.0% (미작동) | 58.8% | 계측 복원 |
| S3 | 0.0% (미작동) | 60.3% | 계측 복원 |
| S4 | 0.0% (미작동) | 32.9% | 계측 복원 |

### Pixel timing — Avg (µs)

| 씬 | Baseline | Optimized | 개선율 (%) |
|----|----------|-----------|-----------|
| S1 | — | 0.5 | — |
| S2 | 3.3 | 1.9 | 42.4% |
| S3 | 6.5 | 3.3 | 49.2% |
| S4 | 56.1 | 55.3 | 1.4% |

### Pixel timing — p99 (µs)

| 씬 | Baseline | Optimized | 개선율 (%) |
|----|----------|-----------|-----------|
| S1 | — | 1.0 | — |
| S2 | 12.0 | 11.0 | 8.3% |
| S3 | 24.0 | 23.0 | 4.2% |
| S4 | 23.0 | 24.0 | -4.3% |

### Raw Output

#### S1 (valid_smoke_simple.rt — 1 object)

```
=== Pixel Timing Statistics ===
Total pixels: 480000
Min time:     0.000 µs (0.000000 ms)
Max time:     30.000 µs (0.030000 ms)
Average:      0.541 µs (0.000541 ms)
Median:       0.000 µs (0.000000 ms)
95th %ile:    1.000 µs (0.001000 ms)
99th %ile:    1.000 µs (0.001000 ms)

=== Render Metrics ===
Frame time:        465.4 ms
FPS:               2.1486
Rays traced:       1296000
Primary tests:     103041
Shadow tests:      864976
Primary tests/ray: 0.1
BVH nodes visited: 1296000
BVH skip rate:     92.0%
Objects:           1
```

#### S2 (perf_spheres_20.rt — 20 spheres)

```
=== Pixel Timing Statistics ===
Total pixels: 480000
Min time:     0.000 µs (0.000000 ms)
Max time:     43.000 µs (0.043000 ms)
Average:      1.934 µs (0.001934 ms)
Median:       0.000 µs (0.000000 ms)
95th %ile:    1.000 µs (0.001000 ms)
99th %ile:    11.000 µs (0.011000 ms)

=== Render Metrics ===
Frame time:        1142.4 ms
FPS:               0.8753
Rays traced:       1296000
Primary tests:     353905
Shadow tests:      15364205
Primary tests/ray: 0.3
BVH nodes visited: 5223492
BVH skip rate:     58.8%
Objects:           20
```

#### S3 (perf_spheres_50.rt — 50 spheres)

```
=== Pixel Timing Statistics ===
Total pixels: 480000
Min time:     0.000 µs (0.000000 ms)
Max time:     56.000 µs (0.056000 ms)
Average:      3.253 µs (0.003253 ms)
Median:       0.000 µs (0.000000 ms)
95th %ile:    1.000 µs (0.001000 ms)
99th %ile:    23.000 µs (0.023000 ms)

=== Render Metrics ===
Frame time:        1775.8 ms
FPS:               0.5631
Rays traced:       1296000
Primary tests:     357899
Shadow tests:      43150396
Primary tests/ray: 0.3
BVH nodes visited: 4465654
BVH skip rate:     60.3%
Objects:           50
```

#### S4 (perf_all_objects.rt — 7sp + 7cy + 3pl)

```
=== Pixel Timing Statistics ===
Total pixels: 480000
Min time:     5.000 µs (0.005000 ms)
Max time:     377.000 µs (0.377000 ms)
Average:      55.314 µs (0.055314 ms)
Median:       19.000 µs (0.019000 ms)
95th %ile:    21.000 µs (0.021000 ms)
99th %ile:    24.000 µs (0.024000 ms)

=== Render Metrics ===
Frame time:        26784.4 ms
FPS:               0.0373
Rays traced:       1296000
Primary tests:     6689782
Shadow tests:      369636978
Primary tests/ray: 5.2
BVH nodes visited: 19629738
BVH skip rate:     32.9%
Objects:           18
```

### 분석

1. **S2, S3 대폭 개선**: 프레임 타임 37–47% 감소, 총 교차 테스트 58–61% 감소. BVH miss ray에서 `check_all_objects()` 이중 탐색이 제거된 직접적 효과.

2. **S4 변화 미미**: 총 교차 테스트 변화 0.0%, 프레임 타임 -1.4%. Shadow 테스트가 전체의 98.2% (369M/376M)를 차지하여 primary 폴백 제거 효과가 매몰됨. S4의 실질적 병목은 shadow brute-force (P1 대상).

3. **BVH skip rate 계측 복원**: Baseline에서 0.0% (미작동)이던 skip rate가 32–92%로 정상 측정됨. 폴백이 BVH 외부에서 `intersect_tests`를 누적하여 skip rate 해석을 왜곡하던 문제가 해소.

4. **Primary tests/ray 극적 감소**: S2 31.2 → 0.3, S3 81.2 → 0.3. Baseline은 합산 값이므로 직접 비교는 불가하나, BVH가 대부분의 오브젝트를 AABB 레벨에서 걸러내고 있음을 확인.

5. **Shadow 경로 지배 확인**: S4에서 shadow_tests/pixel = 770.1 (= 18 objects × ~42.8 samples). Shadow brute-force (P1)가 다음 최적화 우선순위임을 실측으로 재확인.

---

## Round 2: Phase A — Safe Math Optimizations (P3+P8+P7)

브랜치: `029-math-optimizations`
변경 요약:
- P3: `pow(spec, 32.0)` → `fast_pow32()` 반복 제곱 5회 (`src/lighting/lighting.c`)
- P8: Sphere sqrt 캐싱 — `c = sqrt(d)` 저장 후 재사용 (`src/ray/intersect_object.c`)
- P7: Shadow magnitude/normalize 통합 — `mag` 변수로 sqrt 1회만 호출 (`src/lighting/shadow_test.c`)

> **측정 조건**: 1회 측정. Round 1 결과를 baseline으로 사용.

### Timing

| 씬 | Round 1 (ms) | Phase A (ms) | 개선율 (%) |
|----|-------------|--------------|-----------|
| S1 | 465.4 | 466.2 | -0.2% |
| S2 | 1,142.4 | 1,142.8 | -0.0% |
| S3 | 1,775.8 | 1,767.9 | **0.4%** |
| S4 | 26,784.4 | 26,178.2 | **2.3%** |

### Pixel timing — Avg (µs)

| 씬 | Round 1 | Phase A | 개선율 (%) |
|----|---------|---------|-----------|
| S1 | 0.541 | 0.540 | 0.2% |
| S2 | 1.934 | 1.933 | 0.1% |
| S3 | 3.253 | 3.238 | 0.5% |
| S4 | 55.314 | 54.054 | **2.3%** |

### Raw Output

#### S1 (valid_smoke_simple.rt — 1 object)

```
=== Pixel Timing Statistics ===
Total pixels: 480000
Min time:     0.000 µs (0.000000 ms)
Max time:     32.000 µs (0.032000 ms)
Average:      0.540 µs (0.000540 ms)
Median:       0.000 µs (0.000000 ms)
95th %ile:    1.000 µs (0.001000 ms)
99th %ile:    1.000 µs (0.001000 ms)

=== Render Metrics ===
Frame time:        466.2 ms
FPS:               2.1451
Rays traced:       1296000
Primary tests:     103041
Shadow tests:      864976
Primary tests/ray: 0.1
BVH nodes visited: 1296000
BVH skip rate:     92.0%
Objects:           1
```

#### S2 (perf_spheres_20.rt — 20 spheres)

```
=== Pixel Timing Statistics ===
Total pixels: 480000
Min time:     0.000 µs (0.000000 ms)
Max time:     49.000 µs (0.049000 ms)
Average:      1.933 µs (0.001933 ms)
Median:       0.000 µs (0.000000 ms)
95th %ile:    1.000 µs (0.001000 ms)
99th %ile:    11.000 µs (0.011000 ms)

=== Render Metrics ===
Frame time:        1142.8 ms
FPS:               0.8750
Rays traced:       1296000
Primary tests:     353905
Shadow tests:      15364205
Primary tests/ray: 0.3
BVH nodes visited: 5223492
BVH skip rate:     58.8%
Objects:           20
```

#### S3 (perf_spheres_50.rt — 50 spheres)

```
=== Pixel Timing Statistics ===
Total pixels: 480000
Min time:     0.000 µs (0.000000 ms)
Max time:     59.000 µs (0.059000 ms)
Average:      3.238 µs (0.003238 ms)
Median:       0.000 µs (0.000000 ms)
95th %ile:    1.000 µs (0.001000 ms)
99th %ile:    22.000 µs (0.022000 ms)

=== Render Metrics ===
Frame time:        1767.9 ms
FPS:               0.5657
Rays traced:       1296000
Primary tests:     357899
Shadow tests:      43150396
Primary tests/ray: 0.3
BVH nodes visited: 4465654
BVH skip rate:     60.3%
Objects:           50
```

#### S4 (perf_all_objects.rt — 7sp + 7cy + 3pl)

```
=== Pixel Timing Statistics ===
Total pixels: 480000
Min time:     5.000 µs (0.005000 ms)
Max time:     107.000 µs (0.107000 ms)
Average:      54.054 µs (0.054054 ms)
Median:       19.000 µs (0.019000 ms)
95th %ile:    21.000 µs (0.021000 ms)
99th %ile:    22.000 µs (0.022000 ms)

=== Render Metrics ===
Frame time:        26178.2 ms
FPS:               0.0382
Rays traced:       1296000
Primary tests:     6689782
Shadow tests:      369636978
Primary tests/ray: 5.2
BVH nodes visited: 19629738
BVH skip rate:     32.9%
Objects:           18
```

### 분석

1. **S1, S2 변화 없음**: 단순 씬에서 수학 최적화 효과가 측정 노이즈 범위 내. 예상대로 미미한 영향.

2. **S3 소폭 개선 (0.4%)**: 50개 sphere에서 P8(sqrt 캐싱) 효과가 누적되어 측정 가능한 개선 발생.

3. **S4 유의미한 개선 (2.3%)**: 가장 복잡한 씬에서 세 가지 최적화의 누적 효과 확인.
   - P7(shadow magnitude/normalize): 369M shadow tests × sqrt 1회 절감
   - P8(sphere sqrt): 7개 sphere × 다수 ray에서 sqrt 캐싱 효과
   - P3(pow32): specular 계산 경량화

4. **이론 추정 대비 실측**: 연구 보고서의 이론 추정(P3 ~1%, P7 ~8%, P8 ~소량)보다 실측 효과가 작음. Shadow brute-force(P1)가 전체 비용의 98%+를 차지하여 수학 최적화 효과가 상대적으로 희석됨.

5. **다음 단계**: Phase B(P4 Shadow LUT, P1 Shadow BVH)가 S4 병목 해소에 핵심. 현재 shadow_tests가 369M으로 압도적이므로, shadow 경로 최적화가 가장 큰 개선 잠재력 보유.

---

## Round 3: Phase B-1 — P4 Shadow Offset LUT

브랜치: `030-shadow-offset-lut`
변경 요약:
- Shadow offset LUT precompute: `init_shadow_offset_lut()`에서 cos/sin/sqrt 1회 계산 후 저장
- `sample_shadow_ray()`에서 LUT 참조로 삼각함수 호출 제거
- Magnitude guard: `is_in_shadow()`에서 mag < 0.0001 시 early return (NaN/INF 방지)
- LUT cleanup: `scene_destroy()`에서 메모리 해제

> **측정 조건**: 1회 측정. Round 2 결과를 baseline으로 사용.

### Timing

| 씬 | Round 2 (ms) | Round 3 (ms) | 개선율 (%) |
|----|-------------|--------------|-----------|
| S1 | 466.2 | 438.3 | **6.0%** |
| S2 | 1,142.8 | 1,122.9 | **1.7%** |
| S3 | 1,767.9 | 1,753.4 | **0.8%** |
| S4 | 26,178.2 | 25,583.8 | **2.3%** |

### Pixel timing — Avg (µs)

| 씬 | Round 2 | Round 3 | 개선율 (%) |
|----|---------|---------|-----------|
| S1 | 0.540 | 0.477 | **11.7%** |
| S2 | 1.933 | 1.880 | **2.7%** |
| S3 | 3.238 | 3.195 | **1.3%** |
| S4 | 54.054 | 52.800 | **2.3%** |

### Raw Output

#### S1 (valid_smoke_simple.rt — 1 object)

```
=== Pixel Timing Statistics ===
Total pixels: 480000
Min time:     0.000 µs (0.000000 ms)
Max time:     21.000 µs (0.021000 ms)
Average:      0.477 µs (0.000477 ms)
Median:       0.000 µs (0.000000 ms)
95th %ile:    1.000 µs (0.001000 ms)
99th %ile:    1.000 µs (0.001000 ms)

=== Render Metrics ===
Frame time:        438.3 ms
FPS:               2.2815
Rays traced:       1296000
Primary tests:     103041
Shadow tests:      864976
Primary tests/ray: 0.1
BVH nodes visited: 1296000
BVH skip rate:     92.0%
Objects:           1
```

#### S2 (perf_spheres_20.rt — 20 spheres)

```
=== Pixel Timing Statistics ===
Total pixels: 480000
Min time:     0.000 µs (0.000000 ms)
Max time:     57.000 µs (0.057000 ms)
Average:      1.880 µs (0.001880 ms)
Median:       0.000 µs (0.000000 ms)
95th %ile:    1.000 µs (0.001000 ms)
99th %ile:    11.000 µs (0.011000 ms)

=== Render Metrics ===
Frame time:        1122.9 ms
FPS:               0.8906
Rays traced:       1296000
Primary tests:     353905
Shadow tests:      15364205
Primary tests/ray: 0.3
BVH nodes visited: 5223492
BVH skip rate:     58.8%
Objects:           20
```

#### S3 (perf_spheres_50.rt — 50 spheres)

```
=== Pixel Timing Statistics ===
Total pixels: 480000
Min time:     0.000 µs (0.000000 ms)
Max time:     95.000 µs (0.095000 ms)
Average:      3.195 µs (0.003195 ms)
Median:       0.000 µs (0.000000 ms)
95th %ile:    1.000 µs (0.001000 ms)
99th %ile:    22.000 µs (0.022000 ms)

=== Render Metrics ===
Frame time:        1753.4 ms
FPS:               0.5703
Rays traced:       1296000
Primary tests:     357899
Shadow tests:      43150396
Primary tests/ray: 0.3
BVH nodes visited: 4465654
BVH skip rate:     60.3%
Objects:           50
```

#### S4 (perf_all_objects.rt — 7sp + 7cy + 3pl)

```
=== Pixel Timing Statistics ===
Total pixels: 480000
Min time:     4.000 µs (0.004000 ms)
Max time:     1010.000 µs (1.010000 ms)
Average:      52.800 µs (0.052800 ms)
Median:       19.000 µs (0.019000 ms)
95th %ile:    20.000 µs (0.020000 ms)
99th %ile:    21.000 µs (0.021000 ms)

=== Render Metrics ===
Frame time:        25583.8 ms
FPS:               0.0391
Rays traced:       1296000
Primary tests:     6689782
Shadow tests:      369636978
Primary tests/ray: 5.2
BVH nodes visited: 19629738
BVH skip rate:     32.9%
Objects:           18
```

### 분석

1. **S1 6.0% 개선**: 가장 단순한 씬에서 오히려 가장 큰 개선. Shadow sample 수 대비 오버헤드 비율이 높아 LUT 효과가 두드러짐.

2. **S2–S3 1–2% 개선**: Shadow tests가 많아지면서 intersection 비용이 지배적. LUT 효과가 상대적으로 희석됨.

3. **S4 2.3% 개선**: 369M shadow tests에서 cos/sin 호출 제거로 ~600ms 절감. Round 2와 동일한 2.3% 개선율이지만, 절대값으로는 더 큰 효과.

4. **이론 vs 실측**: 연구 보고서 예상(~99% trig 비용 절감)은 per-sample 기준. 전체 프레임 타임에서 trig 비용이 차지하는 비율이 작아 전체 개선율은 1–6% 수준.

5. **누적 효과**: Baseline 대비 S4 총 개선율 = (27,173.7 - 25,583.8) / 27,173.7 = **5.9%**. P0+Phase A+P4 누적 효과.

6. **다음 단계**: P1 (Shadow ray BVH)이 S4 병목 해소의 핵심. Shadow tests 369M을 BVH로 가속하면 대폭 개선 기대.
