# Implementation Plan: Fix HUD Key Guide & Expose Restore

**Branch**: `021-fix-hud-keys-expose` | **Date**: 2026-01-28 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `/specs/021-fix-hud-keys-expose/spec.md`

## Summary

두 가지 수정: (1) expose 이벤트 핸들러에서 HUD/keyguide 오버레이를 씬 버퍼 재출력 후 다시 그리도록 보강, (2) keyguide에 Resize/Rotation 섹션 추가. 모든 렌더링은 기존 직접 프레임버퍼 방식을 유지하며, 레이트레이싱 패스를 트리거하지 않는다.

## Technical Context

**Language/Version**: C (Norm v4.1)
**Primary Dependencies**: MiniLibX, libft
**Storage**: N/A
**Testing**: Manual visual verification (expose toggle, keyguide display)
**Target Platform**: macOS (MiniLibX)
**Project Type**: Single C project
**Performance Goals**: Expose 복원 시 프레임 카운터 미증가 (레이트레이싱 없음)
**Constraints**: Norm v4.1 (25줄/함수, 5함수/파일, 5변수/함수, 4파라미터/함수)
**Scale/Scope**: 2~3개 파일 수정, 1개 파일 신규 생성

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

| Principle | Status | Notes |
|-----------|--------|-------|
| I. Norminette v4.1 | ✅ PASS | 모든 변경 사항 norminette 검증 필수. `keyguide_render.c` 이미 5함수 → 신규 섹션은 별도 파일로 분리 |
| II. Memory Safety | ✅ PASS | 새 할당 없음. 기존 버퍼/문자열만 사용 |
| III. Allowed Functions | ✅ PASS | `mlx_string_put`, `mlx_put_image_to_window` 만 사용 |
| IV. Modular Architecture | ✅ PASS | keyguide/ 모듈 내 파일 추가 |
| V. Visual Correctness | ✅ PASS | 오버레이 표시만, 렌더링 변경 없음 |
| VI. Stability | ✅ PASS | NULL 체크 유지, crash 방지 |
| VII. Window Management | ✅ PASS | expose 후 콘텐츠 일관성 보장이 목적 |

## Project Structure

### Documentation (this feature)

```text
specs/021-fix-hud-keys-expose/
├── plan.md              # This file
├── research.md          # Phase 0 output
├── data-model.md        # Phase 1 output
├── quickstart.md        # Phase 1 output
└── tasks.md             # Phase 2 output (/speckit.tasks)
```

### Source Code (변경 대상)

```text
src/
├── window/
│   └── window_events.c          # [MODIFY] handle_expose에 HUD/keyguide 재출력 추가
├── keyguide/
│   ├── keyguide_render.c        # [MODIFY] keyguide_render()에서 신규 섹션 호출 추가
│   └── keyguide_render_extra.c  # [NEW] Resize/Rotation 섹션 렌더 함수
includes/
├── keyguide.h                   # [MODIFY] 신규 함수 프로토타입 추가 (if needed)
```

## Design Decisions

### D1: Expose 핸들러 수정 방식

**현재 상태**: `handle_expose()`는 `mlx_put_image_to_window()`만 호출 (씬 버퍼만 재출력).

**변경**: expose 핸들러 내에서 `hud.visible` 상태를 확인하고, visible이면 `hud_render()` → `keyguide_render()`를 순차 호출.

**근거**: render_loop에서 HUD를 렌더링하는 기존 패턴과 동일한 순서. expose 이벤트는 `RENDER_DIRTY`를 설정하지 않으므로 render_loop의 HUD 조건(`rendered || dirty`)에 걸리지 않아 별도 호출이 필요.

**구현**:
```c
int	handle_expose(t_render *render)
{
    mlx_put_image_to_window(render->mlx.mlx, render->mlx.win,
        render->mlx.img.img, 0, 0);
    if (render->hud.visible)
    {
        hud_render(render);
        keyguide_render(render);
    }
    return (0);
}
```

### D2: Keyguide 신규 섹션 파일 분리

**제약**: `keyguide_render.c`는 이미 5함수 (Norm 최대). 새 섹션 함수를 추가할 수 없음.

**결정**: `keyguide_render_extra.c` 신규 파일 생성. Resize/Rotation 섹션 렌더 함수를 이 파일에 배치.

**함수 구성**:
- `keyguide_render_resize()` — "Resize:" 헤더 + J/K, N/M 라벨 (2항목)
- `keyguide_render_rotation()` — "Rotation:" 헤더 + U/O, Y/P, ←/→ 라벨 (3항목)
- `keyguide_render_extra()` — 위 두 함수를 호출하는 진입점

### D3: Keyguide 레이아웃 배치

**현재 레이아웃** (위→아래):
1. CONTROLS (헤더)
2. Navigation: ESC, H (2항목)
3. Camera: W/X, A/D, Q/Z, E/C, S (5항목)
4. Objects: TAB, [], R/T, F/G, V/B (5항목)
5. Display: I (1항목)

**추가 레이아웃**:
6. Resize: J/K - Diameter, N/M - Height (2항목)
7. Rotation: U/O - Rot X, Y/P - Rot Y, ←/→ - Rot Z (3항목)

**여백 검증**:
- 현재 사용 높이: 20(top) + 30(헤더) + 16×7(nav+cam) + 8(gap) + 16×6(obj) + 8(gap) + 16×2(display) = 20+30+112+8+96+8+32 = 306px
- 추가 필요: 8(gap) + 16×3(resize) + 8(gap) + 16×4(rotation) = 8+48+8+64 = 128px
- 총: 306 + 128 = 434px > KEYGUIDE_HEIGHT(400px)

**결정**: KEYGUIDE_HEIGHT를 400 → 500으로 변경한다. 434px 필요 + 66px 여백 확보. 기존 섹션 레이아웃은 변경하지 않으며, 컨테이너 높이만 확장한다.

### D4: 불필요한 코드 제거

- `keyguide_state.bg_img`/`bg_data`: keyguide 배경은 `keyguide_render_background()`에서 메인 프레임버퍼에 직접 블렌딩하므로, `bg_img` 별도 이미지는 실제로 사용되지 않음. 단, 기존 init/cleanup 코드가 참조하므로 이번 스코프에서는 제거하지 않음 (Out of Scope: 레이아웃 전면 재설계).

### D5: Norminette 준수 단계

모든 변경 파일에 대해 norminette 검증을 수행:
1. 각 파일 작성/수정 완료 후 `norminette <파일>` 실행
2. 오류 발견 시 즉시 수정 (함수 줄 수, 변수 수, 파라미터 수 등)
3. 최종적으로 변경된 모든 파일에 대해 일괄 norminette 재검증
4. `keyguide_render_extra.c` 신규 파일은 42 헤더 포함, 5함수 이내 확인

## Complexity Tracking

해당 없음 — Constitution 위반 없음.
