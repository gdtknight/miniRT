# Quickstart: Evaluation Compliance Fixes

## 빌드 & 실행

```bash
make re
./miniRT scenes/test_basic.rt
```

## 변경 파일 목록

| 파일 | 변경 유형 | 내용 |
|------|----------|------|
| `includes/window_internal.h` | 수정 | KEY_J/K/N/M/U/O/Y/P/LEFT/RIGHT 상수 추가 |
| `includes/window.h` | 수정 | RENDER_BVH_DIRTY 플래그 추가 |
| `src/parser/parser.c` | 수정 | .rt 확장자 검증 추가 |
| `src/window/window_init.c` | 수정 | expose hook 등록 |
| `src/window/window_events.c` | 수정 | handle_expose() 추가 |
| `src/window/window_key_handlers.c` | 수정 | resize/rotation 키 dispatch |
| `src/window/window_objects.c` | 수정 | handle_object_rotate() 추가 |
| `src/window/window_resize.c` | 신규 | handle_object_resize() |
| `src/ray/intersect_cyl_new.c` | 수정 | body 내부 법선 플립 |
| `src/render/render.c` | 수정 | BVH dirty 체크 + rebuild |

## 구현 순서

1. **Phase A**: `parser.c` — .rt 확장자 검증 (독립)
2. **Phase B**: `window_internal.h` + `window_key_handlers.c` — 키 상수 및 dispatch (독립)
3. **Phase C**: `window_resize.c` — 리사이즈 핸들러 (B 의존)
4. **Phase D**: `window_objects.c` — 회전 핸들러 (B 의존)
5. **Phase E**: `window.h` + `render.c` — BVH dirty rebuild (C,D 의존)
6. **Phase F**: `window_events.c` + `window_init.c` — expose handler (독립)
7. **Phase G**: `intersect_cyl_new.c` — 내부 법선 보정 (독립)

## 키 매핑 요약

| 키 | 기능 |
|-----|------|
| J / K | 지름 증가 / 감소 |
| N / M | 높이 증가 / 감소 (실린더만) |
| U / O | X축 회전 +/- |
| Y / P | Y축 회전 +/- |
| ← / → | Z축 회전 +/- |

## 검증 테스트 씬

### 확장자 테스트
```bash
./miniRT scene.txt 2>&1  # "Error\n" + 설명 메시지 출력 확인 (stderr)
./miniRT scene 2>&1      # 동일 형식 확인
./miniRT scene.rt        # 정상 파싱 확인
```
검증 기준: stderr 첫 줄이 정확히 `Error`이고 두 번째 줄부터 설명 메시지 존재.

### 리사이즈 테스트 씬 (resize_test.rt)
```
A  0.2                255,255,255
C  0,0,-30  0,0,1     70
L  -20,20,-20         0.7  255,255,255

sp 0,0,0              10   255,0,0
cy 15,0,0  0,1,0      8  20  0,0,255
```

### 회전 테스트 씬 (rotation_test.rt)
```
A  0.2                255,255,255
C  0,0,-40  0,0,1     70
L  -20,20,-20         0.7  255,255,255

cy 0,0,0  0,1,0       6  30  0,255,0
pl 0,-10,0  0,1,0               200,200,200
```

### 내부 교차 테스트 씬 (inside_test.rt)
```
A  0.3                255,255,255
C  0,0,0  0,0,1       90
L  0,10,0             0.8  255,255,255

cy 0,0,10  0,0,1      50  100  100,100,255
sp 0,0,15             20       255,100,100
```

## 통합 검증 체크리스트

### 키 매핑 검증 (각 키 입력 → 변화 확인)

```
resize_test.rt 로드 후:
[ ] 구 선택 → J: 지름 증가 (구가 커짐)
[ ] 구 선택 → K: 지름 감소 (구가 작아짐, 지름 0.2 미만 불가 — 반지름 최소 0.1, research R5)
[ ] 구 선택 → N/M: 무시 (변화 없음)
[ ] 실린더 선택 → J: 지름 증가
[ ] 실린더 선택 → K: 지름 감소
[ ] 실린더 선택 → N: 높이 증가
[ ] 실린더 선택 → M: 높이 감소
[ ] 평면 선택 → J/K/N/M: 무시 (FR-011)

rotation_test.rt 로드 후:
[ ] 실린더 선택 → U: X축 회전 확인
[ ] 실린더 선택 → O: X축 역방향 회전 확인
[ ] 실린더 선택 → Y: Y축 회전 확인
[ ] 실린더 선택 → P: Y축 역방향 회전 확인
[ ] 실린더 선택 → LEFT: Z축 회전 확인
[ ] 실린더 선택 → RIGHT: Z축 역방향 회전 확인
[ ] 평면 선택 → U/O/Y/P/LEFT/RIGHT: 법선 방향 변화 확인
[ ] 구 선택 → 회전 키: 무시 (FR-006)

축 퇴화 검증 (FR-012):
[ ] 실린더 축이 회전 축과 평행한 상태에서 반복 회전 → 축 벡터가 zero가 되지 않음
[ ] 연속 회전 18회(= 90°) 후 축 벡터가 정규화 상태 유지 (길이 ≈ 1.0)
```

### Window Stability 검증 (FR-007)

```
[ ] 프로그램 실행 중 다른 창으로 가린 후 복원 → 콘텐츠 정상
[ ] 프로그램 최소화 후 복원 → 콘텐츠 정상
[ ] OS 윈도우 드래그 resize 시도 → 내부 이미지 깨짐/빈 화면 없음
[ ] ESC로 정상 종료 확인
[ ] 닫기 버튼(X)으로 정상 종료 확인
```

### 렌더 중 입력 안전 검증 (FR-013)

```
복잡 씬(10+ 오브젝트) 로드 후:
[ ] 렌더 진행 중 resize 키(J/K) 빠르게 연속 입력 → crash 없음
[ ] 렌더 진행 중 rotation 키(U/O) 빠르게 연속 입력 → crash 없음
[ ] 렌더 진행 중 이동 키(R/T/F/G) 빠르게 연속 입력 → crash 없음
[ ] 위 3가지 혼합 입력 → crash 없음, 최종 렌더 결과 정상
```

### 실린더 내부 교차 검증 (FR-008)

```
inside_test.rt 로드 후:
[ ] 카메라가 실린더 내부에서 벽면/캡을 정상 색상으로 표시
[ ] 검은 면 또는 무한 원근 아티팩트 없음
[ ] 구-실린더 교차 경계에서 z-fighting/깜빡임 없음
[ ] 카메라를 이동(WASD)하여 교차 경계를 다양한 각도에서 확인
```

### 키코드 플랫폼 검증

```
[ ] macOS: 모든 신규 키(J/K/N/M/U/O/Y/P/LEFT/RIGHT) 입력 수신 확인
[ ] Linux: 동일 키 입력 수신 확인 (해당 환경 접근 가능 시)
```
