# miniRT - Ray Tracing Renderer

[![Release](https://img.shields.io/github/v/release/gdtknight/miniRT_final?style=flat-square)](https://github.com/gdtknight/miniRT_final/releases/latest)
[![42 School](https://img.shields.io/badge/42-miniRT-00babc?style=flat-square)](https://github.com/42School)
[![Norminette](https://img.shields.io/badge/norminette-passing-success?style=flat-square)](https://github.com/42School/norminette)
[![Language](https://img.shields.io/badge/language-C-blue?style=flat-square)](https://en.wikipedia.org/wiki/C_(programming_language))
[![License](https://img.shields.io/badge/license-MIT-green?style=flat-square)](LICENSE)
[![Wiki](https://img.shields.io/badge/wiki-한글_문서-blue?style=flat-square)](https://github.com/gdtknight/miniRT_final/wiki)

> **42 Seoul 프로젝트** - C 언어와 MinilibX로 구현한 레이 트레이싱 렌더러

miniRT는 광선 추적(Ray Tracing) 기법을 이용하여 3D 장면을 사실적인 2D 이미지로 렌더링하는 렌더러입니다. Phong 조명 모델, 소프트 섀도우, 반사 효과 등을 지원합니다.

---

## 📋 목차

- [주요 특징](#-주요-특징)
- [예제 렌더링](#-예제-렌더링)
- [빠른 시작](#-빠른-시작)
- [장면 파일 형식](#-장면-파일-형식)
- [프로젝트 구조](#-프로젝트-구조)
- [개발 히스토리](#-개발-히스토리)
- [기여 및 라이선스](#-기여-및-라이선스)

---

## ✨ 주요 특징

### 🎨 렌더링 기능

- **기하학적 객체**
  - 구체 (Sphere)
  - 평면 (Plane)
  - 원기둥 (Cylinder) - 캡(Cap) 포함

- **조명 시스템**
  - Ambient Light (주변광)
  - Diffuse Reflection (난반사 - Lambert 모델)
  - Specular Reflection (정반사 - Phong 모델) ⭐
  - Soft Shadows (부드러운 그림자) ⭐
  - 적응형 Shadow Bias ⭐

- **카메라**
  - 위치, 방향, FOV 설정 가능
  - 원근 투영 (Perspective Projection)
  - 직교 정규 기저 (ONB) 기반

### 🛠️ 기술적 특징

- ✅ **42 Norminette 준수** - 코딩 스타일 가이드 준수
- ✅ **메모리 안전성** - Valgrind 테스트 통과 (메모리 누수 0)
- ✅ **강건한 에러 처리** - 입력 검증 및 명확한 에러 메시지
- ✅ **완전한 한글 문서** - [Wiki](https://github.com/gdtknight/miniRT_final/wiki) 참조

---

## 🖼️ 예제 렌더링

### 기본 장면

<!-- 
TODO: 스크린샷 추가
- test_simple.rt: 기본 구체와 평면
- test_comprehensive.rt: 다중 객체 종합 테스트
-->

```bash
# 기본 장면 렌더링
./miniRT scenes/test_simple.rt
```

### 조명 효과

<!-- 
TODO: 스크린샷 추가
- Ambient + Diffuse
- Specular highlights
-->

```bash
# Phong 조명 모델 데모
./miniRT scenes/us03_lighting_diffuse.rt
```

### 그림자 효과

<!-- 
TODO: 스크린샷 추가
- Soft shadows with multiple samples
- Hard vs Soft shadow 비교
-->

```bash
# 소프트 섀도우 데모
./miniRT scenes/us04_shadow_soft.rt
```

### 복합 장면

<!-- 
TODO: 스크린샷 추가
- test_comprehensive.rt (18개 객체)
-->

```bash
# 종합 테스트 (18개 객체)
./miniRT scenes/test_comprehensive.rt
```

**📸 Note**: 실제 렌더링 이미지는 프로그램을 실행하여 확인할 수 있습니다.

---

## 🚀 빠른 시작

### 설치

```bash
# 1. 저장소 클론
git clone https://github.com/gdtknight/miniRT_final.git
cd miniRT_final

# 2. 필수 라이브러리 설치 (Ubuntu/Debian)
sudo apt-get update
sudo apt-get install libx11-dev libxext-dev libbsd-dev

# 3. 빌드
make

# 4. 실행
./miniRT scenes/test_simple.rt
```

### 기본 사용법

```bash
# 기본 실행
./miniRT <scene_file.rt>

# 예제 장면들
./miniRT scenes/test_simple.rt           # 간단한 테스트
./miniRT scenes/us03_lighting_diffuse.rt # 조명 효과
./miniRT scenes/us04_shadow_soft.rt      # 그림자 효과
./miniRT scenes/test_comprehensive.rt    # 종합 테스트
```

### 종료

- **ESC 키** - 프로그램 종료
- **창 닫기 버튼 (X)** - 프로그램 종료

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
| **A** | `A <ratio> <R,G,B>` | 주변광 (0.0~1.0) |
| **C** | `C <x,y,z> <nx,ny,nz> <fov>` | 카메라 (FOV: 0~180) |
| **L** | `L <x,y,z> <brightness> <R,G,B>` | 조명 (0.0~1.0) |
| **sp** | `sp <x,y,z> <diameter> <R,G,B>` | 구체 |
| **pl** | `pl <x,y,z> <nx,ny,nz> <R,G,B>` | 평면 |
| **cy** | `cy <x,y,z> <nx,ny,nz> <d> <h> <R,G,B>` | 원기둥 |

**📖 자세한 설명**: [Wiki - 씬 파일 파싱](https://github.com/gdtknight/miniRT_final/wiki/씬-파일-파싱)

---

## 📁 프로젝트 구조

```
miniRT_final/
├── src/                    # 소스 코드
│   ├── main.c             # 진입점
│   ├── lighting/          # 조명 및 그림자
│   ├── math/              # 벡터 연산
│   ├── parser/            # 장면 파일 파싱
│   ├── ray/               # 광선-객체 교차
│   ├── render/            # 렌더링 파이프라인
│   ├── utils/             # 유틸리티
│   └── window/            # 윈도우 관리
│
├── includes/              # 헤더 파일
├── scenes/                # 테스트 장면 (32개)
├── docs/                  # 문서
├── lib/                   # 외부 라이브러리
└── Makefile              # 빌드 설정
```

---

## 📚 문서 (Wiki)

완전한 한글 문서가 GitHub Wiki에 제공됩니다:

### 🏠 [Wiki 홈](https://github.com/gdtknight/miniRT_final/wiki)

### 기본 정보
- **[프로그램 개요](https://github.com/gdtknight/miniRT_final/wiki/프로그램-개요)** - 프로젝트 소개, 지원 기능, 실행 방법
- **[프로그램 흐름](https://github.com/gdtknight/miniRT_final/wiki/프로그램-흐름)** - 전체 실행 흐름과 렌더링 파이프라인

### 핵심 알고리즘
- **[레이 트레이싱 알고리즘](https://github.com/gdtknight/miniRT_final/wiki/레이-트레이싱-알고리즘)** - 광선-객체 교차 검사 (구, 평면, 원기둥)
- **[조명 그림자 시스템](https://github.com/gdtknight/miniRT_final/wiki/조명-그림자-시스템)** - Phong 조명, Soft Shadows, Shadow Acne 방지

### 구현 세부사항
- **[씬 파일 파싱](https://github.com/gdtknight/miniRT_final/wiki/씬-파일-파싱)** - .rt 파일 형식 명세 및 파싱 프로세스
- **[벡터 수학](https://github.com/gdtknight/miniRT_final/wiki/벡터-수학)** - 벡터 연산, 내적, 외적, 정규화, 최적화

### 학습 가이드

Wiki에는 다음 내용이 포함되어 있습니다:
- ✅ 수학 공식 및 원리 설명
- ✅ 실제 구현 코드 예제
- ✅ 디버깅 및 최적화 팁
- ✅ 초보자를 위한 학습 순서
- ✅ 문제 해결 가이드

---

## 🧪 테스트

### 자동 테스트

```bash
# 모든 장면 파일 테스트
./test_miniRT.sh

# Norminette 검사
norminette src/ includes/

# 메모리 누수 검사
valgrind --leak-check=full ./miniRT scenes/test_simple.rt
```

### 제공되는 테스트 장면 (32개)

- **기본 테스트** (4개): simple, lighting, cylinders, comprehensive
- **사용자 스토리 테스트** (15개): 카메라, 조명, 그림자, 원기둥, 다중 객체
- **에러 테스트** (6개): 잘못된 입력 처리
- **통합 테스트** (7개): 성능 및 복합 기능

---

## 🛠️ 기술 스택

### 핵심 기술
- **C99** - ISO/IEC 9899:1999 표준
- **MinilibX** - X11 기반 그래픽 라이브러리
- **42 Norminette** - 코딩 스타일 가이드

### 알고리즘
- **Ray Tracing** - 기본 광선 추적
- **Phong Lighting** - Ambient + Diffuse + Specular
- **Soft Shadows** - 다중 샘플링 기반
- **Lambert Reflection** - 난반사 계산

### 개발 도구
- **Make** - 빌드 자동화
- **GCC/Clang** - 컴파일러
- **Valgrind** - 메모리 검사
- **Git** - 버전 관리

---

## 📅 개발 히스토리

### v1.2.0 (2025-12-18) - Realistic Shadows ⭐ Latest

**주요 기능**
- ✅ Soft Shadows - 다중 샘플링으로 부드러운 그림자
- ✅ Specular Reflection - Phong 모델 정반사 하이라이트
- ✅ 적응형 Shadow Bias - Shadow Acne 방지
- ✅ 완전한 한글 Wiki 문서 (7개 페이지)

**구현**
- 13개의 상세 커밋 (논리적 단위 분리)
- 모든 헤더 파일 문서화 (영문)
- docs/00-전체설명/ 한글 문서 추가
- ft_atoi, ft_atof 유틸리티 함수 추가

**테스트**
- ✅ 빌드 성공
- ✅ Norminette 통과
- ✅ 32개 테스트 씬 검증

**🔗 링크**: [Release v1.2.0](https://github.com/gdtknight/miniRT_final/releases/tag/v1.2.0) | [Wiki](https://github.com/gdtknight/miniRT_final/wiki)

### v1.1.0 (2025-12-17) - Constitution

- ✅ Constitution v1.2.1 제정 및 준수
- ✅ GitHub Issue 추적 시스템 도입
- ✅ 문서 구조 재정비

### v1.0.0 (2025-12-15) - Initial Release ✨

- ✅ 기본 레이 트레이싱 기능
- ✅ 구, 평면, 원기둥 렌더링
- ✅ Phong 조명 (Ambient + Diffuse)
- ✅ 하드 그림자
- ✅ .rt 파일 파서
- ✅ 31개 테스트 장면

**📊 전체 변경 이력**: [Releases](https://github.com/gdtknight/miniRT_final/releases)

### 향후 계획 (Roadmap)

- 🔮 반사 (Reflection)
- 🔮 굴절 (Refraction)
- 🔮 텍스처 매핑
- 🔮 안티앨리어싱
- 🔮 다중 조명 지원
- 🔮 BVH 가속 구조
- 🔮 멀티스레딩

---

## 🤝 기여 및 라이선스

### 기여

이 프로젝트는 42 School 개인 과제이지만, 피드백과 제안은 환영합니다!

- **Issues**: [버그 리포트 및 기능 제안](https://github.com/gdtknight/miniRT_final/issues)
- **Pull Requests**: 코드 개선 제안 (42 Norminette 준수 필수)

### 라이선스

MIT License - 자세한 내용은 [LICENSE](LICENSE) 참조

### 연락처

- **GitHub**: [@gdtknight](https://github.com/gdtknight)
- **Repository**: [miniRT_final](https://github.com/gdtknight/miniRT_final)

---

## 🔗 관련 링크

- **GitHub Repository**: https://github.com/gdtknight/miniRT_final
- **Latest Release**: https://github.com/gdtknight/miniRT_final/releases/latest
- **Wiki (한글 문서)**: https://github.com/gdtknight/miniRT_final/wiki
- **Issues**: https://github.com/gdtknight/miniRT_final/issues
- **42 Seoul**: https://www.42seoul.kr/

---

## 📝 문제 해결 (Troubleshooting)

### MinilibX 빌드 오류
```bash
sudo apt-get install libx11-dev libxext-dev libbsd-dev
make re
```

### 씬 파일 로딩 오류
```bash
# 파일 형식 확인 (UTF-8, Unix LF)
file scenes/test_simple.rt

# 문법 검증 (에러 케이스로 테스트)
./miniRT scenes/error_invalid_format.rt
```

### 메모리 오류
```bash
# Valgrind로 디버깅
valgrind --leak-check=full ./miniRT scenes/test_simple.rt
```

더 많은 정보는 [Wiki](https://github.com/gdtknight/miniRT_final/wiki)를 참조하세요.

---

<div align="center">

**⭐ 이 프로젝트가 도움이 되었다면 Star를 눌러주세요! ⭐**

Made with ❤️ by [gdtknight](https://github.com/gdtknight)

[🏠 Wiki](https://github.com/gdtknight/miniRT_final/wiki) | [📦 Releases](https://github.com/gdtknight/miniRT_final/releases) | [🐛 Issues](https://github.com/gdtknight/miniRT_final/issues)

</div>
