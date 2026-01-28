# Data Model: Fix HUD Key Guide & Expose Restore

**Date**: 2026-01-28

## Modified Entities

### t_keyguide_state (기존, 변경 없음)

| Field | Type | Description |
|-------|------|-------------|
| bg_img | void* | 배경 이미지 버퍼 |
| bg_data | char* | 픽셀 데이터 |
| bpp | int | Bytes per pixel |
| size_line | int | Scanline 크기 |
| endian | int | Endianness |
| visible | int | 표시 여부 (0/1) |
| x | int | X 좌표 |
| y | int | Y 좌표 |

구조체 변경 없음. `visible` 플래그를 expose 핸들러에서 읽기만 함.

### Constants 변경

| Constant | Before | After | File |
|----------|--------|-------|------|
| KEYGUIDE_HEIGHT | 400 | 500 | includes/keyguide.h |

## New Functions

| Function | File | Description |
|----------|------|-------------|
| `keyguide_render_extra(t_render*, int*)` | keyguide_render_extra.c | Resize+Rotation 섹션 진입점 |
| `keyguide_render_resize(t_render*, int*)` | keyguide_render_extra.c | Resize 섹션 렌더 |
| `keyguide_render_rotation(t_render*, int*)` | keyguide_render_extra.c | Rotation 섹션 렌더 |

## Modified Functions

| Function | File | Change |
|----------|------|--------|
| `handle_expose(t_render*)` | window_events.c | HUD/keyguide 재출력 조건 추가 |
| `keyguide_render(t_render*)` | keyguide_render.c | `keyguide_render_extra()` 호출 추가 |
