# Module: Parser

`.rt` 씬 파일을 파싱하여 `t_scene` 구조체를 구축하는 모듈입니다.

---

## 소스 파일

| 파일 | 역할 |
|------|------|
| `parser.c` | 파싱 진입점 (`parse_scene`) |
| `parser_dispatch.c` | 요소 식별자별 디스패치 (`dispatch_element`) |
| `parser_utils.c` | 확장자 검증 등 유틸리티 |
| `parse_elements.c` | A (ambient), C (camera), L (light) 파싱 |
| `parse_objects.c` | sp (sphere), pl (plane) 파싱 |
| `parse_cylinder.c` | cy (cylinder) 파싱 |
| `parse_number.c` | double/int 파싱 (`parse_double`, `parse_int`) |
| `parse_number_utils.c` | 정수부/소수부 파싱 헬퍼 |
| `parse_token.c` | 토큰 유틸 (`skip_whitespace`, `next_token`, `at_line_end`) |
| `parse_validation_strict.c` | 벡터/색상 strict 파싱 + 유효성 검증 |
| `parse_vector_validation.c` | 벡터 범위 검증, 방향 벡터 정규화 |
| `parse_line_reader.c` | 버퍼 기반 라인 리더 (GNL 대체) |
| `parse_error.c` | 에러 컨텍스트 관리 |
| `parse_error_msg.c` | 에러 메시지 매핑 |

---

## 파싱 흐름

```
parse_scene(filename, scene)
 ├── validate_extension()      # .rt 확장자 확인
 ├── open()                    # 파일 오픈
 ├── line_reader_init()        # 라인 리더 초기화
 ├── line_reader_next() loop   # 라인 단위 읽기
 │    └── dispatch_element()   # 식별자별 분기
 │         ├── parse_ambient()
 │         ├── parse_camera()
 │         ├── parse_light()
 │         ├── parse_sphere()
 │         ├── parse_plane()
 │         └── parse_cylinder()
 ├── validate_scene()          # 필수 요소 확인
 └── close()
```

---

## 에러 타입

`t_parse_result` 열거형으로 11가지 에러를 구분합니다:

| 코드 | 의미 |
|------|------|
| `PARSE_OK` | 성공 |
| `PARSE_ERR_FORMAT` | 형식 오류 |
| `PARSE_ERR_RANGE` | 값 범위 초과 |
| `PARSE_ERR_COMPONENT_COUNT` | 컴포넌트 수 불일치 |
| `PARSE_ERR_DUPLICATE` | 필수 요소 중복 (A, C, L) |
| `PARSE_ERR_LINE_TOO_LONG` | 라인 길이 초과 (4096) |
| `PARSE_ERR_ZERO_VECTOR` | 영벡터 (방향 벡터) |
| `PARSE_ERR_TRAILING_TOKEN` | 줄 끝에 불필요한 토큰 |
| `PARSE_ERR_UNKNOWN_ELEMENT` | 알 수 없는 식별자 |
| `PARSE_ERR_NUMBER_FORMAT` | 숫자 형식 오류 |
| `PARSE_ERR_MISSING_ELEMENT` | 필수 요소 누락 |
| `PARSE_ERR_IO` | 파일 I/O 에러 |

---

## 라인 리더

`t_line_reader`는 4096 바이트 버퍼를 사용하여 파일을 라인 단위로 읽습니다. GNL(get_next_line) 대신 직접 구현하여 버퍼 오버플로를 방지합니다.

```c
typedef struct s_line_reader
{
    int     fd;                             // 파일 디스크립터
    char    buffer[LINE_READER_BUFFER_SIZE]; // 읽기 버퍼
    int     buf_pos;                        // 현재 위치
    int     buf_len;                        // 버퍼 내 데이터 길이
    int     line_num;                       // 현재 줄 번호
    int     line_too_long;                  // 라인 길이 초과 플래그
    int     io_error;                       // I/O 에러 플래그
}   t_line_reader;
```

---

## 유효성 검증

### 값 범위
- **ratio/brightness**: 0.0 ~ 1.0
- **FOV**: 0 < fov < 180 (exclusive)
- **색상**: R, G, B 각 0 ~ 255
- **방향 벡터**: 각 성분 -1.0 ~ 1.0, 영벡터 거부 후 정규화

### 구조적 규칙
- A, C, L은 씬 당 정확히 1개 (중복 시 에러)
- sp, pl, cy는 0개 이상
- 빈 줄과 `#` 주석은 무시
- 각 줄의 식별자 뒤 추가 토큰은 에러
