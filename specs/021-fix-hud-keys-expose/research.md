# Research: Fix HUD Key Guide & Expose Restore

**Date**: 2026-01-28

## R1: Expose 핸들러에서 HUD 재출력 방식

**Decision**: expose 핸들러에서 `hud.visible` 체크 후 `hud_render()` → `keyguide_render()` 순차 호출.

**Rationale**: 기존 render_loop과 동일한 호출 순서. expose 이벤트는 RENDER_DIRTY를 설정하지 않으므로 render_loop의 HUD 조건에 도달하지 않음. 따라서 expose 핸들러에서 직접 호출이 필요.

**Alternatives considered**:
- expose에서 `hud.dirty = 1` 설정 → render_loop에 위임: render_loop이 다음 프레임에서 처리하지만, RENDER_DIRTY가 없으면 rendered=0이고 dirty만으로 HUD를 그려도 씬 버퍼가 이미 expose에서 put됨. 타이밍 문제 가능. 기각.
- expose에서 RENDER_DIRTY 설정: 불필요한 레이트레이싱 재실행. 기각 (FR-006 위반).

## R2: Keyguide 높이 조정

**Decision**: KEYGUIDE_HEIGHT를 400 → 500으로 변경.

**Rationale**: 현재 레이아웃 306px + 신규 섹션 128px = 434px. 400px 초과. 500px로 여유 확보.

**Alternatives considered**:
- LINE_HEIGHT를 14로 축소: 가독성 저하. 기각.
- 스크롤 구현: Over-engineering, Out of Scope. 기각.

## R3: 신규 파일 분리 (Norm 제약)

**Decision**: `keyguide_render_extra.c` 신규 파일에 3함수 배치.

**Rationale**: `keyguide_render.c`가 이미 5함수(Norm 최대). `keyguide_render_extra.c`에 `keyguide_render_extra()`, `keyguide_render_resize()`, `keyguide_render_rotation()`을 배치.

**Alternatives considered**:
- `keyguide_render.c`에서 기존 함수를 다른 파일로 이동: 기존 코드 변경 최소화 원칙에 위배. 기각.
