<!--
  Sync Impact Report
  ===================
  Version change: 1.0.0 → 1.1.0
  Modified principles:
    - I. 42 Norminette Compliance: expanded with full Norm v4.1 rules
      (struct/union/global prefixes, 5-var limit, 5-func-per-file,
      switch/case/goto/do..while ban, no struct decl in .c, comment
      restrictions, 42 header, return parentheses, multiline macros)
    - III. Minimal External Dependencies: added explicit allowed
      function list from subject
    - V. Visual Correctness: clarified mandatory vs bonus lighting,
      added intersection inside-handling, error output format
  Added sections:
    - VI. Stability & Crash Prevention (from eval criteria)
    - VII. Window Management (from subject + eval)
    - Scene File Contract subsection under Technical Constraints
    - Bonus Scope section
  Removed sections: None
  Templates requiring updates:
    - .specify/templates/plan-template.md ✅ no changes needed
    - .specify/templates/spec-template.md ✅ no changes needed
    - .specify/templates/tasks-template.md ✅ no changes needed
  Follow-up TODOs: None
-->

# miniRT Constitution

## Core Principles

### I. 42 Norminette Compliance

All source code MUST pass the Norminette (v4.1) without errors.
Non-compliant code receives an automatic zero at evaluation.

**Naming**:
- `s_` prefix for structs, `t_` for typedefs, `u_` for unions,
  `e_` for enums, `g_` for globals
- All identifiers MUST be snake_case (lowercase + digits + `_`)
- File and directory names MUST be snake_case
- Identifiers MUST be explicit or mnemonic, readable in English
- Non-const, non-static global variables are forbidden unless
  the project explicitly allows them

**Formatting**:
- Maximum 25 lines per function body (excluding braces)
- Maximum 80 columns per line (tabs count as their width)
- Indent with 4-char tabs (ASCII 9), not spaces
- One variable declaration per line; declarations at top of scope
- No declaration + initialization on the same line (except
  globals, statics, constants)
- Maximum 5 variable declarations per function
- Empty line between declarations and code; no other empty lines
  within a function
- One instruction or control structure per line
- No trailing whitespace; no consecutive empty lines or spaces
- Braces on their own line (except struct/enum/union declarations)

**Functions**:
- Maximum 4 named parameters per function
- Maximum 5 function definitions per `.c` file
- Functions with no arguments MUST use `void`
- Parameters in prototypes MUST be named
- Return values MUST be wrapped in parentheses
- Single tab between return type and function name

**Headers**:
- Include guards required (e.g., `FT_FOO_H` for `ft_foo.h`)
- No `.c` file inclusion in headers or other `.c` files
- All includes at top of file; unused includes forbidden
- Structs MUST NOT be declared in `.c` files

**Forbidden constructs**:
- `for`, `do...while`, `switch`, `case`, `goto`
- Ternary operator (`?:`)
- Variable-length arrays (VLAs)
- Implicit type in variable declarations
- Multiline macros
- Comments inside function bodies (end-of-line or own line only)

**Other**:
- Every `.c` and `.h` file MUST begin with the 42 header
- Macro names MUST be all uppercase
- The file MUST compile

### II. Memory Safety

The program MUST produce zero memory leaks. Evaluation uses
`leaks` (macOS) or Valgrind (Linux); any leak = evaluation
failure.
- Every heap allocation MUST have a corresponding free on all
  exit paths
- `malloc` return values MUST be checked; allocation failure
  MUST trigger graceful shutdown with error message to stderr
- All cleanup MUST be centralized through a single teardown path

### III. Allowed Functions & Dependencies

The ONLY permitted external functions are:
- `open`, `close`, `read`, `write`, `printf`, `malloc`, `free`,
  `perror`, `strerror`, `exit`
- All functions from the math library (`-lm`)
- All functions from the MinilibX library
- `gettimeofday()`
- libft (if used, sources MUST be in a `libft/` directory and
  compiled by the project Makefile)

No other third-party code, package managers, or runtime
dependencies are allowed.

Rationale: The subject explicitly enumerates allowed functions;
using any other function results in a "Forbidden function" flag
and evaluation failure.

### IV. Modular Architecture

Source code MUST be organized into domain-specific modules:
- Each module (ray, render, spatial, parser, window, hud,
  lighting, utils, etc.) MUST reside in its own directory
  under `src/`
- Cross-module coupling MUST be minimized; modules communicate
  through well-defined struct interfaces in `includes/`
- No circular header dependencies
- Each `.c` file MUST contain at most 5 functions (Norm rule)

### V. Visual Correctness

**Mandatory rendering**:
- Three geometric objects MUST be supported: sphere, plane,
  cylinder
- All intersections and object insides MUST be handled correctly
- Objects MUST support translation and rotation (except spheres
  and lights cannot rotate)
- Objects MUST support property resizing (sphere diameter,
  cylinder width/height)
- Ambient lighting + diffuse lighting MUST be implemented
- Hard shadows MUST be implemented
- Camera MUST work at arbitrary positions and orientations
  including all axes and off-axis positions

**Bonus rendering** (only evaluated if mandatory is perfect):
- Specular reflection (full Phong model)
- Checkerboard color disruption
- Colored and multi-spot lights
- Additional 2nd-degree objects (cone, hyperboloid, paraboloid)
- Bump map textures

**Error handling**:
- Malformed `.rt` files MUST cause the program to exit with
  `"Error\n"` followed by an explicit error message to stderr
- The `.rt` extension MUST be validated

### VI. Stability & Crash Prevention

The program MUST NOT crash under any circumstances. Segfault,
bus error, double free, or any unexpected termination results
in an automatic zero.
- All pointer dereferences MUST be guarded against NULL
- Array bounds MUST be respected
- All error paths MUST exit cleanly (no abort, no undefined
  behavior)

Rationale: The evaluation sheet specifies that any crash during
defense = final grade 0, regardless of other scores.

### VII. Window Management

- The window MUST remain fluid: switching windows, minimizing,
  maximizing, and partial occlusion MUST NOT cause display
  corruption or program exit
- Pressing ESC MUST close the window and exit cleanly
- Clicking the window's red cross MUST close and exit cleanly
- Window content MUST remain consistent after resize

## Technical Constraints

- **Language**: C (Norm v4.1 compliant)
- **Build system**: GNU Make
  - MUST support `all`, `clean`, `fclean`, `re`, `bonus` targets
  - `all` MUST be the default target
  - MUST NOT relink unnecessarily
  - All source files MUST be explicitly named (no wildcards)
  - If libft is used, Makefile MUST compile it automatically
- **Compiler flags**: `-Wall -Wextra -Werror` MUST be enabled
- **Executable name**: `miniRT`
- **Input**: Scene description file with `.rt` extension as
  first argument

**Scene file contract** (`.rt` format):
- Elements separated by one or more newlines
- Fields within elements separated by one or more spaces
- Elements can appear in any order
- Capital-letter elements (A, C, L) can only appear once
- Supported elements: `A` (ambient), `C` (camera), `L` (light),
  `sp` (sphere), `pl` (plane), `cy` (cylinder)

## Development Workflow

- Every feature MUST be developed on a dedicated branch
- Commits MUST compile without warnings under
  `-Wall -Wextra -Werror`
- Scene files used for testing MUST be committed alongside code
  changes when they demonstrate new functionality
- Error messages MUST be written to stderr

## Governance

This constitution is the authoritative reference for all miniRT
development decisions. In case of conflict between convenience
and a principle stated above, the principle prevails.

- **Amendments**: Any principle change MUST be documented with
  rationale, approved by the project maintainer, and reflected
  in a version bump
- **Versioning**: MAJOR for principle removals/redefinitions,
  MINOR for new principles or material expansions, PATCH for
  clarifications
- **Compliance**: Every PR and code review MUST verify adherence
  to these principles before merge

**Version**: 1.1.0 | **Ratified**: 2025-12-15 | **Last Amended**: 2026-01-27
