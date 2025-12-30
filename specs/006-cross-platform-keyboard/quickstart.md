# Quick Start: Cross-Platform Keyboard Testing

**Date**: 2025-12-30  
**Feature**: Cross-platform keyboard support  
**Branch**: 006-cross-platform-keyboard

## Overview

This guide provides step-by-step instructions for implementing and testing cross-platform keyboard support in miniRT. Follow these steps in order to ensure correct implementation on both Ubuntu and macOS.

## Prerequisites

### Required Hardware/Software

**Ubuntu Testing**:
- Ubuntu 20.04 or later
- gcc or clang compiler
- X11 development libraries
- Physical keyboard (or VM with keyboard passthrough)

**macOS Testing**:
- macOS 10.14 (Mojave) or later
- Xcode Command Line Tools (for clang)
- Physical keyboard

**Both Platforms**:
- Git (for branch management)
- Make (build automation)
- Test scene files (existing in `scenes/` directory)

## Implementation Steps

### Step 1: Checkout Feature Branch

```bash
cd /path/to/miniRT
git checkout -b 006-cross-platform-keyboard
```

**Verify**: `git branch` shows you're on `006-cross-platform-keyboard` branch.

### Step 2: Backup Current Implementation

```bash
cp src/window/window.c src/window/window.c.backup
```

**Verify**: `ls -l src/window/` shows both `window.c` and `window.c.backup`.

### Step 3: Modify Keycode Definitions

Open `src/window/window.c` and replace lines 37-71 with the following:

```c
/* Platform-specific key code definitions */
#ifdef __APPLE__
/* macOS (Darwin) virtual key codes */
#define KEY_ESC 53
#define KEY_W 13
#define KEY_A 0
#define KEY_S 1
#define KEY_D 2
#define KEY_R 15
#define KEY_F 3
#define KEY_B 11
#define KEY_Q 12
#define KEY_I 34
#define KEY_H 4
#define KEY_TAB 48
#define KEY_SHIFT_L 56
#define KEY_SHIFT_R 60
#define KEY_UP 126
#define KEY_DOWN 125
#define KEY_BRACKET_LEFT 33
#define KEY_BRACKET_RIGHT 30
#define KEY_KP_1 83
#define KEY_KP_2 84
#define KEY_KP_3 85
#define KEY_KP_4 86
#define KEY_KP_6 88
#define KEY_KP_8 91
#define KEY_INSERT 114
#define KEY_HOME 115
#define KEY_PGUP 116
#define KEY_DELETE 117
#define KEY_END 119
#define KEY_PGDN 121

#elif defined(__linux__)
/* Linux/X11 KeySym values */
#define KEY_ESC 65307
#define KEY_W 119
#define KEY_A 97
#define KEY_S 115
#define KEY_D 100
#define KEY_R 114
#define KEY_F 102
#define KEY_B 98
#define KEY_Q 113
#define KEY_I 105
#define KEY_H 104
#define KEY_TAB 65289
#define KEY_SHIFT_L 65505
#define KEY_SHIFT_R 65506
#define KEY_UP 65362
#define KEY_DOWN 65364
#define KEY_BRACKET_LEFT 91
#define KEY_BRACKET_RIGHT 93
#define KEY_KP_1 65436
#define KEY_KP_2 65433
#define KEY_KP_3 65435
#define KEY_KP_4 65430
#define KEY_KP_6 65432
#define KEY_KP_8 65431
#define KEY_INSERT 65379
#define KEY_HOME 65360
#define KEY_PGUP 65365
#define KEY_DELETE 65535
#define KEY_END 65367
#define KEY_PGDN 65366

#else
#error "Unsupported platform - miniRT requires macOS or Linux"
#endif
```

**Important**: 
- Do NOT modify any code below line 71
- All handler functions remain unchanged
- Only the keycode definitions are modified

**Verify**: Check that you replaced exactly lines 37-71 and preserved all other code.

### Step 4: Validate Code Quality

```bash
# Check norminette compliance
norminette src/window/window.c

# Should show: No errors, all checks pass
```

**Fix**: If norminette errors appear, adjust formatting but DO NOT change logic.

### Step 5: Build on Current Platform

```bash
# Clean and rebuild
make fclean
make

# Should complete without errors or warnings
```

**Troubleshooting**:
- If build fails with "undefined reference to mlx_*", check MiniLibX is built
- If undefined KEY_* errors, verify platform detection block is correct
- If compiler errors on #ifdef, check no syntax errors in preprocessor directives

## Testing Procedures

### macOS Testing

#### Test 1: Basic Camera Navigation (P1)

1. Launch miniRT with a test scene:
   ```bash
   ./miniRT scenes/test_scene.rt
   ```

2. Test each camera control:
   - Press **W** - camera should move forward
   - Press **S** - camera should move backward
   - Press **A** - camera should move left
   - Press **D** - camera should move right
   - Press **R** - camera should pitch up
   - Press **F** - camera should pitch down

3. **Expected**: Smooth camera movement with low-quality rendering during interaction, high-quality when released.

4. **Pass Criteria**: All 6 keys respond correctly, camera moves in expected directions.

#### Test 2: Application Controls (P1)

1. With miniRT running:
   - Press **H** - HUD should toggle on/off
   - Press **TAB** (with HUD visible) - should cycle to next section
   - Press **Shift+TAB** - should cycle to previous section
   - Press **UP arrow** - HUD should scroll up
   - Press **DOWN arrow** - HUD should scroll down
   - Press **ESC** - application should exit cleanly

2. **Expected**: All UI controls respond immediately without lag.

3. **Pass Criteria**: All 6 controls work as described.

#### Test 3: Object Manipulation (P2)

1. Load a scene with multiple objects:
   ```bash
   ./miniRT scenes/multi_object.rt
   ```

2. Test object selection:
   - Press **]** - should select next object (sphere → plane → cylinder → sphere)
   - Press **[** - should select previous object

3. Test object movement (with object selected):
   - Press **Numpad 4** - object moves in -X direction
   - Press **Numpad 6** - object moves in +X direction
   - Press **Numpad 2** - object moves in -Y direction
   - Press **Numpad 8** - object moves in +Y direction
   - Press **Numpad 1** - object moves in -Z direction
   - Press **Numpad 3** - object moves in +Z direction

4. **Expected**: Object selection cycles correctly, numpad keys move object smoothly.

5. **Pass Criteria**: All 8 keys work, object moves in correct directions.

**Note**: If laptop lacks physical numpad, test with external numpad or mark as "Requires External Numpad".

#### Test 4: Lighting Controls (P3)

1. With any scene loaded:
   - Press **Insert** - light moves in +X direction
   - Press **Delete** - light moves in -X direction
   - Press **Home** - light moves in +Y direction
   - Press **End** - light moves in -Y direction
   - Press **Page Up** - light moves in +Z direction
   - Press **Page Down** - light moves in -Z direction

2. **Expected**: Lighting changes are visible immediately, scene updates smoothly.

3. **Pass Criteria**: All 6 keys work, lighting position changes correctly.

#### Test 5: Rendering Options (P3)

1. With any scene loaded:
   - Press **B** - BVH acceleration should toggle (may see performance change)
   - Press **Q** - adaptive sampling should toggle (may see quality change)
   - Press **I** - rendering info display should toggle

2. **Expected**: Toggles work without crashing, visual feedback confirms state change.

3. **Pass Criteria**: All 3 keys toggle their respective features.

### Ubuntu Testing

#### Regression Testing (All Priorities)

**Goal**: Verify Linux behavior is unchanged (no regressions).

1. Build on Ubuntu:
   ```bash
   make fclean
   make
   ```

2. Run through ALL tests from macOS section (Tests 1-5).

3. **Expected**: Identical behavior to previous version - all controls work as before.

4. **Pass Criteria**: 
   - All tests pass identically to macOS
   - No new bugs introduced
   - Performance unchanged

### Cross-Platform Consistency Testing

#### Test 6: Identical Behavior Verification

1. Choose a reference scene (e.g., `scenes/test_scene.rt`)

2. On **Ubuntu**:
   - Load scene
   - Press W 5 times
   - Press A 3 times
   - Press R 2 times
   - Note final camera position/orientation (visible in HUD if available)

3. On **macOS**:
   - Load same scene
   - Repeat exact same key sequence
   - Compare camera position/orientation

4. **Expected**: Camera arrives at identical position and orientation.

5. **Pass Criteria**: Results match exactly on both platforms.

### Edge Case Testing

#### Test 7: Rapid Key Presses

1. Rapidly press and release W key multiple times
2. **Expected**: Low-quality rendering during interaction, high-quality after stopping
3. **Pass Criteria**: No crashes, smooth degradation and recovery

#### Test 8: Key Combinations

1. Press Shift, then press Tab (while holding Shift)
2. **Expected**: Reverse HUD navigation
3. **Pass Criteria**: Combination works correctly

#### Test 9: Simultaneous Keys

1. Press W and D simultaneously (diagonal camera movement)
2. **Expected**: Camera moves diagonally (forward-right)
3. **Pass Criteria**: Both keys register, movement is smooth

#### Test 10: Undefined Keys

1. Press random keys not defined in KEY_* constants (e.g., Z, X, C)
2. **Expected**: No crash, no error, keys are silently ignored
3. **Pass Criteria**: Application remains stable

## Validation Checklist

### Code Quality
- [ ] norminette passes on modified file
- [ ] No compiler warnings on macOS
- [ ] No compiler warnings on Ubuntu
- [ ] Code diff shows only lines 37-71 modified in window.c

### Functional Requirements (from spec.md)
- [ ] FR-001: Platform detection works correctly
- [ ] FR-002: Correct keycodes defined for both platforms
- [ ] FR-003: Camera controls work on both platforms
- [ ] FR-004: Object selection works on both platforms
- [ ] FR-005: Object movement works on both platforms
- [ ] FR-006: Lighting controls work on both platforms
- [ ] FR-007: Application controls work on both platforms
- [ ] FR-008: Rendering options work on both platforms
- [ ] FR-009: Conditional compilation used correctly
- [ ] FR-010: Identical behavior on both platforms
- [ ] FR-011: Clean compilation on both platforms
- [ ] FR-012: Platform macros used correctly

### Success Criteria (from spec.md)
- [ ] SC-001: Compiles without errors/warnings on both platforms
- [ ] SC-002: All 30+ controls respond identically
- [ ] SC-003: Full workflow completable on both platforms
- [ ] SC-004: Zero platform-specific bugs
- [ ] SC-005: Changes limited to keycode definitions
- [ ] SC-006: Ubuntu behavior unchanged (regression-free)

### User Stories (from spec.md)
- [ ] US1: Basic camera navigation works on both platforms
- [ ] US2: Object manipulation works on both platforms
- [ ] US3: Lighting controls work on both platforms
- [ ] US4: Application controls work on both platforms
- [ ] US5: Rendering options work on both platforms

## Troubleshooting

### Problem: Keys don't respond on macOS

**Diagnosis**: 
- Check which platform block is being compiled (add debug print if needed)
- Verify `__APPLE__` is defined (run `echo | clang -dM -E - | grep APPLE`)

**Solution**:
- Ensure `#ifdef __APPLE__` appears before macOS keycode definitions
- Check no typos in platform detection macro

### Problem: Wrong keys trigger actions on macOS

**Diagnosis**:
- Verify keycode values match Carbon Virtual Key Codes
- Use MiniLibX hook to print actual keycodes received

**Solution**:
- Cross-reference keycode values with contracts/keycodes.h
- Test with external keyboard if laptop keyboard has issues

### Problem: Build fails with "Unsupported platform" error

**Diagnosis**:
- Neither `__APPLE__` nor `__linux__` is defined

**Solution**:
- Verify compiler is clang (macOS) or gcc/clang (Linux)
- Check compiler flags don't suppress predefined macros

### Problem: norminette errors on preprocessor directives

**Diagnosis**:
- norminette may flag preprocessor usage if incorrectly formatted

**Solution**:
- Ensure # is at column 1
- Check no spaces between # and keyword
- Verify proper indentation of #define lines

### Problem: Performance degradation on macOS

**Diagnosis**:
- Unrelated to keyboard changes (only constants modified)

**Solution**:
- Verify BVH and adaptive sampling enabled (press B and Q)
- Check MiniLibX macOS version is up to date

## Next Steps

After all tests pass:

1. **Commit changes**:
   ```bash
   git add src/window/window.c
   git commit -m "feat: add cross-platform keyboard support

   - Add platform-specific keycode definitions for macOS and Linux
   - Use conditional compilation for zero runtime overhead
   - All 30+ controls work identically on both platforms
   - Tested on macOS 10.14+ and Ubuntu 20.04+
   
   Resolves: #006 Cross-platform keyboard support"
   ```

2. **Push branch**:
   ```bash
   git push origin 006-cross-platform-keyboard
   ```

3. **Create Pull Request**:
   - Title: "Add cross-platform keyboard support (macOS + Linux)"
   - Description: Reference spec.md and testing results
   - Link to spec: `specs/006-cross-platform-keyboard/spec.md`
   - Confirm all validation checklist items

4. **CI/CD Validation**:
   - Wait for automated checks to pass
   - Fix any issues flagged by CI

5. **Code Review**:
   - Address reviewer feedback
   - Re-test if changes requested

6. **Merge**:
   - Squash and merge to main branch
   - Delete feature branch after merge

7. **Verify on Main**:
   - Checkout main branch
   - Test on both platforms one final time

## Reference Documentation

- **Feature Specification**: `specs/006-cross-platform-keyboard/spec.md`
- **Implementation Plan**: `specs/006-cross-platform-keyboard/plan.md`
- **Research Findings**: `specs/006-cross-platform-keyboard/research.md`
- **Data Model**: `specs/006-cross-platform-keyboard/data-model.md`
- **Keycode Contract**: `specs/006-cross-platform-keyboard/contracts/keycodes.h`
- **Constitution**: `.specify/memory/constitution.md`

## Support

If you encounter issues not covered in this guide:

1. Check existing GitHub issues for similar problems
2. Review constitution principles for compliance requirements
3. Consult MiniLibX documentation for platform-specific behavior
4. Test with minimal scene file to isolate keyboard-specific issues

**Success Indicator**: When you can navigate a scene, select objects, adjust lighting, and toggle options using only the keyboard on BOTH macOS and Ubuntu, with identical behavior, you have successfully implemented cross-platform keyboard support! 🎉
