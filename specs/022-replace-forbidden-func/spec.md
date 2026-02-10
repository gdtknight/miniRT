# Feature Specification: Replace Forbidden Functions & Integrate libft

**Feature Branch**: `022-replace-forbidden-func`
**Created**: 2026-01-29
**Updated**: 2026-01-29
**Status**: Draft
**Input**: User description: "Replace forbidden functions per miniRT.md with libft equivalents, fix overflow/underflow issues, ensure macOS/Linux compatibility. Integrate libft library from lib/libft submodule and replace custom ft_* implementations."

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Compile Without Forbidden Function Errors (Priority: P1)

A developer compiles the miniRT project with `-Wall -Wextra -Werror` flags, and the build succeeds without any errors related to forbidden functions or format-truncation warnings.

**Why this priority**: Core requirement - the project cannot pass 42 evaluation if it uses non-allowed functions. This is a blocking issue.

**Independent Test**: Can be fully tested by running `make` and verifying clean compilation without warnings/errors on both macOS and Linux.

**Acceptance Scenarios**:

1. **Given** the codebase contains forbidden function calls, **When** a developer runs `make`, **Then** the build fails with function-related errors
2. **Given** all forbidden functions are replaced with libft equivalents, **When** a developer runs `make`, **Then** the build succeeds without warnings

---

### User Story 2 - Safe String Formatting Without Buffer Overflow (Priority: P1)

When the program generates object IDs or debug strings, it constructs them safely without risk of buffer overflow, even with edge-case input values.

**Why this priority**: Buffer overflows cause undefined behavior, crashes, and potential security vulnerabilities. These must be fixed alongside forbidden function replacement.

**Independent Test**: Can be tested by creating scenes with maximum allowed objects and verifying no crashes or truncated IDs.

**Acceptance Scenarios**:

1. **Given** a scene with 9999 spheres, **When** the parser assigns IDs, **Then** all IDs are correctly generated without buffer overflow
2. **Given** a formatted string operation, **When** the destination buffer is smaller than the source, **Then** the string is safely truncated with null terminator

---

### User Story 3 - Safe Integer Operations Without Overflow (Priority: P2)

When the program performs arithmetic operations (capacity doubling, size calculations, pixel offset computations), it handles edge cases without integer overflow.

**Why this priority**: Integer overflow leads to incorrect memory allocation, buffer overflows, and crashes. Critical for stability but less visible than compilation errors.

**Independent Test**: Can be tested by creating stress-test scenes that exercise capacity growth and large coordinate values.

**Acceptance Scenarios**:

1. **Given** an object list near maximum capacity, **When** a new object is added, **Then** the capacity growth is handled safely or an error is reported
2. **Given** a large image dimension, **When** pixel offset is calculated, **Then** the calculation produces correct results without overflow

---

### User Story 4 - Cross-Platform Compatibility (Priority: P2)

The program compiles and runs correctly on both macOS and Linux environments without platform-specific issues.

**Why this priority**: 42 evaluations may occur on either platform. Ensuring compatibility prevents evaluation failures.

**Independent Test**: Can be tested by compiling and running the same test scenes on both macOS and Linux.

**Acceptance Scenarios**:

1. **Given** the codebase with replaced functions, **When** compiled on macOS with clang, **Then** the build succeeds and program runs correctly
2. **Given** the codebase with replaced functions, **When** compiled on Linux with gcc, **Then** the build succeeds and program runs correctly

---

### User Story 5 - Safe Numeric Conversions (Priority: P3)

When the program parses numeric values from scene files or performs type conversions, it handles boundary values correctly without undefined behavior.

**Why this priority**: Important for robustness but less critical than core functionality. Bad input files should be rejected gracefully.

**Independent Test**: Can be tested with scene files containing boundary numeric values.

**Acceptance Scenarios**:

1. **Given** a scene file with color value 256, **When** the parser processes it, **Then** an appropriate error is reported
2. **Given** a floating-point value outside int range, **When** converted to integer, **Then** the conversion is handled safely

---

### User Story 6 - Integrate libft Library and Remove Custom Implementations (Priority: P1)

The project uses the libft library from lib/libft submodule instead of custom ft_* function implementations scattered across src/ files.

**Why this priority**: Consolidates code, reduces maintenance burden, ensures consistency with 42 libft standards, and eliminates duplicate code.

**Independent Test**: Can be tested by verifying that custom ft_* source files are removed and the project compiles/links against lib/libft.

**Acceptance Scenarios**:

1. **Given** libft submodule is configured at lib/libft, **When** a developer runs `make`, **Then** the build compiles libft first and links it to miniRT
2. **Given** custom ft_* implementations exist in src/, **When** libft integration is complete, **Then** custom implementations are removed and libft functions are used
3. **Given** Makefile references custom ft_* source files, **When** integration is complete, **Then** Makefile is updated to use libft.a library

---

### Edge Cases

- What happens when object count exceeds ID buffer capacity (>9999 objects)?
- How does the system handle malloc failure during capacity growth?
- What happens when color RGB values are negative or exceed 255?
- How does the system behave with extremely large scene files?
- What happens if libft submodule is not initialized?
- How does the build handle missing libft.a?

## Requirements *(mandatory)*

### Functional Requirements

#### Forbidden Function Replacement

- **FR-001**: System MUST NOT use `snprintf()` - replace with libft string construction using `ft_itoa()`, `ft_strlcpy()`, `ft_strlcat()`
- **FR-002**: System MUST NOT use `fprintf()` - replace with `write(2, ...)` for stderr output
- **FR-003**: System MUST NOT use `strcpy()` - replace with `ft_strlcpy()` with explicit buffer size
- **FR-004**: System MUST NOT use `strcat()` - replace with `ft_strlcat()` with explicit buffer size
- **FR-005**: System MUST NOT use `strlen()` - replace with `ft_strlen()`
- **FR-006**: System MUST NOT use `memset()` - replace with `ft_memset()` or `ft_bzero()`
- **FR-007**: System MUST NOT use `memcpy()` - replace with `ft_memcpy()`

#### Buffer Overflow Prevention

- **FR-008**: All string construction operations MUST use bounded copy functions with explicit destination size
- **FR-009**: Object ID generation MUST handle IDs up to at least 4 digits (0-9999) safely within buffer limits
- **FR-010**: String concatenation operations MUST check remaining buffer space before appending

#### Integer Overflow Prevention

- **FR-011**: Capacity doubling operations MUST check for potential overflow before multiplication
- **FR-012**: Size calculations for memory allocation MUST validate against overflow
- **FR-013**: Pixel offset calculations MUST use appropriate data types to prevent overflow on large images

#### Numeric Conversion Safety

- **FR-014**: Integer parsing (`ft_atoi`) MUST handle overflow gracefully
- **FR-015**: RGB color values MUST be validated in range [0, 255] before use
- **FR-016**: Float-to-int conversions MUST clamp values to valid integer range

#### Cross-Platform Compatibility

- **FR-017**: All replacements MUST compile without warnings on both clang (macOS) and gcc (Linux)
- **FR-018**: All replacements MUST produce identical behavior on both platforms

#### libft Library Integration

- **FR-019**: Makefile MUST compile libft library from lib/libft before main project
- **FR-020**: Makefile MUST link miniRT with libft.a library
- **FR-021**: Custom ft_atoi implementation in src/utils/ft_atoi.c MUST be removed and replaced with libft version
- **FR-022**: Custom ft_atof implementation in src/utils/ft_atof.c:
  - IF libft provides ft_atof → remove custom implementation and use libft version
  - IF libft does NOT provide ft_atof → keep custom implementation (standard atof is forbidden)
- **FR-023**: Custom ft_memcpy/ft_memset implementations in src/utils/memory.c MUST be removed and replaced with libft versions
- **FR-024**: Custom ft_strcpy/ft_numlen/ft_itoa_buf implementations in src/hud/hud_format_helpers.c MUST be removed and replaced with libft versions
- **FR-025**: Header files MUST include libft.h instead of declaring custom ft_* function prototypes
- **FR-026**: Makefile MUST handle libft submodule initialization check and provide clear error message if missing

### Key Entities

- **Forbidden Functions**: Standard library functions not in the allowed list (snprintf, fprintf, strcpy, strcat, strlen, memset, memcpy)
- **Libft Library**: External library at lib/libft providing ft_* functions (ft_strlcpy, ft_strlcat, ft_strlen, ft_memset, ft_memcpy, ft_bzero, ft_itoa, ft_atoi, etc.)
- **Custom Implementations to Replace**:
  - `src/utils/ft_atoi.c` - custom ft_atoi (remove entire file, use libft)
  - `src/utils/ft_atof.c` - custom ft_atof (conditional: remove if libft provides equivalent, otherwise retain)
  - `src/utils/memory.c` - custom ft_memcpy, ft_memset (remove entire file, use libft)
  - `src/hud/hud_format_helpers.c` - custom ft_strcpy, ft_numlen, ft_itoa_buf (replace with libft equivalents)
- **Files Using Forbidden Functions** (to be updated):
  - `src/parser/parse_objects.c` - snprintf for object IDs
  - `src/parser/parse_cylinder.c` - snprintf for cylinder IDs
  - `src/parser/parser.c` - strcpy for line buffer
  - `src/utils/format_object_id.c` - snprintf for ID formatting
  - `src/utils/error.c` - strlen for error messages
  - `src/bvh_vis/bvh_vis_node.c` - strcpy, strcat, snprintf
  - `src/bvh_vis/bvh_vis_print.c` - fprintf for warnings
  - `src/window/mlx_context.c` - memset for initialization
  - `src/scene/scene.c` - memset for initialization
  - `src/scene/object_list.c` - memcpy for array growth
- **Build Artifacts**:
  - `lib/libft/libft.a` - compiled libft static library
  - Makefile dependencies and link flags

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Project compiles with zero warnings using `make` on both macOS and Linux
- **SC-002**: All 7 forbidden function types are eliminated from the codebase (0 occurrences)
- **SC-003**: All existing test scenes render identically before and after changes
- **SC-004**: No crashes or undefined behavior when processing scenes with up to 10,000 objects
- **SC-005**: All numeric overflow scenarios are handled without undefined behavior
- **SC-006**: Memory usage remains equivalent (within 5%) after changes
- **SC-007**: Custom ft_* implementations with libft equivalents are removed (ft_atoi, ft_memcpy, ft_memset, ft_strcpy removed; ft_atof retained only if libft lacks equivalent)
- **SC-008**: libft.a is successfully built and linked during `make`

## Assumptions

- The libft library at lib/libft provides all necessary replacement functions (ft_strlcpy, ft_strlcat, ft_strlen, ft_memset, ft_memcpy, ft_bzero, ft_itoa, ft_atoi)
- libft submodule is properly configured and can be compiled with its own Makefile
- ft_atof may not be available in standard libft; if so, keep custom implementation or add to libft
- Object count in practical scenes will not exceed 9999 (4-digit IDs sufficient)
- The ID buffer size of 8 bytes is intentional and should be maintained for memory efficiency
- Performance impact of libft replacements is acceptable (safety over micro-optimization)
- Integer overflow checks may add negligible runtime cost which is acceptable for safety
- libft includes a Makefile with standard targets (all, clean, fclean, re)
- libft header file (libft.h) contains all necessary function prototypes
