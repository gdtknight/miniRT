# Implementation Plan: Parsing Robustness Improvement

**Branch**: `023-parsing-robustness` | **Date**: 2026-01-30 | **Spec**: [spec.md](./spec.md)
**Input**: Feature specification from `/specs/023-parsing-robustness/spec.md`

## Summary

파싱 로직의 견고성/안정성 향상을 위한 구조 개선. 주요 변경사항:
1. **Buffered Reader 도입**: 바이트 단위 I/O를 버퍼 기반으로 개선
2. **토큰 파서 통합**: 중복된 토큰 스캔 로직 통합
3. **숫자 검증 강화**: 잘못된 형식 명확히 거부
4. **에러 처리 통합**: 라인 번호, 요소 타입 포함 메시지

## Technical Context

**Language/Version**: C (42 Norm v4.1 compliant)
**Primary Dependencies**: libft, MiniLibX, math library (-lm)
**Storage**: N/A (파일 파싱만 수행)
**Testing**: Shell scripts (`scripts/test_*.sh`), manual scene file validation
**Target Platform**: macOS (Darwin), Linux
**Project Type**: Single project (miniRT raytracer)
**Performance Goals**: I/O syscall 감소 (현재 바이트당 1회 → 버퍼당 1회)
**Constraints**: 42 Norm v4.1 (25줄/함수, 5변수/함수, 5함수/파일), 4096자 라인 제한
**Scale/Scope**: `.rt` 파일 100-1000 라인 범위

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

| Principle | Status | Notes |
|-----------|--------|-------|
| I. 42 Norminette Compliance | ✅ PASS | 모든 코드 Norm v4.1 준수 필수 |
| II. Memory Safety | ✅ PASS | 버퍼 할당/해제 중앙화, 에러 경로 정리 |
| III. Allowed Functions | ✅ PASS | open, close, read, write, malloc, free만 사용 |
| IV. Modular Architecture | ✅ PASS | src/parser/ 내 모듈 분리 유지 |
| V. Visual Correctness | N/A | 파싱 전용, 렌더링 영향 없음 |
| VI. Stability & Crash Prevention | ✅ PASS | NULL 체크, 범위 검증 강화 |
| VII. Window Management | N/A | 파싱 전용 |

**Constitution Gate: PASSED**

## Project Structure

### Documentation (this feature)

```text
specs/023-parsing-robustness/
├── plan.md              # This file
├── spec.md              # Feature specification
├── research.md          # Phase 0 output
├── data-model.md        # Phase 1 output
├── quickstart.md        # Phase 1 output
└── tasks.md             # Phase 2 output
```

### Source Code (repository root)

```text
src/
├── parser/
│   ├── parser.c           # 수정: buffered line reader 도입
│   ├── parse_elements.c   # 수정: 에러 코드 반환으로 변경
│   ├── parse_objects.c    # 수정: 에러 코드 반환으로 변경
│   ├── parse_cylinder.c   # 수정: 에러 코드 반환으로 변경
│   ├── parse_validation.c # 수정: 컴포넌트 수/범위 검증 강화
│   ├── parse_number.c     # 신규: 숫자 형식 검증
│   ├── parse_token.c      # 신규: 토큰 유틸리티 통합
│   └── parse_error.c      # 신규: 에러 메시지 통합 출력
├── utils/
│   ├── ft_atof.c          # 수정: 형식 검증 추가 (parse_number에서 호출)
│   └── error.c            # 선택: 기존 인터페이스 유지 시 수정 불필요
└── ...

includes/
├── parser.h               # 수정: 새 함수 선언 추가
└── error.h                # 선택: 에러 컨텍스트를 parser.h에 선언 시 수정 불필요

scenes/
├── valid/                 # 회귀 테스트용 유효 씬 파일 (valid_*.rt)
└── invalid/               # 에러 케이스 테스트용 무효 씬 파일 (invalid_*.rt)
```

**Structure Decision**: 기존 `src/parser/` 구조 유지, 신규 파일 3개 추가 (parse_number.c, parse_token.c, parse_error.c)

## Complexity Tracking

> No violations requiring justification.

## Current Implementation Analysis

### Line Reading (`read_line()` in parser.c)

**현재 문제점**:
- 바이트 단위 read() 호출 (1KB 라인 = 1000 syscalls)
- 1024바이트 하드코딩 버퍼
- 라인 초과 시 silent truncate
- 라인 번호 미추적

**개선 방향**:
- 4KB 버퍼 기반 읽기
- 4096자 라인 제한 명시적 에러
- 라인 번호 추적

### Number Parsing (ft_atof.c)

**현재 문제점**:
- `--1`, `++1` → 0 반환 (silent)
- `12..3` → 12.0 반환 (부분 파싱)
- `123abc` → 123.0 반환 (trailing 무시)
- `+`, `-`, `.` 단독 → 0.0 반환

**개선 방향**:
- 형식 검증 선행 (parse_number.c)
- 검증 실패 시 에러 코드 반환
- ft_atof는 검증 통과 후에만 호출

### Vector/Color Parsing (parse_validation.c)

**현재 문제점**:
- 컴포넌트 수 미검증
- 콤마 주변 공백 허용 (의도치 않음)
- trailing 문자 무시

**개선 방향**:
- 정확히 3개 컴포넌트 검증
- 콤마 앞뒤 공백 거부
- 파싱 후 위치 확인

### Error Handling (error.c - Optional)

**현재 문제점**:
- 라인 번호 없음
- 요소 타입 없음
- 각 파서가 직접 print_error() 호출

**개선 방향**:
- Error Context 구조체 도입 (신규 parse_error.c에서 구현)
- 하위 파서는 에러 코드만 반환
- 상위에서 컨텍스트 포함 출력

> **Note**: error.c 수정은 선택 사항. 에러 컨텍스트는 parse_error.c에서 구현하므로 기존 error.c는 수정 불필요할 수 있음.

## Key Design Decisions

### D1: Buffered Reader Architecture

```c
typedef struct s_line_reader {
    int     fd;
    char    buffer[LINE_READER_BUFFER_SIZE];  // 4096
    int     buf_pos;
    int     buf_len;
    int     line_num;
} t_line_reader;

// API
int     line_reader_init(t_line_reader *reader, int fd);
char    *line_reader_next(t_line_reader *reader);  // returns NULL on EOF/error
int     line_reader_get_line_num(t_line_reader *reader);
```

### D2: Parse Result Pattern

```c
typedef enum e_parse_result {
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
    PARSE_ERR_COUNT
} t_parse_result;
```

### D3: Error Context Structure

```c
typedef struct s_error_context {
    int             line_num;
    const char      *element_type;  // "Sphere", "Camera", etc. or NULL
    t_parse_result  error_code;
} t_error_context;

// API
void    error_context_init(t_error_context *ctx);
void    error_context_set_line(t_error_context *ctx, int line);
void    error_context_set_element(t_error_context *ctx, const char *type);
void    error_context_print(t_error_context *ctx);
```

### D4: Number Validation API

```c
// Returns PARSE_OK if valid, error code otherwise
// On success, *value contains the parsed number, *end points after the number
t_parse_result  parse_double(const char *str, double *value, const char **end);
t_parse_result  parse_int(const char *str, int *value, const char **end);
```

### D5: Token Utilities

```c
// Skip whitespace, return pointer to next non-whitespace
const char  *skip_whitespace(const char *str);

// Get next token, return NULL if end of line
// Token is space-separated, not comma-separated
const char  *next_token(const char *str, const char **end);

// Check if at end of meaningful content (whitespace or '\0')
int         at_line_end(const char *str);
```

## File Changes Summary

| File | Action | Changes |
|------|--------|---------|
| `src/parser/parser.c` | Modify | read_line → buffered reader, 라인 번호 추적 |
| `src/parser/parse_elements.c` | Modify | 에러 코드 반환, print_error 제거 |
| `src/parser/parse_objects.c` | Modify | 에러 코드 반환, print_error 제거 |
| `src/parser/parse_cylinder.c` | Modify | 에러 코드 반환, print_error 제거 |
| `src/parser/parse_validation.c` | Modify | 컴포넌트 수/공백 검증 추가 |
| `src/parser/parse_number.c` | Create | 숫자 형식 검증 |
| `src/parser/parse_token.c` | Create | 토큰 유틸리티 통합 |
| `src/parser/parse_error.c` | Create | 에러 컨텍스트 및 출력 |
| `src/utils/error.c` | Optional | 기존 인터페이스 유지 시 수정 불필요 (parse_error.c에서 처리) |
| `includes/parser.h` | Modify | 새 구조체/함수 선언 |
| `includes/error.h` | Optional | 에러 컨텍스트를 parser.h에 선언 시 수정 불필요 |

## Edge Case Decisions

| Edge Case | Decision | Rationale |
|-----------|----------|-----------|
| 연속된 콤마 (`1.0,,2.0,3.0`) | 에러 | 빈 컴포넌트는 invalid |
| 탭/공백 혼합 구분자 | 둘 다 whitespace로 처리 | 기존 동작 유지 |
| 파일 끝 개행 없음 | 마지막 라인 정상 파싱 | 합리적 기본값 |
| 빈 파일 | 필수 요소 누락 에러 | 기존 validate_scene 동작 |

## Testing Strategy

### 회귀 테스트
- `scenes/valid/` 내 모든 파일 파싱 성공 확인
- 개선 전후 동일한 씬 객체 생성 확인

### 에러 케이스 테스트
- `scenes/invalid/` 에 새 테스트 파일 추가 (기존 명명 규칙: `invalid_*.rt`):
  - `invalid_line_too_long.rt` - 4097자 라인
  - `invalid_number_format.rt` - `12..3`, `--1` 등
  - `invalid_vector_components.rt` - 컴포넌트 부족/초과
  - `invalid_color_range.rt` - 범위 오류
  - `invalid_trailing_token.rt` - 불필요한 토큰
  - 기존 파일 활용: `invalid_ambient_duplicate.rt` - A, C, L 중복

### 수동 검증
- 에러 메시지에 라인 번호 포함 확인
- 에러 메시지에 요소 타입 포함 확인
