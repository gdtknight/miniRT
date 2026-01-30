# Feature Specification: Parsing Robustness Improvement

**Feature Branch**: `023-parsing-robustness`
**Created**: 2026-01-30
**Status**: Draft
**Input**: User description: "파싱 로직의 견고성/안정성 향상을 위한 구조 개선 - Buffered Reader 도입, 토큰 파서 통합, 숫자 검증 강화, 에러 처리 통합"

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Stable Line Reading (Priority: P1)

사용자가 다양한 길이의 라인을 포함한 `.rt` 파일을 로드할 때, 시스템은 긴 라인도 안정적으로 처리하고 명확한 피드백을 제공한다.

**Why this priority**: 파일 읽기는 파싱의 첫 단계이며, I/O 안정성이 없으면 후속 파싱 단계가 무의미하다.

**Independent Test**: 4096자 이상의 라인이 포함된 파일을 로드하여 truncate 없이 에러 메시지가 출력되는지 확인할 수 있다.

**Acceptance Scenarios**:

1. **Given** 일반적인 길이(1-1000자)의 라인이 포함된 `.rt` 파일, **When** 파일을 로드하면, **Then** 모든 라인이 정상적으로 파싱된다.
2. **Given** 4096자를 초과하는 라인이 포함된 `.rt` 파일, **When** 파일을 로드하면, **Then** 해당 라인에서 명확한 에러 메시지("Line too long" 등)와 라인 번호가 출력된다.
3. **Given** 빈 라인이나 주석만 있는 라인이 포함된 `.rt` 파일, **When** 파일을 로드하면, **Then** 해당 라인들은 무시되고 유효한 라인만 파싱된다.

---

### User Story 2 - Consistent Number Parsing (Priority: P1)

사용자가 숫자 값을 포함한 `.rt` 파일을 로드할 때, 시스템은 잘못된 숫자 형식을 명확히 거부하고 유효한 숫자만 허용한다.

**Why this priority**: 잘못된 숫자가 유효 값으로 오인되면 렌더링 결과가 예측 불가능해진다. 숫자 검증은 모든 요소 파싱의 기반이다.

**Independent Test**: 거부 형식(`12..3`, `--1`, `+` 단독, `.` 단독, `1e10`)과 허용 형식(`+1`, `-0.0`, `.5`, `+0.`, `-0.`)을 각각 포함한 파일을 로드하여 거부/허용이 올바르게 동작하는지 확인할 수 있다.

**Acceptance Scenarios**:

1. **Given** 허용 형식의 숫자(`123`, `-45.67`, `0.5`, `+1`, `-0.0`, `.5`, `+0.`, `-0.`)가 포함된 `.rt` 파일, **When** 파일을 로드하면, **Then** 숫자가 정상적으로 파싱된다.
2. **Given** 잘못된 형식의 숫자(`12..3`, `--1`, `1.2.3`)가 포함된 `.rt` 파일, **When** 파일을 로드하면, **Then** 해당 값에서 "Invalid number format" 에러와 라인 번호가 출력된다.
3. **Given** 숫자 뒤에 불필요한 문자(`123abc`)가 붙은 `.rt` 파일, **When** 파일을 로드하면, **Then** 해당 값에서 에러가 출력된다.

---

### User Story 3 - Validated Vector and Color Components (Priority: P2)

사용자가 벡터(x,y,z)나 색상(R,G,B) 값을 포함한 `.rt` 파일을 로드할 때, 시스템은 정확한 컴포넌트 수와 범위를 검증한다.

**Why this priority**: 벡터와 색상은 모든 3D 오브젝트 정의에 필수이며, 컴포넌트 검증이 없으면 예측 불가능한 동작이 발생한다.

**Independent Test**: 컴포넌트 수가 부족하거나 초과된 벡터/색상 파일을 로드하여 명확한 에러가 출력되는지 확인할 수 있다.

**Acceptance Scenarios**:

1. **Given** 정확히 3개 컴포넌트를 가진 벡터(`1.0,2.0,3.0`)가 포함된 `.rt` 파일, **When** 파일을 로드하면, **Then** 벡터가 정상적으로 파싱된다.
2. **Given** 2개 컴포넌트만 있는 벡터(`1.0,2.0`)가 포함된 `.rt` 파일, **When** 파일을 로드하면, **Then** "Vector requires exactly 3 components" 에러와 라인 번호가 출력된다.
3. **Given** 색상 값이 범위를 벗어난(`0,256,100`) `.rt` 파일, **When** 파일을 로드하면, **Then** "Color component out of range [0-255]" 에러와 라인 번호가 출력된다.
4. **Given** 색상 값이 음수(`-10,100,100`)인 `.rt` 파일, **When** 파일을 로드하면, **Then** "Color component out of range [0-255]" 에러가 출력된다.
5. **Given** 콤마 앞뒤에 공백이 있는 벡터(`1.0, 2.0, 3.0`)가 포함된 `.rt` 파일, **When** 파일을 로드하면, **Then** 에러가 출력된다. (콤마 앞뒤 공백 불허)

---

### User Story 4 - Clear and Contextual Error Messages (Priority: P2)

사용자가 오류가 있는 `.rt` 파일을 로드할 때, 시스템은 라인 번호와 요소 타입을 포함한 명확한 에러 메시지를 제공한다.

**Why this priority**: 명확한 에러 메시지는 사용자가 문제를 빠르게 수정할 수 있게 하여 개발 효율성을 크게 향상시킨다.

**Independent Test**: 다양한 유형의 에러가 있는 파일을 로드하여 모든 에러 메시지에 라인 번호와 문맥 정보가 포함되는지 확인할 수 있다.

**Acceptance Scenarios**:

1. **Given** 12번째 라인에 유효하지 않은 실린더 높이가 있는 `.rt` 파일, **When** 파일을 로드하면, **Then** `"Error\nLine 12 (Cylinder): Invalid height (expected > 0)"` 형식의 메시지가 출력된다.
2. **Given** 여러 에러가 있는 `.rt` 파일, **When** 파일을 로드하면, **Then** 첫 번째 에러에서 파싱이 중단되고 해당 에러의 라인 번호와 설명이 출력된다.
3. **Given** 필수 요소(A, C, L)가 중복 선언된 `.rt` 파일, **When** 파일을 로드하면, **Then** "Duplicate [element] declaration" 에러와 라인 번호가 출력된다.
4. **Given** 5번째 라인이 4096자를 초과하는 `.rt` 파일, **When** 파일을 로드하면, **Then** `"Error\nLine 5: Line too long (max 4096 characters)"` 형식으로 요소 타입 없이 출력된다. (요소 식별 전 에러 예외)

---

### User Story 5 - Strict End-of-Line Validation (Priority: P3)

사용자가 라인 끝에 불필요한 토큰이 있는 `.rt` 파일을 로드할 때, 시스템은 이를 감지하고 에러를 출력한다.

**Why this priority**: trailing token은 파싱은 되지만 의도하지 않은 입력 오류를 나타낼 수 있어, 엄격한 검증이 파일 무결성을 보장한다.

**Independent Test**: 유효한 요소 정의 뒤에 불필요한 토큰이 있는 파일을 로드하여 에러가 출력되는지 확인할 수 있다.

**Acceptance Scenarios**:

1. **Given** 라인 끝에 불필요한 토큰(`sp 0,0,0 1.0 255,0,0 extra`)이 있는 `.rt` 파일, **When** 파일을 로드하면, **Then** "Unexpected token at end of line" 에러와 라인 번호가 출력된다.
2. **Given** 라인 끝에 공백만 있는 `.rt` 파일, **When** 파일을 로드하면, **Then** 정상적으로 파싱된다.

---

### Edge Cases

- 라인 길이가 정확히 4096자일 때 → 정상 처리 (허용 범위 내)
- 연속된 콤마(`1.0,,2.0,3.0`)는 어떻게 처리되는가?
- 숫자 앞뒤에 공백이 있는 벡터(`1.0, 2.0, 3.0` vs `1.0,2.0,3.0`) → 공백 포함 시 에러 (FR-005 참조)
- 카메라 방향 벡터나 노멀 벡터의 길이가 0에 가까울 때(`0,0,0`) → 에러 출력 (FR-011 참조)
- 탭 문자와 공백이 혼합된 구분자는 어떻게 처리되는가?
- 파일 끝에 개행 문자가 없을 때 마지막 라인이 정상 파싱되는가?
- 빈 파일일 때 적절한 에러 메시지가 출력되는가?

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: 시스템은 버퍼 기반 라인 리더를 사용하여 파일을 읽어야 한다 (최소 4KB 버퍼).
- **FR-002**: 시스템은 4096자 이하의 라인은 정상 처리하고, 4096자 초과 시 명확한 에러를 출력해야 한다. (4096자 포함은 허용, 4097자부터 에러)
- **FR-003**: 시스템은 숫자 파싱 시 형식을 검증해야 한다.
  - **허용 형식**: 정수(`123`, `-45`, `+1`, `0`, `-0`, `+0`), 소수(`0.5`, `-3.14`, `+2.0`, `.5`, `-.5`, `-0.0`, `+0.`, `-0.`, `0.`)
  - **거부 형식**: 연속 부호(`--1`, `++1`), 연속 소수점(`12..3`, `1.2.3`), 불완전 표기(`+`, `-`, `.`만 단독), 과학 표기법(`1e10`, `1E-5`), 숫자 뒤 문자(`123abc`)
  - **경계 케이스**: `-0`과 `+0`은 `0`으로 처리, `+0.`과 `-0.`은 `0.0`으로 처리 (모두 허용)
- **FR-004**: 시스템은 숫자 파싱 후 남은 문자열이 공백이나 구분자인지 확인해야 한다.
- **FR-005**: 시스템은 벡터 파싱 시 정확히 3개의 컴포넌트가 있는지 검증해야 한다.
  - 벡터 컴포넌트는 콤마(`,`)로만 구분하며, 콤마 앞뒤 공백은 허용하지 않는다.
  - **허용**: `1.0,2.0,3.0`
  - **거부**: `1.0, 2.0, 3.0` (콤마 뒤 공백), `1.0 ,2.0,3.0` (콤마 앞 공백)
- **FR-006**: 시스템은 색상 파싱 시 정확히 3개의 컴포넌트가 있고 각각 [0-255] 범위인지 검증해야 한다.
- **FR-007**: 시스템은 모든 에러 메시지에 라인 번호를 포함해야 한다.
- **FR-008**: 시스템은 에러 메시지에 요소 타입을 포함해야 한다. 표기 포맷은 다음과 같다:
  - `A` → "Ambient", `C` → "Camera", `L` → "Light"
  - `sp` → "Sphere", `pl` → "Plane", `cy` → "Cylinder"
  - 예시: `"Error\nLine 12 (Cylinder): Invalid height (expected > 0)"`
  - **예외**: 요소 식별자 파싱 전 발생하는 에러(라인 길이 초과, 알 수 없는 식별자 등)는 요소 타입 없이 출력
    - 예시: `"Error\nLine 5: Line too long (max 4096 characters)"`
    - 예시: `"Error\nLine 8: Unknown element identifier 'xx'"`
- **FR-009**: 시스템은 필수 요소(A, C, L)의 중복 선언을 감지하고 에러를 출력해야 한다.
- **FR-010**: 시스템은 라인 끝에 불필요한 토큰이 있으면 에러를 출력해야 한다.
- **FR-011**: 시스템은 카메라 방향 벡터와 오브젝트 노멀 벡터의 길이가 0에 가까울 때 에러를 출력해야 한다.
- **FR-012**: 시스템은 FOV, 비율, 밝기 등 범위 제한이 있는 값에 대해 범위 검증을 수행해야 한다.
- **FR-013**: 시스템은 빈 라인과 주석 라인(# 으로 시작)을 무시해야 한다.
- **FR-014**: 하위 파서 함수들은 에러 코드만 반환하고, 상위 파서에서 에러 메시지를 통합 출력해야 한다.

### Key Entities

- **Line Buffer**: 파일에서 읽은 라인 데이터를 저장하는 버퍼, 최대 길이 제한 포함
- **Token**: 공백/콤마로 구분된 파싱 단위, 위치 정보 포함
- **Parse Result**: 파싱 결과를 담는 구조체, 성공/실패 상태와 에러 코드 포함
- **Error Context**: 에러 발생 시 문맥 정보(라인 번호, 요소 타입, 에러 메시지)를 담는 구조체

### Error Message Templates

에러 메시지는 다음 템플릿을 따른다:

```
Error
Line {N} ({Element}): {Description}    # 요소 파싱 중 에러
Line {N}: {Description}                 # 요소 식별 전 에러 (예외)
```

| 카테고리 | 템플릿 | 예시 |
|----------|--------|------|
| 라인 길이 | `Line {N}: Line too long (max 4096 characters)` | `Line 5: Line too long (max 4096 characters)` |
| 알 수 없는 식별자 | `Line {N}: Unknown element identifier '{id}'` | `Line 8: Unknown element identifier 'xx'` |
| 숫자 형식 | `Line {N} ({Element}): Invalid number format` | `Line 12 (Sphere): Invalid number format` |
| 벡터 컴포넌트 | `Line {N} ({Element}): Vector requires exactly 3 components` | `Line 8 (Camera): Vector requires exactly 3 components` |
| 색상 범위 | `Line {N} ({Element}): Color component out of range [0-255]` | `Line 15 (Ambient): Color component out of range [0-255]` |
| 중복 선언 | `Line {N}: Duplicate {Element} declaration` | `Line 20: Duplicate Camera declaration` |
| 범위 오류 | `Line {N} ({Element}): {Field} out of range ({expected})` | `Line 7 (Camera): FOV out of range (0-180)` |
| 제로 벡터 | `Line {N} ({Element}): Direction vector cannot be zero` | `Line 10 (Plane): Direction vector cannot be zero` |
| trailing 토큰 | `Line {N} ({Element}): Unexpected token at end of line` | `Line 3 (Light): Unexpected token at end of line` |

> **참고**: "중복 선언" 템플릿의 `{Element}`는 괄호 없이 요소 타입 이름(Ambient, Camera, Light)이 직접 삽입됨. 필수 요소(A, C, L)만 중복 체크 대상이므로 Sphere, Plane, Cylinder는 해당 없음.

**요소 타입 매핑**:
- `A` → `Ambient`, `C` → `Camera`, `L` → `Light`
- `sp` → `Sphere`, `pl` → `Plane`, `cy` → `Cylinder`

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: 거부 형식 숫자(`12..3`, `--1`, `+` 단독, `.` 단독, `1e10` 등)가 포함된 파일 로드 시 100% 에러로 감지되고, 허용 형식(`+1`, `-0.0`, `.5` 등)은 정상 파싱된다.
- **SC-002**: 모든 에러 메시지에 라인 번호가 포함된다.
- **SC-003**: 4096자 이하 라인은 정상 처리되고, 4096자 초과(4097자+) 라인은 truncate 없이 명확한 에러 메시지가 출력된다.
- **SC-004**: 벡터/색상 컴포넌트 수 오류가 100% 감지된다.
- **SC-005**: 색상 범위 오류([0-255] 외)가 100% 감지된다.
- **SC-006**: 기존 유효한 `.rt` 파일들이 모두 정상적으로 파싱된다 (회귀 없음).
  - 테스트 대상: `scenes/valid/` 디렉토리 내 모든 `.rt` 파일
  - 기준: 개선 전후 동일한 파싱 결과 (씬 객체 수, 속성 값 일치)
- **SC-007**: 라인 끝 불필요 토큰이 100% 감지된다.
- **SC-008**: 필수 요소(A, C, L) 중복 선언이 100% 감지된다.

## Assumptions

- 라인 길이 제한 4096자는 일반적인 `.rt` 파일 사용 패턴에서 충분하다.
- 주석은 `#` 문자로 시작하는 라인으로 정의한다.
- 기존 파싱 로직의 기본 구조(요소 식별자 분기 방식)는 유지하고, 검증 로직만 강화한다.
- 42 Norm v4.1을 준수해야 하므로 함수 길이와 복잡도에 제한이 있다.
- 에러 발생 시 첫 번째 에러에서 파싱을 중단한다 (multiple error reporting은 범위 외).
