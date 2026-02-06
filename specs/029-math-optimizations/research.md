# Research: Safe Math Optimizations (Phase A)

**Date**: 2026-02-06
**Branch**: `029-math-optimizations`
**Ref**: `docs/optimization-research-report.md` (P3, P7, P8 섹션)

---

## 1. Background

Phase A는 수학적 동치 변환만 포함하며, 원본 연구 보고서(`docs/optimization-research-report.md`)의 P3, P7, P8 항목에 해당한다. 본 문서는 구현 결정에 필요한 추가 분석을 기록한다.

---

## 2. P3: pow(x, 32) → 반복 제곱

### 2.1 수학적 기반

```
x^32 = ((((x^2)^2)^2)^2)^2
```

5회 곱셈으로 x^32 계산 가능. `pow()` 함수는 일반 실수 지수를 처리하는 범용 함수로, 정수 지수에 대해 비효율적.

### 2.2 성능 분석

| 방식 | 연산 | 예상 사이클 |
|------|------|------------|
| pow(x, 32.0) | exp/log 기반 | ~125 |
| 반복 제곱 | 5회 곱셈 | ~25 |

**예상 절감**: ~100 cycles/pixel × 1,296,000 pixels = **~130M cycles/frame**

### 2.3 부동소수점 정밀도

`pow()`와 반복 곱셈은 연산 경로가 다르므로 최하위 비트(ULP) 차이 발생 가능. specular 값은 0~1 범위이며, 최종 RGB 변환 후 정수 반올림되므로 ±1 오차 허용.

### 2.4 구현 결정

- **static 헬퍼 함수 분리**: `fast_pow32(double x)`
- **인라인 불가**: 42 Norm은 인라인 함수 미지원
- **파일 위치**: `src/lighting/lighting.c` (현재 4/5 함수: clamp_color, calculate_specular, calc_lighting_factor, apply_lighting → fast_pow32 추가 시 5/5)

---

## 3. P8: Sphere sqrt 캐싱

### 3.1 현재 구현 분석

```c
// intersect_sphere_new() in intersect_object.c (line 42-47)
d = b * b - 4 * a * c;
if (d < 0)
    return (0);
d = (-b - sqrt(d)) / (2.0 * a);          // sqrt #1, d 덮어쓰기
if (d < 0.001)
    d = (-b + sqrt(b * b - 4 * a * c)) / (2.0 * a);  // sqrt #2 + discriminant 재계산
```

두 번째 분기에서:
1. `b * b - 4 * a * c` 다시 계산 (불필요)
2. `sqrt()` 다시 호출 (불필요)

**핵심 문제**: line 45에서 `d`를 t1 값으로 덮어쓰므로, line 47에서 discriminant를 다시 계산해야 함.

### 3.2 변수 제약 분석

현재 변수 5개: `oc`, `a`, `b`, `c`, `d`

**변수 사용 범위 분석**:
- `oc`: line 38-41 (a, b, c 계산) → 이후 미사용
- `a`, `b`: line 45, 47 (t 계산) → 계속 사용
- `c`: line 41 (discriminant 계산), line 47 (discriminant 재계산) → **line 42 이후 재활용 가능**
- `d`: line 42 (discriminant), line 45+ (t 값) → 용도 전환

**해결**: `c`를 sqrt(discriminant) 저장에 재활용
- line 41: c = ... (discriminant 계산에 사용)
- line 42: d = b * b - 4 * a * c (c 마지막 사용)
- line 44+: c = sqrt(d) (c 용도 전환: sqrt 결과 저장)

### 3.3 구현 결정

```c
d = b * b - 4 * a * c;
if (d < 0)
    return (0);
c = sqrt(d);                          // c 재활용: sqrt(discriminant) 저장
d = (-b - c) / (2.0 * a);             // d = t1
if (d < 0.001)
    d = (-b + c) / (2.0 * a);         // d = t2, c(sqrt) 재사용
if (d < 0.001 || d > hit->distance)
    return (0);
// 이하 d를 t 값으로 사용 (기존과 동일)
```

변수: oc, a, b, c, d — 5개 유지 ✓

### 3.4 성능 분석

| 항목 | Before | After |
|------|--------|-------|
| sqrt 호출 | 최대 2회 | 1회 |
| discriminant 계산 | 최대 2회 | 1회 |

Shadow ray 16 samples × N spheres → 최대 16N sqrt 절감/pixel

---

## 4. P7: Shadow magnitude/normalize 통합

### 4.1 현재 구현 분석

```c
// is_in_shadow() in shadow_test.c
to_light = vec3_subtract(light_pos, point);
shadow_hit.distance = vec3_magnitude(to_light);  // sqrt #1
light_dir = vec3_normalize(to_light);            // sqrt #2 (내부)
```

`vec3_magnitude()` 구현:
```c
double vec3_magnitude(t_vec3 v)
{
    return (sqrt(v.x * v.x + v.y * v.y + v.z * v.z));
}
```

`vec3_normalize()` 구현:
```c
t_vec3 vec3_normalize(t_vec3 v)
{
    double mag = vec3_magnitude(v);  // sqrt 내부 호출
    return (vec3_multiply(v, 1.0 / mag));
}
```

동일 벡터에 대해 sqrt 2회 호출.

### 4.2 변수 제약 분석

현재 변수 4개: `shadow_ray`, `shadow_hit`, `to_light`, `light_dir`

1개 추가 가능 → `mag` 변수 사용 가능

### 4.3 구현 결정

```c
double mag;

to_light = vec3_subtract(light_pos, point);
mag = vec3_magnitude(to_light);
shadow_hit.distance = mag;
light_dir = vec3_multiply(to_light, 1.0 / mag);  // normalize 인라인
```

**Edge case**: `mag == 0` (광원과 hit point 일치)
- 물리적으로 불가능: 광원은 geometry가 아니므로 ray가 광원 위치에서 hit 불가
- 검사 불필요

### 4.4 성능 분석

| 항목 | Before | After |
|------|--------|-------|
| sqrt 호출 | 2회/sample | 1회/sample |

16 samples × 50 cycles = **800 cycles/pixel 절감**

---

## 5. 부동소수점 오차 분석

### 5.1 오차 발생 원인

| 항목 | 원인 |
|------|------|
| P3 | pow() vs 반복 곱셈의 내부 반올림 경로 차이 |
| P8 | sqrt 호출 순서 변경으로 중간값 변화 |
| P7 | 1/mag 계산 순서 변경 |

### 5.2 영향 범위

- Specular: [0, 1] → RGB 기여 ~0-128 범위
- Sphere t: 교차점 위치 → normal/point 계산 영향
- Shadow direction: 정규화 벡터 → 미세 방향 차이

### 5.3 허용 오차

RGB 컴포넌트 ±1 이내 차이는 인간 시각으로 감지 불가. 허용 범위로 설정.

---

## 6. References

- `docs/optimization-research-report.md` — P3, P7, P8 이론 분석
- `docs/optimization-plan.md` — Phase A 실행 계획
- `docs/benchmark-baseline.md` — Baseline 측정 데이터
- `docs/benchmark-results.md` — Round 1 (P0) 결과
