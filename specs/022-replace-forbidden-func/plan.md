# Implementation Plan: Replace Forbidden Functions & Integrate libft

**Branch**: `022-replace-forbidden-func` | **Date**: 2026-01-29 | **Spec**: [spec.md](./spec.md)
**Input**: Feature specification from `/specs/022-replace-forbidden-func/spec.md`

## Summary

Replace all forbidden standard library functions (snprintf, fprintf, strcpy, strcat, strlen, memset, memcpy) with libft equivalents, integrate the libft library from lib/libft submodule, remove custom ft_* implementations from src/, and fix potential overflow/underflow issues. Ensure cross-platform compatibility (macOS/Linux).

## Technical Context

**Language/Version**: C (42 Norm v4.1 compliant)
**Primary Dependencies**: libft (lib/libft), MiniLibX, math library (-lm)
**Storage**: N/A (file-based scene parsing only)
**Testing**: Manual compilation test (`make`), scene rendering verification
**Target Platform**: macOS (clang), Linux (gcc)
**Project Type**: Single project (raytracer)
**Performance Goals**: Equivalent performance to current implementation
**Constraints**: 42 Norm compliance, allowed functions only, zero memory leaks

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

| Principle | Status | Notes |
|-----------|--------|-------|
| I. 42 Norminette Compliance | ✅ PASS | All replacements must follow Norm v4.1 |
| II. Memory Safety | ✅ PASS | libft functions are memory-safe; custom ft_atof retained |
| III. Allowed Functions | ✅ PASS | libft is explicitly allowed per subject |
| IV. Modular Architecture | ✅ PASS | Centralizes ft_* in lib/libft |
| V. Visual Correctness | ✅ PASS | No rendering logic changes |
| VI. Stability & Crash Prevention | ✅ PASS | Buffer overflow fixes improve stability |
| VII. Window Management | ✅ PASS | No window code changes |

**Gate Result**: PASS - No violations

## Project Structure

### Documentation (this feature)

```text
specs/022-replace-forbidden-func/
├── spec.md              # Feature specification
├── plan.md              # This file
├── research.md          # Phase 0 output
├── data-model.md        # Phase 1 output (function mapping)
├── quickstart.md        # Phase 1 output
└── tasks.md             # Phase 2 output (/speckit.tasks)
```

### Source Code (repository root)

```text
lib/
├── libft/                    # libft submodule (ft_strlen, ft_strlcpy, etc.)
│   ├── includes/libft.h
│   ├── srcs/ft_*.c
│   └── Makefile
└── minilibx-macos/

src/
├── parser/
│   ├── parse_objects.c       # snprintf → ft_strlcpy + ft_itoa
│   ├── parse_cylinder.c      # snprintf → ft_strlcpy + ft_itoa
│   └── parser.c              # strcpy → ft_strlcpy
├── utils/
│   ├── ft_atof.c             # RETAIN (not in libft)
│   ├── ft_atoi.c             # REMOVE (use libft)
│   ├── memory.c              # REMOVE (use libft)
│   ├── format_object_id.c    # snprintf → format_id 헬퍼 함수 추가
│   └── error.c               # strlen → ft_strlen
├── bvh_vis/
│   ├── bvh_vis_node.c        # strcpy, strcat, snprintf → libft
│   └── bvh_vis_print.c       # fprintf → ft_putstr_fd
├── hud/
│   └── hud_format_helpers.c  # Remove ft_strcpy, ft_numlen, ft_itoa_buf
├── window/
│   └── mlx_context.c         # memset → ft_bzero
└── scene/
    ├── scene.c               # memset → ft_bzero
    └── object_list.c         # memcpy → ft_memcpy

includes/
├── minirt.h                  # Add #include "libft.h", remove custom prototypes
└── utils.h                   # Remove ft_memcpy, ft_memset prototypes
```

**Structure Decision**: Single project with libft as external library dependency

## Complexity Tracking

> No violations - no complexity justification needed

## libft Function Availability

| Required Function | libft Status | Action |
|-------------------|--------------|--------|
| ft_strlen | ✅ Available | Use libft |
| ft_strlcpy | ✅ Available | Use libft |
| ft_strlcat | ✅ Available | Use libft |
| ft_memset | ✅ Available | Use libft |
| ft_memcpy | ✅ Available | Use libft |
| ft_bzero | ✅ Available | Use libft |
| ft_atoi | ✅ Available | Use libft |
| ft_itoa | ✅ Available | Use libft |
| ft_putstr_fd | ✅ Available | Use for stderr output |
| ft_atof | ❌ Not available | Retain custom implementation |

## Design Decisions

### 1. ft_itoa 메모리 관리: 헬퍼 함수 캡슐화

**결정**: ft_itoa 사용 시 메모리 해제를 헬퍼 함수로 캡슐화하여 유지보수성 확보

**이유**:
- ft_itoa()는 malloc()으로 문자열 할당 → 반드시 free() 필요
- 헬퍼 함수로 캡슐화하면 메모리 누수 방지 및 코드 재사용성 향상
- 할당 실패(NULL) 처리를 한 곳에서 관리

**헬퍼 함수 설계**:
```c
// src/utils/format_object_id.c (기존 파일에 추가)

/*
** format_id - 접두사와 숫자를 결합하여 ID 문자열 생성
** @buf: 대상 버퍼
** @size: 버퍼 크기
** @prefix: 접두사 ("sp-", "pl-", "cy-" 등)
** @n: 숫자
** @return: 성공 시 1, 실패 시 0
*/
int	format_id(char *buf, size_t size, const char *prefix, int n)
{
    char	*num;

    num = ft_itoa(n);
    if (!num)
        return (0);
    ft_strlcpy(buf, prefix, size);
    ft_strlcat(buf, num, size);
    free(num);
    return (1);
}
```

**검증**: valgrind/leaks로 모든 경로에서 메모리 누수 테스트

### 2. float 포맷팅: float_to_str 헬퍼 함수

**결정**: bvh_vis 모듈의 float 포맷팅(%.2f)을 위한 커스텀 float_to_str() 함수 구현

**이유**:
- libft에 ft_ftoa 없음
- snprintf "%.2f" 패턴은 ft_itoa로 대체 불가
- bvh_vis 디버그 정보에 좌표값 표시 필요

**헬퍼 함수 설계**:
```c
// src/utils/format_helpers.c (신규 파일)

/*
** float_to_str - float 값을 문자열로 변환 (소수점 2자리)
** @buf: 대상 버퍼
** @size: 버퍼 크기
** @value: 변환할 float 값
** @precision: 소수점 이하 자릿수 (1 또는 2)
** @return: 성공 시 1, 실패 시 0
*/
int	float_to_str(char *buf, size_t size, double value, int precision);
```

**위치**: `src/utils/format_helpers.c` (신규 파일)

### 3. bvh_vis 모듈 포함 범위

**결정**: bvh_vis 모듈을 평가 대상 바이너리에 포함

**이유**:
- 평가 시 forbidden function 체크에 걸리므로 교체 필수
- 디버그 전용이 아닌 기본 빌드에 포함

**영향**:
- `src/bvh_vis/bvh_vis_node.c` - strcpy, strcat, snprintf 교체 필수
- `src/bvh_vis/bvh_vis_print.c` - fprintf 교체 필수

---

## Replacement Strategy

### 1. snprintf Replacement Pattern (헬퍼 함수 사용)

```c
// BEFORE
snprintf(obj.id, 8, "sp-%d", count + 1);

// AFTER (헬퍼 함수 사용)
format_id(obj.id, 8, "sp-", count + 1);
```

**헬퍼 함수 없이 직접 사용 시** (단순 케이스):
```c
char *num;
num = ft_itoa(count + 1);
if (!num)
    return (error_handler());  // 할당 실패 처리 필수
ft_strlcpy(obj.id, "sp-", 8);
ft_strlcat(obj.id, num, 8);
free(num);  // 반드시 해제
```

### 2. fprintf(stderr) Replacement Pattern

```c
// BEFORE
fprintf(stderr, "Warning: %s\n", message);

// AFTER
ft_putstr_fd("Warning: ", 2);
ft_putstr_fd(message, 2);
ft_putstr_fd("\n", 2);
```

### 3. strcpy/strcat Replacement Pattern

```c
// BEFORE
strcpy(buffer, "prefix");
strcat(buffer, suffix);

// AFTER
ft_strlcpy(buffer, "prefix", sizeof(buffer));
ft_strlcat(buffer, suffix, sizeof(buffer));
```

### 4. memset/memcpy Replacement Pattern

```c
// BEFORE
memset(ptr, 0, sizeof(t_struct));
memcpy(dst, src, size);

// AFTER
ft_bzero(ptr, sizeof(t_struct));
ft_memcpy(dst, src, size);
```

## Makefile Changes

```makefile
# Add libft compilation
LIBFT_DIR = lib/libft
LIBFT = $(LIBFT_DIR)/libft.a

# Add to INCLUDES
INCLUDES += -I$(LIBFT_DIR)/includes

# Add to LIBS
LIBS += -L$(LIBFT_DIR) -lft

# Add libft target
$(LIBFT):
	@$(MAKE) -C $(LIBFT_DIR)

# Add dependency
$(NAME): $(LIBFT) $(OBJS)

# Update clean/fclean
clean:
	@$(MAKE) -C $(LIBFT_DIR) clean
	...

fclean: clean
	@$(MAKE) -C $(LIBFT_DIR) fclean
	...
```

## Files to Remove from Compilation

Remove from Makefile SRCS:
- `$(SRC_DIR)/utils/ft_atoi.c`
- `$(SRC_DIR)/utils/memory.c`

Keep:
- `$(SRC_DIR)/utils/ft_atof.c` (not in libft)
