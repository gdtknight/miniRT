# Tasks: Parsing Robustness Improvement

**Feature**: 023-parsing-robustness
**Date**: 2026-01-30
**Plan**: [plan.md](./plan.md)

## Task Overview

| Phase | Description | Tasks |
|-------|-------------|-------|
| 1 | Infrastructure Setup | T001-T003 |
| 2 | Header Updates | T004-T005 |
| 3 | US1: Stable Line Reading | T006-T008 |
| 4 | US2: Consistent Number Parsing | T009-T011 |
| 5 | US3: Validated Vector/Color | T012-T014 |
| 6 | US4: Clear Error Messages | T015-T017 |
| 7 | US5: Strict End-of-Line | T018-T020 |
| 8 | Polish & Validation | T021-T024 |

---

## Phase 1: Infrastructure Setup

### T001: Create parse_error.c

**Priority**: P1 | **Effort**: S | **Dependencies**: None

에러 컨텍스트 구조체 및 출력 함수 구현.

**Files**:
- Create: `src/parser/parse_error.c`

**Implementation**:
```c
// error_context_init() - 기본값 초기화
// error_context_set_line() - 라인 번호 설정
// error_context_set_element() - 요소 타입 설정
// error_context_print() - 에러 메시지 출력
```

**Acceptance Criteria**:
- [x] `error_context_init()` 구현 (line_num=0, element_type=NULL, error_code=PARSE_OK)
- [x] `error_context_set_line()` 구현
- [x] `error_context_set_element()` 구현
- [x] `error_context_print()` 구현 (element_type NULL 처리 포함)
- [x] 42 Norm 준수

---

### T002: Create parse_token.c

**Priority**: P1 | **Effort**: S | **Dependencies**: None

토큰 유틸리티 함수 구현.

**Files**:
- Create: `src/parser/parse_token.c`

**Implementation**:
```c
// skip_whitespace() - 공백/탭 건너뛰기
// next_token() - 다음 토큰 반환
// at_line_end() - 라인 끝 확인 (공백, '\0', '#')
```

**Acceptance Criteria**:
- [x] `skip_whitespace()` 구현 (space, tab 처리)
- [x] `next_token()` 구현 (공백 구분 토큰)
- [x] `at_line_end()` 구현 (주석 '#' 포함)
- [x] 42 Norm 준수

---

### T003: Create parse_number.c

**Priority**: P1 | **Effort**: M | **Dependencies**: None

숫자 형식 검증 함수 구현.

**Files**:
- Create: `src/parser/parse_number.c`

**Implementation**:
```c
// parse_double() - 실수 파싱 및 검증
// parse_int() - 정수 파싱 및 검증
// validate_number_format() - 형식 검증 헬퍼 (내부용)
```

**Acceptance Criteria**:
- [x] `parse_double()` 구현
  - 허용: `123`, `-45`, `+1`, `0.5`, `-3.14`, `.5`, `-.5`, `+0.`, `-0.`, `0.`
  - 거부: `--1`, `++1`, `12..3`, `1e10`, `123abc`, `+`, `-`, `.`
- [x] `parse_int()` 구현
  - 허용: `123`, `-45`, `+1`, `0`, `-0`, `+0`
  - 거부: `--1`, `++1`, `123abc`, `+`, `-`
- [x] `end` 포인터 올바르게 설정
- [x] 42 Norm 준수

---

## Phase 2: Header Updates

### T004: Update includes/parser.h

**Priority**: P1 | **Effort**: S | **Dependencies**: T001, T002, T003

새 타입 및 함수 선언 추가.

**Files**:
- Modify: `includes/parser.h`

**Implementation**:
```c
// Constants
# define LINE_READER_BUFFER_SIZE 4096
# define MAX_LINE_LENGTH 4096

// Types
typedef enum e_parse_result {...} t_parse_result;
typedef struct s_line_reader {...} t_line_reader;
typedef struct s_error_context {...} t_error_context;

// Function declarations
```

**Acceptance Criteria**:
- [x] `t_parse_result` enum 추가 (11개 값 + sentinel)
- [x] `t_line_reader` 구조체 추가
- [x] `t_error_context` 구조체 추가
- [x] 새 함수 선언 추가 (line_reader_*, parse_*, error_context_*, skip_whitespace, next_token, at_line_end)
- [x] 42 Norm 준수

---

### T005: Update includes/error.h (Optional)

**Priority**: P3 | **Effort**: S | **Dependencies**: T004

에러 관련 선언이 parser.h에 있으면 수정 불필요.

**Files**:
- Optional Modify: `includes/error.h`

**Acceptance Criteria**:
- [x] parser.h에서 모든 에러 타입 선언 시 수정 불필요
- [x] 또는 t_error_context를 error.h로 이동

---

## Phase 3: US1 - Stable Line Reading

### T006: Implement line_reader_init()

**Priority**: P1 | **Effort**: S | **Dependencies**: T004

라인 리더 초기화 함수 구현.

**Files**:
- Modify: `src/parser/parser.c`

**Implementation**:
```c
int line_reader_init(t_line_reader *reader, int fd)
{
    if (!reader || fd < 0)
        return (0);
    reader->fd = fd;
    reader->buf_pos = 0;
    reader->buf_len = 0;
    reader->line_num = 0;
    return (1);
}
```

**Acceptance Criteria**:
- [x] NULL 체크
- [x] fd 유효성 검사
- [x] 버퍼 위치 초기화
- [x] line_num = 0 초기화

---

### T007: Implement line_reader_next()

**Priority**: P1 | **Effort**: M | **Dependencies**: T006

버퍼 기반 라인 읽기 함수 구현.

**Files**:
- Modify: `src/parser/parser.c`

**Implementation**:
- 버퍼에서 '\n' 또는 EOF까지 읽기
- 버퍼 소진 시 read() 호출로 리필
- 라인 길이 > 4096 시 NULL 반환 (에러)
- line_num 증가

**Acceptance Criteria**:
- [x] 버퍼 기반 읽기 구현
- [x] 4096자 초과 시 에러 처리
- [x] EOF 시 NULL 반환
- [x] line_num 증가
- [x] 메모리 할당 실패 처리
- [x] 42 Norm 준수 (함수 분할 필요 시 헬퍼 생성)

---

### T008: Integrate buffered reader into parse_scene()

**Priority**: P1 | **Effort**: M | **Dependencies**: T007

기존 read_line() 호출을 line_reader_next()로 교체.

**Files**:
- Modify: `src/parser/parser.c`

**Acceptance Criteria**:
- [x] 기존 read_line() 제거 또는 비활성화
- [x] parse_scene()에서 t_line_reader 사용
- [x] 라인 번호 컨텍스트 전달
- [x] 에러 경로에서 메모리 정리

---

## Phase 4: US2 - Consistent Number Parsing

### T009: Implement number format validation

**Priority**: P1 | **Effort**: M | **Dependencies**: T003

parse_number.c의 형식 검증 로직 완성.

**Files**:
- Modify: `src/parser/parse_number.c`

**Acceptance Criteria**:
- [x] 선행 부호 처리 (`+`, `-` 1개만)
- [x] 소수점 처리 (1개만, `.5`, `5.` 허용)
- [x] trailing 문자 감지
- [x] 빈 숫자 거부 (`.`, `+`, `-`)

---

### T010: Integrate parse_double() into element parsers

**Priority**: P1 | **Effort**: M | **Dependencies**: T009

기존 ft_atof() 호출을 parse_double()로 교체.

**Files**:
- Modify: `src/parser/parse_elements.c`
- Modify: `src/parser/parse_objects.c`
- Modify: `src/parser/parse_cylinder.c`

**Acceptance Criteria**:
- [x] 모든 ft_atof() 호출 전 parse_double() 검증
- [x] 실패 시 에러 코드 반환
- [x] 기존 동작 회귀 없음

---

### T011: Integrate parse_int() into element parsers

**Priority**: P1 | **Effort**: S | **Dependencies**: T009

정수 파싱에 parse_int() 적용.

**Files**:
- Modify: `src/parser/parse_elements.c` (FOV)
- Modify: `src/parser/parse_validation.c` (color components)

**Acceptance Criteria**:
- [x] FOV 파싱에 적용
- [x] 색상 컴포넌트 파싱에 적용
- [x] 실패 시 에러 코드 반환

---

## Phase 5: US3 - Validated Vector/Color

### T012: Implement parse_vector_strict()

**Priority**: P1 | **Effort**: M | **Dependencies**: T009

엄격한 벡터 파싱 함수 구현.

**Files**:
- Modify: `src/parser/parse_validation.c`

**Implementation**:
- 정확히 3개 컴포넌트 검증
- 콤마 앞뒤 공백 거부
- 각 컴포넌트 parse_double() 검증

**Acceptance Criteria**:
- [x] 컴포넌트 수 검증 (정확히 3개)
- [x] 콤마 앞뒤 공백 거부
- [x] 각 숫자 형식 검증
- [x] end 포인터 설정

---

### T013: Implement parse_color_strict()

**Priority**: P1 | **Effort**: M | **Dependencies**: T011

엄격한 색상 파싱 함수 구현.

**Files**:
- Modify: `src/parser/parse_validation.c`

**Implementation**:
- 정확히 3개 컴포넌트 검증
- 각 컴포넌트 [0, 255] 범위 검증
- 정수 형식 검증

**Acceptance Criteria**:
- [x] 컴포넌트 수 검증
- [x] 범위 검증 [0, 255]
- [x] 정수 형식 검증
- [x] end 포인터 설정

---

### T014: Implement validate_direction_vector()

**Priority**: P1 | **Effort**: S | **Dependencies**: T012

방향/노멀 벡터 제로 체크.

**Files**:
- Modify: `src/parser/parse_validation.c`

**Acceptance Criteria**:
- [x] 벡터 길이 0 검사
- [x] PARSE_ERR_ZERO_VECTOR 반환

---

## Phase 6: US4 - Clear Error Messages

### T015: Define error message templates

**Priority**: P2 | **Effort**: S | **Dependencies**: T001

에러 코드별 메시지 템플릿 정의.

**Files**:
- Modify: `src/parser/parse_error.c`

**Implementation**:
```c
static const char *g_error_messages[PARSE_ERR_COUNT] = {
    "",                                           // PARSE_OK
    "Invalid format",                             // PARSE_ERR_FORMAT
    "Value out of range",                         // PARSE_ERR_RANGE
    "Vector requires exactly 3 components",       // PARSE_ERR_COMPONENT_COUNT
    "Duplicate declaration",                      // PARSE_ERR_DUPLICATE
    "Line too long (max 4096 characters)",        // PARSE_ERR_LINE_TOO_LONG
    "Direction vector cannot be zero",            // PARSE_ERR_ZERO_VECTOR
    "Unexpected token at end of line",            // PARSE_ERR_TRAILING_TOKEN
    "Unknown element identifier",                 // PARSE_ERR_UNKNOWN_ELEMENT
    "Invalid number format",                      // PARSE_ERR_NUMBER_FORMAT
    "Missing required element"                    // PARSE_ERR_MISSING_ELEMENT
};
```

**Acceptance Criteria**:
- [x] 모든 에러 코드에 메시지 매핑
- [x] 메시지 간결하고 명확

---

### T016: Implement formatted error output

**Priority**: P2 | **Effort**: S | **Dependencies**: T015

error_context_print() 포맷 구현.

**Files**:
- Modify: `src/parser/parse_error.c`

**Implementation**:
```
Error
Line {N} ({Element}): {Description}  # element_type != NULL
Line {N}: {Description}              # element_type == NULL
```

**Acceptance Criteria**:
- [x] "Error\n" 선행 출력
- [x] 라인 번호 포함
- [x] element_type NULL 시 괄호 생략
- [x] 에러 메시지 출력

---

### T017: Integrate error context into parsers

**Priority**: P2 | **Effort**: M | **Dependencies**: T016

각 파서에서 에러 컨텍스트 사용.

**Files**:
- Modify: `src/parser/parse_elements.c`
- Modify: `src/parser/parse_objects.c`
- Modify: `src/parser/parse_cylinder.c`

**Acceptance Criteria**:
- [x] 기존 print_error() 호출 제거
- [x] 에러 코드 반환으로 변경
- [x] 상위 호출자에서 error_context_print() 호출

---

## Phase 7: US5 - Strict End-of-Line

### T018: Implement trailing token detection

**Priority**: P2 | **Effort**: S | **Dependencies**: T002

요소 파싱 후 trailing 토큰 검사.

**Files**:
- Modify: `src/parser/parse_elements.c`
- Modify: `src/parser/parse_objects.c`
- Modify: `src/parser/parse_cylinder.c`

**Implementation**:
```c
// 요소 파싱 완료 후
str = skip_whitespace(str);
if (!at_line_end(str))
    return (PARSE_ERR_TRAILING_TOKEN);
```

**Acceptance Criteria**:
- [x] 모든 요소 파서에 trailing 검사 추가
- [x] 주석 '#' 허용
- [x] 공백만 있으면 허용

---

### T019: Implement unknown element detection

**Priority**: P2 | **Effort**: S | **Dependencies**: T002

알 수 없는 요소 식별자 검사.

**Files**:
- Modify: `src/parser/parser.c`

**Acceptance Criteria**:
- [x] 유효 식별자 목록: A, C, L, sp, pl, cy
- [x] 일치 없으면 PARSE_ERR_UNKNOWN_ELEMENT
- [x] element_type = NULL로 에러 출력

---

### T020: Implement duplicate element detection

**Priority**: P2 | **Effort**: S | **Dependencies**: T001

A, C, L 중복 선언 검사.

**Files**:
- Modify: `src/parser/parse_elements.c`

**Acceptance Criteria**:
- [x] 플래그 기반 중복 체크
- [x] PARSE_ERR_DUPLICATE 반환
- [x] 에러 메시지에 요소 타입 포함

---

## Phase 8: Polish & Validation

### T021: Create invalid test scene files

**Priority**: P2 | **Effort**: S | **Dependencies**: T007-T020

에러 케이스 테스트용 씬 파일 생성.

**Files**:
- Create: `scenes/invalid/invalid_line_too_long.rt`
- Create: `scenes/invalid/invalid_number_format.rt`
- Create: `scenes/invalid/invalid_vector_components.rt`
- Create: `scenes/invalid/invalid_color_range.rt`
- Create: `scenes/invalid/invalid_trailing_token.rt`

**Acceptance Criteria**:
- [x] 각 에러 유형별 테스트 파일
- [x] 명명 규칙 `invalid_*.rt` 준수

---

### T022: Run regression tests

**Priority**: P1 | **Effort**: S | **Dependencies**: T008, T010, T012

기존 유효 씬 파일 테스트.

**Files**:
- Test: `scenes/valid/*.rt`

**Acceptance Criteria**:
- [x] 모든 valid 파일 파싱 성공
- [x] 파싱 결과 동일 (렌더링 결과 동일)

---

### T023: Run error case tests

**Priority**: P1 | **Effort**: S | **Dependencies**: T021

에러 케이스 테스트.

**Test Commands**:
```bash
# 라인 번호 포함 확인
./miniRT scenes/invalid/invalid_number_format.rt 2>&1 | grep "Line [0-9]"

# 모든 에러 파일 테스트
for file in scenes/invalid/*.rt; do
    echo "Testing: $file"
    ./miniRT "$file" 2>&1 | grep -E "^(Error|Line)"
done
```

**Acceptance Criteria**:
- [x] 모든 에러 메시지에 라인 번호 포함
- [x] 요소 타입 적절히 표시
- [x] 예상 에러 유형 일치

---

### T024: Norminette validation

**Priority**: P1 | **Effort**: S | **Dependencies**: T001-T020

42 Norm 준수 확인.

**Test Command**:
```bash
norminette src/parser/*.c includes/*.h
```

**Acceptance Criteria**:
- [x] 모든 파일 Norm 통과
- [x] 함수당 25줄 이하
- [x] 함수당 변수 5개 이하
- [x] 파일당 함수 5개 이하

---

## Task Dependencies Graph

```
T001 ─┬─► T004 ─┬─► T006 ─► T007 ─► T008 ─┬─► T022
T002 ─┤        │                          │
T003 ─┘        └─► T005                   │
                                          │
T009 ─► T010 ─┬─► T012 ─► T014 ───────────┤
       │      │                           │
       └─► T011 ─► T013 ──────────────────┤
                                          │
T015 ─► T016 ─► T017 ─────────────────────┤
                                          │
T018 ─┬─────────────────────────────────►─┤
T019 ─┤                                   │
T020 ─┘                                   │
                                          │
T021 ─────────────────────────────────────┴─► T023
                                              │
                                              ▼
                                            T024
```

---

## Success Criteria Mapping

| SC | Tasks | Verification |
|----|-------|--------------|
| SC-001 | T009, T010, T011 | 거부 형식 숫자 에러, 허용 형식 정상 파싱 |
| SC-002 | T007, T016, T017 | 모든 에러에 라인 번호 포함 |
| SC-003 | T007 | 4097자 라인에서 에러 |
| SC-004 | T012, T013 | 컴포넌트 수 에러 |
| SC-005 | T013 | 색상 범위 에러 |
| SC-006 | T022 | 회귀 테스트 통과 |
| SC-007 | T018 | trailing 토큰 에러 |
| SC-008 | T020 | 중복 선언 에러 |
