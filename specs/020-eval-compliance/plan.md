# Implementation Plan: Evaluation Compliance Fixes

**Branch**: `020-eval-compliance` | **Date**: 2026-01-27 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `/specs/020-eval-compliance/spec.md`

## Summary

miniRT 평가 기준(miniRT_eval.md) 대비 누락된 5가지 기능을 구현한다:
1. `.rt` 파일 확장자 검증 (FR-001/002)
2. 오브젝트 리사이즈 — 구 지름, 실린더 지름/높이 (FR-003/004/010/011)
3. 오브젝트 회전 — 실린더/평면, 월드 좌표 축 기반 (FR-005/006/012)
4. 윈도우 expose 이벤트 처리 (FR-007)
5. 실린더 내부 교차 법선 보정 (FR-008)

## Technical Context

**Language/Version**: C (42 Norm v4.1 compliant)
**Primary Dependencies**: MiniLibX, libft, math library (-lm)
**Storage**: N/A (scene files are read-only input)
**Testing**: Manual visual verification + test .rt scene files
**Target Platform**: macOS (MiniLibX OpenGL), Linux (MiniLibX X11)
**Project Type**: Single C project
**Performance Goals**: 렌더링 debounce 150ms, preview + final 2단계 렌더 (기존 구현, 근거: `includes/render_debounce.h:18-21`)
**Constraints**: Norm v4.1 (25줄/함수, 5함수/파일, 5변수/함수, 4파라미터/함수)
**Scale/Scope**: ~50 소스 파일, 변경 대상 ~10 파일

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

| Principle | Status | Notes |
|-----------|--------|-------|
| I. Norminette | PASS | 모든 신규 코드 Norm v4.1 준수. 파일당 5함수, 함수당 25줄 제한 내 |
| II. Memory Safety | PASS | 신규 heap 할당 없음 (키 핸들러, expose 핸들러는 스택만 사용). BVH rebuild는 기존 destroy+build 경로 재사용 |
| III. Allowed Functions | PASS | 신규 사용 함수: cos(), sin(), vec3_normalize() 등 기존 math+내부 함수만 |
| IV. Modular Architecture | PASS | 기존 모듈 구조(window/, ray/, spatial/) 내에서 확장 |
| V. Visual Correctness | PASS | 이 plan이 직접 해결하는 항목들 |
| VI. Stability | PASS | FR-012(축 퇴화 방지), FR-013(렌더 중 입력 안전) 포함 |
| VII. Window Management | PASS | FR-007(expose 이벤트), 고정 윈도우 크기 |

## Project Structure

### Documentation (this feature)

```text
specs/020-eval-compliance/
├── plan.md              # This file
├── spec.md              # Feature specification
├── research.md          # Phase 0: key mapping, BVH rebuild, expose event research
├── data-model.md        # Phase 1: data structure changes
├── quickstart.md        # Phase 1: implementation quickstart guide
├── checklists/
│   └── requirements.md  # Spec quality checklist
└── tasks.md             # Phase 2 output (/speckit.tasks)
```

### Source Code (변경 대상)

```text
src/
├── parser/
│   └── parser.c              # [수정] .rt 확장자 검증 추가
├── ray/
│   └── intersect_cyl_new.c   # [수정] body 내부 법선 플립 추가
├── spatial/
│   └── bvh_init.c            # [기존] scene_build_bvh() 재사용
├── window/
│   ├── window_init.c         # [수정] expose hook 등록
│   ├── window_events.c       # [수정] expose handler 추가
│   ├── window_key_handlers.c # [수정] resize/rotation 키 dispatch 추가
│   ├── window_objects.c      # [수정] resize/rotation 핸들러 추가
│   └── window_resize.c       # [신규] handle_object_resize() 함수
└── render/
    └── render.c              # [수정] 렌더 전 BVH rebuild 조건부 호출

includes/
├── window_internal.h         # [수정] 신규 KEY_* 상수 추가
└── window.h                  # [수정] BVH_DIRTY 플래그 추가
```

**Structure Decision**: 기존 모듈 구조 유지. 신규 파일은 `window_resize.c` 1개만 추가 (Norm 5함수/파일 제한으로 window_objects.c에 모두 넣을 수 없음).

## Design Decisions

### D1: 키 매핑

| 기능 | 키 | 설명 |
|------|-----|------|
| Resize 지름 +/- | J / K | 구 지름 또는 실린더 지름 증감 |
| Resize 높이 +/- | N / M | 실린더 높이 증감 (구/평면에서는 무시) |
| Rotation X축 +/- | U / O | 월드 X축 기준 회전 |
| Rotation Y축 +/- | Y / P | 월드 Y축 기준 회전 |
| Rotation Z축 +/- | LEFT / RIGHT | 월드 Z축 기준 회전 |

Step 값: resize step = 1.0, rotation step = 5° (π/36 rad) — 기존 카메라 회전과 동일 단위.

### D2: BVH Rebuild 전략

- `RENDER_BVH_DIRTY` 플래그(0x20) 추가
- 오브젝트 변환(이동/리사이즈/회전) 시 플래그 세팅
- 렌더 루프에서 실제 렌더 시작 직전에 1회 rebuild
- 기존 `scene_build_bvh()` 재사용 (내부에서 destroy+build)

### D3: Window Stability (Expose + OS Resize)

- `mlx_hook(win, 12, 1L<<15, handle_expose, render)` 등록
- handler: `mlx_put_image_to_window()` 호출만 수행
- 새 렌더 트리거 없음 (기존 버퍼 재출력만)
- **OS resize에 대한 별도 코드가 불필요한 근거**: MiniLibX의 `mlx_new_image()`는 고정 크기 이미지 버퍼를 생성하며, 윈도우 크기 변경 이벤트와 무관하게 버퍼 크기가 불변. `mlx_put_image_to_window()`는 항상 원본 버퍼 크기로 출력. OS가 윈도우 프레임 드래그를 허용하더라도 내부 렌더링 영역은 초기 WINDOW_WIDTH x WINDOW_HEIGHT로 고정됨. 따라서 ConfigureNotify(event 22) 처리나 resize 차단 코드가 필요 없음.

### D4: 실린더 내부 법선 보정

- `intersect_cyl_body_new()`에서 body hit 후 `vec3_dot(ray->direction, hit->normal) > 0` 체크
- 조건 참이면 `hit->normal = vec3_multiply(hit->normal, -1.0)` (캡과 동일 패턴)

### D5: Minimum Resize Threshold

- 최소값: 0.1 (반지름 기준, 지름으로는 0.2)
- 적용: resize 감소 시 `if (new_value < 0.1) return` 가드

### D6: FR-013 렌더 중 입력 안전 — 별도 구현 불필요

- **근거**: miniRT는 단일 스레드로 동작. MiniLibX의 `mlx_loop`가 이벤트 루프를 순차 실행하며, 키 이벤트 콜백과 렌더 루프 콜백(`mlx_loop_hook`)은 동일 스레드에서 교대 실행됨.
- 키 입력 → 플래그 세팅(RENDER_DIRTY, RENDER_BVH_DIRTY) → 다음 프레임에서 렌더 시작 순서가 보장됨.
- 렌더 진행 중 키 입력이 발생하면: (1) debounce가 `cancel_requested`를 세팅, (2) 렌더 루프가 cancel 체크 후 중단, (3) 다음 프레임에서 새 렌더 시작. 이 흐름에 race condition이 없음.
- 따라서 FR-013은 아키텍처적으로 충족되며, Phase H에서 스트레스 테스트로 검증만 수행.

## Implementation Order

```
Phase A: .rt 확장자 검증 (FR-001/002)
  └── parser.c 수정 (독립, 의존성 없음)

Phase B: 키 인프라 (D1 키 매핑 + dispatch)
  ├── window_internal.h: KEY_J/K/N/M/U/O/Y/P/LEFT/RIGHT 상수 추가
  ├── window_key_handlers.c: handle_transform_keys() 확장
  └── 의존: 없음

Phase C: 오브젝트 리사이즈 (FR-003/004/010/011)
  ├── window_resize.c: handle_object_resize() 신규
  ├── BVH dirty 플래그 세팅
  └── 의존: Phase B

Phase D: 오브젝트 회전 (FR-005/006/012)
  ├── window_objects.c 또는 신규 파일: handle_object_rotate()
  ├── 축 퇴화 방지 (정규화 실패 시 무시)
  ├── BVH dirty 플래그 세팅
  └── 의존: Phase B

Phase E: BVH Rebuild (D2)
  ├── window.h: RENDER_BVH_DIRTY 플래그 추가
  ├── render.c: 렌더 전 조건부 rebuild
  └── 의존: Phase C, D (플래그 세팅하는 코드 필요)

Phase F: Window Stability (D3, FR-007)
  ├── window_events.c: handle_expose() 추가
  ├── window_init.c: expose hook 등록
  ├── 검증: OS resize 드래그 시 내부 버퍼 깨짐 없음 확인
  ├── 검증: minimize/restore, 창 가림/복원 시 콘텐츠 일관성
  └── 의존: 없음 (독립)

Phase G: 실린더 내부 법선 (D4, FR-008)
  ├── intersect_cyl_new.c: body 법선 플립 추가
  └── 의존: 없음 (독립)

Phase H: 통합 검증 (FR-013 + 전체 키 매핑 + 메시지 형식)
  ├── FR-013 검증: 복잡 씬(10+오브젝트) 렌더 중 resize/rotation 키 반복 입력 → crash 없음
  ├── 키 매핑 검증: 각 키(J/K/N/M/U/O/Y/P/LEFT/RIGHT)별 입력→변화 확인
  ├── 키코드 검증: macOS/Linux 양 플랫폼에서 키 입력 정상 수신 확인
  ├── .rt 오류 메시지: stderr 출력이 "Error\n" + 설명 형식과 정확히 일치하는지 확인
  └── 의존: Phase A~G 전체 완료
```

Phase A, B, F, G는 독립적으로 병렬 진행 가능.
Phase C, D는 B 완료 후.
Phase E는 C, D 완료 후.
Phase H는 전체 완료 후 통합 검증.

## Complexity Tracking

> 헌법 위반 없음. 모든 변경이 기존 아키텍처 내에서 수행됨.

N/A
