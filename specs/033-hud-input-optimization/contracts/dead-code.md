# Contract: Dead Code Removal

**Feature**: 033-hud-input-optimization | **Date**: 2026-02-10

---

## 1. KEY_I 핸들러 / RENDER_SHOW_INFO 제거

### 제거 대상

| 파일 | 위치 | 내용 |
|------|------|------|
| `window_events.c` | 39-43 | `KEY_I` 핸들러 분기 (`else if (keycode == KEY_I)`) |
| `window.h` | 33 | `# define RENDER_SHOW_INFO 0x10` |
| `keyguide_render.c` | 96-101 | `"Display:"` 섹션 헤더 + `"I - Info"` 텍스트 |

### 영향 분석

- `RENDER_SHOW_INFO`: src/ 전체에서 읽기 0건. 안전.
- `KEY_I` define: **유지** — Obj Rotate X- 키로 재활용됨. RENDER_SHOW_INFO 핸들러(window_events.c)만 제거.
- 키가이드 `"Display:"` 섹션: I 키 외 내용 없음. 섹션 전체 제거.

---

## 2. hud_render_background 제거

### 제거 대상

| 파일 | 위치 | 내용 |
|------|------|------|
| `hud_text.c` | 32-53 | `hud_render_background_row()` 함수 |
| `hud_text.c` | 60-70 | `hud_render_background()` 함수 |
| `hud_text.h` | 19-20 | 함수 선언 2개 |
| `hud.h` | 294 | `hud_render_background()` 중복 선언 |

### 영향 분석

- 호출 0건. `hud_render()` → `hud_render_content()`만 호출. 안전.

---

## 3. cancel_requested 관련 사문 코드

### 제거 대상

| 파일 | 위치 | 내용 |
|------|------|------|
| `render_debounce.h` | 50 | `int cancel_requested` 필드 |
| `render_debounce.h` | 63 | `debounce_cancel()` 함수 선언 |
| `render_debounce.c` | 57 | `state->cancel_requested = 1` 설정 |
| `render_debounce.c` | debounce_init 내 | `cancel_requested = 0` 초기화 |
| `render_debounce_timer.c` | 23-26 | `debounce_cancel()` 함수 정의 |
| `window_loop.c` | 52-57 | `cancel_requested` 체크 + `debounce_cancel()` 호출 + `RENDER_DIRTY` 재설정 |
| `render.c` | ~119, ~159 | cancel 체크 (`if (state->cancel_requested)`) |

### 영향 분석

- 싱글 스레드 MiniLibX에서 RENDER_RENDERING이 handle_key 시 항상 0. cancel_requested 설정 경로 도달 불가. 안전.
- `debounce_cancel()` 호출처: `window_loop.c:54` (`execute_render_pass` 내부). 동일 사문 코드 경로이므로 함께 제거.
- `render_debounce_timer.c`에서 `debounce_cancel()` 제거 시 5함수→4함수. Norm 준수.

---

## 4. preview_enabled / auto_upgrade

### 제거 대상

| 파일 | 위치 | 내용 |
|------|------|------|
| `render_debounce.h` | 20-21 | `DEBOUNCE_DEFAULT_PREVIEW`, `DEBOUNCE_DEFAULT_AUTO_UPGRADE` defines |
| `render_debounce.h` | 48-49 | `int preview_enabled`, `int auto_upgrade` 필드 |
| `render_debounce.c` | debounce_init 내 | 필드 초기화 |
| `render_debounce.c` | debounce_handle_active 내 | `preview_enabled` 분기 |
| `render_debounce.c` | debounce_handle_preview 내 | `auto_upgrade` 분기 |

### 영향 분석

- 두 값 모두 1로 고정 초기화. 외부 설정 경로 없음. PREVIEW 상태 제거와 함께 안전 삭제.

---

## 5. pixel_timing 무조건 실행

### 변경 대상 (삭제가 아닌 게이트 추가)

| 파일 | 위치 | 내용 |
|------|------|------|
| `render.c` | ~155 | `pixel_timing_reset()` → 조건부 |
| `render.c` | ~169-170 | `pixel_timing_calculate_stats()`, `pixel_timing_print_stats()` → 조건부 |

### 변경 내용

```c
if (render_has_flag(render, RENDER_ENABLE_PIXEL_TIMING))
    pixel_timing_reset(&render->pixel_timing);
// ... render loop ...
if (render_has_flag(render, RENDER_ENABLE_PIXEL_TIMING))
{
    pixel_timing_calculate_stats(&render->pixel_timing);
    pixel_timing_print_stats(&render->pixel_timing);
}
```
