# Research: Parsing Robustness Improvement

**Feature**: 023-parsing-robustness
**Date**: 2026-01-30

## Research Summary

모든 NEEDS CLARIFICATION 항목이 스펙 단계에서 해결되어 추가 연구가 필요하지 않음.
기존 코드베이스 분석을 통해 구현 방향이 명확히 도출됨.

---

## R1: Buffered Line Reader Pattern

### Decision
4KB 버퍼 기반 라인 리더 구현

### Rationale
- 현재: 바이트당 1회 read() syscall (1KB 라인 = 1000 syscalls)
- 개선: 버퍼당 1회 read() syscall (4KB 버퍼 = 1회)
- 42 Norm 준수: 구조체와 함수로 분리하여 25줄 제한 충족

### Alternatives Considered
1. **getline() 사용**: 금지된 함수
2. **get_next_line 재사용**: libft에 없음, 별도 구현 필요
3. **mmap 기반**: 불필요한 복잡성, 작은 파일에 오버헤드

### Implementation Notes
```c
#define LINE_READER_BUFFER_SIZE 4096
#define MAX_LINE_LENGTH 4096

typedef struct s_line_reader {
    int     fd;
    char    buffer[LINE_READER_BUFFER_SIZE];
    int     buf_pos;
    int     buf_len;
    int     line_num;
} t_line_reader;
```

---

## R2: Number Validation Strategy

### Decision
숫자 파싱 전 형식 검증을 별도 함수로 분리

### Rationale
- 현재 ft_atof는 부분 파싱 허용 (잘못된 입력도 숫자 부분만 반환)
- 검증과 변환 분리로 명확한 에러 감지 가능
- 42 Norm 25줄 제한 충족 위해 검증 로직 분리 필수

### Validation Rules
**허용 형식**:
- 정수: `123`, `-45`, `+1`, `0`, `-0`, `+0`
- 소수: `0.5`, `-3.14`, `+2.0`, `.5`, `-.5`, `-0.0`, `+0.`, `-0.`, `0.`

**거부 형식**:
- 연속 부호: `--1`, `++1`
- 연속 소수점: `12..3`, `1.2.3`
- 불완전 표기: `+`, `-`, `.` 단독
- 과학 표기법: `1e10`, `1E-5`
- trailing 문자: `123abc`

### State Machine Approach
```
START -> SIGN? -> (DIGIT+ | DIGIT* '.' DIGIT+) -> END
         ^              ^
         |              |
      optional      at least one digit required
```

---

## R3: Error Context Pattern

### Decision
에러 컨텍스트 구조체로 상위에서 메시지 통합 출력

### Rationale
- 현재: 각 파서가 직접 print_error() 호출 (라인 번호 없음)
- 개선: 하위 파서는 에러 코드만 반환, 상위에서 컨텍스트 포함 출력
- 42 Norm 준수: 함수 파라미터 4개 제한 → 컨텍스트 구조체로 해결

### Error Message Format
```
Error
Line {N} ({Element}): {Description}    # 요소 파싱 중 에러
Line {N}: {Description}                 # 요소 식별 전 에러
```

### Element Type Mapping
| Identifier | Display Name |
|------------|--------------|
| `A` | Ambient |
| `C` | Camera |
| `L` | Light |
| `sp` | Sphere |
| `pl` | Plane |
| `cy` | Cylinder |

---

## R4: Vector/Color Validation Enhancement

### Decision
콤마 기반 파싱 강화, 컴포넌트 수 엄격 검증

### Rationale
- 현재: 콤마 주변 공백 허용 (의도치 않음)
- 현재: 컴포넌트 수 미검증 (2개만 있어도 부분 파싱)
- 개선: 정확히 3개 컴포넌트, 콤마 앞뒤 공백 불허

### Validation Algorithm
```
1. 첫 번째 숫자 파싱
2. 콤마 확인 (앞뒤 공백 불가)
3. 두 번째 숫자 파싱
4. 콤마 확인
5. 세 번째 숫자 파싱
6. 다음 문자가 공백 또는 끝인지 확인
```

---

## R5: Trailing Token Detection

### Decision
각 요소 파싱 후 라인 끝 검증 추가

### Rationale
- 현재: trailing 토큰 무시 (silent)
- 개선: 불필요한 토큰 감지 시 에러

### Implementation
```c
// 요소 파싱 완료 후
str = skip_whitespace(str);
if (*str != '\0' && *str != '#')
    return (PARSE_ERR_TRAILING_TOKEN);
```

---

## R6: 42 Norm Compliance for New Code

### Decision
새 파일 3개로 기능 분리하여 5함수/파일 제한 충족

### File Distribution
| File | Functions | Purpose |
|------|-----------|---------|
| `parse_number.c` | 5 | 숫자 검증 및 파싱 |
| `parse_token.c` | 5 | 토큰 유틸리티 |
| `parse_error.c` | 5 | 에러 컨텍스트 관리 |

### Function Size Strategy
- 각 함수 25줄 이내
- 복잡한 로직은 helper 함수로 분리
- 변수 선언 5개 이내

---

## R7: Backward Compatibility

### Decision
기존 유효 파일은 100% 호환 유지

### Verification
- `scenes/valid/` 내 모든 파일 개선 전후 동일 결과
- 씬 객체 수 및 속성 값 일치 확인

### Risk Mitigation
- 기존 parse_vector, parse_color 시그니처 유지
- 내부 구현만 변경
- 필요시 wrapper 함수로 호환성 보장

---

## Conclusion

모든 연구 항목이 완료됨. 구현 계획이 명확하며 42 Norm 준수 가능.
다음 단계: Phase 1 (data-model.md, quickstart.md 생성)
