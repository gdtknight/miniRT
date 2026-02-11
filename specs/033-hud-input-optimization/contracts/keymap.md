# Contract: Keymap Changes

**Feature**: 033-hud-input-optimization | **Date**: 2026-02-10

---

## Option A (권장): 구두점 키 기반 Light 이동 + 키보드 그리드 Resize/Rotate

### 새 키 바인딩

| 기능 | 현재 키 | 새 키 | 사유 |
|------|---------|-------|------|
| Light X- / X+ | Insert / Delete | `[` / `]` | macOS 호환 |
| Light Y- / Y+ | Home / End | `;` / `'` | macOS 호환 |
| Light Z- / Z+ | PgUp / PgDn | `,` / `.` | macOS 호환 |
| Obj Resize X- / X+ | J / K | `Y` / `U` | 키보드 그리드 레이아웃 |
| Obj Rotate X- / X+ | U / O | `I` / `J` | 키보드 그리드 레이아웃 |
| Obj Rotate Y- / Y+ | Y / P | `O` / `K` | 키보드 그리드 레이아웃 |
| Obj Rotate Z- / Z+ | Left / Right | `P` / `L` | 화살표 키 해방 + 그리드 |
| HUD Page Up/Down | (미구현) | `SHIFT+,` / `SHIFT+.` | 신규 |
| Object Selection | `[` / `]` | `TAB` / `SHIFT+TAB` | Light 이동으로 재할당 |

### 키보드 레이아웃 시각화

```
Resize:  Y U  (X축)    Rotate:  I J  (X축)
         N M  (Y축)             O K  (Y축)
                                P L  (Z축)
```

### 변경 대상 함수

#### handle_transform_keys (window_key_handlers.c)

```c
// 기존: Light 이동 키
if (keycode == KEY_INSERT || keycode == KEY_DELETE || ...)
    handle_light_move(render, keycode);

// 변경: 새 키로 교체
if (keycode == KEY_BRACKET_LEFT || keycode == KEY_BRACKET_RIGHT
    || keycode == KEY_SEMICOLON || keycode == KEY_QUOTE
    || keycode == KEY_COMMA || keycode == KEY_PERIOD)
    handle_light_move(render, keycode);
```

```c
// 기존: Resize 키
if (keycode == KEY_J || keycode == KEY_K || ...)
    handle_object_resize(render, keycode);

// 변경: Y,U,N,M 로 교체
if (keycode == KEY_Y || keycode == KEY_U
    || keycode == KEY_N || keycode == KEY_M)
    handle_object_resize(render, keycode);
```

#### handle_light_move (window_objects.c)

```c
// 기존: Insert/Delete/Home/End/PgUp/PgDn 키코드 매핑
// 변경: [/]/;/'/,/. 키코드 매핑
```

#### handle_object_resize (window_objects.c)

```c
// 기존: J/K (X), N/M (Y) 키코드 매핑
// 변경: Y/U (X), N/M (Y) 키코드 매핑
```

#### handle_object_rotate (window_rotate.c)

```c
// 기존: U/O (X), Y/P (Y), LEFT/RIGHT (Z) 키코드 매핑
// 변경: I/J (X), O/K (Y), P/L (Z) 키코드 매핑
```

#### handle_hud_keys (window_key_handlers.c)

```c
// 기존: HUD 페이지 전환 구현 없음 (Up/Down은 다른 용도?)
// 변경: SHIFT+,/SHIFT+. → HUD 페이지 전환
```

#### handle_object_selection (window_selection.c)

```c
// 기존: [/] 키로 오브젝트 선택
// 변경: TAB/SHIFT+TAB으로 오브젝트 선택 (이미 부분 지원)
```

### 신규 키 defines (window_internal.h)

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

### 제거할 키 defines

```c
// 양 플랫폼 모두
# define KEY_INSERT   (삭제)
# define KEY_HOME     (삭제)
# define KEY_PGUP     (삭제)
# define KEY_DELETE    (삭제)
# define KEY_END      (삭제)
# define KEY_PGDN     (삭제)
# define KEY_LEFT     (삭제 — rotation에서 해방, 다른 사용처 없음)
# define KEY_RIGHT    (삭제 — rotation에서 해방, 다른 사용처 없음)
```

### 유지할 키 defines (이전 계획에서 삭제 예정이었으나 재활용)

```c
// KEY_I — RENDER_SHOW_INFO 핸들러만 제거, define은 rotation X-에 재활용
// KEY_J, KEY_K — resize에서 rotation으로 재배정
// KEY_Y, KEY_U — rotation에서 resize로 재배정
// KEY_O, KEY_P — rotation 축 변경 (X+→Y-, Y+→Z-)
```

### 불필요해진 키 defines (추가하지 않음)

```c
// KEY_BACKSLASH, KEY_SLASH — 원래 Z rotation용이었으나 P/L로 대체. 추가 불필요.
```

---

## 키가이드 업데이트

### 추가할 텍스트

```
Light:
[ ] - Move X
; ' - Move Y
, . - Move Z

Resize:
Y U - Size X
N M - Size Y

Rotate:
I J - Rot X
O K - Rot Y
P L - Rot Z

HUD:
< > - HUD Page
```

### 제거할 텍스트

```
Display:
I - Info
```

### 변경할 텍스트

```
// 기존 (keyguide_render.c:85)
[] - Select

// 변경
TAB - Select
```

```
// 기존 (keyguide_render.c:82) — 이미 TAB 표시
TAB - Next

// 변경: "TAB - Next"와 "TAB - Select" 통합 검토 필요
```

### UI 텍스트 충돌 확인

- `src/hud/` 내 오브젝트 목록(`hud_objects.c`, `hud_navigation.c`)은 `[]` 키 참조 UI 텍스트 없음 (코드 내부 로직만)
- `src/keyguide/keyguide_render.c:82`에 `"TAB - Next"`, `:85`에 `"[] - Select"` — 두 줄 모두 업데이트 대상
- 키가이드 외 다른 안내 텍스트에 `[]` 참조 없음 확인 완료
- Resize/Rotate 키가이드: 기존 텍스트 위치 확인 필요 (keyguide_render.c 내 J,K,N,M 및 U,O,Y,P 관련 텍스트)

---

## Option B / C (미선택 시 참고)

Option B는 Shift 조합 의존으로 macOS MiniLibX 미검증 위험. Option C는 모드 토글 인지 부하 증가. 키맵 옵션은 구현 시 최종 결정.
