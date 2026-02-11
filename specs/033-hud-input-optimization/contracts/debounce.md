# Contract: Debounce State Machine API

**Feature**: 033-hud-input-optimization | **Date**: 2026-02-10

---

## 변경되는 함수

### debounce_init (render_debounce.c)

기존 시그니처 유지. 내부 초기화 값 변경.

```c
void    debounce_init(t_debounce_state *state);
```

**변경 사항**:
- `preview_enabled`, `auto_upgrade`, `cancel_requested` 초기화 제거
- `last_preview_time` 0 초기화 추가
- state를 `DEBOUNCE_IDLE`로 설정 (변경 없음)

---

### debounce_on_input (render_debounce.c)

기존 시그니처 유지. 내부 로직 변경.

```c
void    debounce_on_input(t_debounce_state *state, t_render *render);
```

**변경 사항**:
- IDLE → ACTIVE: 즉시 LQ 프리뷰 트리거 (스로틀 적용)
  ```c
  render_set_flag(render, RENDER_LOW_QUALITY | RENDER_DIRTY);
  ```
- ACTIVE → timer reset: LQ 프리뷰 트리거 (스로틀 적용)
- FINAL → ACTIVE: LQ 프리뷰 + 타이머 리셋 (cancel_requested 제거)
- COOLDOWN → ACTIVE: LQ 프리뷰 + 타이머 리셋 (신규)
- PREVIEW 분기 삭제

**스로틀 조건**: `elapsed_ms(last_preview_time) >= DEBOUNCE_PREVIEW_MIN_INTERVAL_MS`

---

### debounce_update (render_debounce.c)

기존 시그니처 유지. 내부 상태 전이 변경.

```c
void    debounce_update(t_debounce_state *state, t_render *render);
```

**변경 사항**:
- ACTIVE + 타이머 만료 → FINAL (PREVIEW 경유 없이 직행)
- FINAL + `!RENDER_DIRTY` → COOLDOWN + 쿨다운 타이머 시작
- COOLDOWN + 타이머 만료 → IDLE
- PREVIEW 분기 삭제

---

## 신규 함수

### debounce_handle_cooldown (render_debounce.c)

COOLDOWN 상태 처리 헬퍼. `debounce_handle_preview()` 제거로 확보된 슬롯(5→4)에 배치.

```c
static void    debounce_handle_cooldown(t_debounce_state *state);
```

**동작**:
- 쿨다운 타이머 만료 확인
- 만료 시 → DEBOUNCE_IDLE 전환 + 타이머 정지

---

### debounce_check_preview_throttle (render_debounce_timer.c)

LQ 프리뷰 스로틀 판정 헬퍼. `debounce_cancel()` 제거로 확보된 슬롯(5→4)에 배치.

```c
int    debounce_check_preview_throttle(t_debounce_state *state);
```

**동작**:
- `last_preview_time`으로부터 경과 시간 계산
- `>= DEBOUNCE_PREVIEW_MIN_INTERVAL_MS` 이면 1 반환 (LQ 허용)
- 미달이면 0 반환 (LQ 스킵)

---

## 제거되는 함수

### debounce_cancel (render_debounce.h:63)

```c
void    debounce_cancel(t_debounce_state *state);
```

**사유**: cancel_requested 경로 도달 불가능. 호출처 확인 필요 (있으면 호출 제거).

---

## 삭제되는 내부 함수

### debounce_handle_preview (render_debounce.c)

```c
static void    debounce_handle_preview(t_debounce_state *state, t_render *render);
```

**사유**: PREVIEW 상태 제거.
