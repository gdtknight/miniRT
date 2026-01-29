# Tasks: Replace Forbidden Functions & Integrate libft

**Input**: Design documents from `/specs/022-replace-forbidden-func/`
**Prerequisites**: plan.md (required), spec.md (required), data-model.md, quickstart.md

**Tests**: Not explicitly requested - manual verification via `make` and rendering tests

**Organization**: Tasks grouped by user story for independent implementation and testing

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (US1-US6)
- Include exact file paths in descriptions

---

## Phase 1: Setup

**Purpose**: Verify prerequisites and prepare for libft integration

- [ ] T001 Verify libft submodule is initialized at lib/libft/
- [ ] T002 Verify libft.h exists at lib/libft/includes/libft.h
- [ ] T003 Verify libft Makefile works by running `make -C lib/libft`

**Checkpoint**: libft submodule ready for integration

---

## Phase 2: Foundational - libft Integration (US6) 🎯 MVP Foundation

**Goal**: Integrate libft library and remove custom ft_* implementations

**Independent Test**: `make` compiles with libft linked, custom ft_* files removed

**⚠️ CRITICAL**: All subsequent phases depend on this phase completion

### Makefile Integration

- [ ] T004 [US6] Add LIBFT_DIR and LIBFT variables in Makefile
- [ ] T005 [US6] Add libft include path (-Ilib/libft/includes) to INCLUDES in Makefile
- [ ] T006 [US6] Add libft linking (-Llib/libft -lft) to LIBS in Makefile
- [ ] T007 [US6] Add $(LIBFT) target rule to compile libft in Makefile
- [ ] T008 [US6] Add $(LIBFT) as dependency for $(NAME) in Makefile
- [ ] T009 [US6] Update clean target to call `make -C $(LIBFT_DIR) clean` in Makefile
- [ ] T010 [US6] Update fclean target to call `make -C $(LIBFT_DIR) fclean` in Makefile
- [ ] T011 [US6] Remove src/utils/ft_atoi.c from SRCS in Makefile
- [ ] T012 [US6] Remove src/utils/memory.c from SRCS in Makefile

### Header File Updates

- [ ] T013 [P] [US6] Add `#include "libft.h"` to includes/minirt.h
- [ ] T014 [P] [US6] Remove custom ft_atoi prototype from includes/minirt.h
- [ ] T015 [P] [US6] Remove ft_memcpy, ft_memset prototypes from includes/utils.h

### Remove Custom Implementations

- [ ] T016 [US6] Delete src/utils/ft_atoi.c (use libft ft_atoi)
- [ ] T017 [US6] Delete src/utils/memory.c (use libft ft_memcpy, ft_memset)
- [ ] T018 [US6] Remove ft_strcpy function from src/hud/hud_format_helpers.c
- [ ] T019 [US6] Remove ft_numlen function from src/hud/hud_format_helpers.c
- [ ] T020 [US6] Remove ft_itoa_buf function from src/hud/hud_format_helpers.c
- [ ] T021 [US6] Update hud_format_helpers.c callers to use libft equivalents

### Add Helper Functions

- [ ] T022 [US6] Add format_id() helper function to src/utils/format_object_id.c
- [ ] T023 [US6] Add format_id() prototype to includes/format_object_id.h (or appropriate header)
- [ ] T024 [US6] Create src/utils/format_helpers.c with float_to_str() helper function
- [ ] T025 [US6] Add float_to_str() prototype to includes/utils.h (or appropriate header)
- [ ] T026 [US6] Add src/utils/format_helpers.c to Makefile SRCS

**Checkpoint**: `make fclean && make` succeeds with libft linked, custom ft_* removed

---

## Phase 3: Forbidden Function Replacement (US1 + US2)

**Goal**: Replace all forbidden functions with libft equivalents, ensure buffer safety

**Independent Test**: `grep -rn "snprintf\|fprintf\|[^_]strcpy\|[^_]strcat\|[^_]strlen\|[^_]memset\|[^_]memcpy" src/` returns no matches

### Simple Replacements (strlen, memset, memcpy)

- [ ] T027 [P] [US1] Replace strlen with ft_strlen in src/utils/error.c
- [ ] T028 [P] [US1] Replace memset with ft_bzero in src/scene/scene.c
- [ ] T029 [P] [US1] Replace memcpy with ft_memcpy in src/scene/object_list.c
- [ ] T030 [P] [US1] Replace memset with ft_bzero in src/window/mlx_context.c

### strcpy/strcat Replacements

- [ ] T031 [P] [US1] Replace strcpy with ft_strlcpy in src/parser/parser.c

### snprintf Replacements (using format_id helper)

- [ ] T032 [P] [US2] Replace snprintf with format_id() in src/parser/parse_objects.c
- [ ] T033 [P] [US2] Replace snprintf with format_id() in src/parser/parse_cylinder.c
- [ ] T034 [P] [US2] Replace snprintf with format_id() in src/utils/format_object_id.c

### bvh_vis Module Replacements

- [ ] T035 [P] [US1] Replace strcpy with ft_strlcpy in src/bvh_vis/bvh_vis_node.c
- [ ] T036 [P] [US1] Replace strcat with ft_strlcat in src/bvh_vis/bvh_vis_node.c
- [ ] T037 [US2] Replace snprintf "[d=%d]" pattern with ft_itoa+ft_strlcpy in src/bvh_vis/bvh_vis_node.c
- [ ] T038 [US2] Replace snprintf "%.2f" patterns with float_to_str()+ft_strlcpy in src/bvh_vis/bvh_vis_node.c
- [ ] T039 [P] [US1] Replace fprintf with ft_putstr_fd in src/bvh_vis/bvh_vis_print.c

**Checkpoint**: `make` compiles without warnings, grep returns no forbidden functions

---

## Phase 4: Integer Overflow Prevention (US3)

**Goal**: Add overflow checks for capacity doubling and size calculations

**Independent Test**: Stress test with large object counts, no crashes

### Capacity Growth Safety

- [ ] T040 [US3] Add overflow check before capacity doubling in src/scene/object_list.c
- [ ] T041 [US3] Add safe multiplication check for malloc size in src/scene/object_list.c

### Pixel Offset Safety

- [ ] T042 [P] [US3] Review pixel offset calculation in src/render/render.c for overflow risk
- [ ] T043 [P] [US3] Review pixel offset calculation in src/window/mlx_pixel.c for overflow risk
- [ ] T044 [P] [US3] Review pixel calculation in src/hud/hud_init.c for overflow risk

**Checkpoint**: Large scenes (1000+ objects) process without integer overflow

---

## Phase 5: Cross-Platform Verification (US4)

**Goal**: Ensure code compiles and runs on both macOS and Linux

**Independent Test**: `make` succeeds on both platforms with same behavior

- [ ] T045 [US4] Test compilation on macOS with clang
- [ ] T046 [US4] Test compilation on Linux with gcc
- [ ] T047 [US4] Verify rendering output matches on both platforms
- [ ] T048 [US4] Fix any platform-specific warnings or issues

**Checkpoint**: Clean compilation and identical behavior on macOS and Linux

---

## Phase 6: Numeric Conversion Safety (US5)

**Goal**: Ensure safe handling of boundary numeric values

**Independent Test**: Scene files with edge-case values handled gracefully

- [ ] T049 [P] [US5] Review ft_atoi overflow handling (libft version)
- [ ] T050 [P] [US5] Review RGB validation in src/parser/parse_validation.c
- [ ] T051 [P] [US5] Review float-to-int conversions in src/hud/hud_format.c
- [ ] T052 [US5] Add clamping for out-of-range values where needed

**Checkpoint**: Invalid numeric values in scene files produce appropriate errors

---

## Phase 7: Polish & Final Verification

**Purpose**: Complete verification and cleanup

### Verification

- [ ] T053 Run forbidden function grep check: `grep -rn "snprintf\|fprintf\|[^_]strcpy\|[^_]strcat\|[^_]strlen\|[^_]memset\|[^_]memcpy" src/`
- [ ] T054 Run memory leak check with valgrind or leaks tool
- [ ] T055 Test all scenes in scenes/ directory for correct rendering
- [ ] T056 Verify format_id() and float_to_str() properly free memory in all paths

### Documentation

- [ ] T057 [P] Update any affected documentation if needed
- [ ] T058 Run quickstart.md verification checklist

**Checkpoint**: All success criteria (SC-001 through SC-008) verified

---

## Dependencies & Execution Order

### Phase Dependencies

```
Phase 1 (Setup)
    ↓
Phase 2 (Foundational - US6) ← BLOCKS ALL OTHER PHASES
    ↓
Phase 3 (US1+US2) ─┬─→ Phase 4 (US3)
                   │
                   └─→ Phase 5 (US4)
                   │
                   └─→ Phase 6 (US5)
                           ↓
                    Phase 7 (Polish)
```

### User Story Dependencies

| Story | Dependencies | Can Parallelize With |
|-------|--------------|---------------------|
| US6 | Phase 1 | None (foundational) |
| US1 | US6 | US2 (same phase) |
| US2 | US6 | US1 (same phase) |
| US3 | US1, US2 | US4, US5 |
| US4 | US1, US2 | US3, US5 |
| US5 | US1, US2 | US3, US4 |

### Parallel Opportunities

**Phase 2 (after Makefile changes):**
- T013, T014, T015 can run in parallel (different header files)

**Phase 3 (all marked [P]):**
- T024-T028 (simple replacements) can run in parallel
- T029-T031 (snprintf replacements) can run in parallel
- T032-T035 (bvh_vis) can run in parallel

**Phase 4-6:**
- Review tasks (T038-T040, T045-T047) can run in parallel

---

## Parallel Example: Phase 3 Forbidden Function Replacement

```bash
# Launch all simple replacements in parallel:
T024: "Replace strlen with ft_strlen in src/utils/error.c"
T025: "Replace memset with ft_bzero in src/scene/scene.c"
T026: "Replace memcpy with ft_memcpy in src/scene/object_list.c"
T027: "Replace memset with ft_bzero in src/window/mlx_context.c"
T028: "Replace strcpy with ft_strlcpy in src/parser/parser.c"

# Launch all snprintf replacements in parallel:
T029: "Replace snprintf with format_id() in src/parser/parse_objects.c"
T030: "Replace snprintf with format_id() in src/parser/parse_cylinder.c"
T031: "Replace snprintf with format_id() in src/utils/format_object_id.c"
```

---

## Implementation Strategy

### MVP First (Phase 1 + 2 + 3)

1. Complete Phase 1: Setup verification
2. Complete Phase 2: libft integration (US6) - **CRITICAL PATH**
3. Complete Phase 3: Forbidden function replacement (US1 + US2)
4. **STOP and VALIDATE**: `make` succeeds, grep shows no forbidden functions
5. This achieves: SC-001, SC-002, SC-007, SC-008

### Incremental Delivery

| Increment | Phases | Success Criteria Achieved |
|-----------|--------|---------------------------|
| MVP | 1, 2, 3 | SC-001, SC-002, SC-007, SC-008 |
| +Stability | 4 | SC-004, SC-005 |
| +Portability | 5 | SC-001 (Linux) |
| +Robustness | 6 | SC-005 (complete) |
| Complete | 7 | SC-003, SC-006, All verified |

---

## Notes

- [P] tasks = different files, no dependencies on each other
- [US#] label maps task to specific user story
- US6 (libft integration) is foundational - must complete first
- US1 and US2 are tightly coupled (forbidden functions → buffer safety)
- float_to_str() helper needed for bvh_vis float formatting (%.2f patterns)
- format_id() only for "prefix-%d" ID patterns
- Verify after each checkpoint before proceeding
- Commit after each logical task group
- Run `make` frequently to catch issues early
