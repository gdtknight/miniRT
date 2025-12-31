# miniRT_final Development Guidelines

Auto-generated from all feature plans. Last updated: 2025-12-15

## Active Technologies
- C (C99 standard), Makefile, YAML (GitHub Actions), Shell scripts (Bash) (001-cicd-pipeline-github-workflow)
- Git repository, GitHub Actions artifacts for build logs and releases (001-cicd-pipeline-github-workflow)
- Bash 4.0+, C (C99 standard for miniRT), GitHub Actions YAML (001-workflow-improvements)
- File system for logs (logs/), build artifacts (build/), git for version control (001-workflow-improvements)
- C (C99 standard), compiled with cc/gcc + MiniLibX (minilibx-linux on Linux, minilibx-macos on macOS), X11/Xext (Linux), OpenGL/AppKit (macOS) (001-scene-info-hud)
- N/A (in-memory scene data from RT file parsing) (001-scene-info-hud)
- C (C99/C11 standard, gcc/clang compatible) (001-rendering-performance-optimization)
- Scene files (.rt format) loaded at startup, in-memory representation only (001-rendering-performance-optimization)
- C (C99 standard), compiled with gcc/clang + MiniLibX (X11 for Linux, OpenGL/AppKit for macOS), libm (math library) (006-cross-platform-keyboard)
- N/A (no persistent storage for keyboard handling) (006-cross-platform-keyboard)
- C (C99 standard), 42 School norminette compliance + MiniLibX (42 School graphics library), libm (math library), libft (custom C library) (007-hud-improvements)
- N/A (no persistent storage required for this feature) (007-hud-improvements)
- C (C99 standard with 42 School norminette compliance) + MiniLibX (graphics), libft (custom C library), math.h (standard math functions) (008-bvh-performance-diagnostics)
- N/A (all data in-memory during runtime) (008-bvh-performance-diagnostics)

- C (C99 or later with -std=c99 or compatible) + MinilibX (X11/XQuartz graphics library), libm (math library), optional libft (001-raytracer)

## Project Structure

```text
src/
tests/
```

## Commands

# Add commands for C (C99 or later with -std=c99 or compatible)

## Code Style

C (C99 or later with -std=c99 or compatible): Follow standard conventions

## Recent Changes
- 008-bvh-performance-diagnostics: Added C (C99 standard with 42 School norminette compliance) + MiniLibX (graphics), libft (custom C library), math.h (standard math functions)
- 007-hud-improvements: Added C (C99 standard), 42 School norminette compliance + MiniLibX (42 School graphics library), libm (math library), libft (custom C library)
- 006-cross-platform-keyboard: Added C (C99 standard), compiled with gcc/clang + MiniLibX (X11 for Linux, OpenGL/AppKit for macOS), libm (math library)


<!-- MANUAL ADDITIONS START -->
<!-- MANUAL ADDITIONS END -->
