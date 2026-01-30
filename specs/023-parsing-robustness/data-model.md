# Data Model: Parsing Robustness Improvement

**Feature**: 023-parsing-robustness
**Date**: 2026-01-30

## Entity Overview

이 기능은 새로운 데이터 모델을 도입하지 않고, 기존 파싱 인프라에 보조 구조체를 추가합니다.

---

## New Structures

### 1. Line Reader (`t_line_reader`)

버퍼 기반 파일 읽기를 위한 구조체.

```c
#define LINE_READER_BUFFER_SIZE 4096
#define MAX_LINE_LENGTH 4096

typedef struct s_line_reader
{
    int     fd;                                 // 파일 디스크립터
    char    buffer[LINE_READER_BUFFER_SIZE];    // 읽기 버퍼
    int     buf_pos;                            // 버퍼 내 현재 위치
    int     buf_len;                            // 버퍼 내 유효 데이터 길이
    int     line_num;                           // 현재 라인 번호 (1-based)
}   t_line_reader;
```

**Lifecycle**:
1. `line_reader_init()` - fd 설정, 버퍼 초기화, line_num = 0
2. `line_reader_next()` - 라인 읽기, line_num 증가
3. 사용 완료 후 fd는 호출자가 close()

**Invariants**:
- `buf_pos <= buf_len`
- `line_num >= 0`

---

### 2. Parse Result (`t_parse_result`)

파싱 결과를 나타내는 열거형.

```c
typedef enum e_parse_result
{
    PARSE_OK = 0,               // 성공
    PARSE_ERR_FORMAT,           // 형식 오류 (일반)
    PARSE_ERR_RANGE,            // 범위 오류 (값이 허용 범위 밖)
    PARSE_ERR_COMPONENT_COUNT,  // 컴포넌트 수 오류 (벡터/색상)
    PARSE_ERR_DUPLICATE,        // 중복 선언 (A, C, L)
    PARSE_ERR_LINE_TOO_LONG,    // 라인 길이 초과
    PARSE_ERR_ZERO_VECTOR,      // 제로 벡터 (방향/노멀)
    PARSE_ERR_TRAILING_TOKEN,   // 불필요한 trailing 토큰
    PARSE_ERR_UNKNOWN_ELEMENT,  // 알 수 없는 요소 식별자
    PARSE_ERR_NUMBER_FORMAT,    // 숫자 형식 오류
    PARSE_ERR_MISSING_ELEMENT,  // 필수 요소 누락
    PARSE_ERR_COUNT             // 열거형 개수 (sentinel)
}   t_parse_result;
```

---

### 3. Error Context (`t_error_context`)

에러 발생 시 문맥 정보를 저장하는 구조체.

```c
typedef struct s_error_context
{
    int             line_num;       // 에러 발생 라인 (1-based, 0 = unknown)
    const char      *element_type;  // 요소 타입 문자열 (NULL = unknown)
    t_parse_result  error_code;     // 에러 코드
}   t_error_context;
```

**Element Type Values**:
- `"Ambient"`, `"Camera"`, `"Light"`
- `"Sphere"`, `"Plane"`, `"Cylinder"`
- `NULL` (요소 식별 전 에러)

**Usage Pattern**:
```c
t_error_context ctx;
error_context_init(&ctx);
error_context_set_line(&ctx, reader.line_num);
error_context_set_element(&ctx, "Sphere");
ctx.error_code = PARSE_ERR_RANGE;
error_context_print(&ctx);
```

---

## Modified Structures

### 1. Scene (`t_scene`) - No Change

기존 구조체 유지. 파싱 개선은 내부 로직만 변경.

### 2. Object (`t_object`) - No Change

기존 구조체 유지.

---

## Validation Rules

### Number Validation

| Field | Type | Valid Range | Error Code |
|-------|------|-------------|------------|
| Ambient ratio | double | [0.0, 1.0] | PARSE_ERR_RANGE |
| Light brightness | double | [0.0, 1.0] | PARSE_ERR_RANGE |
| FOV | int | [0, 180] | PARSE_ERR_RANGE |
| Color component | int | [0, 255] | PARSE_ERR_RANGE |
| Sphere diameter | double | > 0 | PARSE_ERR_RANGE |
| Cylinder diameter | double | > 0 | PARSE_ERR_RANGE |
| Cylinder height | double | > 0 | PARSE_ERR_RANGE |

### Vector Validation

| Check | Error Code |
|-------|------------|
| 정확히 3개 컴포넌트 | PARSE_ERR_COMPONENT_COUNT |
| 콤마 앞뒤 공백 없음 | PARSE_ERR_FORMAT |
| 방향/노멀 벡터 길이 > 0 | PARSE_ERR_ZERO_VECTOR |

### Line Validation

| Check | Error Code |
|-------|------------|
| 라인 길이 <= 4096자 | PARSE_ERR_LINE_TOO_LONG |
| trailing 토큰 없음 | PARSE_ERR_TRAILING_TOKEN |
| 요소 식별자 유효 | PARSE_ERR_UNKNOWN_ELEMENT |

---

## State Transitions

### Parser State Flow

```
FILE_OPEN
    │
    ▼
LINE_READ ────────────────────┐
    │                         │
    ├──► PARSE_ELEMENT        │
    │       │                 │
    │       ├──► SUCCESS ─────┤
    │       │                 │
    │       └──► ERROR ───────┼──► ERROR_OUTPUT ──► EXIT
    │                         │
    ├──► EMPTY_LINE ──────────┤
    │                         │
    ├──► COMMENT_LINE ────────┤
    │                         │
    └──► EOF                  │
           │                  │
           ▼                  │
    VALIDATE_SCENE            │
           │                  │
           ├──► SUCCESS ──────┼──► PARSE_COMPLETE
           │                  │
           └──► ERROR ────────┘
```

### Required Elements State

```c
// Scene flags (existing)
#define SCENE_HAS_AMBIENT  0x01
#define SCENE_HAS_CAMERA   0x02
#define SCENE_HAS_LIGHT    0x04
```

Validation: 파싱 완료 후 세 플래그 모두 설정 필수.

---

## Error Message Mapping

| Error Code | Message Template |
|------------|------------------|
| PARSE_ERR_FORMAT | "Invalid format" |
| PARSE_ERR_RANGE | "{Field} out of range ({expected})" |
| PARSE_ERR_COMPONENT_COUNT | "Vector requires exactly 3 components" |
| PARSE_ERR_DUPLICATE | "Duplicate {Element} declaration" |
| PARSE_ERR_LINE_TOO_LONG | "Line too long (max 4096 characters)" |
| PARSE_ERR_ZERO_VECTOR | "Direction vector cannot be zero" |
| PARSE_ERR_TRAILING_TOKEN | "Unexpected token at end of line" |
| PARSE_ERR_UNKNOWN_ELEMENT | "Unknown element identifier '{id}'" |
| PARSE_ERR_NUMBER_FORMAT | "Invalid number format" |
| PARSE_ERR_MISSING_ELEMENT | "Missing {Element}" |

**요소 식별 전 에러 규칙**:

다음 에러 코드는 요소 타입이 결정되기 전에 발생하므로 `element_type = NULL`로 설정:
- `PARSE_ERR_LINE_TOO_LONG` - 라인 읽기 단계에서 발생
- `PARSE_ERR_UNKNOWN_ELEMENT` - 요소 식별자 파싱 단계에서 발생

이 경우 에러 메시지 출력 시 `({Element})` 부분 생략:
```
Error
Line {N}: {Description}    # element_type == NULL
Line {N} ({Element}): {Description}  # element_type != NULL
```

---

## Memory Management

### Allocation Points

| Entity | Allocation | Deallocation |
|--------|------------|--------------|
| Line string | `line_reader_next()` | Caller (parse_line 후) |
| Object list | `object_list_add()` | `scene_cleanup()` |
| Error context | Stack (자동) | N/A |

### Error Path Cleanup

모든 에러 경로에서:
1. 현재 라인 문자열 free
2. 파일 디스크립터 close
3. 에러 메시지 출력 후 반환

---

## Integration Points

### Header Changes (`includes/parser.h`)

```c
// New types
typedef struct s_line_reader    t_line_reader;
typedef enum e_parse_result     t_parse_result;
typedef struct s_error_context  t_error_context;

// New functions
int             line_reader_init(t_line_reader *reader, int fd);
char            *line_reader_next(t_line_reader *reader);
int             line_reader_get_line_num(t_line_reader *reader);

t_parse_result  parse_double(const char *str, double *val, const char **end);
t_parse_result  parse_int(const char *str, int *val, const char **end);

const char      *skip_whitespace(const char *str);
const char      *next_token(const char *str, const char **end);
int             at_line_end(const char *str);

void            error_context_init(t_error_context *ctx);
void            error_context_set_line(t_error_context *ctx, int line);
void            error_context_set_element(t_error_context *ctx, const char *type);
void            error_context_print(t_error_context *ctx);
```

### Header Changes (`includes/error.h`)

```c
// Add t_error_context definition
// Add PARSE_ERR_* enum values
```
