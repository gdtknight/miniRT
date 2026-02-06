# Research: Shadow Offset LUT (P4)

**Date**: 2026-02-06
**Branch**: `030-shadow-offset-lut`
**Ref**: `docs/optimization-research-report.md` (P4 섹션)

---

## 1. Background

Soft shadow는 광원을 여러 샘플 위치로 jittering하여 부드러운 그림자 경계를 생성한다. 현재 구현에서는 매 샘플마다 삼각함수를 계산하고 있어 성능 병목이 발생한다.

---

## 2. 현재 구현 분석

### 2.1 generate_shadow_sample_offset() 함수

**위치**: `src/lighting/shadow_calc.c:58-78`

```c
t_vec3  generate_shadow_sample_offset(double radius, int sample_index,
        int total_samples)
{
    t_vec3  offset;
    double  angle;
    double  r;
    int     grid_size;

    if (total_samples <= 1)
        return ((t_vec3){0.0, 0.0, 0.0});
    grid_size = (int)sqrt((double)total_samples);  // sqrt 매번 호출
    if (grid_size < 1)
        grid_size = 1;
    angle = 2.0 * M_PI * (sample_index % grid_size) / (double)grid_size;
    r = radius * (sample_index / (double)grid_size + 0.5) / (double)grid_size;
    offset.x = r * cos(angle);  // cos 매번 호출
    offset.y = r * sin(angle);  // sin 매번 호출
    offset.z = 0.0;
    return (offset);
}
```

### 2.2 호출 빈도

- 픽셀당: 16 samples (기본값)
- 프레임당: 1,296,000 pixels × 16 = **20,736,000 호출**

### 2.3 비용 분석

| 연산 | 호출 횟수/pixel | 사이클/call | 총 사이클/pixel |
|------|----------------|------------|----------------|
| sqrt | 16 | 50-100 | 800-1,600 |
| cos | 16 | 100-150 | 1,600-2,400 |
| sin | 16 | 100-150 | 1,600-2,400 |
| **합계** | | | **4,000-6,400** |

---

## 3. LUT 최적화 분석

### 3.1 핵심 관찰

1. `sqrt(total_samples)`: 동일 값 반복 — 16회 중 16회 동일
2. `cos(angle)`, `sin(angle)`: sample_index만 변화, total_samples 고정 시 동일

### 3.2 LUT 설계

오프셋 패턴은 **정규화된 단위 원 패턴**으로 저장:
- `offset.x = (r / radius) * cos(angle)`
- `offset.y = (r / radius) * sin(angle)`
- 사용 시 `radius`를 곱함

이렇게 하면 `softness` 변경에도 LUT 재생성 불필요.

### 3.3 메모리 사용량

```
samples × sizeof(t_vec3) = 16 × 24 = 384 bytes
```

최대 samples = 64 가정 시에도 1,536 bytes — 무시 가능.

### 3.4 예상 성능

| 항목 | Before | After |
|------|--------|-------|
| 연산 | sqrt + cos + sin | 배열 인덱싱 + vec3_multiply |
| 사이클/pixel | ~4,000-6,400 | ~10-50 |
| **절감율** | | **~99%** |

---

## 4. 구현 고려사항

### 4.1 LUT 생성 시점

**옵션 1**: `init_shadow_config()` 내부
- 장점: 단순함, 별도 호출 불필요
- 단점: config 초기화 시 malloc 발생

**옵션 2**: 첫 렌더 시 lazy init
- 장점: 필요할 때만 할당
- 단점: 첫 프레임 약간 느림, 추가 체크 필요

**결정**: 옵션 1 채택 — 단순성 우선, 384 bytes는 즉시 할당해도 무방.

### 4.2 LUT 해제 시점

**Scene cleanup 경로 확인 필요**:
- `free_scene()` 또는 유사 함수에서 `shadow_config.offset_lut` 해제
- 프로그램 종료 전 호출 보장 필요

### 4.3 Fallback 전략

malloc 실패 시:
```c
if (!config->offset_lut)
    offset = generate_shadow_sample_offset(...);  // 기존 방식
else
    offset = vec3_multiply(config->offset_lut[i], radius);
```

### 4.4 samples 동적 변경

**Non-Goal로 제외**: spec Section 3에서 "샘플 수 동적 변경 UI 지원"이 명시적 Non-Goal.
현재 samples는 초기화 시 고정 (16)이며 런타임 변경 없음.

향후 동적 변경 지원 시 `set_shadow_samples()`에서 LUT 재생성 필요하나, 현재 구현 범위 외.

---

## 5. Norm 제약 분석

### 5.1 shadow_config.c 함수 현황

| # | 함수명 | 역할 |
|---|--------|------|
| 1 | init_shadow_config | config 초기화 |
| 2 | validate_shadow_config | 유효성 검사 |
| 3 | set_shadow_samples | samples 설정 |
| 4 | set_shadow_softness | softness 설정 |
| 5 | **init_shadow_offset_lut** | LUT 초기화 (신규) |

→ **정확히 5함수**. `free_shadow_offset_lut`는 별도 파일 필요.

### 5.2 LUT 해제 위치

**옵션 A**: `src/lighting/shadow_lut.c` 신규 생성
- `free_shadow_offset_lut()` 공개 함수
- 향후 LUT 관련 함수 추가 가능

**옵션 B**: `src/scene/scene.c`에 static helper 추가
- cleanup 로직이 scene 생명주기와 함께 관리
- scene.c 현재 3함수 → 4함수 (Norm 충족)
- 새 파일 생성 불필요

**결정**: 옵션 B — 단순성 우선, scene 생명주기에 cleanup 로직 귀속.

---

## 6. 안전성 수정

### 6.1 magnitude 0 나눗셈 방어

**발견**: `is_in_shadow()`에서 `1.0 / mag` 사용. mag == 0 시 NaN/INF 발생 가능.

**분석**:
- 문서상 "광원은 geometry가 아니므로 물리적으로 불가능"이라 가정
- 그러나 씬 파일에서 광원을 표면 위에 배치하면 발생 가능
- Phase A (P7) 구현 시 `vec3_normalize()` → `1.0 / mag`로 변경하면서 명시적 위험 노출

**수정**:
```c
if (mag < 0.0001)
    return (0);  // 광원이 hit point와 동일 → 그림자 없음
```

**EPSILON 선택**: 0.0001 (기존 shadow bias와 동일 수준)

---

## 7. 테스트 계획

### 7.1 기능 테스트

1. S1–S4 씬 렌더 후 soft shadow 품질 확인
2. samples = 1 (hard shadow) 케이스 동작 확인
3. malloc 실패 시뮬레이션 (optional)

### 7.2 성능 테스트

1. Round 2 baseline과 비교
2. S4 (가장 많은 shadow tests)에서 효과 측정

### 7.3 메모리 테스트

```bash
leaks -atExit -- ./miniRT scenes/perf/perf_spheres_20.rt
```

---

## 8. References

- `docs/optimization-research-report.md` — P4 이론 분석
- `docs/optimization-plan.md` — Phase B 계획
- `docs/benchmark-results.md` — Round 1, 2 데이터
