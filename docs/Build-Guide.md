# Build Guide

miniRT 빌드 및 설치 가이드입니다.

---

## 요구사항

### Linux

| 패키지 | 설명 |
|--------|------|
| gcc | C 컴파일러 |
| make | GNU Make 3.81+ |
| libx11-dev | X Window System 라이브러리 |
| libxext-dev | X Extension 라이브러리 |
| libbsd-dev | BSD 호환 라이브러리 |

### macOS

| 요구사항 | 설명 |
|---------|------|
| Xcode Command Line Tools | clang, make 포함 |
| OpenGL + AppKit | macOS 기본 포함 |

---

## 빌드

### 1. 저장소 클론

```bash
# 서브모듈(libft) 포함 클론
git clone --recursive https://github.com/gdtknight/miniRT.git
cd miniRT
```

서브모듈 누락 시:
```bash
git submodule update --init --recursive
```

### 2. 의존성 설치 (Linux)

```bash
sudo apt-get update
sudo apt-get install libx11-dev libxext-dev libbsd-dev
```

### 3. 빌드

```bash
make        # 빌드
make clean  # 오브젝트 파일 삭제
make fclean # 오브젝트 + 실행 파일 삭제
make re     # 전체 재빌드
```

빌드 산출물:
- `miniRT` — 실행 파일
- `build/` — 오브젝트 파일

---

## 실행

```bash
# 기본 실행
./miniRT scenes/valid/valid_smoke_simple.rt

# BVH 시각화 모드
./miniRT --bvh-vis scenes/valid/valid_scene_comprehensive.rt
```

---

## 프로젝트 의존성

### libft (서브모듈)

`lib/libft/`에 git 서브모듈로 관리됩니다. `make` 시 자동 빌드.

주요 사용 함수: `ft_atof`, `ft_atoi`, `ft_strlcpy`, `ft_memset`, `ft_strcmp` 등

### MiniLibX

`lib/minilibx-macos/` (macOS)에 포함. Linux 빌드 시 별도로 minilibx-linux를 배치해야 합니다.

- Linux: X11 기반 그래픽 라이브러리
- macOS: AppKit/OpenGL 기반 그래픽 라이브러리

---

## Makefile 구조

| 타겟 | 설명 |
|------|------|
| `all` | libft → MiniLibX → miniRT 순차 빌드 |
| `clean` | 오브젝트 파일 삭제 (build/) |
| `fclean` | clean + 실행 파일 + MiniLibX/libft 정리 |
| `re` | fclean → all |
| `norm` | norminette 검사 (src/ + includes/) |

컴파일 플래그: `-Wall -Wextra -Werror`

링크 플래그:
- Linux: `-lmlx -lft -lXext -lX11 -lm`
- macOS: `-lmlx -lft -framework OpenGL -framework AppKit -lm`

---

## CI

`.github/workflows/` 에 4개의 워크플로가 설정되어 있습니다:

| 워크플로 | 트리거 | 설명 |
|---------|--------|------|
| `ci.yml` | push, PR | 빌드 검증 |
| `pr-validation.yml` | PR | PR 네이밍 컨벤션 검증 |
| `release.yml` | tag push (v*) | 릴리스 빌드 |
| `wiki-sync.yml` | tag push (v*), 수동 | docs/ → Wiki 동기화 |

---

## 문제 해결

### MiniLibX 빌드 에러 (Linux)

```bash
# X11 라이브러리 누락
sudo apt-get install libx11-dev libxext-dev libbsd-dev
make re
```

### libft 서브모듈 누락

```bash
git submodule update --init --recursive
make re
```

### 실행 시 세그먼트 폴트

```bash
# 씬 파일 형식 확인
cat scenes/valid/valid_smoke_simple.rt

# 메모리 검사 (Linux)
valgrind --leak-check=full ./miniRT scenes/valid/valid_smoke_simple.rt
```
