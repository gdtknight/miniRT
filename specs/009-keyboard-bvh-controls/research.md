# Research Document: Keyboard Controls and BVH Enforcement

**Feature**: Keyboard Controls Finalization and BVH Enforcement  
**Date**: 2026-01-01  
**Status**: Phase 0 Complete

## Overview

This document consolidates research findings for implementing ergonomic keyboard controls for object manipulation and permanently enforcing BVH acceleration structures in the miniRT ray tracer. All technical unknowns from the Technical Context section have been investigated and resolved.

## Research Tasks

### 1. Keyboard Layout Design for Object Movement

**Research Question**: What is the optimal keyboard layout for 6-axis object movement (X+/X-, Y+/Y-, Z+/Z-) that keeps the user's hand in a compact ergonomic region?

**Findings**:
- **Current Implementation**: Uses Numpad keys (4/6 for X, 2/8 for Y, 1/3 for Z) which requires right-hand positioning away from main keyboard
- **Proposed Layout**: R/T/F/G/V/B keys form a compact 2x3 grid accessible by left hand while right hand uses mouse
  - R = X- (left), T = X+ (right)
  - F = Y- (down), G = Y+ (up)
  - V = Z- (backward), B = Z+ (forward)
- **Ergonomic Analysis**: R/T/F/G/V/B are adjacent keys in QWERTY layout, all within reach of left hand index/middle/ring fingers
- **Conflict Analysis**: These keys do not conflict with existing camera controls (W/A/S/D/Q/Z/E/C)

**Decision**: Adopt R/T/F/G/V/B layout for object movement

**Rationale**:
- Keeps hand in compact region near home row
- No conflicts with existing controls
- Logical pairing: R/T (horizontal pair), F/G (vertical pair), V/B (depth pair)
- Left-hand operation leaves right hand free for future mouse controls

**Alternatives Considered**:
- I/K/J/L arrow-like layout: Rejected because conflicts with potential future bindings and less intuitive axis mapping
- U/O/Y/H/N/M layout: Rejected because more spread out and harder to remember
- Keep Numpad: Rejected because requires repositioning hand away from main keyboard

### 2. Camera Control Key Mapping Extension

**Research Question**: What additional camera control keys are needed beyond existing W/A/S/D to match the specification requirements?

**Findings**:
- **Current Camera Translation**: W (forward), S (backward in spec but conflicts with existing W/S usage), A (left), D (right)
- **Current Camera Rotation**: R/F (pitch up/down in current code)
- **Specification Requirements**: W/X (forward/back), A/D (left/right), Q/Z (up/down in world space), E/C (pitch up/down), S (reset)

**Decision**: Add X, E, C, Z keys and repurpose R/F for object movement, S for reset

**Rationale**:
- X provides explicit backward movement (more intuitive than negative direction)
- Q/Z for vertical movement in world space (common in 3D applications)
- E/C for pitch matches industry standards (E=up, C=down)
- S for reset is mnemonic (S=start/standard position)
- Frees up R/F for object movement X-axis

**Alternatives Considered**:
- Keep R/F for camera: Rejected because object movement needs R/T pair and camera already has E/C
- Use arrow keys for camera: Rejected because already used for HUD navigation

### 3. BVH Enforcement Strategy

**Research Question**: How should BVH acceleration be permanently enabled without breaking existing functionality?

**Findings**:
- **Current Implementation**: BVH enabled via `scene->render_state.bvh_enabled` flag set to 1 in main.c
- **Current Toggle**: No existing UI toggle detected in codebase (already effectively permanent)
- **Build Process**: `scene_build_bvh(scene)` called in main.c after scene parsing

**Decision**: Remove conditional logic and make BVH non-optional at compile time

**Rationale**:
- BVH is already enabled by default in current code
- No performance penalty for always-on BVH (only benefit)
- Simplifies code by removing unused code paths
- Aligns with 42 School best practices (no dead code)

**Alternatives Considered**:
- Keep flag but always set to 1: Rejected because introduces dead code and confusion
- Add compile-time #define: Rejected as unnecessary complexity for single configuration

### 4. Platform-Specific Key Code Handling

**Research Question**: How are macOS and Linux key codes currently handled, and what changes are needed for new keys?

**Findings**:
- **Current Approach**: Conditional compilation using `#ifdef __APPLE__` and `#elif defined(__linux__)`
- **macOS Key Codes**: Virtual key codes (e.g., KEY_W = 13)
- **Linux Key Codes**: X11 KeySym values (e.g., KEY_W = 119)
- **New Keys Needed**:
  - macOS: KEY_T=17, KEY_G=5, KEY_V=9, KEY_E=14, KEY_C=8, KEY_X=7, KEY_Z=6
  - Linux: KEY_T=116, KEY_G=103, KEY_V=118, KEY_E=101, KEY_C=99, KEY_X=120, KEY_Z=122

**Decision**: Add new key code definitions following existing pattern in window.c

**Rationale**:
- Maintains consistency with existing code structure
- Proven cross-platform approach already in use
- No external dependencies required

**Alternatives Considered**:
- Separate header file for key codes: Rejected because current inline approach works and avoids file proliferation
- Runtime key mapping: Rejected because compile-time approach is simpler and sufficient

### 5. Documentation Update Requirements

**Research Question**: What documentation needs to be updated to reflect new keyboard controls and BVH enforcement?

**Findings**:
- **Existing Documentation**: README.md mentions controls, but detailed layout not found
- **Key Guide System**: In-game display in `src/keyguide/` that shows keyboard layout
- **Required Updates**:
  1. Update keyguide_render.c to show new key mappings
  2. Create/update CONTROLS.md with complete keyboard reference
  3. Update README.md if it references old controls
  4. Create Korean translation in docs/specs/009-keyboard-bvh-controls/

**Decision**: Update in-game key guide and create comprehensive CONTROLS.md documentation

**Rationale**:
- In-game display is primary user reference
- Markdown documentation provides detailed reference
- Korean documentation satisfies constitution bilingual requirement

**Alternatives Considered**:
- Only update in-game guide: Rejected because external documentation needed for reference
- Only update documentation files: Rejected because in-game guide is most accessible

### 6. Testing Strategy for Keyboard Changes

**Research Question**: How should new keyboard controls be tested on both platforms?

**Findings**:
- **Existing Test Infrastructure**: Manual testing with test scenes in `scenes/` directory
- **Test Script**: `test_controls.sh` exists at project root for control testing
- **Platform Testing**: Requires testing on actual macOS and Linux machines

**Decision**: Create manual test checklist covering all keyboard scenarios on both platforms

**Rationale**:
- Manual testing appropriate for interactive controls
- Checklist ensures comprehensive coverage
- Both platforms must be tested before merge

**Alternatives Considered**:
- Automated keyboard input testing: Rejected because MLX doesn't provide programmatic input injection
- Single platform testing: Rejected because cross-platform support is constitutional requirement

## Summary of Decisions

| Decision | Chosen Approach | Key Benefit |
|----------|----------------|-------------|
| Object Movement Keys | R/T/F/G/V/B | Ergonomic compact layout, left-hand accessible |
| Camera Extension Keys | X/E/C/Z added, S repurposed | Complete 6DOF camera control, standard bindings |
| BVH Enforcement | Remove conditional logic | Simplifies code, ensures performance |
| Key Code Handling | Extend existing #ifdef pattern | Cross-platform compatible, proven approach |
| Documentation | Update keyguide + create CONTROLS.md | In-game + reference documentation |
| Testing | Manual checklist on both platforms | Thorough coverage, platform validation |

## Open Questions

None - all technical unknowns resolved.

## Next Steps

Proceed to Phase 1: Design & Contracts
- Create data-model.md defining keyboard state structures
- Generate contracts/keyboard_layout.h with key code definitions
- Create quickstart.md for developer reference
- Update agent context with technology decisions
