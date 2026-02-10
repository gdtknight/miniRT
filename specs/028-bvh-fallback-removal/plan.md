# Implementation Plan: BVH Miss Fallback Removal (P0)

**Branch**: `028-bvh-fallback-removal` | **Date**: 2026-02-05 | **Spec**: `specs/028-bvh-fallback-removal/spec.md`
**Input**: Feature specification from `/specs/028-bvh-fallback-removal/spec.md`
**Reference**: `docs/optimization-plan.md` Phase 0 (항목 0-1)

## Summary

`trace_ray()`에서 BVH miss 시 `check_all_objects()` 폴백을 제거한다. BVH는 `scene->objects.count` 전체를 커버하므로 이 폴백은 불필요하며, 제거 시 miss ray의 이중 교차 테스트가 해소되고 BVH skip rate 계측 정확성이 복원된다. BVH enabled이지만 root가 null인 엣지 케이스에서는 `check_all_objects()` 경로를 유지하여 회귀를 방지한다.

## Technical Context

**Language/Version**: C (42 Norm v4.1 compliant)
**Primary Dependencies**: libft, MiniLibX, math library (-lm)
**Storage**: N/A
**Testing**: Manual visual verification + metrics comparison (S1–S4 scenes)
**Target Platform**: macOS (Apple M1)
**Project Type**: Single project
**Performance Goals**: Primary intersect test count 감소 (miss ray 이중 탐색 제거), skip rate 계측 정확성 복원
**Constraints**: 42 Norm v4.1 (25줄/함수, 5변수, 4인자, 5함수/파일), `-Wall -Wextra -Werror`
**Scale/Scope**: 코드 변경 `src/render/trace.c` 1개 파일, 벤치마크 문서 `docs/benchmark-results.md` 기록

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

| Principle | Status | Notes |
|-----------|--------|-------|
| I. 42 Norminette Compliance | PASS | 변경 후 함수 줄 수 감소 (trace_ray body 10줄) |
| II. Memory Safety | PASS | 메모리 할당/해제 변경 없음 |
| III. Allowed Functions | PASS | 신규 외부 함수 없음 |
| IV. Modular Architecture | PASS | 기존 모듈 구조 유지, 파일 추가/삭제 없음 |
| V. Visual Correctness | PASS | BVH 유효 시 전체 오브젝트 커버, 무효 시 brute-force 폴백 유지 |
| VI. Stability & Crash Prevention | PASS | BVH null/disabled/root-null 3중 가드 |
| VII. Window Management | PASS | 윈도우 관련 변경 없음 |

## Project Structure

### Documentation (this feature)

```text
specs/028-bvh-fallback-removal/
├── plan.md              # This file
├── research.md          # Phase 0 output
└── tasks.md             # Phase 2 output (/speckit.tasks)
```

### Source Code (affected files)

```text
src/render/trace.c       # trace_ray() 폴백 로직 수정 (유일한 코드 변경 파일)
```

### Documentation (affected files)

```text
docs/benchmark-results.md  # Round 1 측정 결과 기록 (신규 문서)
```

### Verification files

```text
scenes/perf/
├── valid_smoke_simple.rt   # S1: 1 object
├── perf_spheres_20.rt      # S2: 20 spheres
├── perf_spheres_50.rt      # S3: 50 spheres
└── perf_all_objects.rt     # S4: 7sp + 7cy + 3pl
```

**Structure Decision**: 단일 프로젝트 구조. 코드 변경은 `src/render/trace.c` 한 파일이며, 벤치마크 기록은 `docs/benchmark-results.md`에 문서화한다.

## Complexity Tracking

> No violations — single-file code change + documentation.

N/A
