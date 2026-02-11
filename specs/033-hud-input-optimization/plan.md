# Implementation Plan: HUD / Input / Rendering Optimization

**Branch**: `033-hud-input-optimization` | **Date**: 2026-02-10 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `/specs/033-hud-input-optimization/spec.md`

## Summary

디바운스 상태 머신을 재설계하여 이산 키 입력 시 FQ 렌더 폭주를 방지하고, 즉시 LQ 프리뷰로 입력 피드백을 제공한다. PREVIEW 상태를 제거하고 COOLDOWN 상태를 추가하여 FQ 완료 직후 연속 탭을 병합한다. 데드 코드(KEY_I, hud_render_background, cancel_requested)를 제거하고, pixel_timing 게이트를 추가하며, macOS 호환 키맵으로 교체한다.

## Technical Context

**Language/Version**: C (42 Norm v4.1 compliant)
**Primary Dependencies**: MiniLibX, libft, math library (-lm)
**Storage**: N/A (in-memory rendering)
**Testing**: 수동 테스트 (씬 파일 + 키 입력 시나리오), norminette, `-Wall -Wextra -Werror`
**Target Platform**: Linux (X11), macOS (AppKit)
**Project Type**: single
**Performance Goals**: 이산 탭 5회 시 FQ 2회 이하, LQ 프리뷰 ~20fps during key-hold
**Constraints**: 42 Norm v4.1 (25줄/함수, 5변수/함수, 5함수/파일, 4인자/함수), 싱글 스레드 MiniLibX 이벤트 루프
**Scale/Scope**: ~15개 소스 파일 변경, 상태 머신 4-state → 4-state 전환 (PREVIEW→COOLDOWN)

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

| Principle | Status | Notes |
|-----------|--------|-------|
| I. 42 Norminette Compliance | PASS (주의 필요) | `debounce_on_input()`에 `last_preview_time` 추가 시 변수 5개 제한 주의. 헬퍼 함수 분리로 해결. `debounce_update()`에 COOLDOWN 분기 추가 시 25줄 제한 주의. `debounce_handle_cooldown()` 분리로 해결. |
| II. Memory Safety | PASS | 힙 할당 변경 없음. 기존 struct 필드 추가만. |
| III. Allowed Functions | PASS | `gettimeofday()` 이미 사용 중 (render_debounce_timer.c). 새 외부 함수 추가 없음. |
| IV. Modular Architecture | PASS | 기존 모듈 경계 내 변경. render/, window/, hud/, keyguide/, includes/. |
| V. Visual Correctness | PASS | 렌더링 결과 변경 없음. 타이밍/입력 처리만 변경. |
| VI. Stability & Crash Prevention | PASS | 상태 머신 전이만 변경. NULL 가드 불필요 (기존 struct 포인터 사용). |
| VII. Window Management | PASS | 윈도우 이벤트 처리 구조 유지. 키 바인딩만 변경. |

**Post-Phase 1 Re-check**:
- `render_debounce.c`: 현재 5함수. PREVIEW 제거(-1) + FINAL 핸들러(+1) + 스로틀 헬퍼(+1) = 6. 별도 파일 분리 필요.
- `render_debounce_timer.c`: 현재 5함수(cancel 포함). cancel 제거(-1) = 4. 스로틀 헬퍼를 이 파일에 배치 가능 → 5함수. Norm 준수.
- `window_loop.c:52-57`: cancel_requested 체크 블록 제거 시 `execute_render_pass()` 간소화. 5줄 감소.
- 신규 파일 불필요: `render_debounce_timer.c`에 스로틀 헬퍼 배치로 해결.

## Project Structure

### Documentation (this feature)

```text
specs/033-hud-input-optimization/
├── plan.md              # This file
├── spec.md              # Feature specification
├── research.md          # Phase 0: Technical research
├── data-model.md        # Phase 1: Data model changes
├── quickstart.md        # Phase 1: Build & test guide
└── contracts/           # Phase 1: Function signatures
    ├── debounce.md      # Debounce state machine API
    ├── dead-code.md     # Removal targets
    └── keymap.md        # Keymap changes
```

### Source Code (repository root)

```text
src/
├── render/
│   ├── render_debounce.c        # 상태 머신 재설계 (FR-001~008)
│   ├── render_debounce_timer.c  # 타이머 유틸 (기존 유지)
│   └── render.c                 # pixel_timing 게이트 (FR-013)
├── window/
│   ├── window_events.c          # KEY_I 핸들러 제거 (FR-009)
│   └── window_key_handlers.c    # 키맵 변경 (FR-014)
├── hud/
│   └── hud_text.c               # 배경 블렌딩 데드 코드 제거 (FR-012)
└── keyguide/
    ├── keyguide_render.c        # I-Info 제거 + 키가이드 업데이트 (FR-011, FR-015, FR-016)
    └── keyguide_render_extra.c  # 키가이드 텍스트 업데이트 (FR-015)

includes/
├── render_debounce.h            # COOLDOWN enum, defines, struct 변경 (FR-001, FR-005)
├── window.h                     # RENDER_SHOW_INFO 제거 (FR-010)
├── window_internal.h            # 새 키 define 추가 (FR-014)
├── hud.h                        # hud_render_background 선언 제거 (FR-012)
└── hud_text.h                   # hud_render_background 선언 제거 (FR-012)
```

**Structure Decision**: 기존 모듈 구조 내에서 변경. 신규 파일 생성 불필요 (render_debounce.c 내 함수 수가 5개 초과할 경우에만 render_debounce_cooldown.c 분리 검토).

## Complexity Tracking

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| render_debounce.c 함수 6개 가능성 | PREVIEW 제거(-1) + FINAL 핸들러(+1) + 스로틀 헬퍼(+1) = 6 | 스로틀 헬퍼를 render_debounce_timer.c로 이동 (cancel 제거 후 여유 있음). 신규 파일 불필요. |
