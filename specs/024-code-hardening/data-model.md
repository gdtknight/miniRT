# Data Model: Code Hardening

**Feature**: 024-code-hardening
**Date**: 2026-01-31

## Overview

본 기능은 기존 데이터 모델에 최소한의 필드 추가만 수행합니다. 새로운 엔티티는 없습니다.

---

## Modified Entities

### 1. t_line_reader (parser.h)

버퍼링된 파일 읽기 구조체에 I/O 오류 플래그 추가.

```c
typedef struct s_line_reader
{
    int     fd;
    char    buffer[LINE_READER_BUFFER_SIZE];
    int     buf_pos;
    int     buf_len;
    int     line_num;
    int     line_too_long;
    int     io_error;      // NEW: read() 오류 발생 시 1로 설정
}   t_line_reader;
```

| Field | Type | Description | Validation |
|-------|------|-------------|------------|
| fd | int | 파일 디스크립터 | >= 0 |
| buffer | char[4096] | 읽기 버퍼 | - |
| buf_pos | int | 버퍼 내 현재 위치 | 0 <= buf_pos <= buf_len |
| buf_len | int | 버퍼 내 유효 데이터 길이 | 0 <= buf_len <= 4096 |
| line_num | int | 현재 라인 번호 (1-based) | >= 0 |
| line_too_long | int | 라인 길이 초과 플래그 | 0 or 1 |
| **io_error** | int | **I/O 오류 플래그** | **0 or 1** |

**State Transitions**:
- 초기화: `io_error = 0`
- `read()` 반환값 < 0: `io_error = 1`

---

### 2. t_progressive_state (render_progressive.h)

프로그레시브 렌더링 상태에 윈도우 크기 필드 추가.

```c
typedef struct s_progressive_state
{
    int     tile_size;
    int     total_tiles;
    int     current_tile;
    int     tiles_completed;
    int     enabled;
    int     width;         // NEW: 실제 윈도우 너비
    int     height;        // NEW: 실제 윈도우 높이
}   t_progressive_state;
```

| Field | Type | Description | Validation |
|-------|------|-------------|------------|
| tile_size | int | 타일 크기 (픽셀) | > 0 |
| total_tiles | int | 전체 타일 수 | >= 0 |
| current_tile | int | 현재 타일 인덱스 | 0 <= current_tile <= total_tiles |
| tiles_completed | int | 완료된 타일 수 | 0 <= tiles_completed <= total_tiles |
| enabled | int | 활성화 플래그 | 0 or 1 |
| **width** | int | **윈도우 너비** | **> 0** |
| **height** | int | **윈도우 높이** | **> 0** |

---

### 3. t_parse_result (parser.h)

파싱 결과 열거형에 I/O 오류 코드 추가.

```c
typedef enum e_parse_result
{
    PARSE_OK = 0,
    PARSE_ERR_FORMAT,
    PARSE_ERR_RANGE,
    PARSE_ERR_COMPONENT_COUNT,
    PARSE_ERR_DUPLICATE,
    PARSE_ERR_LINE_TOO_LONG,
    PARSE_ERR_ZERO_VECTOR,
    PARSE_ERR_TRAILING_TOKEN,
    PARSE_ERR_UNKNOWN_ELEMENT,
    PARSE_ERR_NUMBER_FORMAT,
    PARSE_ERR_MISSING_ELEMENT,
    PARSE_ERR_IO,              // NEW: I/O 오류
    PARSE_ERR_COUNT
}   t_parse_result;
```

---

## Unchanged Entities (Reference)

### t_hit_record (ray.h)

교차 검사 결과. distance 필드가 상한값으로 사용됨.

```c
typedef struct s_hit_record
{
    double      distance;   // 교차점까지 거리 (상한값으로도 사용)
    t_vec3      point;      // 교차점 좌표
    t_vec3      normal;     // 표면 노멀
    t_color     color;      // 표면 색상
}   t_hit_record;
```

**수정 없음**: 구조체 자체는 변경하지 않음. 사용 패턴만 수정.

---

### t_mlx_img (mlx_context.h)

MLX 이미지 버퍼. 픽셀 범위 검사를 위해 width/height 필드 존재 여부 확인 필요.

```c
typedef struct s_mlx_img
{
    void    *img;
    char    *data;
    int     bpp;
    int     size_line;
    int     endian;
    int     width;      // 필요시 추가
    int     height;     // 필요시 추가
}   t_mlx_img;
```

**조건부 수정**: width/height 필드가 없으면 추가 필요.

---

## Relationships

```
t_scene
  └── t_line_reader (파싱 시 사용)
        └── io_error → PARSE_ERR_IO

t_window
  └── t_progressive_state
        └── width/height → 타일 계산에 사용

t_bvh_node
  └── bvh_leaf_intersect()
        └── temp_hit.distance = hit->distance (초기화)
```

---

## Data Volume Assumptions

| Entity | Expected Max Count | Notes |
|--------|-------------------|-------|
| t_line_reader | 1 | 파싱 시 단일 인스턴스 |
| t_progressive_state | 1 | 렌더링 시 단일 인스턴스 |
| t_hit_record | 스택 | 함수 내 지역 변수 |

---

## Migration Notes

기존 코드와의 호환성:
- `t_line_reader`: `io_error` 필드 추가. 초기화 함수에서 0으로 설정 필요.
- `t_progressive_state`: `width`/`height` 필드 추가. `progressive_init()`에서 설정 필요.
- `t_parse_result`: `PARSE_ERR_IO` 추가. 에러 메시지 테이블 업데이트 필요.
