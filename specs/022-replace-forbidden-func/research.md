# Research: Replace Forbidden Functions & Integrate libft

**Date**: 2026-01-29
**Feature**: 022-replace-forbidden-func

## Research Questions Resolved

### 1. libft Function Availability

**Decision**: Use libft from lib/libft submodule for all available functions

**Rationale**:
- libft submodule correctly configured at `lib/libft` (git@github.com:gdtknight/libft.git)
- Provides all necessary functions except ft_atof
- 42 subject explicitly allows libft usage

**Available Functions** (from libft.h):
| Function | Signature |
|----------|-----------|
| ft_strlen | `size_t ft_strlen(const char *s)` |
| ft_strlcpy | `size_t ft_strlcpy(char *dst, const char *src, size_t size)` |
| ft_strlcat | `size_t ft_strlcat(char *dst, const char *src, size_t size)` |
| ft_memset | `void *ft_memset(void *s, int c, size_t n)` |
| ft_memcpy | `void *ft_memcpy(void *dest, const void *src, size_t n)` |
| ft_bzero | `void ft_bzero(void *s, size_t n)` |
| ft_atoi | `int ft_atoi(const char *nptr)` |
| ft_itoa | `char *ft_itoa(int n)` |
| ft_putstr_fd | `void ft_putstr_fd(char *s, int fd)` |
| ft_putchar_fd | `void ft_putchar_fd(char c, int fd)` |

**Alternatives Considered**:
- Implement missing functions in libft → Not needed, all critical functions available
- Keep all custom implementations → Rejected, causes code duplication

### 2. ft_atof Handling

**Decision**: Retain custom ft_atof implementation in src/utils/ft_atof.c

**Rationale**:
- Standard `atof()` is not in the allowed function list
- libft does not provide ft_atof
- Custom implementation is required for .rt file parsing (float values)

**Alternatives Considered**:
- Add ft_atof to libft → Possible but modifies external dependency
- Use ft_atoi with manual decimal handling → Complex and error-prone

### 3. snprintf Replacement Strategy

**Decision**: Use ft_itoa + ft_strlcpy + ft_strlcat combination

**Rationale**:
- snprintf is forbidden
- libft provides ft_itoa for integer-to-string conversion
- ft_strlcpy/ft_strlcat provide safe bounded string operations
- Pattern: `prefix + ft_itoa(number) + suffix`

**Example**:
```c
// snprintf(id, 8, "sp-%d", n) becomes:
char *num = ft_itoa(n);
ft_strlcpy(id, "sp-", 8);
ft_strlcat(id, num, 8);
free(num);
```

**Alternatives Considered**:
- Custom snprintf implementation → Overkill for limited use cases
- Static buffer approach → Risk of buffer overflow

### 4. fprintf(stderr) Replacement Strategy

**Decision**: Use ft_putstr_fd with fd=2

**Rationale**:
- fprintf is forbidden
- ft_putstr_fd(str, 2) writes directly to stderr
- Simple and compliant with allowed functions

**Alternatives Considered**:
- Use write(2, ...) directly → Works but less readable
- Combine with ft_strlen for write() → More complex

### 5. Buffer Overflow Prevention

**Decision**: Always use bounded functions with explicit size parameters

**Rationale**:
- ft_strlcpy/ft_strlcat require destination size
- Prevents buffer overflow by design
- GCC format-truncation warnings resolved

**Pattern**:
```c
ft_strlcpy(dst, src, sizeof(dst));  // Always specify size
ft_strlcat(dst, suffix, sizeof(dst));
```

### 6. Custom Implementations to Remove

**Decision**: Remove files/functions that duplicate libft

| File | Functions | Action |
|------|-----------|--------|
| src/utils/ft_atoi.c | ft_atoi, ft_isspace, ft_isdigit | REMOVE entire file |
| src/utils/memory.c | ft_memcpy, ft_memset | REMOVE entire file |
| src/hud/hud_format_helpers.c | ft_strcpy, ft_numlen, ft_itoa_buf | REMOVE these functions only |

**Functions to Keep in hud_format_helpers.c**:
- Any HUD-specific formatting functions not duplicating libft

### 7. Header File Changes

**Decision**: Include libft.h centrally, remove duplicate prototypes

**Changes**:
- minirt.h: Add `#include "libft.h"`
- minirt.h: Remove `int ft_atoi(const char *str);` prototype
- utils.h: Remove `ft_memcpy`, `ft_memset` prototypes
- hud.h: Remove `ft_strcpy`, `ft_numlen`, `ft_itoa_buf` prototypes

### 8. Makefile Integration

**Decision**: Standard libft integration pattern

**Rationale**:
- 42 projects commonly use this pattern
- libft compiled first, then linked
- Clean/fclean propagate to libft

**Key Changes**:
```makefile
LIBFT = lib/libft/libft.a
INCLUDES += -Ilib/libft/includes
LIBS += -Llib/libft -lft

$(NAME): $(LIBFT) $(OBJS)
```

### 9. Cross-Platform Compatibility

**Decision**: No platform-specific changes needed

**Rationale**:
- libft is portable C code
- ft_putstr_fd uses POSIX write() which works on both platforms
- No platform-specific functions used in replacements

**Verification**: Compile with both clang (macOS) and gcc (Linux)

## Unresolved Issues

None - all research questions resolved.
