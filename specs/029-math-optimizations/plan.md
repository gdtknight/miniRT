# Implementation Plan: Safe Math Optimizations (Phase A)

**Branch**: `029-math-optimizations` | **Date**: 2026-02-06 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `/specs/029-math-optimizations/spec.md`

---

## Summary

수학적 동치 변환 3건(P3, P8, P7)을 적용하여 hot path의 CPU 사이클을 절감한다. 모든 변경은 부동소수점 반올림 차이만 발생하며 렌더링 결과는 RGB ±1 범위 내에서 동일하다.

---

## Technical Context

**Language/Version**: C (42 Norm v4.1 compliant)
**Primary Dependencies**: libft, MiniLibX, math library (-lm)
**Storage**: N/A (씬 파일 읽기 전용)
**Testing**: 수동 시각 검증 + metrics 비교
**Target Platform**: macOS (42 환경)
**Project Type**: Single project (raytracer)
**Performance Goals**: 프레임 타임 감소 (baseline 대비)
**Constraints**: 42 Norm (25줄/함수, 5변수, 4인자, 5함수/파일)
**Scale/Scope**: 3개 파일, 각 ~5줄 변경

---

## Constitution Check

| 제약 | 상태 | 비고 |
|------|------|------|
| 함수 25줄 | ✓ PASS | P3 헬퍼 분리 시 12줄 |
| 변수 5개 | ✓ PASS | P8은 변수 재활용, P7은 1개 추가 |
| 인자 4개 | ✓ PASS | 함수 시그니처 변경 없음 |
| 파일 5함수 | ✓ PASS | lighting.c 4→5 (검증됨), 나머지 변경 없음 |

---

## Project Structure

### Documentation (this feature)

```text
specs/029-math-optimizations/
├── spec.md              # 기능 명세
├── plan.md              # 본 문서
├── research.md          # 기술 분석 (optimization-research-report 참조)
└── tasks.md             # 작업 목록 (/speckit.tasks 생성)
```

### Source Code (수정 대상)

```text
src/
├── lighting/
│   ├── lighting.c       # P3: pow32 → 반복 제곱
│   └── shadow_test.c    # P7: magnitude/normalize 통합
└── ray/
    └── intersect_object.c  # P8: sqrt 캐싱
```

**Structure Decision**: 기존 파일 내 인라인 수정. 신규 파일 불필요.

---

## Implementation Details

### P3: Specular pow32 → 반복 제곱

**위치**: `src/lighting/lighting.c`

**현재** (line 60):
```c
spec = pow(spec, 32.0);
```

**변경**: static 헬퍼 함수 추가 + 호출 대체
```c
static double	fast_pow32(double x)
{
    x = x * x;
    x = x * x;
    x = x * x;
    x = x * x;
    x = x * x;
    return (x);
}
// ...
spec = fast_pow32(spec);
```

**Norm 검증**:
- fast_pow32: 8줄 (25줄 이하 ✓)
- lighting.c 함수 수: 4 → 5 (검증됨: clamp_color, calculate_specular, calc_lighting_factor, apply_lighting + fast_pow32)

---

### P8: Sphere sqrt 캐싱

**위치**: `src/ray/intersect_object.c` (intersect_sphere_new 함수)

**현재** (line 42-47):
```c
d = b * b - 4 * a * c;
if (d < 0)
    return (0);
d = (-b - sqrt(d)) / (2.0 * a);
if (d < 0.001)
    d = (-b + sqrt(b * b - 4 * a * c)) / (2.0 * a);  // sqrt + discriminant 재계산
```

**문제점**:
- Line 45: `sqrt(d)` 1회
- Line 47: `sqrt(b * b - 4 * a * c)` 재계산 + sqrt 2회째
- `c`는 line 42 이후 line 47에서만 재사용 (discriminant 재계산용)

**변경**: `c` 변수를 sqrt(discriminant) 저장에 재활용
```c
d = b * b - 4 * a * c;
if (d < 0)
    return (0);
c = sqrt(d);                      // c 재활용: sqrt(discriminant) 저장
d = (-b - c) / (2.0 * a);         // d = t1
if (d < 0.001)
    d = (-b + c) / (2.0 * a);     // d = t2, c 재사용
```

**Norm 검증**:
- 변수: oc, a, b, c, d — 5개 유지 ✓
- `c`: discriminant 계산(line 41) → sqrt 결과 저장(line 44 이후) 용도 전환
- `d`: discriminant → t 값 용도 전환

---

### P7: Shadow magnitude/normalize 통합

**위치**: `src/lighting/shadow_test.c` (is_in_shadow 함수)

**현재** (line 67-68):
```c
shadow_hit.distance = vec3_magnitude(to_light);
light_dir = vec3_normalize(to_light);
```

**변경**: magnitude 1회 계산, normalize에 재사용
```c
double	mag;
// ...
mag = vec3_magnitude(to_light);
shadow_hit.distance = mag;
light_dir = vec3_multiply(to_light, 1.0 / mag);
```

**Norm 검증**:
- 변수: shadow_ray, shadow_hit, to_light, light_dir + mag = 5개 ✓

**Edge case**: `mag == 0` 처리
- 광원이 hit point와 정확히 일치하는 경우는 물리적으로 불가능 (광원은 geometry 아님)
- 추가 검사 불필요

---

## Execution Order

```text
P3 → P8 → P7 → 검증 → 벤치마크
```

각 항목은 독립적이므로 순서 무관하나, 위 순서는 구현 복잡도 순.

---

## Validation Plan

### 기능 검증

1. **빌드**: `make re` 경고 0
2. **Norm**: `norminette src/lighting/lighting.c src/ray/intersect_object.c src/lighting/shadow_test.c` 오류 0
3. **시각 검증**: S1–S4 씬 렌더 후 기존 결과와 비교 (동일)

### 성능 검증

1. S1–S4 씬 5회 측정, 중앙값 기록
2. `docs/benchmark-results.md` Round 2 작성
3. 개선율 = `(baseline - optimized) / baseline × 100%`

---

## Risks and Mitigations

| 위험 | 확률 | 영향 | 대응 |
|------|------|------|------|
| RGB 오차 ±1 초과 | Low | 시각적 차이 | 롤백 또는 허용 범위 재검토 |
| 성능 저하 | Low | 목표 미달성 | 컴파일러 최적화 확인, 필요 시 롤백 |
| Norm 위반 | Medium | 빌드 불가 | 변수 재활용, 헬퍼 분리 |

---

## Complexity Tracking

> Constitution Check 위반 없음 — 해당 섹션 불필요
