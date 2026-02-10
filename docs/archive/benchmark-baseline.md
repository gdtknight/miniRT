# miniRT Baseline Benchmark Report

작성일: 2026-02-05
기준 브랜치: `027-baseline-benchmark` (develop 기반)
수집 계획: `docs/optimization-metrics-collection-plan.md` 준수

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
| 렌더 경로 | full-quality only (low-quality 제외) |
| 측정 횟수 | 5회 중앙값 (권장) / 1회 (현재, 재측정 필요) |

---

## 2. 시나리오

| ID | 씬 | Objects | 타입 |
|----|-----|---------|------|
| S1 | `valid_smoke_simple.rt` | 1 | sp 1 |
| S2 | `perf_spheres_20.rt` | 20 | sp 20 |
| S3 | `perf_spheres_50.rt` | 50 | sp 50 |
| S4 | `perf_all_objects.rt` | 18 | sp 7 + cy 7 + pl 3 |

---

## 3. Baseline 데이터

> **수집 상태**: metrics 분리 이전 코드로 1회 측정. primary/shadow 미분리, skip rate 미측정.
> 재측정 후 이 섹션을 갱신할 것.

### 3.1 Timing

| 씬 | Frame time (ms) | FPS |
|----|----------------|-----|
| S1 | — | — |
| S2 | 1,836 | 0.54 |
| S3 | 3,366 | 0.30 |
| S4 | 27,174 | 0.04 |

### 3.2 Primary path

| 씬 | Rays | Primary tests | Primary tests/ray |
|----|------|---------------|-------------------|
| S1 | — | — | — |
| S2 | 1,296,000 | — | — |
| S3 | 1,296,000 | — | — |
| S4 | 1,296,000 | — | — |

### 3.3 Shadow path

| 씬 | Shadow tests | Shadow tests/pixel |
|----|-------------|-------------------|
| S1 | — | — |
| S2 | — | — |
| S3 | — | — |
| S4 | — | — |

### 3.4 BVH

| 씬 | Nodes visited | Tests skipped | Skip rate (%) |
|----|---------------|---------------|---------------|
| S1 | — | — | — |
| S2 | 5,223,492 | — | — |
| S3 | 4,465,654 | — | — |
| S4 | 19,629,738 | — | — |

### 3.5 Pixel timing (µs)

| 씬 | Min | Avg | Median | p95 | p99 | Max |
|----|-----|-----|--------|-----|-----|-----|
| S1 | — | — | — | — | — | — |
| S2 | 0.0 | 3.3 | 1.0 | 2.0 | 12.0 | 56.0 |
| S3 | 1.0 | 6.5 | 1.0 | 3.0 | 24.0 | 138.0 |
| S4 | 5.0 | 56.1 | 20.0 | 22.0 | 23.0 | 270.0 |

### 3.6 Combined intersect (참고 — primary + shadow 합산, 분리 이전)

| 씬 | Intersect tests (합산) | Tests/ray (합산) |
|----|----------------------|-----------------|
| S2 | 40,416,610 | 31.2 |
| S3 | 105,172,495 | 81.2 |
| S4 | 376,326,760 | 290.4 |

---

## 4. 분석

### 4.1 프레임 타임 스케일링

- sphere-only: 20→50 = 2.5x objects, 1.8x time (선형에 가까움)
- perf_all_objects (18 mixed): perf_spheres_20보다 오브젝트 적지만 14.8x 느림

### 4.2 실린더 교차 비용

S4 합산 Tests/ray 290.4 vs S2 31.2 (9.3x). 원인:
1. 실린더 교차 계산 복합성 (측면+캡)
2. shadow ray 폭증 (그림자 영역 증가)
3. BVH miss 폴백 (`check_all_objects` 이중 카운트)

### 4.3 BVH 이중 탐색 문제

`trace_ray()`에서 BVH miss 시 `check_all_objects()` 폴백 실행 → intersect_test 이중 카운트.
BVH가 전체 오브젝트를 커버하므로 이 폴백은 불필요.

### 4.4 Shadow 경로 지배 (추정)

shadow samples 16 × object_count per hit pixel.
정확한 primary/shadow 비율은 분리 측정 후 확인 필요.

---

## 5. 최적화 우선순위

| 우선순위 | 항목 | 근거 |
|----------|------|------|
| **1** | BVH miss 폴백 제거 | 이중 탐색 제거 → miss ray intersect_test 절반 감소 |
| **2** | Shadow ray BVH 적용 (P1) | shadow 비용 지배적 → BVH 전환 시 최대 효과 |
| **3** | Shadow offset LUT (P4) | 16 samples × sqrt/cos/sin 제거 |
| **4** | Specular pow32 경량화 (P3) | 모든 hit pixel 적용, 낮은 위험 |
| **5** | BVH traversal 개선 (P5, P6) | nodes_visited 절대 수 대 → per-node 비용 감소 유효 |

---

## Appendix A: Raw Output (metrics 분리 이전)

> `Intersect tests` = primary + shadow 합산.
> `BVH box tests` = nodes_visited 동치.
> `BVH efficiency` = 미작동 (0.0%).

### A.1 perf_timing.rt (3 spheres) — 참고용

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

---

## Appendix B: Benchmark Round Template

> 최적화 적용 후 `docs/benchmark-results.md`에 아래 템플릿을 복사하여 기록한다.
> `개선율 = (baseline - optimized) / baseline × 100%`

```markdown
## Round N: <최적화 항목명>

브랜치: `<branch-name>`
변경 요약: <1-2줄 설명>

### Timing

| 씬 | Baseline (ms) | Optimized (ms) | 개선율 (%) |
|----|--------------|----------------|-----------|
| S1 |              |                |           |
| S2 |              |                |           |
| S3 |              |                |           |
| S4 |              |                |           |

### Primary path

| 씬 | Baseline tests/ray | Optimized tests/ray | 개선율 (%) |
|----|-------------------|--------------------|-----------|
| S1 |                   |                    |           |
| S2 |                   |                    |           |
| S3 |                   |                    |           |
| S4 |                   |                    |           |

### Shadow path

| 씬 | Baseline tests/pixel | Optimized tests/pixel | 개선율 (%) |
|----|---------------------|----------------------|-----------|
| S1 |                     |                      |           |
| S2 |                     |                      |           |
| S3 |                     |                      |           |
| S4 |                     |                      |           |

### BVH

| 씬 | Baseline skip rate (%) | Optimized skip rate (%) | 변화 (pp) |
|----|----------------------|--------------------------|----------|
| S1 |                      |                          |          |
| S2 |                      |                          |          |
| S3 |                      |                          |          |
| S4 |                      |                          |          |

### Pixel timing — Avg (µs)

| 씬 | Baseline | Optimized | 개선율 (%) |
|----|----------|-----------|-----------|
| S1 |          |           |           |
| S2 |          |           |           |
| S3 |          |           |           |
| S4 |          |           |           |

### Pixel timing — p99 (µs)

| 씬 | Baseline | Optimized | 개선율 (%) |
|----|----------|-----------|-----------|
| S1 |          |           |           |
| S2 |          |           |           |
| S3 |          |           |           |
| S4 |          |           |           |

### Raw Output

<씬별 stdout 출력 붙여넣기>

### 분석

<변경 사항이 각 지표에 미친 영향 정리>
```
