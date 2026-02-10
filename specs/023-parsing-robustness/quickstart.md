# Quickstart: Parsing Robustness Improvement

**Feature**: 023-parsing-robustness
**Date**: 2026-01-30

## Overview

miniRT 파서의 견고성 향상을 위한 구현 가이드.

---

## Prerequisites

- miniRT 프로젝트 빌드 환경 (macOS/Linux)
- 42 Norm v4.1 준수 코드 작성 능력
- `make`, `gcc`, `norminette` 설치

---

## Quick Setup

```bash
# 1. 브랜치 확인
git checkout 023-parsing-robustness

# 2. 빌드 확인
make re

# 3. 기존 동작 확인
./miniRT scenes/valid/valid_scene_complex.rt

# 4. Norm 체크
norminette src/parser/*.c includes/*.h
```

---

## Implementation Order

### Phase 1: Infrastructure (신규 파일)

1. **parse_error.c** - 에러 컨텍스트 구조체 및 출력
2. **parse_token.c** - 토큰 유틸리티 함수
3. **parse_number.c** - 숫자 형식 검증

### Phase 2: Core Changes

4. **parser.c** - Buffered line reader 도입
5. **parse_validation.c** - 벡터/색상 검증 강화

### Phase 3: Element Parsers

6. **parse_elements.c** - 에러 코드 반환으로 변경
7. **parse_objects.c** - 에러 코드 반환으로 변경
8. **parse_cylinder.c** - 에러 코드 반환으로 변경

### Phase 4: Headers & Integration

9. **includes/parser.h** - 새 타입/함수 선언
10. **includes/error.h** - 에러 컨텍스트 추가

---

## Key Code Snippets

### Line Reader Initialization

```c
t_line_reader   reader;

if (!line_reader_init(&reader, fd))
    return (print_error("Failed to initialize line reader"));
```

### Number Parsing with Validation

```c
double          value;
const char      *end;
t_parse_result  result;

result = parse_double(str, &value, &end);
if (result != PARSE_OK)
{
    ctx.error_code = result;
    error_context_print(&ctx);
    return (0);
}
```

### Error Context Usage

```c
t_error_context ctx;

error_context_init(&ctx);
error_context_set_line(&ctx, reader.line_num);
error_context_set_element(&ctx, "Sphere");

// ... parsing ...

if (parse_error)
{
    ctx.error_code = PARSE_ERR_RANGE;
    error_context_print(&ctx);
    return (0);
}
```

---

## Testing

### Regression Test

```bash
# 기존 유효 파일 테스트
for file in scenes/valid/*.rt; do
    ./miniRT "$file" &
    sleep 1
    kill $! 2>/dev/null
done
```

### Error Case Test

```bash
# 에러 케이스 테스트
for file in scenes/invalid/*.rt; do
    echo "Testing: $file"
    ./miniRT "$file" 2>&1 | grep -E "^(Error|Line)"
done
```

### Line Number Verification

```bash
# 라인 번호 포함 확인 (신규 테스트 파일 필요)
./miniRT scenes/invalid/invalid_number_format.rt 2>&1 | grep "Line [0-9]"
```

---

## Norm Compliance Checklist

- [ ] 각 함수 25줄 이내
- [ ] 각 함수 변수 선언 5개 이내
- [ ] 각 파일 함수 5개 이내
- [ ] 함수 파라미터 4개 이내
- [ ] 금지 구문 없음 (for, switch, goto, ?:)
- [ ] snake_case 명명 규칙
- [ ] 42 헤더 포함
- [ ] return 괄호 포함

---

## Common Issues

### Issue: Line too long error on valid files

**원인**: 기존 라인 길이 제한이 없었으나 새로 4096자 제한 추가
**해결**: 유효 씬 파일의 라인이 4096자 이내인지 확인

### Issue: Norm error on new files

**원인**: 42 헤더 누락 또는 형식 오류
**해결**: `norminette` 실행 후 메시지 확인

### Issue: Segfault on error path

**원인**: 에러 경로에서 메모리 해제 누락
**해결**: 모든 에러 반환 전 현재 라인 문자열 free 확인

---

## File Structure After Implementation

```
src/parser/
├── parser.c           # Modified: buffered reader
├── parse_elements.c   # Modified: error codes
├── parse_objects.c    # Modified: error codes
├── parse_cylinder.c   # Modified: error codes
├── parse_validation.c # Modified: strict validation
├── parse_number.c     # NEW: number validation
├── parse_token.c      # NEW: token utilities
└── parse_error.c      # NEW: error context

includes/
├── parser.h           # Modified: new declarations
└── error.h            # Modified: error context

scenes/
├── valid/             # Regression test files
└── invalid/           # Error case test files
```

---

## Success Criteria Verification

| SC | Test Command | Expected |
|----|--------------|----------|
| SC-001 | `./miniRT scenes/invalid/invalid_number_format.rt` | Error with line number |
| SC-002 | `./miniRT scenes/invalid/*.rt 2>&1 \| grep "Line"` | All errors have line numbers |
| SC-003 | `./miniRT scenes/invalid/invalid_line_too_long.rt` | "Line too long" error |
| SC-004 | `./miniRT scenes/invalid/invalid_vector_components.rt` | Component count error |
| SC-005 | `./miniRT scenes/invalid/invalid_color_range.rt` | Color range error |
| SC-006 | `for f in scenes/valid/*.rt; do ./miniRT "$f"; done` | All pass |
| SC-007 | `./miniRT scenes/invalid/invalid_trailing_token.rt` | Trailing token error |
| SC-008 | `./miniRT scenes/invalid/invalid_ambient_duplicate.rt` | Duplicate error |

> **Note**: SC-001~005, SC-007 테스트를 위해 새 invalid 파일 생성 필요 (명명 규칙: `invalid_*.rt`)
