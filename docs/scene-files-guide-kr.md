# miniRT 장면 파일 상세 설명 (한글)

**작성일**: 2025-12-15  
**총 장면 수**: 28개  
**목적**: 각 사용자 스토리 검증 및 테스트

---

## 목차

1. [사용자 스토리별 테스트 장면](#사용자-스토리별-테스트-장면)
2. [오류 케이스 테스트](#오류-케이스-테스트)
3. [추가 테스트 장면](#추가-테스트-장면)
4. [실행 방법](#실행-방법)
5. [검증 체크리스트](#검증-체크리스트)

---

## 사용자 스토리별 테스트 장면

### US-01: 기본 장면 렌더링 (3개 파일)

#### 1. `us01_basic_scene.rt` - 전체 요소 포함
```
내용: 평면(plane) + 구(sphere) + 원기둥(cylinder) + 조명
목적: 모든 기본 객체 타입이 하나의 장면에서 렌더링되는지 확인
검증:
  - 파란 평면이 바닥에 렌더링됨
  - 파란 구가 중앙에 렌더링됨
  - 파란 원기둥이 오른쪽에 렌더링됨
  - 조명이 객체들을 비춤
```

**실행**:
```bash
./miniRT scenes/us01_basic_scene.rt
```

**예상 결과**: 창이 열리고 세 가지 객체가 모두 표시되어야 함

---

#### 2. `us01_sphere_only.rt` - 구만 있는 단순 장면
```
내용: 구(sphere) 1개 + 조명
목적: 가장 단순한 렌더링 테스트
검증:
  - 빨간 구가 화면 중앙에 표시됨
  - 구가 완벽한 원형으로 보임
  - 조명에 의한 음영 효과가 보임
```

**실행**:
```bash
./miniRT scenes/us01_sphere_only.rt
```

**예상 결과**: 빨간 구 하나만 표시됨

---

#### 3. `us01_multiple_objects.rt` - 여러 객체
```
내용: 평면 1개 + 구 3개 + 조명
목적: 여러 객체가 동시에 렌더링되는지 확인
검증:
  - 회색 평면(바닥)
  - 파란 구 (중앙)
  - 빨간 구 (왼쪽)
  - 초록 구 (오른쪽)
  - 모든 객체가 동시에 보임
```

**실행**:
```bash
./miniRT scenes/us01_multiple_objects.rt
```

**예상 결과**: 평면 위에 세 개의 구가 표시됨

---

### US-02: 장면 파일 파싱 (1개 파일)

#### 4. `us02_parse_test.rt` - 파싱 테스트
```
내용: miniRT_requirements.md의 예제 장면
목적: 표준 파일 형식 파싱 검증
검증:
  - A(주변광), C(카메라), L(조명) 요소 파싱
  - sp(구), pl(평면), cy(원기둥) 객체 파싱
  - 공백과 줄바꿈 유연하게 처리
  - 요소 순서 무관하게 파싱
```

**실행**:
```bash
./miniRT scenes/us02_parse_test.rt
```

**예상 결과**: 모든 요소가 올바르게 파싱되고 렌더링됨

---

### US-03: 조명 효과 구현 (3개 파일)

#### 5. `us03_lighting_ambient.rt` - 주변광 테스트
```
내용: 어두운 주변광(0.1) + 평면 + 구 2개
목적: 주변광이 객체를 완전히 어둡지 않게 만드는지 확인
검증:
  - 조명이 직접 닿지 않는 부분도 약간 보임
  - 주변광 비율(0.1)에 따라 밝기가 조절됨
  - 모든 객체에 주변광이 균일하게 적용됨
```

**실행**:
```bash
./miniRT scenes/us03_lighting_ambient.rt
```

**예상 결과**: 그림자 부분도 완전히 검지 않고 약간 밝게 보임

---

#### 6. `us03_lighting_diffuse.rt` - 확산광 테스트
```
내용: 평면 + 구 3개 + 강한 조명
목적: 확산광으로 조명 방향에 따른 밝기 변화 확인
검증:
  - 조명을 향한 면이 더 밝음
  - 조명 반대편이 어두움
  - 부드러운 밝기 그라데이션
  - 각 객체마다 조명 효과가 다름
```

**실행**:
```bash
./miniRT scenes/us03_lighting_diffuse.rt
```

**예상 결과**: 각 구의 조명 받는 쪽이 밝고 반대편이 어두움

---

#### 7. `us03_shadows.rt` - 그림자 테스트
```
내용: 평면 + 구 2개 + 원기둥 + 조명
목적: 하드 섀도우(Hard Shadow) 구현 확인
검증:
  - 평면에 객체들의 그림자가 생김
  - 그림자 경계가 선명함 (하드 섀도우)
  - 그림자 방향이 조명 위치와 일치
  - 여러 객체의 그림자가 겹칠 수 있음
```

**실행**:
```bash
./miniRT scenes/us03_shadows.rt
```

**예상 결과**: 평면에 객체들의 선명한 그림자가 보임

---

### US-04: 객체 교차점 계산 (4개 파일)

#### 8. `us04_sphere_intersection.rt` - 구 교차점
```
내용: 구 1개
목적: 광선-구 교차점 계산 정확성 검증
검증:
  - 구가 완벽한 원형으로 보임
  - 구의 가장자리가 부드럽게 처리됨
  - 구의 뒷면이 올바르게 처리됨
```

**실행**:
```bash
./miniRT scenes/us04_sphere_intersection.rt
```

---

#### 9. `us04_plane_intersection.rt` - 평면 교차점
```
내용: 평면 2개 (바닥 + 벽)
목적: 광선-평면 교차점 계산 정확성 검증
검증:
  - 평면이 무한히 확장되는 것처럼 보임
  - 평면의 법선 방향이 올바름
  - 두 평면이 정확히 교차함
```

**실행**:
```bash
./miniRT scenes/us04_plane_intersection.rt
```

---

#### 10. `us04_cylinder_intersection.rt` - 원기둥 교차점
```
내용: 원기둥 3개 (다양한 방향)
목적: 광선-원기둥 교차점 계산 정확성 검증
검증:
  - 원기둥 측면이 직선으로 보임
  - 원기둥 상단/하단 캡이 평평함
  - 원기둥의 축 방향이 올바름
  - 다양한 방향의 원기둥이 모두 정확함
```

**실행**:
```bash
./miniRT scenes/us04_cylinder_intersection.rt
```

---

#### 11. `us04_all_intersections.rt` - 모든 교차점
```
내용: 평면 + 구 2개 + 원기둥
목적: 모든 객체 타입의 교차점이 동시에 정확한지 확인
검증:
  - 구, 평면, 원기둥이 모두 올바르게 렌더링됨
  - 객체들이 서로 겹칠 때 앞뒤 관계가 정확함
  - 가려진 부분이 올바르게 처리됨
```

**실행**:
```bash
./miniRT scenes/us04_all_intersections.rt
```

---

### US-05: 윈도우 제어 (1개 파일)

#### 12. `us05_window_test.rt` - 윈도우 테스트
```
내용: 단순 장면 (구 1개)
목적: 윈도우 상호작용 테스트
테스트 절차:
  1. 프로그램 실행
  2. ESC 키를 눌러 종료 → 정상 종료 확인
  3. 다시 실행
  4. X 버튼 클릭으로 종료 → 정상 종료 확인
  5. 창 최소화/복원 → 유동적으로 동작 확인
```

**실행**:
```bash
./miniRT scenes/us05_window_test.rt
# ESC 또는 X 버튼으로 종료 테스트
```

**메모리 누수 테스트**:
```bash
valgrind --leak-check=full ./miniRT scenes/us05_window_test.rt
# 창 열림 → ESC 눌러 종료 → 메모리 누수 0 확인
```

---

### US-06: 객체 변환 (1개 파일)

#### 13. `us06_transformations.rt` - 변환 테스트
```
내용: 다양한 위치와 방향의 평면, 구, 원기둥
목적: 객체의 이동(translation)과 회전(rotation) 확인
검증:
  - 평면의 법선 벡터가 올바르게 적용됨
  - 원기둥의 축 벡터가 올바르게 적용됨
  - 카메라의 방향 벡터가 올바르게 적용됨
  - 객체들이 지정된 위치에 정확히 배치됨
```

**실행**:
```bash
./miniRT scenes/us06_transformations.rt
```

---

### US-07: 객체 속성 조정 (2개 파일)

#### 14. `us07_sphere_sizes.rt` - 구 크기 테스트
```
내용: 같은 위치에 다른 크기의 구 3개 (겹침)
목적: 구의 직경(diameter) 파라미터 반영 확인
검증:
  - 작은 구 (직경 5)
  - 중간 구 (직경 10)
  - 큰 구 (직경 15)
  - 각 구의 크기가 직경에 비례함
```

**실행**:
```bash
./miniRT scenes/us07_sphere_sizes.rt
```

---

#### 15. `us07_cylinder_sizes.rt` - 원기둥 크기 테스트
```
내용: 다른 크기의 원기둥 3개
목적: 원기둥의 직경과 높이 파라미터 반영 확인
검증:
  - 작은 원기둥 (직경 4, 높이 8)
  - 중간 원기둥 (직경 6, 높이 12)
  - 큰 원기둥 (직경 8, 높이 16)
  - 크기가 파라미터와 일치함
```

**실행**:
```bash
./miniRT scenes/us07_cylinder_sizes.rt
```

---

## 오류 케이스 테스트

### 범위 오류 (3개 파일)

#### 16. `error_fov_out_of_range.rt`
```
오류: FOV 값이 180 (범위: 0-180, 경계값 테스트)
예상: "Error\n" + "FOV must be in range [0, 180]" 메시지
```

#### 17. `error_ambient_out_of_range.rt`
```
오류: 주변광 비율이 1.5 (범위: 0.0-1.0)
예상: "Error\n" + "Ambient ratio must be in range [0.0, 1.0]" 메시지
```

#### 18. `error_rgb_out_of_range.rt`
```
오류: RGB 값이 300 (범위: 0-255)
예상: "Error\n" + "RGB values must be in range [0, 255]" 메시지
```

---

### 필수 요소 오류 (2개 파일)

#### 19. `error_missing_sphere.rt`
```
오류: 렌더링할 객체 없음 (A, C, L만 있음)
예상: "Error\n" + "No objects to render" 메시지
또는: 정상 실행 (빈 장면 렌더링)
```

#### 20. `error_duplicate_ambient.rt`
```
오류: A(주변광) 요소가 2번 선언됨
예상: "Error\n" + "Ambient lighting declared multiple times" 메시지
```

---

### 형식 오류 (1개 파일)

#### 21. `error_invalid_format.rt`
```
오류: 유효하지 않은 파일 내용
예상: "Error\n" + "Invalid scene file format" 메시지
```

---

## 추가 테스트 장면

### 조명 강도 테스트 (2개 파일)

#### 22. `test_dark_ambient.rt` - 어두운 주변광
```
내용: 매우 어두운 주변광 (0.05) + 약한 조명 (0.3)
목적: 낮은 조명 환경에서의 렌더링 확인
검증: 장면이 전체적으로 어두우나 객체 구분 가능
```

#### 23. `test_bright_ambient.rt` - 밝은 주변광
```
내용: 밝은 주변광 (0.8) + 약한 조명 (0.2)
목적: 높은 주변광 환경에서의 렌더링 확인
검증: 장면이 전체적으로 밝고 그림자가 흐릿함
```

---

### FOV 테스트 (2개 파일)

#### 24. `test_fov_narrow.rt` - 좁은 FOV (30도)
```
목적: 좁은 시야각으로 확대된 듯한 효과 확인
검증: 구가 크게 보이고 시야각이 좁음
```

#### 25. `test_fov_wide.rt` - 넓은 FOV (120도)
```
목적: 넓은 시야각으로 광각 렌즈 효과 확인
검증: 여러 객체가 한 번에 보이고 왜곡 발생
```

---

### 카메라 각도 테스트 (1개 파일)

#### 26. `test_camera_angle.rt` - 카메라 각도 변경
```
내용: 비스듬한 각도에서 본 복잡한 장면
목적: 카메라 방향 벡터가 올바르게 적용되는지 확인
검증: 카메라가 대각선 방향을 보고 있음
```

---

### 복잡한 장면 (1개 파일)

#### 27. `test_complex_scene.rt` - 복잡한 장면
```
내용: 평면 4개 + 구 3개 + 원기둥 2개 + 강한 조명
목적: 성능 및 복잡한 장면 렌더링 테스트
검증:
  - 모든 객체가 올바르게 렌더링됨
  - 렌더링 시간이 합리적임 (< 30초)
  - 메모리 사용량이 적절함
```

---

### 최소 장면 (1개 파일)

#### 28. `test_simple.rt` - 최소 장면
```
내용: 구 1개 (가장 단순)
목적: 기본 기능 빠른 테스트
용도: 
  - 빠른 기능 확인
  - Valgrind 메모리 테스트
  - 디버깅용 최소 장면
```

---

## 실행 방법

### 개별 실행
```bash
# 특정 장면 실행
./miniRT scenes/us01_basic_scene.rt

# ESC 또는 X 버튼으로 종료
```

### 자동 테스트
```bash
# 모든 장면 테스트
./scripts/test_scenes.sh

# 출력 예시:
# [1] Testing Valid Scenes
# ✓ us01_basic_scene.rt
# ✓ us01_sphere_only.rt
# ...
# Summary: 30 tests, 30 passed
```

### 메모리 누수 테스트
```bash
# 개별 장면 메모리 테스트
valgrind --leak-check=full ./miniRT scenes/test_simple.rt

# 모든 종료 경로 테스트
valgrind --leak-check=full ./miniRT scenes/test_simple.rt
# → ESC로 종료

valgrind --leak-check=full ./miniRT scenes/test_simple.rt
# → X 버튼으로 종료
```

---

## 검증 체크리스트

### ✅ US-01: 기본 장면 렌더링
- [ ] `us01_basic_scene.rt` 실행 시 3가지 객체 모두 표시
- [ ] `us01_sphere_only.rt` 실행 시 구 하나만 표시
- [ ] `us01_multiple_objects.rt` 실행 시 여러 구 동시 표시

### ✅ US-02: 장면 파일 파싱
- [ ] `us02_parse_test.rt` 모든 요소 정상 파싱
- [ ] 공백과 줄바꿈 유연하게 처리
- [ ] 요소 순서 무관하게 동작

### ✅ US-03: 조명 효과
- [ ] `us03_lighting_ambient.rt` 주변광 적용 확인
- [ ] `us03_lighting_diffuse.rt` 확산광 그라데이션 확인
- [ ] `us03_shadows.rt` 선명한 그림자 확인

### ✅ US-04: 교차점 계산
- [ ] `us04_sphere_intersection.rt` 구가 완벽한 원형
- [ ] `us04_plane_intersection.rt` 평면이 무한 확장
- [ ] `us04_cylinder_intersection.rt` 원기둥이 정확한 형태
- [ ] `us04_all_intersections.rt` 모든 객체 정확

### ✅ US-05: 윈도우 제어
- [ ] `us05_window_test.rt` ESC 키로 종료
- [ ] X 버튼으로 종료
- [ ] 창 최소화/복원 유동적
- [ ] 메모리 누수 0

### ✅ US-06: 객체 변환
- [ ] `us06_transformations.rt` 모든 변환 정확

### ✅ US-07: 속성 조정
- [ ] `us07_sphere_sizes.rt` 구 크기 정확
- [ ] `us07_cylinder_sizes.rt` 원기둥 크기 정확

### ❌ 오류 케이스
- [ ] 모든 `error_*.rt` 파일에서 "Error" 메시지 출력
- [ ] 명확한 에러 설명 포함
- [ ] 정상 종료 (크래시 없음)

---

## 성능 기준

- **단순 장면** (< 5개 객체): < 5초
- **복잡한 장면** (< 20개 객체): < 30초
- **메모리 누수**: 0 바이트
- **메모리 사용량**: < 100MB

---

## 참고 자료

- **장면 파일 형식**: `miniRT_requirements.md`
- **테스팅 가이드**: `docs/TESTING_GUIDE.md`
- **체크리스트**: `docs/release-gate-checklist-kr.md`
- **장면 README**: `scenes/README.md`

---

**작성자**: GitHub Copilot CLI  
**최종 수정**: 2025-12-15  
**총 장면 파일**: 28개  
**테스트 스크립트**: `scripts/test_scenes.sh`
