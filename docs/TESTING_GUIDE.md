# miniRT 테스팅 가이드

**작성일**: 2025-12-15  
**목적**: 단위 테스트, 장면 렌더링, 메모리 누수 검증 가이드

---

## 빠른 시작

```bash
# 1. 전체 검증 실행
./scripts/verify_release.sh

# 2. 빌드 및 실행
make re
./miniRT scenes/test.rt

# 3. 메모리 누수 검사
valgrind --leak-check=full ./miniRT scenes/test.rt
```

---

## 1. 단위 테스트 (함수 수준)

### 1.1 Criterion 프레임워크 설치

```bash
# Ubuntu/Debian
sudo apt-get install libcriterion-dev

# macOS
brew install criterion

# 수동 설치
git clone https://github.com/Snaipe/Criterion
cd Criterion
meson build
ninja -C build install
```

### 1.2 테스트 파일 구조

```
tests/
├── unit/
│   ├── test_vector.c      # 벡터 연산 테스트
│   ├── test_parser.c      # 파서 테스트
│   ├── test_ray.c         # 광선 테스트
│   ├── test_sphere.c      # 구 교차점 테스트
│   ├── test_plane.c       # 평면 교차점 테스트
│   ├── test_cylinder.c    # 원기둥 교차점 테스트
│   └── test_lighting.c    # 조명 계산 테스트
└── Makefile
```

### 1.3 테스트 작성 예제

```c
// tests/unit/test_vector.c
#include <criterion/criterion.h>
#include "../../includes/vec3.h"

Test(vector, vec3_add)
{
    t_vec3 a = {1.0, 2.0, 3.0};
    t_vec3 b = {4.0, 5.0, 6.0};
    t_vec3 result = vec3_add(a, b);
    
    cr_assert_float_eq(result.x, 5.0, 1e-6);
    cr_assert_float_eq(result.y, 7.0, 1e-6);
    cr_assert_float_eq(result.z, 9.0, 1e-6);
}

Test(vector, vec3_normalize)
{
    t_vec3 v = {3.0, 4.0, 0.0};
    t_vec3 result = vec3_normalize(v);
    double magnitude = vec3_magnitude(result);
    
    cr_assert_float_eq(magnitude, 1.0, 1e-6);
}
```

### 1.4 테스트 실행

```bash
# 모든 테스트 실행
make test

# 특정 테스트만 실행
./tests/test_vector

# 상세 출력
./tests/test_vector --verbose

# 특정 테스트 케이스만
./tests/test_vector --filter=vector/vec3_add
```

### 1.5 필수 테스트 항목

✅ **수학 함수** (`src/math/`)
- [ ] 벡터 덧셈/뺄셈/곱셈
- [ ] 내적/외적
- [ ] 크기/정규화

✅ **파서** (`src/parser/`)
- [ ] 정상 파일 파싱
- [ ] 각 요소 타입 파싱 (A, C, L, sp, pl, cy)
- [ ] 범위 검증 (FOV, 비율, RGB)
- [ ] 오류 처리 (잘못된 형식, 범위 초과, 필수 요소 누락)

✅ **광선-객체 교차** (`src/ray/`, `src/objects/`)
- [ ] 광선-구 교차
- [ ] 광선-평면 교차
- [ ] 광선-원기둥 교차 (측면, 상단, 하단)
- [ ] 교차점 없음 케이스

✅ **조명 계산** (`src/lighting/`)
- [ ] 주변광 계산
- [ ] 확산광 계산
- [ ] 그림자 광선 테스트

---

## 2. 장면 파일 렌더링 검증

### 2.1 테스트 장면 파일 생성

```bash
# scenes/ 디렉토리 구조
scenes/
├── test_sphere.rt      # 구만
├── test_plane.rt       # 평면만
├── test_cylinder.rt    # 원기둥만
├── test_all.rt         # 모든 객체
├── test_lighting.rt    # 조명 효과
├── test_shadows.rt     # 그림자
├── error_invalid.rt    # 오류 테스트
└── reference/          # 참조 이미지
    ├── sphere.png
    ├── plane.png
    └── ...
```

### 2.2 기본 테스트 장면

**test_sphere.rt** - 단순 구 렌더링
```
A  0.2                     255,255,255
C  0,0,-20    0,0,1        70
L  -10,10,-10              0.7    255,255,255
sp 0,0,0                   10     255,0,0
```

**test_plane.rt** - 평면과 구
```
A  0.2                     255,255,255
C  0,5,-20    0,-0.2,1     70
L  0,10,-10                0.7    255,255,255
pl 0,0,0      0,1,0               200,200,200
sp 0,2,0                   4      0,0,255
```

**test_cylinder.rt** - 원기둥
```
A  0.2                     255,255,255
C  0,0,-20    0,0,1        70
L  -10,10,-10              0.7    255,255,255
cy 0,0,0      0,1,0        4  10  0,255,0
```

### 2.3 렌더링 테스트 실행

```bash
# 각 장면 렌더링
./miniRT scenes/test_sphere.rt
./miniRT scenes/test_plane.rt
./miniRT scenes/test_cylinder.rt
./miniRT scenes/test_all.rt

# 오류 케이스 테스트
./miniRT scenes/error_invalid.rt    # "Error" 출력 확인
./miniRT nonexistent.rt              # 파일 없음 에러
./miniRT                             # 사용법 메시지
```

### 2.4 시각적 검증 체크리스트

✅ **구 (Sphere)**
- [ ] 완벽한 원형 투영
- [ ] 중심이 정확한 위치
- [ ] 크기가 직경과 일치
- [ ] 표면이 부드러운 그라데이션

✅ **평면 (Plane)**
- [ ] 무한히 확장
- [ ] 법선 방향이 올바름
- [ ] 색상이 균일

✅ **원기둥 (Cylinder)**
- [ ] 축 방향이 정확
- [ ] 측면이 직선
- [ ] 상/하단 캡이 평평
- [ ] 높이와 직경이 정확

✅ **조명**
- [ ] 주변광으로 완전히 어두워지지 않음
- [ ] 확산광이 부드러운 그라데이션
- [ ] 그림자가 선명한 경계 (hard shadow)

---

## 3. 메모리 누수 검증

### 3.1 Valgrind 기본 사용법

```bash
# 기본 메모리 누수 검사
valgrind --leak-check=full ./miniRT scenes/test_simple.rt

# 상세 출력
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         ./miniRT scenes/test_simple.rt

# 로그 파일 저장
valgrind --leak-check=full \
         --log-file=valgrind-out.txt \
         ./miniRT scenes/test_simple.rt
```

### 3.2 예상 출력 (누수 없음)

```
==12345== HEAP SUMMARY:
==12345==     in use at exit: 0 bytes in 0 blocks
==12345==   total heap usage: 100 allocs, 100 frees, 10,000 bytes allocated
==12345== 
==12345== All heap blocks were freed -- no leaks are possible
==12345== 
==12345== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

### 3.3 모든 종료 경로 테스트

```bash
# 정상 종료 (ESC 키)
valgrind --leak-check=full ./miniRT scenes/test.rt
# -> 창 열림 -> ESC 누름 -> 종료

# X 버튼 클릭 종료
valgrind --leak-check=full ./miniRT scenes/test.rt
# -> 창 열림 -> X 버튼 클릭 -> 종료

# 오류 종료
valgrind --leak-check=full ./miniRT scenes/error_invalid.rt
# -> 에러 메시지 출력 -> 종료

# 파일 없음 오류
valgrind --leak-check=full ./miniRT nonexistent.rt
# -> 에러 메시지 출력 -> 종료

# 인자 없음 오류
valgrind --leak-check=full ./miniRT
# -> 사용법 메시지 -> 종료
```

### 3.4 메모리 누수 0 확인 사항

✅ **할당/해제 균형**
- `malloc` 호출 횟수 = `free` 호출 횟수
- 모든 할당된 메모리가 해제됨

✅ **정리 함수 호출**
- `cleanup_scene()` 호출 확인
- `cleanup_render()` 호출 확인
- `cleanup_all()` 호출 확인

✅ **오류 경로 정리**
- 파싱 오류 시 할당된 메모리 해제
- 렌더링 오류 시 리소스 해제
- 조기 종료 시 정리 함수 호출

### 3.5 일반적인 메모리 누수 원인

❌ **실수하기 쉬운 부분**
- 파서에서 오류 발생 시 이미 할당된 메모리 미해제
- MinilibX 이미지/창 리소스 미정리
- 연결 리스트 노드 미해제
- 오류 경로에서 `exit()` 직접 호출 (정리 함수 건너뜀)

---

## 4. 자동화 검증 스크립트

### 4.1 검증 스크립트 실행

```bash
# 전체 검증
./scripts/verify_release.sh

# 출력 예시:
# [1] Build System
# ✓ PASSED: Build success
# ✓ PASSED: Executable exists
# 
# [2] Norminette
# ✓ PASSED: Norminette OK
# 
# [3] 42 Headers
# ✓ PASSED: All headers present (27/27)
#
# Summary:
# Passed: 10 | Failed: 0 | Warnings: 2
# ✓ PASSED
```

### 4.2 CI/CD 통합 (선택사항)

```bash
# .github/workflows/test.yml
name: miniRT Tests

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Install dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y valgrind libcriterion-dev
      - name: Build
        run: make
      - name: Run tests
        run: make test
      - name: Memory leak check
        run: valgrind --leak-check=full --error-exitcode=1 ./miniRT scenes/test.rt
```

---

## 5. 체크리스트 요약

### ✅ 제출 전 필수 확인 사항

**빌드 & 코드 품질**
- [ ] `make` 빌드 성공
- [ ] `norminette` 모든 파일 통과
- [ ] 모든 파일에 42 헤더 포함
- [ ] 컴파일 경고 없음

**단위 테스트**
- [ ] 모든 수학 함수 테스트 통과
- [ ] 모든 파서 테스트 통과
- [ ] 모든 교차점 계산 테스트 통과
- [ ] 모든 조명 계산 테스트 통과
- [ ] 테스트 커버리지 > 80%

**장면 렌더링**
- [ ] 구 렌더링 정상
- [ ] 평면 렌더링 정상
- [ ] 원기둥 렌더링 정상
- [ ] 복합 장면 렌더링 정상
- [ ] 조명/그림자 효과 정상

**메모리 관리**
- [ ] Valgrind 메모리 누수 0
- [ ] 정상 종료 경로 누수 없음
- [ ] 오류 종료 경로 누수 없음
- [ ] ESC 종료 시 누수 없음
- [ ] X 버튼 종료 시 누수 없음

**문서화**
- [ ] README.md 작성 완료
- [ ] docs/ 한글 문서 작성
- [ ] 모든 함수 Doxygen 주석
- [ ] 업데이트 히스토리 기록

---

## 참고 자료

- **릴리즈 게이트 체크리스트**: `docs/release-gate-checklist-kr.md`
- **원본 체크리스트**: `specs/001-raytracer/checklists/release-gate.md`
- **프로젝트 헌법**: `.specify/memory/constitution.md`
- **요구사항**: `miniRT_requirements.md`

---

**작성**: GitHub Copilot CLI  
**최종 수정**: 2025-12-15
