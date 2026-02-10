# Vector Range Validation - Implementation Task List

**Date**: 2026-02-09  
**Reference**: `./docs/vector-range-validation-plan.md`  
**Feature**: Add vector component range [-1,1] validation for camera/plane/cylinder

---

## Pre-Implementation Checklist

Before starting implementation, verify current codebase state:

- [ ] Confirm `in_range()` is still public (not static) in `parse_validation_strict.c`
- [ ] Verify `in_range()` is declared in `parser.h` (around line 196)
- [ ] Check `parse_validation_strict.c` still has exactly 5 functions
- [ ] Locate `validate_direction_vector()` calls in:
  - [ ] `parse_elements.c` (camera direction)
  - [ ] `parse_objects.c` (plane normal)
  - [ ] `parse_cylinder.c` (cylinder axis)
- [ ] Ensure clean working directory: `git status`
- [ ] Current code builds successfully: `make re`

---

## Implementation Tasks

### Phase 1: Create New Validation File

- [ ] **Task 1.1**: Create `src/parser/parse_vector_validation.c`
  - [ ] Add 42 header comment with correct date/author
  - [ ] Include `minirt.h` and `parser.h`
  - [ ] Implement `validate_vector_range()` function:
    ```c
    t_parse_result validate_vector_range(const t_vec3 *vec)
    {
        if (!vec)
            return (PARSE_ERR_FORMAT);
        if (!in_range(vec->x, -1.0, 1.0))
            return (PARSE_ERR_RANGE);
        if (!in_range(vec->y, -1.0, 1.0))
            return (PARSE_ERR_RANGE);
        if (!in_range(vec->z, -1.0, 1.0))
            return (PARSE_ERR_RANGE);
        return (PARSE_OK);
    }
    ```
  - [ ] Verify function is exactly 10 lines (Norm compliant)
  - [ ] Verify no norm violations: `norminette src/parser/parse_vector_validation.c`

---

### Phase 2: Update Build System

- [ ] **Task 2.1**: Update `Makefile`
  - [ ] Locate line with `parse_validation_strict.c` (around line 77)
  - [ ] Add new line after it: `$(SRC_DIR)/parser/parse_vector_validation.c \`
  - [ ] Verify alphabetical order in parser section
  - [ ] Test build: `make re`
  - [ ] Verify new object file created: `ls -la build/parser/parse_vector_validation.o`

---

### Phase 3: Update Header Declaration

- [ ] **Task 3.1**: Update `includes/parser.h`
  - [ ] Locate "Validation API (Enhanced)" section (around line 110-118)
  - [ ] Find `validate_direction_vector()` declaration
  - [ ] Add new declaration before it:
    ```c
    t_parse_result	validate_vector_range(const t_vec3 *vec);
    ```
  - [ ] Verify no norm violations: `norminette includes/parser.h`
  - [ ] Test build: `make re`

---

### Phase 4: Update Camera Parsing

- [ ] **Task 4.1**: Update `src/parser/parse_elements.c`
  - [ ] Locate `validate_direction_vector(&scene->camera.direction)` call
  - [ ] Add 3 lines BEFORE the validate_direction_vector call:
    ```c
    result = validate_vector_range(&scene->camera.direction);
    if (result != PARSE_OK)
        return (result);
    ```
  - [ ] Verify validation order: parse → range → zero → normalize
  - [ ] Verify no norm violations: `norminette src/parser/parse_elements.c`
  - [ ] Test build: `make re`
  - [ ] Test with valid camera: `./miniRT scenes/valid/valid_us01_basic.rt`

---

### Phase 5: Update Plane Parsing

- [ ] **Task 5.1**: Update `src/parser/parse_objects.c`
  - [ ] Locate `validate_direction_vector(&obj->data.plane.normal)` call
  - [ ] Add 3 lines BEFORE the validate_direction_vector call:
    ```c
    result = validate_vector_range(&obj->data.plane.normal);
    if (result != PARSE_OK)
        return (result);
    ```
  - [ ] Verify validation order: parse → range → zero → normalize
  - [ ] Verify no norm violations: `norminette src/parser/parse_objects.c`
  - [ ] Test build: `make re`
  - [ ] Test with valid plane: `./miniRT scenes/valid/valid_us04_intersect_plane.rt`

---

### Phase 6: Update Cylinder Parsing

- [ ] **Task 6.1**: Update `src/parser/parse_cylinder.c`
  - [ ] Locate `validate_direction_vector(&obj->data.cylinder.axis)` call
  - [ ] Add 3 lines BEFORE the validate_direction_vector call:
    ```c
    result = validate_vector_range(&obj->data.cylinder.axis);
    if (result != PARSE_OK)
        return (result);
    ```
  - [ ] Verify validation order: parse → range → zero → normalize
  - [ ] Verify no norm violations: `norminette src/parser/parse_cylinder.c`
  - [ ] Test build: `make re`
  - [ ] Test with valid cylinder: `./miniRT scenes/valid/valid_us04_intersect_cylinder.rt`

---

## Testing Phase

### Valid Scene Tests (Regression)

- [ ] **Task 7.1**: Test all valid scenes still work
  - [ ] `scenes/valid/valid_us01_basic.rt` - Basic scene
  - [ ] `scenes/valid/valid_us04_intersect_plane.rt` - Plane with normal
  - [ ] `scenes/valid/valid_us04_intersect_cylinder.rt` - Cylinder with axis
  - [ ] `scenes/valid/valid_scene_complex.rt` - Complex scene
  - [ ] Run all valid scenes: `for f in scenes/valid/*.rt; do timeout 2 ./miniRT "$f" 2>&1 | grep -q "Error" && echo "FAIL: $f" || echo "PASS"; done`

### Invalid Scene Tests (New Behavior)

- [ ] **Task 7.2**: Test invalid plane normal (existing test)
  - [ ] `./miniRT scenes/invalid/invalid_plane_normal_out_of_range.rt`
  - [ ] Expected: "Error" message with "Value out of range"
  - [ ] Verify parser rejects it (exit without rendering)

- [ ] **Task 7.3**: Create and test invalid cylinder axis
  - [ ] Create `scenes/invalid/invalid_cylinder_axis_out_of_range.rt`:
    ```rt
    A 0.2 255,255,255
    C 0,0,-15 0,0,1 70
    L 0,8,-8 0.7 255,255,255
    cy 0,0,0 5,0,0 10 20 255,0,0
    ```
  - [ ] Test: `./miniRT scenes/invalid/invalid_cylinder_axis_out_of_range.rt`
  - [ ] Expected: Error message

- [ ] **Task 7.4**: Create and test invalid camera direction
  - [ ] Create `scenes/invalid/invalid_camera_direction_out_of_range.rt`:
    ```rt
    A 0.2 255,255,255
    C 0,0,-15 0,1.5,0 70
    L 0,8,-8 0.7 255,255,255
    sp 0,0,20 10 255,0,0
    ```
  - [ ] Test: `./miniRT scenes/invalid/invalid_camera_direction_out_of_range.rt`
  - [ ] Expected: Error message

### Boundary Value Tests

- [ ] **Task 7.5**: Test boundary values (±1.0)
  - [ ] Create test with `pl 0,0,0 1.0,0,0 255,0,0` (should pass)
  - [ ] Create test with `pl 0,0,0 -1.0,0,0 255,0,0` (should pass)
  - [ ] Create test with `cy 0,0,0 0,1.0,0 10 20 255,0,0` (should pass)
  - [ ] Create test with `pl 0,0,0 1.0001,0,0 255,0,0` (should fail)

### Edge Cases

- [ ] **Task 7.6**: Test near-boundary values
  - [ ] Test with `0.99999` components (should pass and normalize)
  - [ ] Test with `1.00001` components (should fail)
  - [ ] Test with mixed valid/invalid: `2.0,0.5,0.3` (should fail on first check)

---

## Quality Assurance

### Code Quality Checks

- [ ] **Task 8.1**: Run norminette on all modified files
  - [ ] `norminette src/parser/parse_vector_validation.c`
  - [ ] `norminette src/parser/parse_elements.c`
  - [ ] `norminette src/parser/parse_objects.c`
  - [ ] `norminette src/parser/parse_cylinder.c`
  - [ ] `norminette includes/parser.h`
  - [ ] Verify: No norm violations

- [ ] **Task 8.2**: Verify function count compliance
  - [ ] `parse_vector_validation.c`: 1 function (within limit)
  - [ ] `parse_validation_strict.c`: Still 5 functions (unchanged)
  - [ ] No file exceeds 5 functions

- [ ] **Task 8.3**: Check for memory leaks
  - [ ] Test with valgrind: `valgrind --leak-check=full ./miniRT scenes/valid/valid_us01_basic.rt`
  - [ ] Test with invalid scene: `valgrind ./miniRT scenes/invalid/invalid_plane_normal_out_of_range.rt`
  - [ ] Verify: No leaks from validation code

### Build System Checks

- [ ] **Task 8.4**: Verify clean build
  - [ ] `make fclean`
  - [ ] `make`
  - [ ] Verify no warnings
  - [ ] Verify no errors

- [ ] **Task 8.5**: Verify relink behavior
  - [ ] `make` (no changes) - should not relink
  - [ ] Touch source file - should relink only affected objects

---

## Documentation

- [ ] **Task 9.1**: Update inline documentation
  - [ ] Verify all function headers have proper doxygen comments
  - [ ] Check parameter descriptions are accurate

- [ ] **Task 9.2**: Verify commit message prepared
  - [ ] Review commit plan in `vector-range-validation-plan.md`
  - [ ] Prepare commit message following project conventions

---

## Final Verification

- [ ] **Task 10.1**: Complete functionality check
  - [ ] All valid scenes render correctly
  - [ ] All invalid scenes rejected with proper error messages
  - [ ] Boundary cases handled correctly
  - [ ] No regression in existing functionality

- [ ] **Task 10.2**: Code review self-check
  - [ ] Validation order correct: parse → range → zero → normalize
  - [ ] Error messages appropriate
  - [ ] No code duplication
  - [ ] Consistent style with existing code

- [ ] **Task 10.3**: Pre-commit verification
  - [ ] `git status` - verify only intended files modified
  - [ ] `git diff` - review all changes
  - [ ] `make re && make clean` - verify build system works
  - [ ] Run full test suite one more time

---

## Commit

- [ ] **Task 11.1**: Stage changes
  - [ ] `git add src/parser/parse_vector_validation.c`
  - [ ] `git add Makefile`
  - [ ] `git add includes/parser.h`
  - [ ] `git add src/parser/parse_elements.c`
  - [ ] `git add src/parser/parse_objects.c`
  - [ ] `git add src/parser/parse_cylinder.c`
  - [ ] Optional: `git add scenes/invalid/invalid_*.rt` (new test scenes)

- [ ] **Task 11.2**: Commit with conventional commit message
  - [ ] Use format from plan: `fix(parser): Vector range validation (Subject compliance)`
  - [ ] Include detailed body with Changes/Location/Testing/Technical notes
  - [ ] Reference: See commit plan in `vector-range-validation-plan.md`

---

## Summary

**Total Tasks**: ~60 checkboxes across 11 major phases  
**Estimated Time**: 2-3 hours (including testing)  
**Files Modified**: 6 files (+1 new)  
**Lines Added**: +49  

**Key Success Criteria**:
1. All norm checks pass
2. All valid scenes still work
3. Invalid scenes properly rejected
4. No memory leaks
5. Clean build with no warnings

---

## Notes

- Work incrementally: complete one phase before moving to next
- Test after each modification
- If build fails, revert last change and investigate
- Keep git commits atomic: one logical change per commit
- Refer to `vector-range-validation-plan.md` for detailed implementation notes
