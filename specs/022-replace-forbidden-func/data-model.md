# Data Model: Function Replacement Mapping

**Date**: 2026-01-29
**Feature**: 022-replace-forbidden-func

## Function Replacement Matrix

### Forbidden → libft Mapping

| Forbidden Function | libft Replacement | Notes |
|--------------------|-------------------|-------|
| `snprintf(buf, size, fmt, ...)` | `format_id()` 헬퍼 함수 또는 `ft_itoa()` + `ft_strlcpy()` + `ft_strlcat()` | 헬퍼 함수로 캡슐화하여 메모리 관리 |
| `fprintf(stderr, ...)` | `ft_putstr_fd(str, 2)` | fd=2 for stderr |
| `strcpy(dst, src)` | `ft_strlcpy(dst, src, size)` | Must specify size |
| `strcat(dst, src)` | `ft_strlcat(dst, src, size)` | Must specify size |
| `strlen(s)` | `ft_strlen(s)` | Direct replacement |
| `memset(ptr, val, size)` | `ft_memset(ptr, val, size)` or `ft_bzero(ptr, size)` | Use ft_bzero for zeroing |
| `memcpy(dst, src, size)` | `ft_memcpy(dst, src, size)` | Direct replacement |

### Custom → libft Mapping

| Custom Function | Location | libft Replacement |
|-----------------|----------|-------------------|
| `ft_atoi()` | src/utils/ft_atoi.c | `ft_atoi()` from libft |
| `ft_memcpy()` | src/utils/memory.c | `ft_memcpy()` from libft |
| `ft_memset()` | src/utils/memory.c | `ft_memset()` from libft |
| `ft_strcpy()` | src/hud/hud_format_helpers.c | `ft_strlcpy()` from libft |
| `ft_numlen()` | src/hud/hud_format_helpers.c | Calculate via `ft_itoa()` + `ft_strlen()` |
| `ft_itoa_buf()` | src/hud/hud_format_helpers.c | `ft_itoa()` from libft |
| `ft_atof()` | src/utils/ft_atof.c | **RETAIN** (not in libft) |

## File Change Summary

### Files to Modify

| File | Changes |
|------|---------|
| `src/parser/parse_objects.c` | Replace snprintf with ft_strlcpy + ft_itoa |
| `src/parser/parse_cylinder.c` | Replace snprintf with ft_strlcpy + ft_itoa |
| `src/parser/parser.c` | Replace strcpy with ft_strlcpy |
| `src/utils/format_object_id.c` | Add format_id helper, replace snprintf |
| `src/utils/error.c` | Replace strlen with ft_strlen |
| `src/bvh_vis/bvh_vis_node.c` | Replace strcpy, strcat, snprintf with libft |
| `src/bvh_vis/bvh_vis_print.c` | Replace fprintf with ft_putstr_fd |
| `src/window/mlx_context.c` | Replace memset with ft_bzero |
| `src/scene/scene.c` | Replace memset with ft_bzero |
| `src/scene/object_list.c` | Replace memcpy with ft_memcpy |
| `src/hud/hud_format_helpers.c` | Remove duplicate ft_* functions |
| `includes/minirt.h` | Add libft.h include, remove custom prototypes |
| `includes/utils.h` | Remove ft_memcpy, ft_memset prototypes |
| `Makefile` | Add libft compilation and linking |

### Files to Remove

| File | Reason |
|------|--------|
| `src/utils/ft_atoi.c` | Duplicates libft ft_atoi |
| `src/utils/memory.c` | Duplicates libft ft_memcpy, ft_memset |

### Files to Retain

| File | Reason |
|------|--------|
| `src/utils/ft_atof.c` | Not available in libft, required for parsing |

## Entity Relationships

```
Makefile
    │
    ├── compiles ──► lib/libft/libft.a
    │
    └── links ──► miniRT executable
                      │
                      ├── uses ──► libft functions (ft_strlen, ft_strlcpy, etc.)
                      │
                      └── uses ──► custom ft_atof (retained)
```

## New Helper Functions

### format_id (신규)

```c
/*
** format_id - 접두사와 숫자를 결합하여 ID 문자열 생성
** @buf: 대상 버퍼
** @size: 버퍼 크기
** @prefix: 접두사 ("sp-", "pl-", "cy-" 등)
** @n: 숫자
** @return: 성공 시 1, 실패 시 0
**
** ft_itoa 메모리 할당/해제를 캡슐화하여 메모리 누수 방지
*/
int	format_id(char *buf, size_t size, const char *prefix, int n);
```

**위치**: `src/utils/format_object_id.c` (기존 파일에 추가)

### float_to_str (신규)

```c
/*
** float_to_str - float 값을 문자열로 변환
** @buf: 대상 버퍼
** @size: 버퍼 크기
** @value: 변환할 float 값
** @precision: 소수점 이하 자릿수 (1 또는 2)
** @return: 성공 시 1, 실패 시 0
**
** bvh_vis 모듈의 좌표 표시용 (snprintf "%.2f" 대체)
*/
int	float_to_str(char *buf, size_t size, double value, int precision);
```

**위치**: `src/utils/format_helpers.c` (신규 파일)

**용도**: bvh_vis_node.c의 "%.1f", "%.2f" 패턴 대체

## Module Scope

### bvh_vis 모듈 (평가 대상 포함)

| 파일 | 교체 대상 | 우선순위 |
|------|-----------|----------|
| `src/bvh_vis/bvh_vis_node.c` | strcpy, strcat, snprintf | 필수 |
| `src/bvh_vis/bvh_vis_print.c` | fprintf | 필수 |

**결정**: bvh_vis 모듈은 평가 바이너리에 포함되므로 forbidden function 교체 필수

## Validation Rules

1. **No forbidden functions**: grep for snprintf, fprintf, strcpy, strcat, strlen, memset, memcpy in src/
2. **No duplicate implementations**: No ft_atoi, ft_memcpy, ft_memset definitions in src/
3. **Proper includes**: All files using ft_* functions must include libft.h (directly or via minirt.h)
4. **Buffer safety**: All string operations use bounded ft_strl* functions with explicit size
5. **Memory safety**: 모든 ft_itoa() 호출 후 free() 보장 (헬퍼 함수 사용 권장)
6. **Allocation check**: ft_itoa() NULL 반환 시 적절한 에러 처리
