# Contract: Keymap Extensions

## 신규 키 정의

| Key | macOS Code | Linux Code | 기능 |
|-----|-----------|------------|------|
| `1` | 18 | 49 | Camera yaw left (반시계 5도) |
| `3` | 20 | 51 | Camera yaw right (시계 5도) |
| `=` | 24 | 61 | 다음 광원 선택 (순환) |

## 핸들러 매핑

### Camera Keys (window_camera.c)

```c
/* 신규 함수 */
void  handle_camera_yaw(t_render *render, int keycode);
```

- KEY_1: Y축 기준 반시계 5도 회전
- KEY_3: Y축 기준 시계 5도 회전
- 기존 `rotate_dir()` 함수 재사용 (axis = (0,1,0))

### Light Selection (window_key_handlers.c)

```c
/* handle_hud_keys 또는 별도 핸들러에서 처리 */
if (keycode == KEY_EQUAL)
    select_next_light(render);
```

### Keyguide 추가 표시

Camera 섹션에 추가:
```
1/3 - Yaw
```

Light 섹션에 추가:
```
= - Next Light
```
