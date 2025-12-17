# Tasks: Realistic Shadow Rendering

## Task Execution Rules

### Dependency Management
- **Sequential Tasks**: Must complete in order (no [P] marker)
- **Parallel Tasks**: Can run concurrently (marked with [P])
- **Phase Completion**: All tasks in a phase must complete before next phase

### TDD Approach
- Write tests before implementation where applicable
- Verify tests pass after implementation
- Use tests to drive design decisions

### Commit Strategy
- All commits must reference issue #4: `[#4] commit message`
- Commit after each major task completion
- Run norminette before each commit

---

## Phase 1: Foundation & Setup

### SETUP-001: Create Shadow Header [P]
**Description**: Create shadow.h header file with data structures and function declarations

**Files**:
- `includes/shadow.h` (create)

**Tasks**:
1. Add 42 header
2. Add include guards
3. Define `t_shadow_config` structure
4. Declare function prototypes
5. Add English Doxygen comments

**Acceptance**:
- File passes norminette
- Compiles without errors
- Header guards work correctly

**Estimated Time**: 30 minutes

---

### SETUP-002: Create Shadow Config Module [P]
**Description**: Implement shadow configuration management

**Files**:
- `src/lighting/shadow_config.c` (create)

**Functions**:
1. `init_shadow_config()` - Initialize with defaults
2. `validate_shadow_config()` - Validate parameters
3. `set_shadow_samples()` - Set sample count
4. `set_shadow_softness()` - Set softness factor

**Acceptance**:
- All functions pass norminette
- Default config is valid
- Validation catches invalid parameters
- Compiles without warnings

**Estimated Time**: 45 minutes

---

### SETUP-003: Create Shadow Calc Module Skeleton [P]
**Description**: Create shadow_calc.c with empty function stubs

**Files**:
- `src/lighting/shadow_calc.c` (create)

**Functions**:
1. `calculate_shadow_bias()` - Stub
2. `calculate_shadow_factor()` - Stub
3. `generate_shadow_sample_offset()` - Stub
4. `calculate_shadow_attenuation()` - Stub
5. `is_in_shadow()` - Stub

**Acceptance**:
- File passes norminette
- Compiles without errors
- All functions have English comments

**Estimated Time**: 30 minutes

---

### SETUP-004: Update Makefile
**Description**: Add new source files to Makefile

**Files**:
- `Makefile` (modify)

**Tasks**:
1. Add shadow_config.c to SRCS
2. Add shadow_calc.c to SRCS
3. Add shadow.h to dependencies
4. Test compilation

**Acceptance**:
- `make` succeeds without errors
- `make clean` works
- `make re` rebuilds correctly

**Estimated Time**: 15 minutes

---

### SETUP-005: Create Unit Test Files [P]
**Description**: Create unit test files for shadow system

**Files**:
- `tests/test_shadow_config.c` (create)
- `tests/test_shadow_calc.c` (create)

**Tasks**:
1. Add test framework includes
2. Create test function stubs
3. Add main() for each test file
4. Document test plan in comments

**Acceptance**:
- Test files compile
- Can run (even if tests not implemented)
- Clear test structure

**Estimated Time**: 30 minutes

---

## Phase 2: Adaptive Shadow Bias

### BIAS-001: Implement calculate_shadow_bias()
**Description**: Implement adaptive shadow bias calculation

**Files**:
- `src/lighting/shadow_calc.c` (modify)

**Algorithm**:
1. Calculate angle between normal and light direction
2. Apply bias scaling based on angle (grazing angles need more bias)
3. Return adjusted bias value
4. Handle edge cases (parallel vectors, etc.)

**Formula**:
```c
bias = base_bias * (1.0 + (1.0 - fabs(dot(normal, light_dir))) * bias_scale);
```

**Acceptance**:
- Function passes norminette
- Returns valid bias for all inputs
- Handles edge cases correctly
- Has comprehensive English comments

**Estimated Time**: 45 minutes

---

### BIAS-002: Write Unit Tests for Shadow Bias
**Description**: Create unit tests for bias calculation

**Files**:
- `tests/test_shadow_calc.c` (modify)

**Test Cases**:
1. Test perpendicular angle (90°) - minimum bias
2. Test parallel angle (0°) - maximum bias
3. Test 45° angle - medium bias
4. Test edge case: zero normal vector
5. Test edge case: zero light direction
6. Test negative bias scale

**Acceptance**:
- All tests pass
- Edge cases handled gracefully
- Test output is clear

**Estimated Time**: 30 minutes

---

### BIAS-003: Refactor is_in_shadow() Function
**Description**: Refactor existing in_shadow() to use adaptive bias

**Files**:
- `src/lighting/shadow_calc.c` (modify)

**Tasks**:
1. Move code from lighting.c to shadow_calc.c
2. Add adaptive bias calculation
3. Use calculate_shadow_bias() function
4. Keep same functionality for backward compatibility

**Acceptance**:
- Function passes norminette
- Same results as before (with better bias)
- No shadow acne on test scenes
- Compiles without warnings

**Estimated Time**: 30 minutes

---

## Phase 3: Soft Shadows Core

### SOFT-001: Implement generate_shadow_sample_offset()
**Description**: Generate random offsets for shadow ray sampling

**Files**:
- `src/lighting/shadow_calc.c` (modify)

**Algorithm**:
1. Use stratified sampling for better distribution
2. Generate point on unit disk
3. Scale by softness radius
4. Return 3D offset vector

**Acceptance**:
- Function passes norminette
- Offsets distributed evenly
- Respects softness parameter
- Unit tests pass

**Estimated Time**: 45 minutes

---

### SOFT-002: Write Unit Tests for Sample Offset
**Description**: Test shadow sample offset generation

**Files**:
- `tests/test_shadow_calc.c` (modify)

**Test Cases**:
1. Test offset magnitude is within bounds
2. Test distribution uniformity (statistical test)
3. Test zero softness returns zero offset
4. Test sample count affects distribution
5. Test deterministic behavior with same seed

**Acceptance**:
- All tests pass
- Offsets are well-distributed
- Edge cases handled

**Estimated Time**: 30 minutes

---

### SOFT-003: Implement calculate_shadow_factor()
**Description**: Calculate shadow factor using multiple shadow rays

**Files**:
- `src/lighting/shadow_calc.c` (modify)

**Algorithm**:
1. Cast N shadow rays with random offsets
2. Count how many hit objects
3. Return ratio: hits / total_samples
4. Handle sample count = 1 (hard shadow)

**Acceptance**:
- Function passes norminette
- Returns value in [0.0, 1.0]
- Works with 1 sample (backward compatible)
- Works with multiple samples
- Proper bias applied to all rays

**Estimated Time**: 60 minutes

---

### SOFT-004: Write Unit Tests for Shadow Factor
**Description**: Test soft shadow calculation

**Files**:
- `tests/test_shadow_calc.c` (modify)

**Test Cases**:
1. Test fully lit point (no shadow)
2. Test fully occluded point (full shadow)
3. Test partially occluded point (soft shadow)
4. Test single sample equals hard shadow
5. Test multiple samples give smooth transition
6. Test performance with various sample counts

**Acceptance**:
- All tests pass
- Shadow factor in valid range
- Performance acceptable
- Results match expectations

**Estimated Time**: 45 minutes

---

## Phase 4: Shadow Attenuation

### ATTEN-001: Implement calculate_shadow_attenuation()
**Description**: Calculate distance-based shadow attenuation

**Files**:
- `src/lighting/shadow_calc.c` (modify)

**Algorithm**:
1. Calculate distance from light
2. Apply falloff curve (inverse square or linear)
3. Apply softness parameter
4. Return attenuation factor [0.0, 1.0]

**Formula**:
```c
attenuation = 1.0 / (1.0 + distance * distance * softness);
```

**Acceptance**:
- Function passes norminette
- Returns valid attenuation factor
- Smooth falloff curve
- Configurable softness

**Estimated Time**: 30 minutes

---

### ATTEN-002: Write Unit Tests for Attenuation
**Description**: Test shadow attenuation calculation

**Files**:
- `tests/test_shadow_calc.c` (modify)

**Test Cases**:
1. Test zero distance (full intensity)
2. Test infinite distance (zero intensity)
3. Test intermediate distances
4. Test softness parameter effects
5. Test falloff curve shape

**Acceptance**:
- All tests pass
- Attenuation factor in [0.0, 1.0]
- Smooth falloff behavior
- Edge cases handled

**Estimated Time**: 30 minutes

---

## Phase 5: Integration

### INTEG-001: Add Shadow Config to Scene
**Description**: Add shadow configuration to scene structure

**Files**:
- `includes/minirt.h` (modify)

**Tasks**:
1. Add `t_shadow_config` member to `t_scene`
2. Update scene initialization
3. Set default shadow config values

**Acceptance**:
- Compiles without errors
- Scene structure extended correctly
- Default config applied
- Backward compatible

**Estimated Time**: 15 minutes

---

### INTEG-002: Refactor apply_lighting() Function
**Description**: Update lighting to use shadow factor instead of binary shadow

**Files**:
- `src/lighting/lighting.c` (modify)

**Tasks**:
1. Replace in_shadow() call with calculate_shadow_factor()
2. Use shadow factor to modulate diffuse lighting
3. Apply shadow attenuation
4. Maintain backward compatibility

**Algorithm**:
```c
shadow_factor = calculate_shadow_factor(scene, hit->point, scene->light.position, &scene->shadow_config);
shadow_atten = calculate_shadow_attenuation(distance, max_distance, softness);
final_shadow = shadow_factor * shadow_atten;
diffuse = diffuse * (1.0 - final_shadow);
```

**Acceptance**:
- Function passes norminette
- Soft shadows visible in renders
- Hard shadows still work (1 sample)
- Performance acceptable
- No visual artifacts

**Estimated Time**: 45 minutes

---

### INTEG-003: Integration Testing
**Description**: Test shadow system integration

**Tasks**:
1. Create test scene with soft shadows
2. Render and verify visual results
3. Test various sample counts (1, 4, 9, 16)
4. Test shadow attenuation effects
5. Performance benchmark

**Acceptance**:
- Soft shadows render correctly
- No crashes or errors
- Performance meets targets
- Visual quality improved

**Estimated Time**: 30 minutes

---

## Phase 6: Testing & Scenes

### TEST-001: Create Test Scene - Soft Shadows [P]
**Description**: Create scene demonstrating soft shadows

**Files**:
- `scenes/test_soft_shadows.rt` (create)

**Scene Elements**:
1. Single sphere with soft shadow
2. Light positioned to show penumbra
3. Clear shadow softness demonstration
4. Ground plane

**Acceptance**:
- Scene file valid
- Renders correctly
- Soft shadows visible
- Good demonstration

**Estimated Time**: 20 minutes

---

### TEST-002: Create Test Scene - Shadow Attenuation [P]
**Description**: Create scene demonstrating shadow attenuation

**Files**:
- `scenes/test_shadow_attenuation.rt` (create)

**Scene Elements**:
1. Multiple objects at various distances
2. Light source showing distance falloff
3. Clear attenuation effect
4. Good visual comparison

**Acceptance**:
- Scene file valid
- Renders correctly
- Attenuation visible
- Demonstrates feature well

**Estimated Time**: 20 minutes

---

### TEST-003: Create Test Scene - Complex Shadows [P]
**Description**: Create complex scene with all shadow features

**Files**:
- `scenes/test_complex_shadows.rt` (create)

**Scene Elements**:
1. Multiple objects casting shadows
2. Various materials and colors
3. Soft shadows with attenuation
4. Visually appealing composition

**Acceptance**:
- Scene file valid
- Renders correctly
- All features visible
- Good showcase

**Estimated Time**: 30 minutes

---

### TEST-004: Run All Unit Tests
**Description**: Execute all unit tests and verify results

**Tasks**:
1. Compile test executables
2. Run test_shadow_config
3. Run test_shadow_calc
4. Verify all tests pass
5. Document test results

**Acceptance**:
- All tests compile
- All tests pass
- No memory leaks (valgrind)
- Test coverage adequate

**Estimated Time**: 30 minutes

---

## Phase 7: Documentation & Polish

### DOC-001: Add English Doxygen Comments [P]
**Description**: Add comprehensive English comments to all functions

**Files**:
- `src/lighting/shadow_calc.c` (modify)
- `src/lighting/shadow_config.c` (modify)
- `src/lighting/lighting.c` (modify)
- `includes/shadow.h` (modify)

**Requirements**:
1. Function description
2. Parameter descriptions
3. Return value description
4. Example usage (for complex functions)
5. Notes on edge cases

**Acceptance**:
- All functions documented
- Comments clear and helpful
- Doxygen-compatible format
- Follows project style

**Estimated Time**: 45 minutes

---

### DOC-002: Write Korean Documentation [P]
**Description**: Create comprehensive Korean documentation

**Files**:
- `docs/03-기능/조명/그림자_시스템.md` (create)

**Content**:
1. 시스템 개요 (System Overview)
2. 구현 상세 (Implementation Details)
3. 함수 설명 (Function Descriptions)
4. 사용 예제 (Usage Examples)
5. 성능 고려사항 (Performance Considerations)
6. 문제 해결 (Troubleshooting)

**Acceptance**:
- Complete documentation in Korean
- Clear explanations
- Code examples included
- Matches English comments

**Estimated Time**: 60 minutes

---

### DOC-003: Update README [P]
**Description**: Update project README with shadow features

**Files**:
- `README.md` (modify)

**Updates**:
1. Add soft shadows to feature list
2. Update version history
3. Add performance notes
4. Update example scenes

**Acceptance**:
- README updated
- Changes accurate
- Maintains format
- Clear descriptions

**Estimated Time**: 20 minutes

---

## Phase 8: Norminette & Final Verification

### NORM-001: Run Norminette Check
**Description**: Verify all files pass norminette

**Files**:
- All .c and .h files

**Tasks**:
1. Run norminette on all modified/new files
2. Fix any violations
3. Re-run until clean
4. Document norminette results

**Acceptance**:
- All files pass norminette
- No errors or warnings
- Proper 42 headers
- Style compliance 100%

**Estimated Time**: 30-60 minutes

---

### NORM-002: Build Verification
**Description**: Complete build-run-verify cycle

**Tasks**:
1. `make clean`
2. `make` - verify successful compilation
3. Test with all scene files
4. Verify no crashes or errors
5. Check for warnings

**Acceptance**:
- Clean compilation
- No warnings
- All tests pass
- Scenes render correctly

**Estimated Time**: 20 minutes

---

### NORM-003: Memory Leak Check
**Description**: Run valgrind to check for memory leaks

**Tasks**:
1. Compile with debug symbols
2. Run valgrind on various test scenes
3. Fix any memory leaks
4. Re-test until clean
5. Document results

**Acceptance**:
- Zero memory leaks
- No invalid memory access
- Clean valgrind report
- Performance acceptable

**Estimated Time**: 30 minutes

---

### NORM-004: Performance Benchmark
**Description**: Measure and document performance

**Tasks**:
1. Benchmark hard shadows (1 sample)
2. Benchmark soft shadows (4, 9, 16 samples)
3. Measure frame times
4. Compare with baseline
5. Document results

**Acceptance**:
- Performance meets targets
- Measurements documented
- Reasonable overhead
- No performance regression

**Estimated Time**: 30 minutes

---

### NORM-005: Final Integration Test
**Description**: Complete end-to-end testing

**Tasks**:
1. Test all scene files
2. Verify backward compatibility
3. Test edge cases
4. Visual quality check
5. User acceptance testing

**Acceptance**:
- All scenes work
- Backward compatible
- Quality improved
- No regressions
- Ready for merge

**Estimated Time**: 30 minutes

---

## Summary

### Total Tasks: 37
- Setup Phase: 5 tasks
- Bias Phase: 3 tasks
- Soft Shadows Phase: 4 tasks
- Attenuation Phase: 2 tasks
- Integration Phase: 3 tasks
- Testing Phase: 4 tasks
- Documentation Phase: 3 tasks
- Verification Phase: 5 tasks

### Parallel Tasks: 12 (marked with [P])
### Sequential Tasks: 25

### Estimated Total Time: 14-17 hours

### Critical Path:
1. SETUP-001 → SETUP-004 (foundation)
2. BIAS-001 → BIAS-002 → BIAS-003 (bias)
3. SOFT-001 → SOFT-003 (soft shadows)
4. INTEG-001 → INTEG-002 (integration)
5. NORM-001 → NORM-005 (verification)

### Commit Points:
- After Phase 1 completion: `[#4] Add shadow system foundation`
- After Phase 2 completion: `[#4] Implement adaptive shadow bias`
- After Phase 3 completion: `[#4] Implement soft shadows`
- After Phase 4 completion: `[#4] Add shadow attenuation`
- After Phase 5 completion: `[#4] Integrate shadow system with lighting`
- After Phase 6 completion: `[#4] Add test scenes and unit tests`
- After Phase 7 completion: `[#4] Add documentation (English + Korean)`
- After Phase 8 completion: `[#4] Final verification and norminette compliance`
