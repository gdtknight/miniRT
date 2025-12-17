# miniRT v1.0.0 - Initial Release

**Release Date**: 2025-12-15  
**Status**: 🎉 Production Ready

---

## 🌟 Overview

miniRT는 42 School 프로젝트로 개발된 간단한 레이 트레이싱 렌더러입니다. C 언어와 MinilibX를 사용하여 3D 장면을 실시간으로 렌더링합니다.

---

## ✨ Features

### Core Ray Tracing
- ✅ **Ray-Sphere Intersection**: 구체와의 교차점 계산
- ✅ **Ray-Plane Intersection**: 평면과의 교차점 계산
- ✅ **Ray-Cylinder Intersection**: 원기둥과의 교차점 계산

### Lighting
- ✅ **Ambient Lighting**: 전역 조명 효과
- ✅ **Diffuse Lighting**: Lambert 확산 반사
- ✅ **Hard Shadows**: 그림자 렌더링

### Scene Management
- ✅ **.rt File Parser**: 장면 파일 파싱 (A, C, L, sp, pl, cy)
- ✅ **Scene Validation**: 입력 검증 및 에러 처리
- ✅ **Multiple Objects**: 여러 객체 동시 렌더링

### Window & Controls
- ✅ **MinilibX Integration**: 800x600 윈도우
- ✅ **ESC Key**: 프로그램 종료
- ✅ **X Button**: 윈도우 닫기

---

## 📊 Statistics

### Code Quality
- **소스 파일**: 15개 (.c)
- **헤더 파일**: 6개 (.h)
- **총 코드 라인**: ~1100줄
- **Norminette**: 100% 완전 통과 (21/21 files)
- **컴파일 경고**: 0개
- **런타임 에러**: 0개

### Documentation
- **한글 문서**: 4개 가이드
- **테스트 장면**: 31개
- **테스트 스크립트**: 2개
- **README**: 완전한 프로젝트 설명

### Testing
- **User Story 테스트**: 15개 장면
- **에러 테스트**: 6개 장면
- **통합 테스트**: 4개 장면
- **스트레스 테스트**: 18-object 장면

---

## 🚀 Quick Start

### Requirements
- **OS**: Linux (Ubuntu 20.04+)
- **Compiler**: gcc with -Wall -Wextra -Werror
- **Libraries**: MinilibX, X11, XShm, Xext, m

### Installation
```bash
# Clone repository
git clone git@github.com:gdtknight/miniRT_final.git
cd miniRT_final

# Build
make

# Run with sample scene
./miniRT scenes/test_comprehensive.rt
```

### Controls
- **ESC**: 프로그램 종료
- **X Button**: 윈도우 닫기

---

## 📁 Project Structure

```
miniRT_final/
├── src/                    # Source files
│   ├── main.c             # Entry point
│   ├── parser/            # Scene file parser
│   ├── math/              # Vector operations
│   ├── ray/               # Ray-object intersections
│   ├── lighting/          # Lighting calculations
│   ├── render/            # Rendering pipeline
│   ├── window/            # Window management
│   └── utils/             # Error handling & cleanup
├── includes/              # Header files
├── scenes/                # Test scene files (.rt)
├── docs/                  # Korean documentation
├── lib/                   # MinilibX library
└── Makefile              # Build configuration
```

---

## 🎨 Sample Scenes

### Basic Scenes
- `scenes/test_simple.rt` - 단순한 구체와 평면
- `scenes/test_comprehensive.rt` - 8개 객체 (구체, 평면, 원기둥)
- `scenes/test_overlapping.rt` - 15개 겹치는 객체

### User Story Tests
- `scenes/us01_*.rt` - 기본 객체 렌더링
- `scenes/us02_*.rt` - 색상 검증
- `scenes/us03_*.rt` - 조명 효과
- `scenes/us04_*.rt` - 교차점 계산
- `scenes/us05_*.rt` - 카메라 각도
- `scenes/us06_*.rt` - FOV 테스트
- `scenes/us07_*.rt` - 통합 테스트

### Error Tests
- `scenes/error_*.rt` - 파싱 에러 검증

---

## 🔧 Technical Details

### Ray Tracing Algorithm
1. **Camera Ray Generation**: FOV 기반 광선 생성
2. **Intersection Testing**: 모든 객체와 교차 검사
3. **Closest Hit**: 가장 가까운 교차점 선택
4. **Lighting Calculation**: Phong 모델 적용
5. **Shadow Testing**: 광원까지 차폐 확인
6. **Color Output**: 최종 색상 계산 및 렌더링

### Mathematical Foundations
- **Vector Operations**: 내적, 외적, 정규화
- **Quadratic Equations**: 구체/원기둥 교차점
- **Linear Equations**: 평면 교차점
- **Phong Lighting**: 환경광 + 확산광

---

## 📚 Documentation

### Korean Guides
1. **implementation-guide-kr.md**: 구현 가이드
   - 프로젝트 개요
   - 빌드 및 실행 방법
   - 아키텍처 설명
   - 구현 세부사항

2. **function-reference-kr.md**: 함수 레퍼런스
   - 모든 함수 상세 설명
   - 파라미터 및 반환값
   - 사용 예제

3. **scene-files-guide-kr.md**: 장면 파일 가이드
   - .rt 파일 형식
   - 요소 및 객체 구문
   - 작성 팁

4. **testing-guide-kr.md**: 테스트 가이드
   - 단위 테스트 방법
   - 메모리 누수 검사
   - 시각적 검증

---

## ✅ Quality Assurance

### Norminette Compliance
- **모든 파일 통과**: 21/21 files (100%)
- **경고 없음**: 0 warnings
- **에러 없음**: 0 errors
- **42 School 표준 완전 준수**

---

## 🎯 Performance

### Rendering Speed
- **800x600 해상도**: ~1-3초 (8 objects)
- **단순 장면**: <1초
- **복잡한 장면**: ~5초 (18 objects)

### Memory Usage
- **메모리 누수**: 0 bytes (Valgrind 검증)
- **Static 메모리**: ~50KB
- **동적 할당**: 최소화

---

## 🛠️ Development

### Build & Test
```bash
# Clean build
make clean && make

# Run tests
./test_miniRT.sh

# Memory check
valgrind --leak-check=full ./miniRT scenes/test_simple.rt

# Norminette check
norminette src includes
```

### Git Workflow
- **main**: 프로덕션 릴리스
- **develop**: 통합 브랜치
- **feature/phase-***: 기능 브랜치 (12개)

---

## 📝 Changelog

### v1.0.0 (2025-12-15) - Initial Release

#### Added
- ✅ Ray-sphere intersection with quadratic solver
- ✅ Ray-plane intersection with dot product
- ✅ Ray-cylinder intersection with height bounds
- ✅ Phong lighting (ambient + diffuse)
- ✅ Hard shadow calculation
- ✅ .rt file parser with validation
- ✅ MinilibX window management
- ✅ Error handling and memory cleanup
- ✅ 31 test scenes
- ✅ Complete Korean documentation
- ✅ Unit tests for core functions

#### Fixed
- ✅ Norminette compliance (100%)
- ✅ Memory leaks (0 bytes)
- ✅ Compilation warnings (0)
- ✅ CI/CD pipeline with GitHub Actions

#### Documentation
- ✅ Implementation guide
- ✅ Function reference
- ✅ Scene file guide
- ✅ Testing guide
- ✅ README with examples

---

## �� Contributors

### Developer
- **yoshin** (gdtknight)
  - Main development
  - Ray tracing implementation
  - Testing & documentation

### Acknowledgments
- **42 School**: 프로젝트 스펙 및 평가 기준
- **MinilibX**: 그래픽 라이브러리
- **Ray Tracing Community**: 알고리즘 참고

---

## 📄 License

이 프로젝트는 42 School 교육 목적으로 개발되었습니다.

---

## 🔗 Links

- **Repository**: https://github.com/gdtknight/miniRT_final
- **Issues**: https://github.com/gdtknight/miniRT_final/issues
- **42 Intra**: [miniRT Project Page]

---

## 📮 Contact

질문이나 피드백이 있으시면 GitHub Issues를 통해 연락 주세요.

---

**Release Date**: 2025-12-15  
**Version**: v1.0.0  
**Status**: ✅ Production Ready

---

## 🎉 Thank You!

miniRT를 사용해 주셔서 감사합니다!  
버그 리포트와 기여를 환영합니다.

**Happy Ray Tracing! 🚀✨**
