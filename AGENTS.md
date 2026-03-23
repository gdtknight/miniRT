# miniRT Development Guidelines

Auto-generated from all feature plans. Last updated: 2026-03-22

## Active Technologies

- C (42 Norm v4.1 compliant)
- libft (`lib/libft`)
- MiniLibX (macOS/Linux variants)
- math library (`-lm`)
- Bash helper scripts in `scripts/`

## Project Structure

```text
src/                 # miniRT source code by module
includes/            # public/internal headers
tests/               # ad-hoc C tests and baselines
scenes/              # valid/invalid/perf .rt scenes
lib/                 # libft + minilibx-macos/linux
docs/                # architecture, build, module docs
scripts/             # build/validation/test helper scripts
```

## Commands

```bash
# Build
make
make bonus           # same as make (42 evaluator compatibility)
make clean
make fclean
make re

# Norm / style
make norm
bash scripts/validate_norminette.sh

# Run
./miniRT scenes/valid/valid_smoke_simple.rt
./miniRT scenes/valid/valid_smoke_simple.rt --bvh-vis

# Scripted checks (GUI/session environment may be required)
bash scripts/test_miniRT.sh
bash scripts/test_scenes.sh
bash scripts/test/test_controls.sh
bash scripts/test/test_progress.sh
```

## Code Style

C (42 Norm v4.1 compliant): Follow standard conventions

## Recent Changes

- 023-parsing-robustness: Added strict parsing/validation hardening and parser contracts

<!-- MANUAL ADDITIONS START -->
## Agent Notes

- Prefer touching files under `src/` + matching headers in `includes/`.
- Keep functions within 42 Norm limits (line count/parameters/locals/file function count).
- Prefer project Makefile targets over custom compile commands.
- For behavior checks, start with `scenes/valid/valid_smoke_simple.rt` before larger perf scenes.
<!-- MANUAL ADDITIONS END -->
