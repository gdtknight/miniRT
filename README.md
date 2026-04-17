# miniRT — C 레이트레이서

[![Language](https://img.shields.io/badge/language-C-blue?style=flat-square)](https://en.wikipedia.org/wiki/C_(programming_language))
[![License](https://img.shields.io/badge/license-MIT-green?style=flat-square)](LICENSE)

C로 작성한 실시간 인터랙티브 레이트레이서. BVH 가속 구조, Phong 조명, 소프트 섀도우, 카메라/오브젝트 인터랙티브 조작을 지원합니다.

---

## 주요 기능

- **지오메트리**: 구(sphere), 평면(plane), 원기둥(cylinder), 원뿔(cone)
- **조명**: Phong 모델 (ambient + diffuse + specular)
- **그림자**: 하드/소프트 섀도우 (스토캐스틱 샘플링, offset LUT 사전 계산)
- **BVH 가속 구조**: median split, plane 분리, any-hit shadow 가속
- **인터랙티브 조작**: 카메라 이동/회전, 오브젝트 선택/이동/리사이즈/회전, 광원 이동/선택
- **HUD 오버레이**: 씬 정보, 성능 메트릭, 키가이드, 페이지네이션
- **텍스처**: 체커보드 패턴, XPM 범프맵

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
| **카메라 회전** | E / C | 상하 회전 |
| | 1 / 3 | 좌우 회전 |
| **카메라 리셋** | S | 초기 위치 복원 |
| **오브젝트 선택** | TAB | 다음 오브젝트 |
| **오브젝트 이동** | R/T, F/G, V/B | X, Y, Z축 |
| **오브젝트 리사이즈** | Y/U | 반지름 -/+ |
| | N/M | 높이 -/+ (원기둥/원뿔) |
| **오브젝트 회전** | I/J, O/K, P/L | X, Y, Z축 |
| **광원 선택** | = | 다음 광원 순환 |
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

# 조명 (필수, 1개 이상)
L  0,8,-8  0.7  255,255,255

# 구 (선택)
sp  0,0,0  6  255,0,0

# 평면 (선택)
pl  0,-10,0  0,1,0  200,200,200

# 원기둥 (선택)
cy  0,0,0  0,1,0  5  10  0,255,0

# 원뿔 (보너스)
co  0,5,0  0,1,0  4  8  255,128,0
```

| 식별자 | 형식 | 설명 |
|--------|------|------|
| A | `A <ratio> <R,G,B>` | 주변광 (ratio: 0.0~1.0) |
| C | `C <x,y,z> <nx,ny,nz> <fov>` | 카메라 (FOV: 1~179) |
| L | `L <x,y,z> <brightness> <R,G,B>` | 조명 (brightness: 0.0~1.0, 다중 광원) |
| sp | `sp <x,y,z> <diameter> <R,G,B>` | 구 |
| pl | `pl <x,y,z> <nx,ny,nz> <R,G,B>` | 평면 |
| cy | `cy <x,y,z> <nx,ny,nz> <d> <h> <R,G,B>` | 원기둥 |
| co | `co <x,y,z> <nx,ny,nz> <d> <h> <R,G,B>` | 원뿔 (보너스) |

보너스 옵션: 오브젝트 뒤에 `checker:R,G,B` (체커보드) 또는 `bump:file.xpm` (범프맵)을 추가할 수 있습니다.

---

## 프로젝트 구조

v2.8.0 이후 7개의 feature 기반 패키지로 재조직되었습니다.

```
miniRT/
├── src/
│   ├── main.c
│   ├── common/             # vec3, 에러, 타이머, 포맷 헬퍼
│   ├── scene/              # 씬 라이프사이클, 오브젝트 리스트
│   │   └── parser/         # .rt 파싱 (엄격 검증 + 보너스 옵션)
│   ├── spatial/
│   │   ├── aabb/           # AABB 생성/병합/교차
│   │   ├── bvh/            # BVH 구축/순회/any-hit
│   │   ├── debug/          # BVH 트리 콘솔 덤프 (`--bvh-vis`)
│   │   └── intersect/      # 광선-오브젝트 교차 (sp/pl/cy/co)
│   ├── render/             # 렌더 루프, 카메라, 디바운스, MLX 윈도우
│   ├── lighting/
│   │   ├── shading/        # Phong 모델
│   │   ├── shadow/         # 소프트 섀도우, offset LUT
│   │   └── texture/        # 체커보드, XPM 범프맵
│   ├── interact/           # 이벤트 디스패치 (close/key/expose)
│   │   ├── input/          # 키보드 핸들러
│   │   ├── hud/            # HUD 오버레이
│   │   └── keyguide/       # 온스크린 키 가이드
│   └── metrics/            # 프레임 타이밍, shadow/intersection 카운터
├── includes/               # 모든 헤더 (src/ 구조 미러링)
├── scenes/                 # 씬 파일 (valid/, invalid/, perf/)
├── textures/               # XPM 범프맵 에셋
├── lib/                    # libft, MiniLibX (서브모듈)
├── wiki/                   # Wiki 문서 소스
├── docs/                   # 상세 모듈 레퍼런스, CHANGELOG
├── specs/                  # Feature specs (speckit)
├── scripts/                # norminette 검사, 벤치마크 스크립트
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
| v2.8.1 | 2026-04 | 유지보수 아티팩트 정리 (`tests/`, 죽은 `scripts/` 15개, 낡은 `.gitignore` 항목) |
| v2.8.0 | 2026-04 | 7-패키지 구조 재조직 (common/scene/spatial/render/lighting/interact/metrics), 카메라 pitch 360° 복원 |
| v2.7.0 | 2026-03 | 헤더 의존성 최소화, 다량 심볼 리네이밍 (bvh_vis→bvh_debug, is_in_shadow→shadow_is_occluded 등) |
| v2.6.2 | 2026-03 | 카메라 상대 up 벡터로 수직 이동 수정 |
| v2.6.1 | 2026-03 | `lib/` 서브모듈 인라인, wiki 용어 갱신 |
| v2.6.0 | 2026-03 | 모듈 재구조화 (ray→intersect, lighting→shading+shadow), 헤더 리네이밍, 파서 강화 |
| v2.5.1 | 2026-03 | display 리네이밍, dead code 제거 (`aabb_surface_area` 등) |
| v2.5.0 | 2026-03 | 키 디스패치 바인드 테이블, `render_init` 통합, 다수 버그 수정 |
| v2.4.1 | 2026-03 | keyguide dirty 플래그 조건 수정 |
| v2.4.0 | 2026-03 | 모듈 리팩토링 (input/metrics 분리), 파일 리네이밍, 문서 전면 갱신 |
| v2.3.0 | 2026-02 | 디바운스 FSM 재설계, 키맵 재배치 (리사이즈/회전/광원), dead code 제거 |
| v2.2.0 | 2026-02 | Plane BVH 분리, shadow BVH any-hit, 4라운드 성능 최적화 (S4 77.7%↑) |
| v2.1.0 | 2026-01 | BVH 가속 기본 활성화, 디바운스 렌더링, BVH 시각화 |
| v2.0.0 | 2026-01 | 코드 리팩토링, 모듈화 개선 |

전체 변경 이력은 [CHANGELOG](docs/CHANGELOG.md)를 참조하세요.

---

## 라이선스

MIT License — [LICENSE](LICENSE) 참조
