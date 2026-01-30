# miniRT - Advanced Ray Tracing Renderer

[![42 School](https://img.shields.io/badge/42-miniRT-00babc?style=flat-square)](https://github.com/42School)
[![Norminette](https://img.shields.io/badge/norminette-passing-success?style=flat-square)](https://github.com/42School/norminette)
[![Language](https://img.shields.io/badge/language-C-blue?style=flat-square)](https://en.wikipedia.org/wiki/C_(programming_language))
[![License](https://img.shields.io/badge/license-MIT-green?style=flat-square)](LICENSE)

> **42 School 프로젝트** - C 언어와 MinilibX로 구현한 고성능 레이 트레이싱 렌더러

miniRT는 광선 추적(Ray Tracing) 기법을 이용하여 3D 장면을 사실적인 2D 이미지로 렌더링하는 렌더러입니다. Phong 조명 모델, 소프트 섀도우, BVH 가속 구조, 실시간 인터랙티브 컨트롤을 지원합니다.

---

## ✨ 주요 특징

### 🎨 렌더링 기능

- **기하학적 객체**
  - 구체 (Sphere) - 완전한 교차 검사
  - 평면 (Plane) - 무한 평면
  - 원기둥 (Cylinder) - 양쪽 캡(Cap) 포함

- **조명 시스템**
  - Ambient Light (주변광)
  - Diffuse Reflection (난반사 - Lambert 모델)
  - Specular Reflection (정반사 - Phong 모델) ⭐
  - Soft Shadows (부드러운 그림자) ⭐
  - 적응형 Shadow Bias (Shadow Acne 방지) ⭐

- **성능 최적화** ⭐
  - **BVH (Bounding Volume Hierarchy)** - 공간 가속 구조
    - 복잡한 장면에서 10-100배 성능 향상
    - 항상 활성화되어 최적 성능 보장
  - **적응형 품질 제어** - 인터랙션 중 자동 품질 조정
  - **타일 기반 렌더링** - 효율적인 화면 갱신

- **실시간 인터랙티브 컨트롤** ⭐
  - 카메라 이동 및 회전 (W/A/S/D/Q/E/Z/C)
  - 객체 선택 및 이동 ([ ] 키로 선택, R/T/F/G/V/B로 이동)
  - 광원 위치 조정 (Insert/Delete/Home/End/PageUp/PageDown)
  - HUD 정보 표시 (H 키로 토글)
  - 성능 메트릭 표시 (I 키로 토글)

### 🛠️ 기술적 특징

- ✅ **42 Norminette 준수** - 모든 코딩 스타일 규칙 준수
- ✅ **메모리 안전성** - Valgrind/leaks 테스트 통과 (메모리 누수 0)
- ✅ **모듈화 설계** - 명확한 책임 분리 및 계층 구조
- ✅ **강건한 에러 처리** - 포괄적인 입력 검증 및 명확한 에러 메시지
- ✅ **완전한 문서화** - 소스 코드 주석 및 Wiki 문서

---

## 🚀 빠른 시작

### 필수 요구사항

- **OS**: Linux (Ubuntu 20.04+) 또는 macOS
- **컴파일러**: GCC 또는 Clang
- **라이브러리**: X11, Xext (Linux), AppKit (macOS)
- **Make**: GNU Make 3.81+

### 설치

```bash
# 1. 저장소 클론
git clone https://github.com/YOUR_USERNAME/miniRT.git
cd miniRT

# 2. 필수 라이브러리 설치 (Ubuntu/Debian)
sudo apt-get update
sudo apt-get install libx11-dev libxext-dev libbsd-dev

# 3. 빌드
make

# 4. 실행
./miniRT scenes/valid/valid_smoke_simple.rt
```

### 기본 사용법

```bash
# 기본 실행
./miniRT <scene_file.rt>

# 예제 장면들
./miniRT scenes/valid/valid_smoke_simple.rt           # 간단한 테스트
./miniRT scenes/valid/valid_scene_comprehensive.rt    # 종합 테스트 (40개 객체)
```

### 인터랙티브 컨트롤

#### 🎮 카메라 컨트롤
- **W/X** - 전진/후진 (시야 방향)
- **A/D** - 좌/우 이동 (스트레이프)
- **Q/Z** - 상승/하강 (월드 Y축)
- **E/C** - 피치 회전 (위/아래 고개)
- **S** - 초기 위치로 리셋

#### 🎯 객체 조작
- **[ / ]** - 이전/다음 객체 선택
- **R/T** - X축 이동 (좌/우)
- **F/G** - Y축 이동 (하/상)
- **V/B** - Z축 이동 (전/후)

#### 💡 광원 조작
- **Insert/Delete** - X축 이동
- **Home/End** - Y축 이동
- **PageUp/PageDown** - Z축 이동

#### 🎨 UI 컨트롤
- **H** - HUD 표시/숨김
- **I** - 성능 정보 표시/숨김
- **ESC** - 프로그램 종료

---

## 📄 장면 파일 형식

장면 파일(`.rt`)은 렌더링할 3D 장면을 정의합니다.

### 기본 예제

```rt
# 주변광 (필수 1개)
A  0.2  255,255,255

# 카메라 (필수 1개)
C  0,0,20  0,0,-1  70

# 조명 (필수 1개)
L  -10,10,10  0.7  255,255,255

# 구체 (선택)
sp  0,0,0  12.6  255,0,0

# 평면 (선택)
pl  0,-10,0  0,1,0  200,200,200

# 원기둥 (선택)
cy  0,0,0  0,1,0  5  10  0,255,0
```

### 객체 형식

| 식별자 | 형식 | 설명 |
|--------|------|------|
| **A** | `A <ratio> <R,G,B>` | 주변광 강도 (0.0~1.0) |
| **C** | `C <x,y,z> <nx,ny,nz> <fov>` | 카메라 위치, 방향, FOV (0~180°) |
| **L** | `L <x,y,z> <brightness> <R,G,B>` | 조명 위치, 밝기 (0.0~1.0), 색상 |
| **sp** | `sp <x,y,z> <diameter> <R,G,B>` | 구체 중심, 지름, 색상 |
| **pl** | `pl <x,y,z> <nx,ny,nz> <R,G,B>` | 평면 점, 법선, 색상 |
| **cy** | `cy <x,y,z> <nx,ny,nz> <d> <h> <R,G,B>` | 원기둥 중심, 축, 지름, 높이, 색상 |

---

## 📁 프로젝트 구조

```
miniRT/
├── src/                    # 소스 코드
│   ├── main.c             # 프로그램 진입점
│   ├── hud/               # HUD 시스템 (정보 표시)
│   ├── keyguide/          # 키가이드 렌더링
│   ├── lighting/          # 조명 및 그림자 시스템
│   ├── math/              # 벡터 및 수학 연산
│   ├── parser/            # 장면 파일 파싱
│   ├── ray/               # 광선-객체 교차 검사
│   ├── render/            # 렌더링 파이프라인
│   ├── spatial/           # BVH 공간 가속 구조
│   ├── utils/             # 유틸리티 함수
│   └── window/            # 윈도우 및 이벤트 관리
│
├── includes/              # 헤더 파일
│   ├── minirt.h          # 메인 헤더
│   ├── objects.h         # 객체 정의
│   ├── render_state.h    # 렌더 상태
│   ├── spatial.h         # BVH 구조
│   ├── hud*.h            # HUD 관련 헤더
│   └── ...
│
├── scenes/                # 테스트 장면 (40개)
├── docs/                  # 프로젝트 문서
├── wiki/                  # GitHub Wiki 소스
├── lib/                   # 외부 라이브러리 (MinilibX)
├── scripts/               # 빌드 및 테스트 스크립트
├── tests/                 # 단위 및 통합 테스트
└── Makefile              # 빌드 설정
```

---

## 🧪 테스트

### 빌드 및 실행 테스트

```bash
# 빌드
make

# 재빌드
make re

# 정리
make clean      # 오브젝트 파일 삭제
make fclean     # 모든 빌드 파일 삭제
```

### 코드 품질 검사

```bash
# Norminette 검사
norminette src/ includes/

# 메모리 누수 검사 (Linux)
valgrind --leak-check=full --show-leak-kinds=all ./miniRT scenes/valid/valid_smoke_simple.rt

# 메모리 누수 검사 (macOS)
leaks -atExit -- ./miniRT scenes/valid/valid_smoke_simple.rt
```

### 제공되는 테스트 장면 (40개)

- **기본 테스트** (4개): simple, comprehensive, lighting, cylinders
- **기능 테스트** (36개): 다양한 조명, 그림자, 객체 배치 시나리오

---

## 📚 문서

### 로컬 문서
- **[문서 인덱스](docs/INDEX.md)** - 전체 문서 구조 및 빠른 참조
- **[문서 가이드](docs/README.md)** - 문서 사용법 및 학습 순서

### GitHub Wiki

완전한 한글 문서가 Wiki에 제공됩니다:

- **[Home](wiki/Home.md)** - Wiki 홈페이지
- **[프로그램 개요](wiki/프로그램-개요.md)** - 프로젝트 소개 및 주요 기능
- **[빠른 시작 가이드](wiki/빠른-시작-가이드.md)** - 설치 및 실행 방법
- **[레이 트레이싱 알고리즘](wiki/레이-트레이싱-알고리즘.md)** - 광선 추적 원리
- **[조명 시스템](wiki/조명-시스템.md)** - Phong 조명 및 그림자
- **[BVH 가속 구조](wiki/BVH-가속-구조.md)** - 공간 최적화
- **[씬 파일 형식](wiki/씬-파일-형식.md)** - .rt 파일 명세
- **[인터랙티브 컨트롤](wiki/인터랙티브-컨트롤.md)** - 키보드 조작 가이드
- **[개발 가이드](wiki/개발-가이드.md)** - 코드 구조 및 확장 방법
- **[FAQ](wiki/FAQ.md)** - 자주 묻는 질문

---

## 🛠️ 기술 스택

### 핵심 기술
- **C99** - ISO/IEC 9899:1999 표준
- **MinilibX** - X11/AppKit 기반 그래픽 라이브러리
- **42 Norminette** - 42 School 코딩 스타일 가이드

### 알고리즘
- **Ray Tracing** - 광선-객체 교차 검사
- **Phong Lighting** - Ambient + Diffuse + Specular
- **Soft Shadows** - 다중 샘플링 기반 부드러운 그림자
- **BVH** - Bounding Volume Hierarchy 가속 구조
- **AABB** - Axis-Aligned Bounding Box

### 개발 도구
- **Make** - 빌드 자동화
- **GCC/Clang** - C 컴파일러
- **Valgrind/leaks** - 메모리 검사
- **Git** - 버전 관리
- **Norminette** - 코드 스타일 검사

---

## 📊 프로젝트 통계

### 코드 규모
- **소스 파일**: 50+ (.c 파일)
- **헤더 파일**: 15+ (.h 파일)
- **총 라인 수**: 10,000+ 줄
- **테스트 씬**: 40개
- **문서 페이지**: 20+ (한글/영문)

### 구현 완료 기능
- ✅ 기본 레이 트레이싱 (구, 평면, 원기둥)
- ✅ Phong 조명 모델 (Ambient + Diffuse + Specular)
- ✅ Soft Shadows (다중 샘플링)
- ✅ 적응형 Shadow Bias (Shadow Acne 방지)
- ✅ BVH 공간 가속 구조
- ✅ 실시간 인터랙티브 컨트롤 (카메라, 객체, 광원)
- ✅ HUD 정보 표시 시스템
- ✅ 성능 메트릭 모니터링
- ✅ 장면 파일 파서 (.rt 형식)
- ✅ 메모리 안전성 (누수 0)
- ✅ 42 Norminette 완전 준수

### 품질 지표
- ✅ Norminette: 100% 통과
- ✅ 메모리 누수: 0 bytes
- ✅ 빌드 경고: 0개
- ✅ 테스트 장면: 40개 모두 검증

---

## 📅 개발 히스토리

### 최근 업데이트

#### v2.0.0 (2026-01-06) - Norminette 완전 준수 ⭐ Latest
- ✅ 42 Norminette 100% 준수 (모든 에러 수정)
- ✅ 코드 리팩토링 및 최적화
- ✅ 함수 분리 및 모듈화 개선
- ✅ 문서 정리 및 구조화

#### v1.5.0 (2025-12-20) - 성능 최적화
- ✅ BVH (Bounding Volume Hierarchy) 구현
- ✅ 복잡한 장면에서 10-100배 성능 향상
- ✅ 적응형 품질 제어 시스템

#### v1.2.0 (2025-12-18) - 사실적 그림자
- ✅ Soft Shadows 구현
- ✅ Specular Reflection (Phong 모델)
- ✅ 적응형 Shadow Bias

#### v1.0.0 (2025-12-15) - 초기 릴리스 ✨
- ✅ 기본 레이 트레이싱 구현
- ✅ Phong 조명 (Ambient + Diffuse)
- ✅ 장면 파일 파서

---

## 🤝 기여

이 프로젝트는 42 School 개인 과제이지만, 피드백과 제안은 환영합니다!

### 기여 방법
1. 이슈 생성 - 버그 리포트 또는 기능 제안
2. Fork 및 Pull Request - 코드 개선 제안
3. 문서 개선 - Wiki 또는 README 업데이트

### 코드 기여 시 주의사항
- ✅ 42 Norminette 준수 필수
- ✅ 메모리 누수 검사 통과 필수
- ✅ 기존 테스트 통과 필수

---

## 📄 라이선스

MIT License

Copyright (c) 2025 miniRT Team

자세한 내용은 [LICENSE](LICENSE) 파일을 참조하세요.

---

## 🔗 관련 링크

- **42 Seoul**: https://www.42seoul.kr/
- **42 Norminette**: https://github.com/42School/norminette
- **MinilibX**: https://github.com/42Paris/minilibx-linux
- **Ray Tracing in One Weekend**: https://raytracing.github.io/

---

## 📝 문제 해결

### 빌드 오류

```bash
# MinilibX 라이브러리 오류
sudo apt-get install libx11-dev libxext-dev libbsd-dev
make re

# 컴파일러 오류
make clean && make
```

### 실행 오류

```bash
# 장면 파일 경로 확인
ls scenes/

# 장면 파일 형식 검증
cat scenes/valid/valid_smoke_simple.rt

# 권한 확인
chmod +x miniRT
```

### 메모리 오류

```bash
# Valgrind 디버깅 (Linux)
valgrind --leak-check=full --track-origins=yes ./miniRT scenes/valid/valid_smoke_simple.rt

# leaks 디버깅 (macOS)
leaks -atExit -- ./miniRT scenes/valid/valid_smoke_simple.rt
```

### 성능 이슈

- BVH가 자동으로 활성화됩니다 (복잡한 장면 최적화)
- 객체 수가 많으면 초기 BVH 빌드 시간이 소요될 수 있습니다
- HUD를 숨기면 (H 키) 약간의 성능 향상이 있습니다

---

<div align="center">

**⭐ 이 프로젝트가 도움이 되었다면 Star를 눌러주세요! ⭐**

Made with ❤️ by miniRT Team

[📚 Wiki](wiki/Home.md) | [🐛 Issues](../../issues) | [📦 Releases](../../releases)

</div>
