# Research: HUD / Input / Rendering Optimization

**Feature**: 033-hud-input-optimization | **Date**: 2026-02-10

---

## R1. MiniLibX 이벤트 루프 모델

**Decision**: 싱글 스레드 이벤트 루프. 렌더 중 이벤트 처리 불가.

**Rationale**: `mlx_loop()` → `render_loop()` (expose callback)이 단일 스레드에서 실행된다. `render_scene_to_buffer()`는 블로킹 호출이며, 실행 중 `handle_key()` 이벤트는 큐에 대기 후 다음 루프 반복에서 처리된다. 따라서:
- `RENDER_RENDERING` 플래그는 `handle_key()` 시점에 항상 0
- `cancel_requested` 설정 경로(`render_debounce.c:57`)는 도달 불가능
- 렌더 중 키 입력을 중단하는 것은 구조적으로 불가능

**Alternatives considered**:
- 멀티스레드 렌더링: MiniLibX API가 thread-safe하지 않으며, 42 Norm에서 pthread 사용 금지
- Non-blocking partial render: 프레임 분할 렌더링 가능하나 복잡도 대폭 증가. 본 스펙 범위 외.

---

## R2. 디바운스 상태 머신 설계 패턴

**Decision**: 4-state FSM (IDLE → ACTIVE → FINAL → COOLDOWN) + 즉시 LQ 프리뷰 + 스로틀

**Rationale**: 5가지 대안을 비교 분석:

| 방안 | 설명 | 장점 | 단점 |
|------|------|------|------|
| 현재 | IDLE→ACTIVE→PREVIEW→FINAL | key-hold 정상 동작 | 이산 탭마다 이중 렌더, 150ms 피드백 지연 |
| 순수 지연(800ms) | LQ 없이 800ms 대기 후 FQ | 완벽한 입력 병합 | 800ms 무반응, UX 저하 |
| 550ms 단일 지연 | LQ 없이 550ms 대기 | 대부분 탭 병합 | 단일 입력도 550ms 지연 |
| 150ms + 즉시 LQ | 즉시 LQ + 150ms 후 FQ | 빠른 피드백 | FQ 직후 탭 병합 불가 |
| **150ms + LQ + COOLDOWN** | 즉시 LQ + 150ms→FQ + 350ms 쿨다운 | 빠른 피드백 + FQ 직후 탭 병합 | 상태 1개 추가 |

COOLDOWN 상태(350ms)는 FQ 완료 후 IDLE 전환 전 유예 기간을 두어, 사람의 의도적 재탭 간격(300-400ms)을 포착한다.

**Alternatives considered**:
- FQ skip counter: 구현 간단하나 예측 불가능한 스킵 동작
- Exponential backoff: 과도한 복잡성

---

## R3. LQ 프리뷰 스로틀링

**Decision**: `DEBOUNCE_PREVIEW_MIN_INTERVAL_MS = 50`으로 최소 간격 제어

**Rationale**: 키 반복 간격(~30ms)이 LQ 렌더 시간(30-50ms)과 겹치면 프레임 폭주 가능. `gettimeofday()`로 `last_preview_time`을 추적하고, 50ms 미만이면 LQ 스킵. 이미 프로젝트에서 `gettimeofday()`를 사용 중이므로 함수 추가 없음.

**Alternatives considered**:
- Frame counter 기반: 프레임 시간이 가변적이므로 부정확
- Fixed interval timer: 불필요한 복잡성

---

## R4. 42 Norm 함수/파일 제한 대응

**Decision**: 신규 파일 불필요. 기존 2파일 내 재배치로 해결.

**Rationale**: 현재 `render_debounce.c`에 5개 함수:
1. `debounce_init()`
2. `debounce_on_input()`
3. `debounce_handle_active()` (static)
4. `debounce_handle_preview()` (static)
5. `debounce_update()`

현재 `render_debounce_timer.c`에 5개 함수:
1. `debounce_cancel()`
2. `debounce_timer_start()`
3. `debounce_timer_reset()`
4. `debounce_timer_stop()`
5. `debounce_timer_expired()`

변경 후 함수 배치:
- `render_debounce.c` (5개): debounce_init, debounce_on_input, debounce_handle_active, debounce_handle_cooldown(static), debounce_update
  - PREVIEW 제거(-1) + COOLDOWN 추가(+1) = 5. Norm 준수.
- `render_debounce_timer.c` (5개): debounce_check_preview_throttle, debounce_timer_start, debounce_timer_reset, debounce_timer_stop, debounce_timer_expired
  - cancel 제거(-1) + 스로틀 헬퍼 추가(+1) = 5. Norm 준수.

**Alternatives considered**:
- `render_debounce_cooldown.c` 신규 파일: 가능하지만, 기존 파일에서 제거되는 함수 슬롯을 활용하면 불필요
- 기존 함수에 COOLDOWN 로직 병합: 25줄/함수 제한 초과

---

## R5. macOS 키코드 호환성 + 키보드 그리드 레이아웃

**Decision**: Option A (구두점 키 `[ ] ; ' , .` Light용) + 키보드 그리드 레이아웃 (Resize `Y,U/N,M`, Rotate `I,J/O,K/P,L`). `\`/`/` 키는 불필요 (Z rotation이 P/L로 대체됨).

**Rationale**: macOS 랩탑에 Insert/Delete/Home/End/PgUp/PgDn 물리 키 없음. MiniLibX X11 (Linux)에서는 keycode가 하드웨어 기반이므로 구두점 키와 영문 키 모두 안정적. macOS MiniLibX (AppKit)에서는:
- 기본 영문 키: 정상 동작 (keycode = 가상 키코드)
- Shift 조합: `SHIFT+;`는 keycode 0x29(`;`) + modifier flag. MiniLibX의 `key_hook`이 modifier를 전달하는지 플랫폼별 차이 가능.
- Resize/Rotate 키 (Y,U,I,J,O,K,P,L): 모두 기본 영문 키 → 양 플랫폼 안정적.

**키보드 그리드 레이아웃 근거**: QWERTY 자판에서 Resize(Y,U/N,M)와 Rotate(I,J/O,K/P,L)가 2열 그리드를 형성하여 직관적 조작 가능. LEFT/RIGHT 화살표 키에서 해방.

**KEY_L 추가**: macOS 가상 키코드 37, Linux X11 KeySym 108. KEY_I는 RENDER_SHOW_INFO 데드 코드 제거 후 Rotate X- 로 재활용.

**위험 지표**: Option B의 Shift 조합 의존은 macOS MiniLibX 미검증 위험이 있으나, HUD 페이지 전환(SHIFT+,/.)만 Shift에 의존하므로 영향 범위가 제한적.

**Alternatives considered**:
- `\`/`/` Z rotation: 키보드 그리드 P/L로 대체하여 불필요
- Option C (모드 토글): 키 충돌 없으나 인지 부하 증가
- Fn+화살표 조합: MiniLibX가 Fn 키 이벤트를 전달하지 않을 가능성

---

## R6. cancel_requested / preview_enabled / auto_upgrade 제거 영향

**Decision**: 세 필드 모두 안전하게 제거 가능

**Rationale**:
- `cancel_requested`: R1에서 확인된 대로 도달 불가능 경로. `render_low_quality()`와 `render_scene_to_buffer()`의 cancel 체크도 사문 코드.
- `preview_enabled`: DEBOUNCE_DEFAULT_PREVIEW=1로 고정. PREVIEW 상태 자체를 제거하므로 불필요.
- `auto_upgrade`: DEBOUNCE_DEFAULT_AUTO_UPGRADE=1로 고정. PREVIEW→FINAL 자동 전환 로직 삭제와 함께 불필요.

`t_debounce_state` struct에서 3개 필드 제거 → `last_preview_time` 1개 추가. 순 2개 필드 감소.

---

## R7. RENDER_SHOW_INFO 제거 영향

**Decision**: 안전하게 제거 가능. 0x10 비트 위치 재사용 불필요.

**Rationale**: `src/` 전체에서 `RENDER_SHOW_INFO` 읽기가 0건. HUD (`H` 키, `hud.visible`)가 info overlay를 완전 대체. 비트 위치 0x10은 비워두면 됨 (다른 플래그와 충돌 없음).

---

## R8. hud_render_background 제거 영향

**Decision**: 안전하게 제거 가능

**Rationale**: `hud_render_background()` (hud_text.c:60-70)와 `hud_render_background_row()` (hud_text.c:32-53)는 호출 0건. `hud_render()` (hud_render.c:49-55)는 `hud_render_content()`만 호출. 헤더 선언(hud_text.h:19-20, hud.h:294)만 존재.
