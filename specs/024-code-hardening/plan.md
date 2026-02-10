# Implementation Plan: Code Hardening

**Branch**: `024-code-hardening` | **Date**: 2026-01-31 | **Spec**: [spec.md](./spec.md)
**Input**: Feature specification from `/specs/024-code-hardening/spec.md`

## Summary

코드 리뷰에서 식별된 9개의 결함을 수정하여 레이트레이서의 안정성, 정확성, 견고성을 향상시킵니다. 수정 대상은 BVH 교차 검사, AABB 슬랩 테스트, 카메라 기저 계산, 프로그레시브 타일 렌더링, 그림자 바이어스 계산, I/O 오류 처리, 정수 오버플로 방지, 리스트 확장 안전성, 픽셀 범위 검사입니다.

## Technical Context

**Language/Version**: C (42 Norm v4.1 compliant)
**Primary Dependencies**: libft, MinilibX, math library (-lm)
**Storage**: N/A (scene files read-only)
**Testing**: Manual testing + scene file regression testing
**Target Platform**: macOS (Darwin), Linux
**Project Type**: Single project (desktop raytracer)
**Performance Goals**: 기존 성능 유지 (렌더링 속도 저하 없음)
**Constraints**: 42 Norm v4.1 (25 lines/function, 5 vars/function, 5 functions/file)
**Scale/Scope**: 9개 결함 수정, 9개 파일 수정

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

| Principle | Status | Notes |
|-----------|--------|-------|
| I. 42 Norminette | ✅ PASS | 모든 수정 코드는 Norm v4.1 준수 |
| II. Memory Safety | ✅ PASS | 새로운 할당 없음, 기존 할당 패턴 유지 |
| III. Allowed Functions | ✅ PASS | 기존 허용 함수만 사용 (math.h fabs 포함) |
| IV. Modular Architecture | ✅ PASS | 기존 모듈 구조 유지 |
| V. Visual Correctness | ✅ PASS | 렌더링 정확성 개선 (버그 수정) |
| VI. Stability & Crash Prevention | ✅ PASS | 크래시 방지가 본 기능의 핵심 목표 |
| VII. Window Management | ✅ PASS | 윈도우 관련 수정 없음 |

## Project Structure

### Documentation (this feature)

```text
specs/024-code-hardening/
├── plan.md              # This file
├── research.md          # Phase 0 output
├── data-model.md        # Phase 1 output
├── quickstart.md        # Phase 1 output
├── contracts/           # Phase 1 output
└── tasks.md             # Phase 2 output (via /speckit.tasks)
```

### Source Code (affected files)

```text
src/
├── spatial/
│   ├── bvh_traverse.c   # FR-001: temp_hit.distance 초기화
│   └── aabb.c           # FR-002: div-by-zero 방지
├── render/
│   ├── camera.c         # FR-003: gimbal lock 방지
│   └── render_progressive.c  # FR-004: 하드코딩 제거
├── lighting/
│   └── shadow_calc.c    # FR-005: 실제 표면 노멀 사용
├── parser/
│   ├── parse_line_reader.c  # FR-006: I/O 오류 구분
│   └── parse_number.c   # FR-007: 정수 오버플로 검사
├── scene/
│   └── object_list.c    # FR-008: capacity 오버플로 검사
└── window/
    └── mlx_pixel.c      # FR-009: 좌표 범위 검사

includes/
├── parser.h             # t_line_reader.io_error 필드 추가
└── render_progressive.h # t_progressive_state.width/height 필드 추가
```

**Structure Decision**: 기존 모듈 구조 유지. 9개 파일에 대한 국소적 수정만 수행.

## Design Decisions

### D1: BVH temp_hit.distance 초기화 (FR-001)

**Decision**: `temp_hit.distance = hit->distance` (호출자 상한값 유지)

**Rationale**:
- 호출자가 설정한 clip distance를 유지해야 함
- 고정값(1e30)은 그림자 등에서 상한 초과 허용으로 오동작 가능

**Alternatives Rejected**:
- `temp_hit.distance = INFINITY`: 호출자 의도 무시

### D2: AABB div-by-zero 처리 (FR-002)

**Decision**: 방향 성분이 0에 가까울 때 (|d| < 1e-8) slab 내부 판정으로 대체

**Rationale**:
- IEEE 754 INF는 비교에서 동작하지만 NaN (0/0)은 문제
- origin이 slab 외부면 교차 없음, 내부면 통과

**Alternatives Rejected**:
- 역수 사전 계산 + 큰 값 대체: 추가 메모리 필요

### D3: 카메라 gimbal lock 방지 (FR-003)

**Decision**: `dot = fabs(vec3_dot(direction, world_up)) > 0.999` 시 대체 up 벡터 (0,0,1) 사용

**Rationale**:
- 0.999 threshold는 약 2.56도 이내의 근사 수직을 커버
- (0,0,1)은 수직 방향과 직교하므로 유효한 기저 생성

### D4: 프로그레시브 타일 너비 (FR-004)

**Decision**: `t_progressive_state`에 width/height 필드 추가, `progressive_init`에서 저장

**Rationale**:
- 런타임에 실제 윈도우 크기 반영
- 리사이즈 지원을 위한 확장성

### D5: 그림자 표면 노멀 (FR-005)

**Decision**: `calculate_shadow_factor` 함수 시그니처에 `t_vec3 surface_normal` 파라미터 추가

**Rationale**:
- 기존 코드의 고정 노멀 (0,1,0) 제거
- 호출 지점에서 실제 hit.normal 전달

### D6: I/O 오류 플래그 (FR-006)

**Decision**: `t_line_reader`에 `io_error` 필드 추가, `read() == -1` 시 설정

**Rationale**:
- EOF(0)와 오류(-1) 명확히 구분
- 파싱 루프에서 플래그 확인 후 오류 보고

### D7: 정수 오버플로 검사 (FR-007)

**Decision**: `parse_int_digits`에서 `result > (INT_MAX - digit) / 10` 검사

**Rationale**:
- 곱셈 전 오버플로 사전 감지
- 오버플로 시 `PARSE_ERR_NUMBER_FORMAT` 반환

### D8: Capacity 오버플로 검사 (FR-008)

**Decision**: `object_list_grow`에서 `capacity > INT_MAX / 2` 검사

**Rationale**:
- 실제 발생 가능성 낮지만 방어적 코딩
- 오버플로 시 0 반환 (실패)

### D9: 픽셀 범위 검사 (FR-009)

**Decision**: `mlx_img_put_pixel`/`mlx_img_get_pixel`에 범위 검사 추가

**Rationale**:
- Assumptions에서 "함수 내 검사 추가"로 결정됨
- 성능 영향 미미 (조건 분기 1회)

## Error Handling

I/O 오류 시 새로운 에러 코드 추가:

```c
typedef enum e_parse_result
{
    // ... existing codes ...
    PARSE_ERR_IO,           // 추가: I/O 오류
}   t_parse_result;
```

오류 메시지: `"Error\nI/O error while reading file\n"`

## Complexity Tracking

> 본 기능은 버그 수정으로, Constitution 위반 사항 없음.

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| (없음) | - | - |
