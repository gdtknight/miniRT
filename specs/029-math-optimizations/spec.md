# Feature Specification: Safe Math Optimizations (Phase A)

**Date**: 2026-02-06
**Branch**: `029-math-optimizations`
**Status**: Draft
**Ref**: `docs/optimization-plan.md` Phase A

---

## 1. Overview

Phase A는 수학적 동치 변환만 포함하는 안전한 최적화 3건을 구현한다. 모든 항목은 위험도 LOW이며, 부동소수점 반올림 차이만 존재하므로 렌더링 결과는 RGB ±1 범위 내에서 동일하다.

### 대상 항목

| ID | 항목 | 현재 비용 | 예상 절감 |
|----|------|----------|----------|
| **P3** | Specular pow(x, 32) → 반복 제곱 | ~125 cycles/pixel | ~100 cycles/pixel |
| **P8** | Sphere sqrt 이중 호출 → 캐싱 | sqrt 2회/sphere | sqrt 1회/sphere |
| **P7** | Shadow magnitude+normalize → 통합 | sqrt 2회/sample | sqrt 1회/sample |

---

## 2. Requirements

### 2.1 P3: Specular pow32 경량화

**현재 구현** (`src/lighting/lighting.c:60`):
```c
spec = pow(spec, 32.0);
```

**문제점**:
- `pow()`는 범용 부동소수점 거듭제곱 함수로 ~125 cycles
- 지수가 정수 32이므로 반복 제곱으로 대체 가능

**요구사항**:
- `pow(spec, 32.0)`을 5회 반복 제곱으로 대체
- 함수 분리 시 `static` 헬퍼 사용 (Norm 준수)
- 결과값: 수학적 동치 (spec^32)

### 2.2 P8: Sphere sqrt 캐싱

**현재 구현** (`src/ray/intersect_object.c:42-47`):
```c
d = b * b - 4 * a * c;
if (d < 0)
    return (0);
d = (-b - sqrt(d)) / (2.0 * a);
if (d < 0.001)
    d = (-b + sqrt(b * b - 4 * a * c)) / (2.0 * a);  // discriminant + sqrt 재계산
```

**문제점**:
- Line 45: sqrt(d) 1회
- Line 47: discriminant 재계산 + sqrt 2회째

**변수 사용 범위 분석**:
- `c`는 line 41에서 계산, line 42(`d = b*b - 4*a*c`)에서 마지막 사용
- Line 47의 `c` 사용은 discriminant 재계산용 — sqrt 캐싱으로 제거 대상

**요구사항**:
- sqrt 결과를 `c` 변수에 저장 (line 42 이후 용도 전환)
- 두 번째 근 계산 시 저장된 값 재사용
- 변수 5개 제한 유지 (oc, a, b, c, d)
  - 해결: `c` = sqrt(discriminant) 저장, `d` = t 값

### 2.3 P7: Shadow magnitude/normalize 통합

**현재 구현** (`src/lighting/shadow_test.c:67-68`):
```c
shadow_hit.distance = vec3_magnitude(to_light);
light_dir = vec3_normalize(to_light);
```

**문제점**:
- `vec3_magnitude()`: 내부에서 sqrt 1회
- `vec3_normalize()`: 내부에서 magnitude 계산 → sqrt 1회
- 동일 벡터에 대해 sqrt 2회 호출

**요구사항**:
- magnitude 1회 계산, normalize와 distance에 공유
- 인라인 구현 또는 헬퍼 함수 사용 (어느 쪽이든 Norm 준수)
- 변수 5개 제한 유지 (현재 4개: shadow_ray, shadow_hit, to_light, light_dir)
  - 여유 1개 → `mag` 변수 추가 가능

---

## 3. Non-Goals

- 새로운 유틸리티 함수를 별도 파일에 추가하지 않음 (인라인 또는 static 헬퍼)
- 함수 시그니처 변경 없음
- 렌더링 로직 변경 없음 (수학 연산 경로만 변경)

---

## 4. Acceptance Criteria

### 4.1 기능 검증

| 항목 | 검증 방법 |
|------|----------|
| 렌더링 동일성 | S1–S4 씬 렌더 후 기존 결과와 시각적 비교 |
| RGB 허용 오차 | 동일 픽셀 RGB 값 차이 ±1 이내 |

### 4.2 성능 검증

| 항목 | 기준 |
|------|------|
| 프레임 타임 | Baseline 대비 감소 또는 동일 (증가 불가) |
| 개선율 기록 | `docs/benchmark-results.md` Round 2 작성 |

### 4.3 코드 품질

| 항목 | 기준 |
|------|------|
| Norm 준수 | `norminette` 오류 0 |
| 컴파일 | `-Wall -Wextra -Werror` 경고 0 |

---

## 5. Technical Constraints

### 5.1 42 Norm

| 제약 | 영향 | 대응 |
|------|------|------|
| 함수 25줄 | P3 헬퍼 분리 시 적용 | static 함수로 분리 |
| 변수 5개 | P8 변수 재활용 필요 | discriminant → sqrt → t 변환 |
| 파일 5함수 | lighting.c 현재 4/5 (검증됨: clamp_color, calculate_specular, calc_lighting_factor, apply_lighting) | 1개 여유 있음 |

### 5.2 부동소수점

- 반복 제곱은 pow()와 반올림 경로가 다름
- sqrt 재활용 시 중간 값 저장으로 연산 순서 변경
- 예상 오차: RGB 컴포넌트 ±1 (허용 범위)

---

## 6. File Changes

| 파일 | 변경 내용 |
|------|----------|
| `src/lighting/lighting.c` | P3: pow() → 반복 제곱 |
| `src/ray/intersect_object.c` | P8: sqrt 결과 캐싱 |
| `src/lighting/shadow_test.c` | P7: magnitude/normalize 통합 |

총 3개 파일, 각 파일 ~5줄 변경.

---

## 7. Measurement Plan

### 7.1 측정 조건

- 시나리오: S1–S4 (docs/benchmark-baseline.md 참조)
- 측정 횟수: 5회 실행, 중앙값
- 개선율: `(baseline - optimized) / baseline × 100%`

### 7.2 기록 위치

- `docs/benchmark-results.md` Round 2
- Phase A 완료 시점에 P3+P8+P7 누적 효과 측정

---

## 8. Risks

| 위험 | 영향 | 대응 |
|------|------|------|
| 부동소수점 오차 | RGB ±1 초과 시 시각적 차이 | 적용 전후 동일 씬 렌더 비교 |
| 성능 저하 | 컴파일러 최적화 간섭 | 측정 후 개선 미확인 시 롤백 |
| Norm 위반 | 변수/줄 수 초과 | 헬퍼 분리 또는 변수 재활용 |
