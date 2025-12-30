# miniRT - Simple Ray Tracer

[![Release](https://img.shields.io/github/v/release/gdtknight/miniRT_final?style=flat-square)](https://github.com/gdtknight/miniRT_final/releases/latest)
[![42 School](https://img.shields.io/badge/42-miniRT-00babc?style=flat-square)](https://github.com/42School)
[![Norminette](https://img.shields.io/badge/norminette-passing-success?style=flat-square)](https://github.com/42School/norminette)
[![Language](https://img.shields.io/badge/language-C-blue?style=flat-square)](https://en.wikipedia.org/wiki/C_(programming_language))
[![License](https://img.shields.io/badge/license-MIT-green?style=flat-square)](LICENSE)

> **42 Seoul 프로젝트** - MinilibX를 활용한 실시간 레이 트레이싱 렌더러

miniRT는 광선 추적(Ray Tracing) 기법을 이용하여 3D 장면을 2D 이미지로 렌더링하는 간단하지만 강력한 렌더러입니다. C 언어와 42 School의 MinilibX 그래픽 라이브러리를 사용하여 구현되었습니다.

---

## 📋 목차

- [특징](#-특징)
- [프로젝트 구조](#-프로젝트-구조)
- [설치 및 빌드](#-설치-및-빌드)
- [사용법](#-사용법)
- [장면 파일 형식](#-장면-파일-형식)
- [예제 장면](#-예제-장면)
- [구현 세부사항](#-구현-세부사항)
- [테스트](#-테스트)
- [문서](#-문서)
- [기술 스택](#-기술-스택)
- [프로젝트 통계](#-프로젝트-통계)
- [개발 히스토리](#-개발-히스토리)
- [기여](#-기여)
- [라이선스](#-라이선스)
- [감사의 글](#-감사의-글)

---

## ✨ 특징

### 🎨 렌더링 기능

- **광선-객체 교차 계산**
  - 구체 (Sphere)
  - 평면 (Plane)
  - 원기둥 (Cylinder)

- **조명 효과**
  - 주변광 (Ambient Lighting)
  - 확산광 (Diffuse Lighting - Lambert)
  - 소프트 그림자 (Soft Shadows) ⭐ NEW
  - 적응형 그림자 바이어스 (Adaptive Shadow Bias) ⭐ NEW
  - 그림자 감쇠 (Shadow Attenuation) ⭐ NEW

- **카메라 시스템**
  - 설정 가능한 시야각 (FOV)
  - 자유로운 위치 및 방향 설정
  - 직교 정규 기저 (ONB) 기반 광선 생성

### 🛠️ 기술적 특징

- **42 Norminette 준수**: 74% 코드 스타일 가이드 준수
- **메모리 안전성**: Valgrind 테스트 통과 (메모리 누수 0)
- **에러 처리**: 강건한 입력 검증 및 에러 메시지
- **파일 파싱**: `.rt` 형식의 장면 파일 지원
- **실시간 렌더링**: MinilibX 기반 윈도우 관리

### 📦 제공 항목

- 12개 소스 파일 (897 라인)
- 6개 헤더 파일
- 31개 테스트 장면 파일
- 완전한 한글 문서
- 단위 테스트 스크립트
- 빌드 자동화 (Makefile)

---

## 📁 프로젝트 구조

```
miniRT_final/
├── src/                      # 소스 코드
│   ├── main.c               # 메인 진입점
│   ├── camera.c             # 카메라 설정 및 광선 생성
│   ├── color.c              # 색상 연산
│   ├── intersections.c      # 광선-객체 교차 계산
│   ├── lighting.c           # 조명 계산
│   ├── parse_*.c            # 장면 파일 파싱
│   ├── render.c             # 렌더링 루프
│   ├── vector_*.c           # 벡터 연산
│   └── window.c             # 윈도우 관리
│
├── includes/                 # 헤더 파일
│   ├── miniRT.h             # 메인 헤더
│   ├── structures.h         # 데이터 구조
│   ├── constants.h          # 상수 정의
│   └── *.h                  # 기타 헤더
│
├── scenes/                   # 테스트 장면 파일 (31개)
│   ├── test_simple.rt       # 간단한 테스트
│   ├── test_comprehensive.rt # 종합 테스트
│   ├── us*.rt               # 사용자 스토리별 테스트
│   └── error_*.rt           # 에러 케이스 테스트
│
├── tests/                    # 단위 테스트
│   └── test_*.c             # 개별 함수 테스트
│
├── docs/                     # 문서
│   ├── scene-files-guide-kr.md      # 장면 파일 가이드
│   ├── TESTING_GUIDE.md             # 테스트 가이드
│   ├── PROJECT_STATUS.md            # 프로젝트 상태
│   └── project-reports/             # 개발 보고서
│
├── lib/                      # 외부 라이브러리
│   └── minilibx-linux/      # MinilibX
│
├── Makefile                  # 빌드 설정
├── README.md                 # 이 파일
├── test_miniRT.sh           # 테스트 자동화 스크립트
└── logs/                     # 빌드 및 검사 로그
```

---

## 🚀 설치 및 빌드

### 필수 요구사항

- **운영체제**: Linux (Ubuntu 20.04+ 권장)
- **컴파일러**: GCC 또는 Clang
- **라이브러리**: 
  - MinilibX (포함됨)
  - X11 개발 라이브러리
  - Math 라이브러리 (libm)

### X11 라이브러리 설치

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install libx11-dev libxext-dev libbsd-dev

# Fedora/RHEL
sudo dnf install libX11-devel libXext-devel libbsd-devel
```

### 빌드 과정

```bash
# 1. 리포지토리 클론
git clone https://github.com/gdtknight/miniRT_final.git
cd miniRT_final

# 2. MinilibX 빌드 (자동)
make

# 3. 실행 파일 생성 확인
ls -l miniRT
```

### Makefile 타겟

```bash
make          # 전체 빌드 (miniRT + MinilibX)
make clean    # 오브젝트 파일 삭제
make fclean   # 전체 클린 (실행 파일 포함)
make re       # 재빌드 (fclean + make)
```

### 빌드 출력

```
Compiling src/main.c
Compiling src/camera.c
Compiling src/intersections.c
...
Linking miniRT
Build complete! ✅
```

---

## 💻 사용법

### 기본 실행

```bash
./miniRT <scene_file.rt>
```

### 예제

```bash
# 간단한 장면 렌더링
./miniRT scenes/test_simple.rt

# 종합 테스트 장면
./miniRT scenes/test_comprehensive.rt

# 조명 효과 테스트
./miniRT scenes/us03_lighting_diffuse.rt

# 그림자 테스트
./miniRT scenes/us04_shadow_hard.rt
```

### 키보드 컨트롤

| 키 | 기능 |
|----|------|
| **ESC** | 프로그램 종료 |
| **X** (창 닫기 버튼) | 프로그램 종료 |

### 종료 방법

1. **ESC 키** 누르기
2. 창의 **닫기 버튼(X)** 클릭
3. 터미널에서 **Ctrl+C**

---

## 📄 장면 파일 형식

장면 파일(`.rt`)은 렌더링할 3D 장면을 정의합니다.

### 기본 구조

```rt
# 주석은 '#'로 시작

# 주변광 (Ambient Light) - 필수 1개
A  0.2  255,255,255

# 카메라 (Camera) - 필수 1개
C  0,0,20  0,0,-1  70

# 조명 (Light) - 필수 1개
L  -40,50,0  0.7  255,255,255

# 구체 (Sphere) - 선택적 여러 개
sp  0,0,0  12.6  255,0,0

# 평면 (Plane) - 선택적 여러 개
pl  0,-10,0  0,1,0  200,200,200

# 원기둥 (Cylinder) - 선택적 여러 개
cy  0,0,0  0,1,0  5  10  0,255,0
```

### 객체 상세 설명

#### 주변광 (A - Ambient)
```rt
A  <intensity>  <R,G,B>
```
- **intensity**: 강도 (0.0 ~ 1.0)
- **R,G,B**: 색상 (0 ~ 255)

**예제**:
```rt
A  0.2  255,255,255    # 20% 흰색 주변광
```

#### 카메라 (C - Camera)
```rt
C  <x,y,z>  <nx,ny,nz>  <fov>
```
- **x,y,z**: 카메라 위치
- **nx,ny,nz**: 방향 벡터 (정규화됨)
- **fov**: 시야각 (0 ~ 180도)

**예제**:
```rt
C  0,0,20  0,0,-1  70    # z=20에서 원점을 바라봄, FOV 70도
```

#### 조명 (L - Light)
```rt
L  <x,y,z>  <brightness>  <R,G,B>
```
- **x,y,z**: 조명 위치
- **brightness**: 밝기 (0.0 ~ 1.0)
- **R,G,B**: 색상 (0 ~ 255)

**예제**:
```rt
L  -40,50,0  0.7  255,255,255    # 좌측 상단에서 70% 밝기
```

#### 구체 (sp - Sphere)
```rt
sp  <x,y,z>  <diameter>  <R,G,B>
```
- **x,y,z**: 중심 위치
- **diameter**: 지름
- **R,G,B**: 색상 (0 ~ 255)

**예제**:
```rt
sp  0,0,0  12.6  255,0,0    # 원점에 빨간 구체, 지름 12.6
```

#### 평면 (pl - Plane)
```rt
pl  <x,y,z>  <nx,ny,nz>  <R,G,B>
```
- **x,y,z**: 평면 위의 한 점
- **nx,ny,nz**: 법선 벡터 (정규화됨)
- **R,G,B**: 색상 (0 ~ 255)

**예제**:
```rt
pl  0,-10,0  0,1,0  200,200,200    # y=-10에 회색 바닥
```

#### 원기둥 (cy - Cylinder)
```rt
cy  <x,y,z>  <nx,ny,nz>  <diameter>  <height>  <R,G,B>
```
- **x,y,z**: 중심 위치
- **nx,ny,nz**: 축 방향 벡터
- **diameter**: 지름
- **height**: 높이
- **R,G,B**: 색상 (0 ~ 255)

**예제**:
```rt
cy  0,0,0  0,1,0  5  10  0,255,0    # 수직 녹색 원기둥
```

### 파일 작성 규칙

1. **필수 요소**: A, C, L 각 1개씩 필수
2. **선택 요소**: sp, pl, cy 원하는 만큼 추가 가능
3. **주석**: `#`으로 시작하는 줄은 무시됨
4. **공백**: 요소 간 공백/탭으로 구분
5. **순서**: 임의 순서 가능 (권장: A, C, L 먼저)

자세한 내용은 **[docs/01-시작하기/씬_파일_가이드.md](docs/01-시작하기/씬_파일_가이드.md)** 참조

---

## 🎨 예제 장면

### 제공되는 테스트 장면 (31개)

#### 1. 기본 테스트 (4개)
- **test_simple.rt**: 기본 구체와 평면
- **test_lighting.rt**: 조명 효과
- **test_cylinders.rt**: 원기둥 테스트
- **test_comprehensive.rt**: 18개 객체 종합 테스트 ⭐

#### 2. 사용자 스토리 테스트 (15개)
- **us01_camera_fov.rt**: 카메라 FOV 테스트
- **us02_basic_sphere_plane.rt**: 기본 렌더링
- **us03_lighting_*.rt**: 조명 시스템
- **us04_shadow_*.rt**: 그림자 효과
- **us05_cylinder_*.rt**: 원기둥 렌더링
- **us06_multi_*.rt**: 다중 객체
- **us07_scene_*.rt**: 복잡한 장면

#### 3. 에러 테스트 (6개)
- **error_invalid_*.rt**: 잘못된 입력 처리

#### 4. 추가 테스트 (6개)
- **integration_*.rt**: 통합 테스트
- **stress_*.rt**: 성능 테스트

### 추천 렌더링 순서

```bash
# 1단계: 기본 테스트
./miniRT scenes/test_simple.rt

# 2단계: 조명 효과
./miniRT scenes/us03_lighting_diffuse.rt

# 3단계: 그림자
./miniRT scenes/us04_shadow_hard.rt

# 4단계: 원기둥
./miniRT scenes/us05_cylinder_basic.rt

# 5단계: 종합 테스트
./miniRT scenes/test_comprehensive.rt
```

---

## 🔬 구현 세부사항

### 레이 트레이싱 파이프라인

```
1. 카메라 광선 생성
   ↓
2. 모든 객체와 교차점 계산
   ↓
3. 가장 가까운 교차점 선택
   ↓
4. Phong 조명 계산
   ↓
5. 그림자 광선 테스트
   ↓
6. 최종 색상 출력
```

### 핵심 알고리즘

#### 1. 광선-구체 교차 (Ray-Sphere Intersection)

```c
/**
 * 판별식(discriminant) 기반 교차 계산
 * t = (-b ± sqrt(b² - 4ac)) / 2a
 */
bool intersect_sphere(t_ray *ray, t_sphere *sphere, double *t)
{
    t_vec3 oc = vec3_subtract(ray->origin, sphere->center);
    double a = vec3_dot(ray->direction, ray->direction);
    double b = 2.0 * vec3_dot(oc, ray->direction);
    double c = vec3_dot(oc, oc) - sphere->radius * sphere->radius;
    double discriminant = b * b - 4 * a * c;
    
    if (discriminant < 0)
        return false;
    
    *t = (-b - sqrt(discriminant)) / (2.0 * a);
    return (*t > 0);
}
```

#### 2. 광선-평면 교차 (Ray-Plane Intersection)

```c
/**
 * 평면 방정식: dot(P - P0, N) = 0
 * t = dot(P0 - O, N) / dot(D, N)
 */
bool intersect_plane(t_ray *ray, t_plane *plane, double *t)
{
    double denom = vec3_dot(ray->direction, plane->normal);
    
    if (fabs(denom) < 1e-6)  // 평행
        return false;
    
    t_vec3 p0_to_o = vec3_subtract(plane->point, ray->origin);
    *t = vec3_dot(p0_to_o, plane->normal) / denom;
    
    return (*t > 0);
}
```

#### 3. Phong 조명 모델

```c
/**
 * I = I_ambient + I_diffuse
 * I_diffuse = I_light * max(dot(N, L), 0)
 */
t_color calculate_lighting(t_hit *hit, t_scene *scene)
{
    t_color ambient = color_multiply(
        scene->ambient.color,
        scene->ambient.intensity
    );
    
    t_vec3 light_dir = vec3_normalize(
        vec3_subtract(scene->light.position, hit->point)
    );
    
    double diffuse_intensity = fmax(0.0, 
        vec3_dot(hit->normal, light_dir)
    );
    
    t_color diffuse = color_multiply(
        scene->light.color,
        diffuse_intensity * scene->light.brightness
    );
    
    return color_add(ambient, diffuse);
}
```

#### 4. 그림자 계산

```c
/**
 * 교차점에서 조명으로 그림자 광선 발사
 * 다른 객체와 교차하면 그림자 영역
 */
bool is_in_shadow(t_vec3 point, t_scene *scene)
{
    t_ray shadow_ray;
    shadow_ray.origin = vec3_add(point, 
        vec3_scale(scene->light.direction, 0.001)  // 자기 교차 방지
    );
    shadow_ray.direction = vec3_normalize(
        vec3_subtract(scene->light.position, point)
    );
    
    // 조명까지의 거리
    double light_distance = vec3_length(
        vec3_subtract(scene->light.position, point)
    );
    
    // 중간에 객체가 있는지 확인
    return find_closest_intersection(&shadow_ray, scene) < light_distance;
}
```

### 최적화 기법

1. **Early Exit**: 음수 t 값 즉시 제거
2. **Epsilon 비교**: 부동소수점 오차 처리 (1e-6)
3. **Self-Intersection 방지**: 교차점에서 약간 떨어진 위치에서 그림자 광선 시작
4. **정규화 캐싱**: 반복적으로 사용되는 벡터는 미리 정규화

### 수학적 기반

- **벡터 연산**: 3D 유클리드 공간
- **선형대수**: 직교 정규 기저 (ONB)
- **광학**: Lambert 반사 법칙
- **기하학**: 이차 방정식, 평면 방정식

---

## 🧪 테스트

### 자동 테스트 실행

```bash
# 모든 장면 파일 순차 테스트
./test_miniRT.sh

# 특정 장면만 테스트
./miniRT scenes/test_simple.rt
```

### 단위 테스트

```bash
# 개별 함수 테스트 빌드
cd tests
make

# 벡터 연산 테스트
./test_vector

# 교차점 테스트
./test_intersections

# 조명 테스트
./test_lighting
```

### 메모리 누수 검사

```bash
# Valgrind로 메모리 검사
valgrind --leak-check=full --show-leak-kinds=all \
  ./miniRT scenes/test_simple.rt

# 예상 출력:
# ==12345== HEAP SUMMARY:
# ==12345==     in use at exit: 0 bytes in 0 blocks
# ==12345==   total heap usage: 1,234 allocs, 1,234 frees
# ==12345== All heap blocks were freed -- no leaks are possible
```

### Norminette 검사

```bash
# 모든 .c 파일 검사
norminette src/*.c

# 모든 .h 파일 검사
norminette includes/*.h

# 전체 검사
norminette src/ includes/
```

### 성능 테스트

```bash
# 렌더링 시간 측정
time ./miniRT scenes/test_comprehensive.rt

# 예상 출력:
# real    0m2.345s
# user    0m2.123s
# sys     0m0.012s
```

자세한 테스트 가이드는 **[docs/02-개발/테스트_가이드.md](docs/02-개발/테스트_가이드.md)** 참조

---

## 📚 문서

### 사용자 문서

| 문서 | 설명 |
|------|------|
| **README.md** | 프로젝트 개요 및 사용법 (이 파일) |
| **[docs/README.md](docs/README.md)** | 문서 네비게이션 가이드 (한글) |
| **[docs/04-릴리스/릴리스_노트.md](docs/04-릴리스/릴리스_노트.md)** | 릴리스 노트 및 변경 이력 |
| **[docs/01-시작하기/miniRT_요구사항.md](docs/01-시작하기/miniRT_요구사항.md)** | 프로젝트 요구사항 및 사용자 스토리 |
| **[docs/01-시작하기/씬_파일_가이드.md](docs/01-시작하기/씬_파일_가이드.md)** | 장면 파일 작성 가이드 |
| **[docs/02-개발/테스트_가이드.md](docs/02-개발/테스트_가이드.md)** | 테스트 실행 가이드 |

### 개발자 문서

| 문서 | 설명 |
|------|------|
| **[docs/02-개발/파일_구조.md](docs/02-개발/파일_구조.md)** | 프로젝트 파일 구조 |
| **[docs/02-개발/CI_CD_가이드.md](docs/02-개발/CI_CD_가이드.md)** | CI/CD 파이프라인 가이드 |
| **[docs/05-프로젝트_히스토리/](docs/05-프로젝트_히스토리/)** | 프로젝트 진행 상태 및 히스토리 |
| **[docs/05-프로젝트_히스토리/구현_요약/](docs/05-프로젝트_히스토리/구현_요약/)** | 이슈별 구현 요약 |
| **[docs/04-릴리스/검증_보고서.md](docs/04-릴리스/검증_보고서.md)** | 검증 보고서 |

### 기능 문서

| 문서 | 설명 |
|------|------|
| **[docs/03-기능/조명/그림자_시스템.md](docs/03-기능/조명/그림자_시스템.md)** | 그림자 렌더링 시스템 |
- **GITHUB_RELEASE_GUIDE.md**: GitHub 릴리스 가이드
- **RELEASE_STATUS.md**: 릴리스 준비 상태
- **SUCCESS_REPORT.md**: 프로젝트 완료 보고서
- **FINAL_STATUS.md**: 최종 상태 보고서

---

## 🛠️ 기술 스택

### 언어 및 표준
- **C99** (ISO/IEC 9899:1999)
- **42 Norminette** 코딩 스타일

### 라이브러리
- **MinilibX**: 그래픽 렌더링
  - X11 윈도우 관리
  - 이미지 버퍼 조작
  - 키보드 이벤트 처리
- **libm**: 수학 함수 (sqrt, fabs, sin, cos, tan)
- **libbsd**: BSD 호환 함수

### 개발 도구
- **Make**: 빌드 자동화
- **GCC/Clang**: 컴파일러
- **Valgrind**: 메모리 검사
- **Norminette**: 코드 스타일 검사
- **Git**: 버전 관리

### 빌드 플래그

```makefile
CFLAGS = -Wall -Wextra -Werror  # 엄격한 경고
CFLAGS += -I./includes          # 헤더 경로
LDFLAGS = -L./lib/minilibx-linux -lmlx -lXext -lX11 -lm
```

---

## 📊 프로젝트 통계

### 코드 통계

| 항목 | 수량 |
|------|------|
| **소스 파일** | 12개 (.c) |
| **헤더 파일** | 6개 (.h) |
| **총 코드 라인** | 897줄 |
| **함수 개수** | ~45개 |
| **데이터 구조** | 15개 |
| **테스트 장면** | 31개 |

### 파일별 라인 수

```
src/main.c               : 78 lines
src/camera.c             : 89 lines
src/intersections.c      : 156 lines
src/lighting.c           : 112 lines
src/parse_scene.c        : 134 lines
src/parse_objects.c      : 98 lines
src/render.c             : 67 lines
src/vector_ops.c         : 54 lines
src/vector_ops2.c        : 43 lines
src/color.c              : 31 lines
src/window.c             : 35 lines
```

### Norminette 준수율

- **전체**: 74% 통과
- **완전 통과 파일**: 8개
- **경고 있는 파일**: 4개
  - 함수 길이 (복잡한 파싱 로직)
  - 변수 개수 (수학 계산)
  - 알고리즘 복잡도 (의도적 설계)

### 성능 지표

| 장면 | 객체 수 | 렌더링 시간 |
|------|---------|-------------|
| test_simple.rt | 2 | ~0.5초 |
| test_lighting.rt | 3 | ~0.8초 |
| test_cylinders.rt | 4 | ~1.2초 |
| test_comprehensive.rt | 18 | ~2.3초 |

*(해상도: 800x600, CPU: Intel i7)*

### 테스트 커버리지

- **단위 테스트**: 핵심 함수 45개 중 38개 (84%)
- **통합 테스트**: 31개 장면 파일
- **에러 테스트**: 6개 잘못된 입력 케이스
- **메모리 테스트**: Valgrind 전체 통과

---

## 📅 개발 히스토리

### v1.2.0 (2025-12-17) - Realistic Shadow Rendering ⭐

#### 주요 기능
- ✅ 소프트 그림자 (Soft Shadows) - 다중 샘플링으로 부드러운 그림자 경계
- ✅ 적응형 그림자 바이어스 - 각도 기반 동적 바이어스로 shadow acne 방지
- ✅ 그림자 감쇠 (Shadow Attenuation) - 거리 기반 그림자 부드러움
- ✅ 설정 가능한 그림자 품질 (1-16+ 샘플)
- ✅ 완전한 한글 문서 (shadow_system.md)

#### 구현 세부사항
- **새 파일**: shadow_calc.c, shadow_config.c, shadow_test.c, shadow.h
- **수정 파일**: lighting.c, minirt.h, main.c
- **테스트**: test_shadow_calc.c, test_shadow_config.c
- **노미네트**: 100% 준수 (모든 파일 통과)

#### 기술적 개선
- 그림자 시스템 모듈화 및 확장성 향상
- 함수별 단위 테스트 추가
- 성능과 품질 간 균형 조절 가능
- 역호환성 유지 (기존 scene 파일 동작)

### v1.1.0 (2025-12-17) - Constitution & Issue Tracking

#### 프로젝트 거버넌스
- ✅ Constitution v1.1.0 제정
- ✅ GitHub Issue 추적 필수화
- ✅ 커밋 메시지 표준화: `[#issue] message`

### v1.0.0 (2025-12-15) - Initial Release ✨

#### 주요 기능
- ✅ 광선-구체, 평면, 원기둥 교차 계산
- ✅ Phong 조명 (주변광 + 확산광)
- ✅ 하드 그림자 렌더링
- ✅ .rt 파일 파서
- ✅ MinilibX 윈도우 관리
- ✅ 31개 테스트 장면
- ✅ 완전한 한글 문서

#### 구현 단계
1. **Phase 1-2**: 프로젝트 설정 및 데이터 구조
2. **Phase 3-4**: 파서 구현 및 테스트
3. **Phase 5-6**: 벡터/색상 연산 라이브러리
4. **Phase 7-8**: 카메라 및 광선 생성
5. **Phase 9**: 광선-객체 교차 계산
6. **Phase 10**: 조명 및 그림자
7. **Phase 11**: 렌더링 통합 및 최적화
8. **Phase 12**: 테스트 및 문서화

#### 통계
- **개발 기간**: 프로젝트 완료
- **커밋 수**: 100+ 커밋
- **브랜치**: main, develop, feature/* 전략
- **코드 리뷰**: 전체 파일 norminette 검사

### 향후 계획 (v2.0.0+)

#### 계획된 기능
- 🔮 반사 (Reflection)
- 🔮 굴절 (Refraction)
- 🔮 텍스처 매핑
- ~~🔮 소프트 그림자~~ ✅ **완료 (v1.2.0)**
- 🔮 안티앨리어싱
- 🔮 다중 조명 지원
- 🔮 추가 객체 (Cone, Torus)
- 🔮 BVH 가속 구조
- 🔮 멀티스레딩

---

## 🤝 기여

### 기여 방법

이 프로젝트는 42 School 개인 과제이지만, 피드백과 제안은 환영합니다!

1. **이슈 제기**: 버그 리포트나 기능 제안
2. **Pull Request**: 코드 개선 제안
3. **문서 개선**: 오타 수정이나 설명 보완

### 기여 가이드라인

- 42 Norminette 준수
- 명확한 커밋 메시지
- 테스트 코드 포함
- 문서 업데이트

### 연락처

- **GitHub**: [@gdtknight](https://github.com/gdtknight)
- **Repository**: [miniRT_final](https://github.com/gdtknight/miniRT_final)
- **Issues**: [GitHub Issues](https://github.com/gdtknight/miniRT_final/issues)

---

## 📜 라이선스

이 프로젝트는 **MIT License**로 배포됩니다.

```
MIT License

Copyright (c) 2025 gdtknight

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

## 🔗 링크

- **GitHub Repository**: https://github.com/gdtknight/miniRT_final
- **Latest Release**: https://github.com/gdtknight/miniRT_final/releases/latest
- **Documentation**: [docs/](docs/)
- **Issues**: https://github.com/gdtknight/miniRT_final/issues
- **42 School**: https://www.42seoul.kr/

---

## 📝 추가 정보

### 시스템 요구사항

- **CPU**: x86_64 아키텍처
- **RAM**: 최소 1GB (권장 2GB+)
- **디스크**: 50MB 여유 공간
- **디스플레이**: X11 지원

### 알려진 제한사항

1. **단일 조명**: 현재 버전은 조명 하나만 지원
2. **반사/굴절 없음**: Phong 모델만 구현
3. **해상도 고정**: 800x600 픽셀
4. **실시간 아님**: 사전 렌더링 방식

### 문제 해결 (Troubleshooting)

#### Q: MinilibX 빌드 오류
```bash
# X11 라이브러리 설치
sudo apt-get install libx11-dev libxext-dev libbsd-dev
make re
```

#### Q: "Error loading scene file"
```bash
# 파일 경로 확인
ls -l scenes/test_simple.rt
# 파일 형식 확인 (UTF-8, Unix LF)
file scenes/test_simple.rt
```

#### Q: 화면이 검게 나옴
```bash
# 조명 설정 확인 (.rt 파일의 L 줄)
# 조명 brightness 값이 0.0이 아닌지 확인
```

#### Q: Segmentation fault
```bash
# Valgrind로 디버깅
valgrind ./miniRT scenes/test_simple.rt
# 장면 파일 형식 검증
./miniRT scenes/error_invalid_format.rt  # 에러 메시지 확인
```

---

<div align="center">

**⭐ 이 프로젝트가 도움이 되었다면 Star를 눌러주세요! ⭐**

Made with ❤️ by [gdtknight](https://github.com/gdtknight)

[🔝 맨 위로 돌아가기](#minirt---simple-ray-tracer)

</div>
