# 심화: HUD와 키가이드

> 이 문서는 누구를 위한 것인가
> - 화면 상단에 뜨는 정보창과 오른쪽의 키 가이드가 어떻게 구성되는지 알고 싶은 사람
> - 객체 선택 하이라이트가 어디서 결정되는지 궁금한 사람

## 1. HUD vs 키 가이드

- **HUD (`src/interact/hud/`)**: 카메라, 주변광, 광원, 객체 목록, 퍼포먼스 정보. 좌측 상단.
- **Keyguide (`src/interact/keyguide/`)**: 조작 키 설명. 우측 상단.

둘 다 MLX의 `mlx_string_put` 으로 렌더됩니다. 이미지 버퍼에 그리지 않고 창에 직접 텍스트를 올리는 방식이라, 프레임 버퍼를 `put_image_to_window` 로 올린 "다음"에 호출되어야 합니다 (`render_loop.c:82`).

## 2. 진입점

### HUD

`src/interact/hud/hud_render.c:47`:

```c
void hud_render(t_render *render)
{
    if (!render->hud.visible) return;
    hud_render_content(render);
    render->hud.dirty = 0;
}
```

`hud_render_content()` (`hud_render.c:24`) 가 섹션 헬퍼들을 y 좌표 누적 방식으로 호출합니다:

```c
y = HUD_MARGIN_Y + 20;
hud_render_camera(render, &y);
hud_render_camera_fov(render, &y);
y += HUD_LINE_HEIGHT / 2;
hud_render_ambient(render, &y);
y += HUD_LINE_HEIGHT / 2;
hud_render_light(render, &y);
hud_render_light_bright(render, &y);
y += HUD_LINE_HEIGHT;
hud_render_objects(render, &y);
y += HUD_LINE_HEIGHT;
hud_render_performance(render, &y);
```

### Keyguide

`src/interact/keyguide/keyguide_render.c:104`:

```c
if (!render->keyguide.visible || !render->keyguide.dirty) return;
keyguide_render_content(render);
...
keyguide_render_objects_section(render, &y);
keyguide_render_extra(render, &y);
render->keyguide.dirty = 0;
```

dirty 플래그가 꺼져 있으면 아무 것도 하지 않아 중복 렌더를 방지.

## 3. HUD 페이지네이션 — 객체 목록

씬에 객체가 많을 때 한 화면에 다 띄울 수 없으므로 페이지를 나눕니다. `src/interact/hud/hud_objects.c:74`:

```c
render->hud.total_pages = hud_calculate_total_pages(render->scene);
if (render->hud.current_page >= render->hud.total_pages)
    render->hud.current_page = 0;
render_object_header(render, y);       // "--- Objects (Page 2/5) ---"
start_idx = current_page * HUD_OBJECTS_PER_PAGE;
end_idx   = min(start_idx + HUD_OBJECTS_PER_PAGE, total_objects);
for (i = start_idx; i < end_idx; i++)
    render_object_by_index(render, i, y);
```

`UP`/`DOWN` 키가 `hud_change_page()` 를 통해 `current_page` 를 증감시킴 (`input_dispatch.c:125`).

## 4. 선택 객체 하이라이트

`render_object_by_index()` (`hud_objects.c:55`) 는 전역 인덱스를 씬 내 타입별 인덱스로 변환(`hud_get_selection_from_global`)해 현재 선택과 비교.

```c
color = HUD_COLOR_TEXT;
if (sel.index == render->selection.index
    && sel.type  == render->selection.type)
    color = HUD_COLOR_HIGHLIGHT;
hud_render_object_entry(render, g_idx, y, color);
```

선택은 `TAB` 키로 순환 (`hud_select_next`). 객체 이동·리사이즈·회전 키는 이 selection 을 기준으로 적용됩니다.

## 5. 키 가이드 섹션 구성

`keyguide_render.c` 는 섹션별 헬퍼를 엮어 출력:

```
CONTROLS
Navigation:
  ESC - Exit
  H   - Toggle HUD
Camera:
  W/X - Fwd/Back
  A/D - Left/Right
  Q/Z - Up/Down
  E/C - Pitch
  S   - Reset
  1/3 - Yaw
Objects:
  TAB - Select
  R/T - X axis
  F/G - Y axis
  V/B - Z axis
...
```

좌표는 `render->keyguide.x`, `y` 에서 `KEYGUIDE_LINE_HEIGHT` 씩 증가시키며 `mlx_string_put` 호출. 색상은 섹션 제목(HEADING)과 본문(TEXT) 두 가지.

긴 섹션은 `keyguide_render_extra()` 로 분할되어 있습니다 — 5-function/파일 제한(42 Norm) 때문에 파일을 나눈 결과입니다.

## 6. 더티 플래그 협업

```
키 입력 ──► hud_mark_dirty / debounce_on_input
            │
            ▼
    render_loop 에서 rendered=1 혹은 hud.dirty
            │
            ▼
    mlx_put_image_to_window (프레임 버퍼 올림)
            │
            ▼
    hud_render()       → dirty=0
    keyguide_render()  → dirty=0
```

프레임이 안 바뀌는데 HUD만 바뀌는 경우 (예: TAB로 선택 변경)도 `rendered=0` 이지만 `hud.dirty=1` 이라 `mlx_put_image_to_window` 를 한 번 더 호출해 이전 HUD를 지운 뒤 다시 그립니다 (`render_loop.c:85`).

## 7. 성능 정보 표시

`hud_render_performance()` 는 `t_metrics` 에서 FPS, rays_traced, intersect_tests 등을 읽어 문자열로 포맷합니다. metrics 수집은 렌더 루프에서 틱됩니다. 자세한 내용은 [심화: 메트릭스와 디버그](심화-메트릭스와-디버그).

## 관련 문서

- [심화: 이벤트와 입력 처리](심화-이벤트와-입력처리)
- [인터랙티브 컨트롤](인터랙티브-컨트롤)
- [모듈 디스플레이·입력](모듈-디스플레이-입력)
