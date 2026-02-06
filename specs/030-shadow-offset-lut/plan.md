# Implementation Plan: Shadow Offset LUT (P4)

**Branch**: `030-shadow-offset-lut` | **Date**: 2026-02-06 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `/specs/030-shadow-offset-lut/spec.md`

---

## Summary

Soft shadow 샘플링에서 매 픽셀마다 반복되는 삼각함수(cos, sin) 및 sqrt 호출을 사전 계산된 LUT로 대체한다. 16 samples 기준 ~4,000 cycles/pixel → ~10 cycles/pixel로 절감 예상.

---

## Technical Context

**Language/Version**: C (42 Norm v4.1 compliant)
**Primary Dependencies**: libft, MiniLibX, math library (-lm)
**Storage**: N/A
**Testing**: 수동 시각 검증 + metrics 비교 + leaks 검사
**Target Platform**: macOS (42 환경)
**Project Type**: Single project (raytracer)
**Performance Goals**: 프레임 타임 감소 (Round 2 대비)
**Constraints**: 42 Norm, 메모리 누수 금지
**Scale/Scope**: 5개 파일, ~60줄 변경/추가

---

## Constitution Check

| 제약 | 상태 | 비고 |
|------|------|------|
| 함수 25줄 | ✓ PASS | LUT 초기화 함수 ~15줄 |
| 변수 5개 | ✓ PASS | 기존 로직 재사용 |
| 인자 4개 | ✓ PASS | 함수 시그니처 변경 없음 |
| 파일 5함수 | ✓ PASS | shadow_config.c 4→5 |
| 메모리 안전 | ✓ PASS | malloc 체크 + cleanup 보장 |

---

## Project Structure

### Documentation (this feature)

```text
specs/030-shadow-offset-lut/
├── spec.md              # 기능 명세
├── plan.md              # 본 문서
├── research.md          # 기술 분석
└── tasks.md             # 작업 목록 (/speckit.tasks 생성)
```

### Source Code (수정 대상)

```text
includes/
└── shadow.h             # t_shadow_config에 offset_lut 필드 추가

src/lighting/
├── shadow_config.c      # init_shadow_offset_lut() 추가
├── shadow_calc.c        # sample_shadow_ray()에서 LUT 참조
└── shadow_test.c        # magnitude guard 추가

src/scene/
└── scene.c              # scene_destroy()에서 LUT 해제 (static helper)
```

---

## Implementation Details

### 0. 선행 수정: magnitude 0 나눗셈 방어 (`src/lighting/shadow_test.c`)

**문제**: `is_in_shadow()`에서 `1.0 / mag` 사용 시 mag == 0이면 NaN/INF 발생.

**현재**:
```c
mag = vec3_magnitude(to_light);
shadow_hit.distance = mag;
light_dir = vec3_multiply(to_light, 1.0 / mag);
```

**변경**:
```c
mag = vec3_magnitude(to_light);
if (mag < 0.0001)
    return (0);
shadow_hit.distance = mag;
light_dir = vec3_multiply(to_light, 1.0 / mag);
```

---

### 1. 구조체 확장 (`includes/shadow.h`)

**현재**:
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
    t_vec3  *offset_lut;
}   t_shadow_config;
```

### 2. LUT 초기화 (`src/lighting/shadow_config.c`)

```c
/**
 * @brief Initialize shadow offset LUT for soft shadow sampling.
 *
 * Precomputes circular stratified offsets for each sample.
 * If allocation fails, offset_lut remains NULL and fallback is used.
 *
 * @param config Shadow configuration with samples set.
 */
void    init_shadow_offset_lut(t_shadow_config *config)
{
    int     i;
    int     grid_size;
    double  angle;
    double  r;

    config->offset_lut = malloc(sizeof(t_vec3) * config->samples);
    if (!config->offset_lut)
        return ;
    grid_size = (int)sqrt((double)config->samples);
    if (grid_size < 1)
        grid_size = 1;
    i = 0;
    while (i < config->samples)
    {
        angle = 2.0 * M_PI * (i % grid_size) / (double)grid_size;
        r = (i / (double)grid_size + 0.5) / (double)grid_size;
        config->offset_lut[i].x = r * cos(angle);
        config->offset_lut[i].y = r * sin(angle);
        config->offset_lut[i].z = 0.0;
        i++;
    }
}
```

**Norm 검증**:
- 줄 수: ~20줄 (25줄 이하 ✓)
- 변수: i, grid_size, angle, r = 4개 (5개 이하 ✓)
- shadow_config.c 함수 수: 4 → 5 (5개 이하 ✓)

### 3. init_shadow_config() 수정

**현재**:
```c
t_shadow_config init_shadow_config(void)
{
    t_shadow_config config;

    config.samples = 16;
    config.softness = 0.3;
    config.bias_scale = 2.0;
    config.enable_ao = 0;
    return (config);
}
```

**변경**:
```c
t_shadow_config init_shadow_config(void)
{
    t_shadow_config config;

    config.samples = 16;
    config.softness = 0.3;
    config.bias_scale = 2.0;
    config.enable_ao = 0;
    config.offset_lut = NULL;
    init_shadow_offset_lut(&config);
    return (config);
}
```

### 4. sample_shadow_ray() 수정 (`src/lighting/shadow_calc.c`)

**현재**:
```c
static int  sample_shadow_ray(t_shadow_sample *params, int index)
{
    t_vec3  offset;
    t_vec3  sample_light_pos;

    offset = generate_shadow_sample_offset(params->config->softness * 2.0,
            index, params->config->samples);
    sample_light_pos = vec3_add(params->light_pos, offset);
    return (is_in_shadow(params->scene, params->point,
            sample_light_pos, params->bias));
}
```

**변경**:
```c
static int  sample_shadow_ray(t_shadow_sample *params, int index)
{
    t_vec3  offset;
    t_vec3  sample_light_pos;
    double  radius;

    radius = params->config->softness * 2.0;
    if (params->config->offset_lut)
        offset = vec3_multiply(params->config->offset_lut[index], radius);
    else
        offset = generate_shadow_sample_offset(radius, index,
                params->config->samples);
    sample_light_pos = vec3_add(params->light_pos, offset);
    return (is_in_shadow(params->scene, params->point,
            sample_light_pos, params->bias));
}
```

**Norm 검증**:
- 변수: offset, sample_light_pos, radius = 3개 (5개 이하 ✓)

### 5. LUT 해제

Scene cleanup 시 LUT 메모리 해제 필요. 기존 cleanup 함수 위치 확인 필요.

```c
void    free_shadow_offset_lut(t_shadow_config *config)
{
    if (config && config->offset_lut)
    {
        free(config->offset_lut);
        config->offset_lut = NULL;
    }
}
```

### 6. set_shadow_samples() 수정 — 제외

**제외 근거**: spec Section 3 Non-Goal에서 "샘플 수 동적 변경 UI 지원" 명시적 제외.
현재 samples는 초기화 시 16으로 고정되며 런타임 변경 없음.

향후 동적 변경 지원 시 해당 함수 수정 필요하나 현재 구현 범위 외.

---

## Execution Order

```text
1. shadow.h 구조체 확장
2. shadow_config.c에 init_shadow_offset_lut() 추가
3. init_shadow_config() 수정
4. shadow_calc.c의 sample_shadow_ray() 수정
5. cleanup 함수 추가 (별도 파일 또는 기존 cleanup 위치)
6. 검증 → 벤치마크
```

---

## Validation Plan

### 기능 검증

1. **빌드**: `make re` 경고 0
2. **Norm**: `norminette` 오류 0
3. **시각 검증**: S1–S4 씬 렌더 후 soft shadow 품질 동일
4. **메모리**: `leaks` 검사 통과

### 성능 검증

1. S1–S4 씬 측정
2. `docs/benchmark-results.md` Round 3 작성
3. 개선율 = `(Round 2 - Round 3) / Round 2 × 100%`

---

## Risks and Mitigations

| 위험 | 확률 | 영향 | 대응 |
|------|------|------|------|
| malloc 실패 | Low | LUT 미사용 | NULL 체크 후 폴백 |
| 메모리 누수 | Medium | 평가 실패 | cleanup 경로 철저 검증 |
| Norm 위반 (5함수) | Medium | 빌드 불가 | 파일 분리 |
| samples 동적 변경 | Low | LUT 불일치 | set_shadow_samples 수정 |

---

## Complexity Tracking

> Constitution Check 위반 없음 — 해당 섹션 불필요
