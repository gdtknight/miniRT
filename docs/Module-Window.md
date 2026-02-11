# Module: Window

MiniLibX 윈도우 초기화, 이벤트 핸들링, 키 바인딩을 담당하는 모듈입니다.

---

## 소스 파일

| 파일 | 역할 |
|------|------|
| `window_init.c` | 윈도우/이미지/UI 초기화, 훅 등록 |
| `window_lifecycle.c` | `close_window` — 리소스 정리 및 종료 |
| `window_events.c` | `handle_key`, `handle_key_release` — 키 이벤트 분기 |
| `window_key_handlers.c` | HUD, 오브젝트 선택, 변환 키 처리 |
| `window_loop.c` | `render_loop` — 프레임 루프 콜백 |
| `window_camera.c` | 카메라 이동/회전/리셋 |
| `window_objects.c` | 오브젝트 이동 처리 |
| `window_selection.c` | 오브젝트 선택 (이전/다음 순환) |
| `window_resize.c` | 오브젝트 리사이즈 처리 |
| `window_rotate.c` | 오브젝트 회전 처리 |
| `mlx_context.c` | MiniLibX 초기화 (`mlx_init`, `mlx_new_window`) |
| `mlx_pixel.c` | 픽셀 쓰기 (`put_pixel_to_buffer`) |
| `mlx_pixel_codec.c` | 픽셀 형식 변환 |
| `render_flags_set.c` | 렌더 상태 플래그 헬퍼 |

---

## MiniLibX 초기화

```
init_window(scene)
 ├── malloc(t_render)
 ├── init_mlx()
 │    ├── mlx_init()
 │    ├── mlx_new_window(1440, 900, "miniRT")
 │    ├── mlx_new_image()
 │    └── mlx_get_data_addr()
 ├── init_render_state()
 │    └── pixel_timing_init()
 ├── init_ui_components()
 │    ├── hud_init()
 │    ├── keyguide_init()
 │    └── hud_calculate_total_pages()
 └── register_hooks()
      ├── mlx_hook(close_window)       // 윈도우 닫기
      ├── mlx_hook(handle_key)         // 키 누름
      ├── mlx_hook(handle_key_release) // 키 해제
      └── mlx_loop_hook(render_loop)   // 프레임 루프
```

해상도: `1440 x 900` (고정)

---

## 키 바인딩 전체 목록

### 카메라 이동
| 키 | 동작 | 설명 |
|----|------|------|
| W | 전진 | 시야 방향으로 이동 |
| X | 후진 | 시야 반대 방향으로 이동 |
| A | 좌 | 시야 직교 방향 좌측 |
| D | 우 | 시야 직교 방향 우측 |
| Q | 상승 | 월드 Y축 상향 |
| Z | 하강 | 월드 Y축 하향 |

### 카메라 회전
| 키 | 동작 |
|----|------|
| E | 피치 위 (고개 올림) |
| C | 피치 아래 (고개 내림) |

### 카메라 리셋
| 키 | 동작 |
|----|------|
| S | 초기 위치 및 방향 복원 |

### 오브젝트 선택
| 키 | 동작 |
|----|------|
| TAB | 다음 오브젝트 |

### 오브젝트 이동
| 키 | 축 | 방향 |
|----|----|------|
| R | X | - |
| T | X | + |
| F | Y | - |
| G | Y | + |
| V | Z | - |
| B | Z | + |

### 오브젝트 리사이즈
| 키 | 대상 | 방향 |
|----|------|------|
| Y | 반지름 | - |
| U | 반지름 | + |
| N | 높이 (원기둥) | - |
| M | 높이 (원기둥) | + |

### 오브젝트 회전
| 키 | 축 | 방향 |
|----|------|------|
| I / J | X | - / + |
| O / K | Y | - / + |
| P / L | Z | - / + |

### 광원 이동
| 키 | 축 | 방향 |
|----|----|------|
| \[ | X | - |
| \] | X | + |
| ; | Y | - |
| ' | Y | + |
| , | Z | - |
| . | Z | + |

### UI
| 키 | 동작 |
|----|------|
| H | HUD 표시/숨김 |
| Up / Down | HUD 페이지 이동 |
| ESC | 프로그램 종료 |

---

## 이벤트 처리 흐름

```
handle_key(keycode, render)
 ├── ESC → close_window()
 ├── H/TAB/Up/Down → handle_hud_keys()
 ├── W/X/A/D/Q/Z/E/C/S → handle_camera_keys()
 └── R/T/F/G/V/B/[/]/;/'/,/./Y/U/N/M/I/J/O/K/P/L → handle_transform_keys()
      ├── R/T/F/G/V/B → handle_object_move()
      ├── [/]/;/'/,/. → handle_light_move()
      ├── Y/U/N/M → handle_object_resize()
      └── I/J/O/K/P/L → handle_object_rotate()

handle_key_release(keycode, render)
 └── (no-op)
```

키 누름 시 디바운스 FSM이 LQ preview를 트리거하고 (50ms throttle), 150ms 입력 없으면 full quality 렌더링을 수행합니다.

---

## 리소스 정리

```
close_window(render)
 ├── pixel_timing_cleanup()
 ├── keyguide_cleanup()
 ├── hud_cleanup()
 ├── mlx_destroy_image()
 ├── cleanup_scene()
 │    ├── bvh_destroy()
 │    └── free(scene)
 └── exit(0)
```
