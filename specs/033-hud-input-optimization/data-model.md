# Data Model: HUD / Input / Rendering Optimization

**Feature**: 033-hud-input-optimization | **Date**: 2026-02-10

---

## 변경 대상 엔티티

### 1. t_debounce_state_enum (render_debounce.h:27-33)

**현재**:
```c
typedef enum e_debounce_state_enum
{
    DEBOUNCE_IDLE,
    DEBOUNCE_ACTIVE,
    DEBOUNCE_PREVIEW,
    DEBOUNCE_FINAL
}   t_debounce_state_enum;
```

**변경 후**:
```c
typedef enum e_debounce_state_enum
{
    DEBOUNCE_IDLE,
    DEBOUNCE_ACTIVE,
    DEBOUNCE_FINAL,
    DEBOUNCE_COOLDOWN
}   t_debounce_state_enum;
```

| 필드 | 변경 | 사유 |
|------|------|------|
| DEBOUNCE_PREVIEW | 삭제 | 즉시 LQ 프리뷰로 대체 (IDLE→ACTIVE 시 즉시 트리거) |
| DEBOUNCE_COOLDOWN | 추가 | FQ 완료 후 350ms 유예 기간. 연속 탭 병합 |

---

### 2. t_debounce_state (render_debounce.h:44-51)

**현재**:
```c
typedef struct s_debounce_state
{
    t_debounce_state_enum   state;
    t_debounce_timer        timer;
    int                     preview_enabled;
    int                     auto_upgrade;
    int                     cancel_requested;
}   t_debounce_state;
```

**변경 후**:
```c
typedef struct s_debounce_state
{
    t_debounce_state_enum   state;
    t_debounce_timer        timer;
    struct timeval          last_preview_time;
}   t_debounce_state;
```

| 필드 | 변경 | 사유 |
|------|------|------|
| preview_enabled | 삭제 | PREVIEW 상태 제거로 불필요 |
| auto_upgrade | 삭제 | PREVIEW→FINAL 자동 전환 로직 삭제로 불필요 |
| cancel_requested | 삭제 | 싱글 스레드에서 도달 불가능 (research R1) |
| last_preview_time | 추가 | LQ 프리뷰 스로틀링용 (50ms 최소 간격) |

**유효성 규칙**:
- `state`: 반드시 enum 값 중 하나 (IDLE, ACTIVE, FINAL, COOLDOWN)
- `timer`: debounce_timer_start()로 초기화 후 사용
- `last_preview_time`: 0 초기화, gettimeofday()로 갱신

---

### 3. 렌더 플래그 defines (window.h:28-36)

**현재**:
```c
# define RENDER_DIRTY       0x01
# define RENDER_RENDERING   0x02
# define RENDER_LOW_QUALITY 0x04
# define RENDER_SHIFT_HELD  0x08
# define RENDER_SHOW_INFO  0x10
# define RENDER_BVH_DIRTY  0x20
# define RENDER_ENABLE_PIXEL_TIMING  0x40
# define RENDER_ENABLE_METRICS_PRINT 0x80
```

**변경 후**:
```c
# define RENDER_DIRTY       0x01
# define RENDER_RENDERING   0x02
# define RENDER_LOW_QUALITY 0x04
# define RENDER_SHIFT_HELD  0x08
# define RENDER_BVH_DIRTY  0x20
# define RENDER_ENABLE_PIXEL_TIMING  0x40
# define RENDER_ENABLE_METRICS_PRINT 0x80
```

| 필드 | 변경 | 사유 |
|------|------|------|
| RENDER_SHOW_INFO (0x10) | 삭제 | 읽기 0건. HUD가 완전 대체 (research R7) |

---

### 4. 디바운스 설정 defines (render_debounce.h:19-21)

**현재**:
```c
# define DEBOUNCE_DEFAULT_DELAY_MS      150
# define DEBOUNCE_DEFAULT_PREVIEW       1
# define DEBOUNCE_DEFAULT_AUTO_UPGRADE  1
```

**변경 후**:
```c
# define DEBOUNCE_DEFAULT_DELAY_MS          150
# define DEBOUNCE_COOLDOWN_MS               350
# define DEBOUNCE_PREVIEW_MIN_INTERVAL_MS   50
```

| define | 변경 | 사유 |
|--------|------|------|
| DEBOUNCE_DEFAULT_PREVIEW | 삭제 | PREVIEW 상태 제거 |
| DEBOUNCE_DEFAULT_AUTO_UPGRADE | 삭제 | auto_upgrade 로직 제거 |
| DEBOUNCE_COOLDOWN_MS | 추가 | COOLDOWN 상태 지속 시간 (350ms) |
| DEBOUNCE_PREVIEW_MIN_INTERVAL_MS | 추가 | LQ 프리뷰 스로틀 간격 (50ms) |

---

### 5. 키 defines (window_internal.h)

**추가 대상** (Option A 선택 시):

```c
/* macOS */
# define KEY_SEMICOLON    41
# define KEY_QUOTE        39
# define KEY_COMMA        43
# define KEY_PERIOD       47
# define KEY_L            37

/* Linux/X11 */
# define KEY_SEMICOLON    59
# define KEY_QUOTE        39
# define KEY_COMMA        44
# define KEY_PERIOD       46
# define KEY_L            108
```

**제거 대상**:
- `KEY_INSERT`, `KEY_HOME`, `KEY_PGUP`, `KEY_DELETE`, `KEY_END`, `KEY_PGDN` (양 플랫폼): Light 이동 키 교체
- `KEY_LEFT`, `KEY_RIGHT` (양 플랫폼): rotation에서 해방, 다른 사용처 없음

**유지 대상** (이전 계획에서 삭제 예정이었으나 재활용):
- `KEY_I`: RENDER_SHOW_INFO 핸들러만 제거, define은 Obj Rotate X- 에 재활용
- `KEY_J`, `KEY_K`: Resize → Rotate 재배정
- `KEY_Y`, `KEY_U`: Rotate → Resize 재배정

**불필요** (추가하지 않음):
- `KEY_BACKSLASH`, `KEY_SLASH`: 원래 Z rotation용이었으나 P/L로 대체

---

## 상태 전이도

```
        입력(LQ+스로틀)              150ms 무입력           FQ 완료
IDLE ──────────────→ ACTIVE ──────────────→ FINAL ──────────→ COOLDOWN
 ↑                    ↑  ↑                    │                  │ │
 │                    │  └── 입력(timer reset) │                  │ │
 │                    │                        │   입력(LQ+스로틀)│ │
 │                    │   입력(LQ+스로틀)       │                  │ │
 │                    ├────────────────────────┘                  │ │
 │                    └───────────────────────────────────────────┘ │
 │                                                 350ms 무입력    │
 └─────────────────────────────────────────────────────────────────┘
```

---

## 엔티티 간 관계

```
t_render
├── state_flags (int, bit flags) ── RENDER_SHOW_INFO 제거
├── debounce (t_debounce_state)
│   ├── state (t_debounce_state_enum) ── PREVIEW→COOLDOWN 교체
│   ├── timer (t_debounce_timer) ── 변경 없음
│   └── last_preview_time (struct timeval) ── 신규
├── hud (t_hud_state) ── 변경 없음
└── keyguide (t_keyguide_state) ── 변경 없음
```
