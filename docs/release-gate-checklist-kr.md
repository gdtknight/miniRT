# miniRT 릴리즈 게이트 체크리스트 (한글 설명)

**작성일**: 2025-12-15  
**목적**: 42 스쿨 프로젝트 제출 전 요구사항 품질 검증

---

## 개요

이 체크리스트는 miniRT 프로젝트의 제출 전 최종 검증을 위한 133개 항목을 포함하고 있습니다.
모든 항목은 **요구사항의 품질**을 테스트하며, 구현 검증이 아닌 명세 완성도를 확인합니다.

**원본 파일**: `specs/001-raytracer/checklists/release-gate.md`

---

## 주요 검증 영역

### 1. 단위 테스트 요구사항 (CHK093-CHK100)

**헌법 원칙**: "개별 함수에 대한 단위테스트가 반드시 필요해" (NON-NEGOTIABLE)

- **CHK093**: 모든 공개 함수에 대한 단위 테스트 요구사항이 명시되어 있는가?
- **CHK094**: 테스트 커버리지 목표가 정량화되어 있는가? (예: 90% 라인 커버리지)
- **CHK095**: Criterion 프레임워크 통합 요구사항이 문서화되어 있는가?
- **CHK096**: 수학 함수에 대한 테스트 정확도 요구사항이 명시되어 있는가?
- **CHK097**: 교차점 계산 함수에 대한 알려진 입력/출력 테스트 케이스가 정의되어 있는가?
- **CHK098**: 파싱 함수에 대한 정상/오류 케이스 테스트 요구사항이 명시되어 있는가?
- **CHK099**: 조명 계산 함수에 대한 테스트 시나리오가 문서화되어 있는가?
- **CHK100**: 테스트 실패 처리 요구사항이 지정되어 있는가?

**검증 방법**:
- `tests/unit/` 디렉토리에 각 모듈별 테스트 파일 존재 확인
- 각 `.c` 파일에 대응하는 `test_*.c` 파일 확인
- `make test` 명령어로 모든 테스트 실행 및 통과 여부 확인
- Criterion 테스트 결과 보고서 확인

---

### 2. 장면 파일 렌더링 검증 (CHK036-CHK043, CHK130-CHK133)

**헌법 원칙**: "프로젝트 전체가 제대로 컴파일이 되는지, 실행이 되는지, 실행 중에 에러가 발생하진 않는지 마지막에 항상 확인해야해"

#### 2.1 렌더링 출력 요구사항

- **CHK036**: 특정 장면 파일 참조가 있는 예상 렌더링 출력이 정의되어 있는가?
- **CHK037**: 픽셀 완벽 참조 이미지 또는 허용 오차 임계값이 정의되어 있는가?
- **CHK038**: 구 렌더링 요구사항이 측정 가능한가? (원형 투영, 왜곡 없음)
- **CHK039**: 원기둥 뚜껑 렌더링 요구사항이 명시적으로 정의되어 있는가?
- **CHK040**: 평면의 무한 범위 렌더링 요구사항이 명시되어 있는가?
- **CHK041**: 조명 그라데이션 요구사항이 정량화 가능한가? (부드러운 전환, 밴딩 없음)
- **CHK042**: 하드 섀도우 엣지 요구사항이 측정 가능한가? (선명한 경계)
- **CHK043**: 카메라 원근 요구사항이 정량화 가능한가? (올바른 FOV 왜곡)

#### 2.2 테스트 장면 파일 요구사항

- **CHK130**: 제출용 참조 장면 파일 요구사항이 명시되어 있는가?
- **CHK131**: 테스트 장면 다양성 요구사항이 문서화되어 있는가? (정상, 경계, 오류 케이스)
- **CHK132**: 각 기하학적 프리미티브에 대한 최소 테스트 장면 요구사항이 정의되어 있는가?
- **CHK133**: 평가 데모 시나리오 요구사항이 문서화되어 있는가?

**검증 방법**:
```bash
# 각 객체 타입별 테스트 장면
./miniRT scenes/test_sphere.rt      # 구만 있는 장면
./miniRT scenes/test_plane.rt       # 평면만 있는 장면
./miniRT scenes/test_cylinder.rt    # 원기둥만 있는 장면
./miniRT scenes/test_all.rt         # 모든 객체 포함
./miniRT scenes/test_lighting.rt    # 조명 효과 테스트
./miniRT scenes/test_shadows.rt     # 그림자 테스트
./miniRT scenes/test_multi.rt       # 복잡한 장면 (20+ 객체)

# 오류 케이스 테스트
./miniRT scenes/error_invalid.rt    # 잘못된 형식
./miniRT scenes/error_missing.rt    # 필수 요소 누락
./miniRT scenes/error_range.rt      # 범위 밖 값
```

**참조 이미지 생성**:
- `scenes/reference/` 디렉토리에 각 테스트 장면의 예상 출력 이미지 저장
- 렌더링 결과를 참조 이미지와 시각적으로 비교

---

### 3. 메모리 누수 검증 (CHK069-CHK076)

**헌법 원칙**: "작업 이후에는 개별 함수에 대한 단위테스트가 반드시 필요해" (메모리 관리 포함)

**CRITICAL**: 42 스쿨 제출 기준 - 메모리 누수 0% 필수

- **CHK069**: 메모리 누수 허용 오차가 정량화되어 있는가? (42 제출 기준: 0 바이트)
- **CHK070**: Valgrind 테스트 절차 요구사항이 명시되어 있는가?
- **CHK071**: Valgrind 플래그 사양이 문서화되어 있는가? (`--leak-check=full`)
- **CHK072**: 정리 함수 호출 요구사항이 모든 종료 경로에 대해 명시되어 있는가?
- **CHK073**: 오류 경로 메모리 정리 요구사항이 지정되어 있는가?
- **CHK074**: MinilibX 리소스 정리 요구사항이 문서화되어 있는가?
- **CHK075**: 장면 데이터 할당 해제 요구사항이 명시되어 있는가?
- **CHK076**: 파서 메모리 정리 요구사항이 지정되어 있는가?

**검증 방법**:

```bash
# Valgrind 기본 테스트
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=valgrind-out.txt \
         ./miniRT scenes/test_simple.rt

# 결과 확인
cat valgrind-out.txt | grep "definitely lost"
cat valgrind-out.txt | grep "indirectly lost"
cat valgrind-out.txt | grep "ERROR SUMMARY"

# 예상 결과:
# ==PID== LEAK SUMMARY:
# ==PID==    definitely lost: 0 bytes in 0 blocks
# ==PID==    indirectly lost: 0 bytes in 0 blocks
# ==PID==      possibly lost: 0 bytes in 0 blocks
# ==PID==    still reachable: 0 bytes in 0 blocks
# ==PID==         suppressed: 0 bytes in 0 blocks
# ==PID== ERROR SUMMARY: 0 errors from 0 contexts
```

**오류 경로 테스트**:
```bash
# 잘못된 파일로 메모리 누수 테스트
valgrind --leak-check=full ./miniRT scenes/invalid.rt
valgrind --leak-check=full ./miniRT nonexistent.rt
valgrind --leak-check=full ./miniRT

# ESC 키로 종료 시 메모리 정리 테스트
valgrind --leak-check=full ./miniRT scenes/test_simple.rt
# (창이 열리면 ESC 키로 종료)

# X 버튼으로 종료 시 메모리 정리 테스트
valgrind --leak-check=full ./miniRT scenes/test_simple.rt
# (창이 열리면 X 버튼으로 종료)
```

**정리 함수 검증**:
- `src/utils/cleanup.c`의 모든 정리 함수 단위 테스트
- `cleanup_scene()`: 장면 데이터 해제
- `cleanup_render()`: 렌더링 컨텍스트 해제
- `cleanup_all()`: 전체 리소스 해제

---

## 추가 검증 영역

### 4. 42 Norminette 준수 (CHK101-CHK105)

**헌법 원칙**: "42 norminette을 준수해야해" (NON-NEGOTIABLE)

```bash
# 전체 프로젝트 norminette 검사
norminette src/ includes/

# 개별 파일 검사
norminette src/main.c
norminette includes/minirt.h

# 예상 결과: 모든 파일 "OK!"
```

### 5. 빌드 시스템 검증 (CHK118-CHK129)

```bash
# Makefile 규칙 테스트
make all          # 정상 빌드
make clean        # 오브젝트 파일 삭제
make fclean       # 전체 정리
make re           # 재빌드
make dev          # 디버그 빌드 (-g -fsanitize=address)
make bonus        # 보너스 빌드

# 빌드 검증
ls miniRT         # 실행 파일 존재 확인
./miniRT          # 사용법 메시지 확인
```

### 6. 문서화 검증 (CHK106-CHK117)

**헌법 원칙**: "모든 함수, 코드를 한글로 설명하는 별도의 markdown을 프로젝트 루트/docs 디렉토리에 항상 생성해야해"

확인 사항:
- [ ] `docs/` 디렉토리에 각 모듈별 한글 설명 파일 존재
- [ ] 각 함수에 대한 한글 설명 포함
- [ ] Doxygen 주석이 모든 함수에 영문으로 작성됨
- [ ] `README.md`에 프로젝트 설명 및 히스토리 포함

---

## 체크리스트 사용 방법

### 1단계: 요구사항 완성도 확인
- CHK001-CHK030 항목 검토
- [Gap] 표시된 항목은 `spec.md`에 추가 필요

### 2단계: 단위 테스트 구현 및 실행
- CHK093-CHK100 항목 기반으로 테스트 작성
- `make test` 실행하여 모든 테스트 통과 확인

### 3단계: 장면 파일 렌더링 테스트
- CHK036-CHK043, CHK130-CHK133 항목 기반 테스트 장면 작성
- 각 장면 렌더링 및 시각적 검증

### 4단계: 메모리 누수 검증 (CRITICAL)
- CHK069-CHK076 항목 기반 Valgrind 테스트
- 모든 종료 경로에서 메모리 누수 0 확인

### 5단계: 최종 검증
- Norminette 준수 확인
- Makefile 규칙 테스트
- 문서화 완성도 확인
- 42 헤더 모든 파일 적용 확인

---

## 성공 기준

✅ **필수 통과 조건** (42 스쿨 제출 기준):
1. 모든 단위 테스트 통과 (Criterion)
2. Valgrind 메모리 누수 0 바이트
3. Norminette 모든 파일 OK
4. 모든 테스트 장면 정상 렌더링
5. ESC/X 버튼으로 정상 종료
6. 오류 케이스 적절한 에러 메시지 출력

✅ **문서화 요구사항** (헌법 준수):
1. 모든 함수에 영문 Doxygen 주석
2. `docs/` 디렉토리에 한글 설명 문서
3. `README.md` 프로젝트 설명 및 히스토리
4. 모든 파일에 42 헤더 포함

---

## 참고 자료

- **원본 체크리스트**: `specs/001-raytracer/checklists/release-gate.md`
- **프로젝트 명세**: `specs/001-raytracer/spec.md`
- **구현 계획**: `specs/001-raytracer/plan.md`
- **태스크 목록**: `specs/001-raytracer/tasks.md`
- **요구사항 문서**: `miniRT_requirements.md`
- **프로젝트 헌법**: `.specify/memory/constitution.md`

---

**작성자**: GitHub Copilot CLI  
**최종 수정**: 2025-12-15
