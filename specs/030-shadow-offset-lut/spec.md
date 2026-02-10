# Feature Specification: Shadow Offset LUT (P4)

**Date**: 2026-02-06
**Branch**: `030-shadow-offset-lut`
**Status**: Draft
**Ref**: `docs/optimization-plan.md` Phase B (B-1)

---

## 1. Overview

Soft shadow 계산에서 매 샘플마다 반복되는 삼각함수(cos, sin) 및 sqrt 호출을 사전 계산된 Look-Up Table(LUT)로 대체하여 성능을 개선한다.

### 현재 문제점

`generate_shadow_sample_offset()` (`shadow_calc.c:58-78`)에서 매 샘플마다:
- `sqrt((double)total_samples)` — 동일 값 반복 계산
- `cos(angle)`, `sin(angle)` — 삼각함수 2회/sample

16 samples 기준:
- sqrt: 16회 (모두 동일 결과)
- cos+sin: 32회 × ~125 cycles = 4,000 cycles/pixel

### 예상 효과

- LUT precompute 시: 배열 룩업 = ~10 cycles/pixel
- **예상 절감: ~3,990 cycles/pixel, 프레임당 ~5.2B cycles (40%)**

---

## 2. Requirements

### 2.0 선행 수정: Shadow magnitude 0 나눗셈 방어

**문제**: `is_in_shadow()`에서 `mag = vec3_magnitude(to_light)` 후 `1.0 / mag` 사용. 광원이 hit point와 동일 위치일 경우 mag == 0으로 NaN/INF 발생 가능.

**현재 코드** (`src/lighting/shadow_test.c:67-71`):
```c
mag = vec3_magnitude(to_light);
shadow_hit.distance = mag;
light_dir = vec3_multiply(to_light, 1.0 / mag);
```

**수정**:
```c
mag = vec3_magnitude(to_light);
if (mag < 0.0001)
    return (0);
shadow_hit.distance = mag;
light_dir = vec3_multiply(to_light, 1.0 / mag);
```

**근거**: 광원이 hit point와 동일하면 "그림자 안에 있지 않음"으로 처리 (광원 자체가 표면 위).

### 2.1 LUT 구조체 확장

**현재** (`includes/shadow.h`):
```c
typedef struct s_shadow_config
{
    int     samples;
    double  softness;
    double  bias_scale;
    int     enable_ao;
}   t_shadow_config;
```

**변경**:
```c
typedef struct s_shadow_config
{
    int     samples;
    double  softness;
    double  bias_scale;
    int     enable_ao;
    t_vec3  *offset_lut;    // Precomputed sample offsets
}   t_shadow_config;
```

### 2.2 LUT 생성

- `init_shadow_config()` 또는 별도 함수에서 LUT 초기화
- 샘플 수(samples)만큼 오프셋 벡터 사전 계산
- 기존 `generate_shadow_sample_offset()` 로직을 1회만 실행하여 저장

### 2.3 LUT 사용

- `sample_shadow_ray()`에서 `generate_shadow_sample_offset()` 호출 대신 LUT 참조
- `offset = config->offset_lut[index]` 형태로 변경

### 2.4 LUT 해제

- Scene 정리 시 (`scene_destroy()`) LUT 메모리 해제
- `set_shadow_samples()` LUT 재생성: **제외** (Section 3 Non-Goal — 동적 변경 미지원)

---

## 3. Non-Goals

- 샘플 수 동적 변경 UI 지원 (현재 고정값 사용)
- offset 계산 알고리즘 변경 (기존 circular stratified sampling 유지)
- softness 값이 LUT에 반영되지 않음 (radius는 사용 시점에 곱셈)

---

## 4. Acceptance Criteria

### 4.1 기능 검증

| 항목 | 검증 방법 |
|------|----------|
| 렌더링 동일성 | S1–S4 씬 렌더 후 기존 결과와 시각적 비교 |
| Soft shadow 품질 | S2–S4에서 그림자 경계 품질 동일 |

### 4.2 성능 검증

| 항목 | 기준 |
|------|------|
| 프레임 타임 | Round 2 대비 감소 |
| 개선율 기록 | `docs/benchmark-results.md` Round 3 작성 |

### 4.3 코드 품질

| 항목 | 기준 |
|------|------|
| Norm 준수 | `norminette` 오류 0 |
| 컴파일 | `-Wall -Wextra -Werror` 경고 0 |
| 메모리 누수 | `leaks` 검사 통과 |

---

## 5. Technical Constraints

### 5.1 42 Norm

| 제약 | 영향 | 대응 |
|------|------|------|
| 파일 5함수 | shadow_config.c 현재 4/5 | LUT 초기화 함수 1개 추가 가능 |
| 변수 5개 | LUT 생성 함수 내 | 기존 generate_shadow_sample_offset 로직 재사용 |

### 5.2 메모리

- LUT 크기: `samples × sizeof(t_vec3)` = 16 × 24 = **384 bytes**
- 동적 할당 필요 (samples 변경 가능성 대비)
- malloc 실패 시 graceful degradation (LUT 없이 기존 방식 사용)

---

## 6. File Changes

| 파일 | 변경 내용 |
|------|----------|
| `includes/shadow.h` | `t_vec3 *offset_lut` 필드 추가 |
| `src/lighting/shadow_config.c` | LUT 초기화 함수 추가, `init_shadow_config()` 수정 |
| `src/lighting/shadow_calc.c` | `sample_shadow_ray()`에서 LUT 참조로 변경 |
| `src/lighting/shadow_test.c` | magnitude guard 추가 |
| `src/scene/scene.c` | `scene_destroy()`에서 LUT 메모리 해제 (static helper) |

---

## 7. Measurement Plan

### 7.1 측정 조건

- 시나리오: S1–S4
- 측정 횟수: 1회 (Round 3)
- 개선율: `(Round 2 - Round 3) / Round 2 × 100%`

### 7.2 기록 위치

- `docs/benchmark-results.md` Round 3
- P4 단독 효과 측정

---

## 8. Risks

| 위험 | 영향 | 대응 |
|------|------|------|
| malloc 실패 | LUT 미생성 | NULL 체크 후 기존 방식 폴백 |
| 메모리 누수 | 리소스 낭비 | `scene_destroy()`에서 해제 보장 |

> **Note**: samples 동적 변경은 Non-Goal (Section 3)으로 제외됨.
