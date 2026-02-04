# Implementation Plan: Dead Code Removal & Legacy Cleanup

**Branch**: `025-dead-code-removal` | **Date**: 2026-02-04 | **Spec**: `specs/025-dead-code-removal/spec.md`
**Input**: Feature specification from `/specs/025-dead-code-removal/spec.md`
**Reference**: `docs/codebase-review-2026-02-04.md`

## Summary

miniRT 코드베이스에서 정적 분석으로 검증된 dead code를 제거합니다. 파일 13개(헤더 5 + 소스 8) 삭제, 부분 선언/함수 22개 삭제, 래퍼 3곳 치환. 기능 변경 없이 코드베이스 규모를 축소하고 유지보수성을 향상합니다.

## Technical Context

**Language/Version**: C (42 Norm v4.1 compliant)
**Primary Dependencies**: libft, MiniLibX, math library (-lm)
**Storage**: N/A (scene files read-only)
**Testing**: 수동 빌드 + norminette + 씬 파일 회귀 테스트
**Target Platform**: macOS (Darwin) / Linux
**Project Type**: single
**Performance Goals**: N/A (삭제 전용, 성능 변화 없음)
**Constraints**: 42 Norm v4.1 (25줄/함수, 5변수/함수, 4인자/함수, 5함수/파일)
**Scale/Scope**: 삭제 13파일, 수정 11파일, Makefile 1파일

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

| 원칙 | 상태 | 비고 |
|------|------|------|
| I. Norminette Compliance | PASS | 삭제만 수행, 수정 파일은 norminette 검증 |
| II. Memory Safety | PASS | cleanup_all 치환 시 동일한 destroy 호출 순서 유지 |
| III. Allowed Functions | PASS | 새로운 외부 함수 도입 없음 |
| IV. Modular Architecture | PASS | 모듈 구조 변경 없음, dead 모듈만 제거 |
| V. Visual Correctness | PASS | 렌더링 코드 변경 없음 |
| VI. Stability | PASS | 에러 경로 동일 (error_print 동일 동작) |
| VII. Window Management | PASS | close_window 동작 동일 |

**Post-Design Re-check**: PASS. 모든 변경이 삭제/치환이며, 새로운 위반 사항 없음.

## Project Structure

### Documentation (this feature)

```text
specs/025-dead-code-removal/
├── plan.md                          # This file
├── spec.md                          # Feature specification
├── research.md                      # Phase 0: research findings
├── data-model.md                    # Phase 1: deletion targets
├── quickstart.md                    # Phase 1: execution summary
├── contracts/
│   └── deletion-manifest.md         # Phase 1: detailed diff manifest
└── tasks.md                         # /speckit.tasks 실행 시 생성
```

### Source Code (affected)

```text
includes/                            # 헤더 수정/삭제
├── overlay.h                        # DELETE
├── format_object_id.h               # DELETE
├── render_quality.h                 # DELETE
├── render_progressive.h             # DELETE
├── render_state.h                   # DELETE
├── objects.h                        # MODIFY (typedef 3개 삭제)
├── parser.h                         # MODIFY (선언 2개 삭제)
├── window_internal.h                # MODIFY (선언 8개 삭제)
├── minirt.h                         # MODIFY (선언 2개 삭제)
├── error.h                          # MODIFY (선언 1개 삭제)
└── keyguide.h                       # MODIFY (선언 1개 삭제)

src/
├── spatial/
│   └── aabb_shapes.c                # DELETE
├── render/
│   ├── render_state.c               # DELETE
│   ├── render_quality.c             # DELETE
│   └── render_progressive.c         # DELETE
├── utils/
│   ├── format_object_id.c           # DELETE
│   ├── ft_atof.c                    # DELETE
│   ├── cleanup.c                    # DELETE
│   └── error.c                      # MODIFY (print_error 삭제)
├── parser/
│   ├── parse_validation.c           # DELETE (in_range 이동 후)
│   ├── parse_validation_strict.c    # MODIFY (in_range 수신)
│   └── parser.c                     # MODIFY (print_error 치환)
├── keyguide/
│   └── keyguide_render.c            # MODIFY (함수 1개 삭제)
└── window/
    └── window_lifecycle.c           # MODIFY (cleanup_all 치환)

Makefile                             # MODIFY (8개 SRCS 행 제거)
```

## Implementation Phases

### Phase 1: 무참조 Dead Code 삭제 (FR-001, FR-002, FR-003, FR-004)

안전한 삭제만 수행. 호출자가 0건인 파일/함수/선언 제거.

**User Stories**:
- US1: 미사용 헤더 파일 삭제 (overlay.h, format_object_id.h, render_quality.h, render_progressive.h, render_state.h)
- US2: 미사용 소스 파일 삭제 (aabb_shapes.c, render_state.c, render_quality.c, render_progressive.c, format_object_id.c)
- US3: 레거시 typedef 삭제 (objects.h)
- US4: 미구현/중복 선언 삭제 (parser.h, window_internal.h, keyguide.h)
- US5: Dead 함수 삭제 (keyguide_render_background)
- US6: Makefile 동기화 (Phase 1 삭제분)

**빌드 중간 검증**: `make re` + `norminette`

### Phase 2: 의존성 분리 (FR-005)

활성 함수 이동 후 잔여 파일 삭제.

**User Stories**:
- US7: `in_range` 이동 (parse_validation.c → parse_validation_strict.c)
- US8: parse_validation.c 삭제 + Makefile
- US9: ft_atof.c 삭제 + minirt.h 선언 + Makefile

**빌드 중간 검증**: `make re` + `norminette`

### Phase 3: 래퍼/에러 경로 치환 (FR-006, FR-007)

callsite 치환 후 래퍼 삭제.

**User Stories**:
- US10: cleanup_all → render_destroy + scene_destroy 치환 (window_lifecycle.c)
- US11: cleanup.c 삭제 + minirt.h 선언 + Makefile
- US12: print_error → error_print 치환 (parser.c 7곳)
- US13: print_error 함수/선언 삭제 (error.c, error.h)

**빌드 중간 검증**: `make re` + `norminette`

### Phase 4: 최종 검증

- US14: 전체 빌드 + norminette
- US15: 정상 씬 렌더링 회귀 테스트
- US16: 비정상 씬 18개 에러 출력 회귀 테스트

## Risk Assessment

| 리스크 | 영향 | 완화 |
|--------|------|------|
| close_window에서 render->scene 접근 순서 오류 | use-after-free | 지역 변수에 scene 먼저 저장 (FR-006) |
| print_error 반환값(0) vs error_print 반환값(1) 혼동 | 파서 동작 오류 | comma operator 패턴으로 0 반환 유지 (FR-007) |
| Makefile SRCS 누락 | 빌드 실패 | Phase별 중간 빌드 검증 |
| parse_validation.c 삭제 시 in_range 유실 | 컴파일 에러 | in_range 이동 먼저 수행 (FR-005 → FR-001) |

## Complexity Tracking

> Constitution Check에 위반 사항 없음. 이 섹션은 해당 없음.
