# miniRT — C 레이트레이서

[![Language](https://img.shields.io/badge/language-C-blue?style=flat-square)](https://en.wikipedia.org/wiki/C_(programming_language))
[![License](https://img.shields.io/badge/license-MIT-green?style=flat-square)](LICENSE)

C로 작성한 실시간 인터랙티브 레이트레이서. BVH 가속 구조, Phong 조명, 소프트 섀도우, 카메라/오브젝트 인터랙티브 조작을 지원합니다.

---

## 주요 기능

- **지오메트리**: 구(sphere), 평면(plane), 원기둥(cylinder)
- **조명**: Phong 모델 (ambient + diffuse + specular)
- **그림자**: 하드/소프트 섀도우 (스토캐스틱 샘플링, offset LUT 사전 계산)
- **BVH 가속 구조**: median split, plane 분리, any-hit shadow 가속
- **인터랙티브 조작**: 카메라 이동/회전, 오브젝트 선택/이동/리사이즈/회전, 광원 이동
- **HUD 오버레이**: 씬 정보, 성능 메트릭, 키가이드

---

## 성능 벤치마크

> 측정 환경: Apple M1, 1440x900, `-O0` (최적화 플래그 없음)

### 4라운드 최적화 결과

| 라운드 | 변경 내용 | S2 (20sp) | S3 (50sp) | S4 (혼합 18obj) |
|--------|----------|-----------|-----------|-----------------|
| Baseline | — | 1,835.7ms | 3,366.0ms | 27,173.7ms |
| R1: BVH fallback 제거 | `check_all_objects()` 이중 탐색 삭제 | 1,142.4ms | 1,775.8ms | 26,784.4ms |
| R2: 수학 연산 최적화 | `fast_pow32`, sphere sqrt 캐싱, shadow mag 통합 | 1,142.8ms | 1,767.9ms | 26,178.2ms |
| R3: Shadow offset LUT | cos/sin/sqrt 사전 계산 | 1,122.9ms | 1,753.4ms | 25,583.8ms |
| R4: Plane 분리 + BVH pruning | plane BVH 분리, shadow BVH any-hit, threshold 하향 | 1,218.5ms | 1,432.2ms | **6,066.8ms** |

### Before / After 요약

| 씬 | Baseline | 최종 (R4) | 누적 개선율 |
|----|----------|-----------|-----------|
| S2 (20 spheres) | 1,835.7ms | 1,218.5ms | **33.6%** |
| S3 (50 spheres) | 3,366.0ms | 1,432.2ms | **57.4%** |
| S4 (7sp+7cy+3pl) | 27,173.7ms | 6,066.8ms | **77.7%** |

### 핵심 지표 변화 (S4)

| 지표 | Before | After |
|------|--------|-------|
| Shadow tests | 369M | 83.7M |
| BVH skip rate | 32.9% | 82.0% |
| Frame time | 27.2s | 6.1s |

---

## 빠른 시작

```bash
# 저장소 클론 (서브모듈 포함)
git clone --recursive https://github.com/gdtknight/miniRT.git
cd miniRT

# Linux: 필수 라이브러리 설치
sudo apt-get install libx11-dev libxext-dev libbsd-dev

# 빌드 및 실행
make
./miniRT scenes/valid/valid_smoke_simple.rt
```

### 조작법

| 카테고리 | 키 | 동작 |
|---------|-----|------|
| **카메라 이동** | W / X | 전진 / 후진 |
| | A / D | 좌 / 우 |
| | Q / Z | 상승 / 하강 |
| **카메라 회전** | E / C | 피치 위 / 아래 |
| **카메라 리셋** | S | 초기 위치 복원 |
| **오브젝트 선택** | TAB | 다음 오브젝트 |
| **오브젝트 이동** | R/T, F/G, V/B | X, Y, Z축 |
| **오브젝트 리사이즈** | Y/U | 반지름 -/+ |
| | N/M | 높이 -/+ (원기둥) |
| **오브젝트 회전** | I/J, O/K, P/L | X, Y, Z축 |
| **광원 이동** | [ / ], ; / ', , / . | X, Y, Z축 |
| **UI** | H | HUD 토글 |
| | Up / Down | HUD 페이지 이동 |
| | ESC | 종료 |

---

## 씬 파일 형식

`.rt` 파일로 3D 씬을 정의합니다.

```
# 주변광 (필수, 1개)
A  0.2  255,255,255

# 카메라 (필수, 1개)
C  0,0,-15  0,0,1  70

# 조명 (필수, 1개)
L  0,8,-8  0.7  255,255,255

# 구 (선택)
sp  0,0,0  6  255,0,0

# 평면 (선택)
pl  0,-10,0  0,1,0  200,200,200

# 원기둥 (선택)
cy  0,0,0  0,1,0  5  10  0,255,0
```

| 식별자 | 형식 | 설명 |
|--------|------|------|
| A | `A <ratio> <R,G,B>` | 주변광 (ratio: 0.0~1.0) |
| C | `C <x,y,z> <nx,ny,nz> <fov>` | 카메라 (FOV: 0~180) |
| L | `L <x,y,z> <brightness> <R,G,B>` | 조명 (brightness: 0.0~1.0) |
| sp | `sp <x,y,z> <diameter> <R,G,B>` | 구 |
| pl | `pl <x,y,z> <nx,ny,nz> <R,G,B>` | 평면 |
| cy | `cy <x,y,z> <nx,ny,nz> <d> <h> <R,G,B>` | 원기둥 |

---

## 프로젝트 구조

```
miniRT/
├── src/
│   ├── main.c              # 진입점
│   ├── parser/             # .rt 파일 파싱 (14개 파일)
│   ├── render/             # 렌더링 파이프라인, 메트릭
│   ├── spatial/            # BVH 공간 가속 구조
│   ├── lighting/           # Phong 조명, 그림자
│   ├── ray/                # 광선-오브젝트 교차 판정
│   ├── math/               # 벡터 연산
│   ├── hud/                # HUD 오버레이
│   ├── keyguide/           # 키가이드 렌더링
│   ├── window/             # MiniLibX 윈도우/이벤트
│   ├── scene/              # 씬 관리
│   ├── bvh_vis/            # BVH 시각화 (--bvh-vis)
│   └── utils/              # 에러 처리, 타이머
├── includes/               # 헤더 파일 (23개)
├── scenes/                 # 테스트 씬 파일 (29개)
├── lib/                    # libft, MiniLibX
├── docs/                   # Wiki 문서 소스
└── Makefile
```

---

## 빌드 요구사항

**Linux**
- gcc, make
- libx11-dev, libxext-dev, libbsd-dev

**macOS**
- clang, make
- OpenGL, AppKit (Xcode Command Line Tools)

---

## 릴리스 히스토리

| 버전 | 날짜 | 주요 변경 |
|------|------|----------|
| v2.4.0 | 2026-03 | 모듈 리팩토링 (input/metrics 분리), 파일 리네이밍, 문서 전면 갱신 |
| v2.3.0 | 2026-02 | 디바운스 FSM 재설계, 키맵 재배치 (리사이즈/회전/광원), dead code 제거 |
| v2.2.0 | 2026-02 | Plane BVH 분리, shadow BVH any-hit, 4라운드 성능 최적화 (S4 77.7%↑) |
| v2.1.0 | 2026-01 | BVH 가속 기본 활성화, 디바운스 렌더링, BVH 시각화 |
| v2.0.0 | 2026-01 | 코드 리팩토링, 모듈화 개선 |

전체 변경 이력은 [CHANGELOG](docs/CHANGELOG.md)를 참조하세요.

---

## 라이선스

MIT License — [LICENSE](LICENSE) 참조
