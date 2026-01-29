# Quickstart: Replace Forbidden Functions & Integrate libft

**Feature**: 022-replace-forbidden-func
**Date**: 2026-01-29

## Prerequisites

1. libft submodule initialized:
   ```bash
   git submodule update --init lib/libft
   ```

2. Verify libft contents:
   ```bash
   ls lib/libft/includes/libft.h  # Should exist
   ls lib/libft/srcs/ft_strlen.c  # Should exist
   ```

## Implementation Order

### Phase 1: Makefile Integration

1. Update Makefile to compile and link libft
2. Add libft include path
3. Remove custom ft_* source files from SRCS

### Phase 2: Remove Custom Implementations

1. Delete `src/utils/ft_atoi.c`
2. Delete `src/utils/memory.c`
3. Remove duplicate functions from `src/hud/hud_format_helpers.c`

### Phase 3: Update Header Files

1. Add `#include "libft.h"` to `includes/minirt.h`
2. Remove custom ft_* prototypes from headers

### Phase 4: Add Helper Function

**위치**: `src/utils/format_object_id.c`

```c
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

**중요**: 모든 snprintf("%s-%d", ...) 패턴은 이 헬퍼 함수를 사용

### Phase 5: Replace Forbidden Functions

Replace in order of dependency:
1. `src/utils/error.c` - strlen → ft_strlen
2. `src/scene/scene.c` - memset → ft_bzero
3. `src/scene/object_list.c` - memcpy → ft_memcpy
4. `src/window/mlx_context.c` - memset → ft_bzero
5. `src/parser/parser.c` - strcpy → ft_strlcpy
6. `src/parser/parse_objects.c` - snprintf → **format_id() 사용**
7. `src/parser/parse_cylinder.c` - snprintf → **format_id() 사용**
8. `src/utils/format_object_id.c` - snprintf → **format_id() 사용**
9. `src/bvh_vis/bvh_vis_node.c` - strcpy, strcat → ft_strlcpy/ft_strlcat, snprintf "[d=%d]" → ft_itoa, snprintf "%.2f" → **float_to_str()**
10. `src/bvh_vis/bvh_vis_print.c` - fprintf → ft_putstr_fd

### Phase 6: Verification

```bash
# Compile (macOS)
make fclean && make

# Verify no forbidden functions
grep -rn "snprintf\|fprintf\|[^_]strcpy\|[^_]strcat\|[^_]strlen\|[^_]memset\|[^_]memcpy" src/

# Test rendering
./miniRT scenes/basic.rt
```

## Common Patterns

### snprintf replacement (헬퍼 함수 사용 - 권장)

```c
// Before
snprintf(id, 8, "sp-%d", count);

// After (format_id 헬퍼 사용)
format_id(id, 8, "sp-", count);
```

### snprintf replacement (직접 변환 - 단순 케이스)

```c
// Before
snprintf(id, 8, "sp-%d", count);

// After (직접 변환 시 반드시 free 필요)
char *num = ft_itoa(count);
if (!num)
    return (error_handler());  // NULL 체크 필수
ft_strlcpy(id, "sp-", 8);
ft_strlcat(id, num, 8);
free(num);  // 반드시 해제
```

### fprintf(stderr) replacement

```c
// Before
fprintf(stderr, "Error: %s\n", msg);

// After
ft_putstr_fd("Error: ", 2);
ft_putstr_fd(msg, 2);
ft_putstr_fd("\n", 2);
```

### memset → ft_bzero

```c
// Before
memset(ptr, 0, sizeof(t_struct));

// After
ft_bzero(ptr, sizeof(t_struct));
```

## Verification Checklist

### 필수 검증 (SC-001, SC-002, SC-003)

- [ ] `make` compiles without warnings (macOS)
- [ ] `make` compiles without warnings (Linux)
- [ ] No forbidden functions in src/ (grep check)
- [ ] All test scenes render correctly (visual comparison)
- [ ] No memory leaks (valgrind/leaks)

### 확장 검증 (SC-004, SC-006) - 실측 방법

**SC-004: 대용량 객체 처리**
```bash
# 테스트 씬 생성 (Python 스크립트 또는 수동)
# 10,000개 객체가 포함된 .rt 파일 생성 후 실행
./miniRT scenes/stress_test_10k.rt
# 기대 결과: crash 없이 렌더링 완료
```

**SC-006: 메모리 사용량 비교**
```bash
# 변경 전 메모리 측정 (macOS)
/usr/bin/time -l ./miniRT scenes/complex.rt 2>&1 | grep "maximum resident"

# 변경 후 동일 명령 실행, 결과 비교
# 기대 결과: 5% 이내 차이

# Linux (valgrind massif)
valgrind --tool=massif ./miniRT scenes/complex.rt
ms_print massif.out.*
```

### 검증 우선순위

| 우선순위 | 항목 | 필수 여부 |
|----------|------|-----------|
| P1 | 컴파일 성공 | 필수 |
| P1 | forbidden function 제거 | 필수 |
| P1 | 메모리 누수 없음 | 필수 |
| P2 | 렌더링 결과 동일 | 필수 |
| P3 | 대용량 처리 | 권장 |
| P3 | 메모리 사용량 동등 | 권장 |
