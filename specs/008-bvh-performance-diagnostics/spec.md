# Feature Specification: BVH Performance Verification and Debugging Tools

**Feature Branch**: `008-bvh-performance-diagnostics`  
**Created**: 2025-12-31  
**Status**: Draft  
**Input**: User description: "BVH 성능 검증 및 디버깅 도구 추가"

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Real-time Performance Monitoring (Priority: P1)

Developers need to see immediate visual feedback on rendering performance to understand if BVH acceleration is working as expected. When toggling BVH on/off with 'B' key, they should instantly see performance differences in frame time and FPS displayed on screen.

**Why this priority**: This is the core diagnostic capability that makes BVH debugging possible. Without visible performance metrics, developers cannot determine if BVH is functioning or what impact it has.

**Independent Test**: Can be fully tested by launching the application with any scene, observing FPS and frame time displayed in HUD, toggling BVH with 'B' key, and verifying that metrics update in real-time. Delivers immediate diagnostic value.

**Acceptance Scenarios**:

1. **Given** application is running with BVH enabled, **When** user views the HUD, **Then** current FPS, frame rendering time in milliseconds, and BVH status (ON/OFF) are displayed
2. **Given** BVH is enabled, **When** user presses 'B' key to disable BVH, **Then** HUD immediately updates to show BVH OFF status and changed performance metrics
3. **Given** user is rendering a complex scene (50+ objects), **When** comparing metrics with BVH ON vs OFF, **Then** FPS increases and frame time decreases when BVH is enabled

---

### User Story 2 - BVH Effectiveness Analysis (Priority: P2)

Developers need detailed statistics about how BVH is reducing computational work to validate that the acceleration structure is providing meaningful optimization. This includes seeing how many intersection tests are avoided through spatial culling.

**Why this priority**: While visible FPS is crucial (P1), understanding why BVH improves (or doesn't improve) performance requires detailed statistics about node traversal and test reduction.

**Independent Test**: Can be tested independently by enabling detailed info mode with 'I' key and verifying that ray-bounding box tests, ray-object tests, nodes visited, and nodes skipped are displayed when BVH is active. Delivers diagnostic depth beyond basic FPS.

**Acceptance Scenarios**:

1. **Given** BVH is enabled and detail mode is active, **When** rendering a frame, **Then** HUD shows total ray-bounding box intersection tests, ray-object intersection tests, BVH nodes visited, and BVH nodes skipped
2. **Given** BVH is enabled, **When** user views per-frame statistics, **Then** average tests per ray is calculated and displayed
3. **Given** scene with 100 objects, **When** BVH traversal skips branches, **Then** object intersection tests are significantly lower than naive approach (100 tests per ray)
4. **Given** BVH is disabled, **When** detail mode is active, **Then** only ray-object intersection tests are shown (no BVH-specific metrics)

---

### User Story 3 - BVH Structure Validation (Priority: P3)

Developers need to verify that BVH tree construction is optimal by inspecting tree statistics such as depth, node count, and object distribution. This helps identify if scenes are well-suited for BVH or if tree construction has issues.

**Why this priority**: This is advanced debugging information useful for optimization but not required for basic validation that BVH is working. Tree structure analysis comes after confirming BVH provides performance benefit.

**Independent Test**: Can be tested by launching application and checking terminal output for BVH build statistics (total nodes, tree depth, average objects per leaf, build time). Delivers insights into BVH quality without requiring HUD.

**Acceptance Scenarios**:

1. **Given** application starts with a scene, **When** BVH is built, **Then** terminal displays total node count, tree depth, average objects per leaf node, and build time in milliseconds
2. **Given** scene has 50 objects, **When** BVH build completes, **Then** tree depth is logarithmic relative to object count (indicating balanced tree)
3. **Given** BVH is enabled in detail mode, **When** user views HUD, **Then** tree statistics (node count, depth) are displayed in performance section

---

### User Story 4 - Performance Anomaly Detection (Priority: P4)

System should automatically detect and warn when BVH is causing performance degradation instead of improvement, helping developers identify problematic scenarios where BVH overhead exceeds benefits.

**Why this priority**: This is a quality-of-life enhancement that prevents developers from misusing BVH in simple scenes. Not critical for initial validation.

**Independent Test**: Can be tested by running application with very simple scene (3-5 objects) where BVH overhead likely exceeds benefits, and verifying that terminal outputs performance warning. Delivers proactive guidance.

**Acceptance Scenarios**:

1. **Given** scene has fewer than 10 objects, **When** BVH render time exceeds non-BVH render time, **Then** terminal displays warning "BVH overhead detected: consider disabling for simple scenes"
2. **Given** user is rendering frames, **When** BVH efficiency is calculated, **Then** system tracks rolling average of BVH ON vs OFF performance ratio
3. **Given** detailed logging is enabled, **When** each frame completes, **Then** terminal optionally outputs per-frame statistics (rays traced, tests performed, render time)

---

### Edge Cases

- What happens when scene has only 1-2 objects? BVH should gracefully handle minimal scenes without crashing, though performance benefit may be negative due to overhead.
- How does system handle BVH toggling during active rendering? Metrics should reset and recalculate for the new mode without visual artifacts.
- What if BVH build fails or tree is malformed? System should fall back to non-BVH rendering and display error message in terminal.
- How are metrics displayed when window is resized? Performance section in HUD should maintain readability and update positions accordingly.
- What happens to frame counters after extended runtime (overflow)? System should handle long-running sessions without integer overflow in counters (use appropriate data types).

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST collect and display frames per second (FPS) calculated from frame render time
- **FR-002**: System MUST measure and display frame rendering time in milliseconds
- **FR-003**: System MUST display current BVH status (ON/OFF) in HUD
- **FR-004**: System MUST count ray-bounding box intersection tests per frame when BVH is enabled
- **FR-005**: System MUST count ray-object intersection tests per frame
- **FR-006**: System MUST count BVH nodes visited during traversal per frame
- **FR-007**: System MUST count BVH nodes skipped (pruned branches) per frame
- **FR-008**: System MUST track total rays traced per frame
- **FR-009**: System MUST calculate average intersection tests per ray
- **FR-010**: System MUST reset all per-frame counters at the start of each frame
- **FR-011**: System MUST integrate performance metrics into left-side HUD below existing controls
- **FR-012**: System MUST toggle detailed BVH statistics visibility using existing 'I' key
- **FR-013**: System MUST display BVH tree statistics (node count, depth, average objects per leaf) when detail mode is active
- **FR-014**: System MUST output BVH build information to terminal when tree construction completes
- **FR-015**: System MUST measure and report BVH build time in milliseconds
- **FR-016**: System MUST calculate BVH efficiency as percentage reduction in intersection tests compared to naive approach
- **FR-017**: System MUST support optional per-frame statistics output to terminal for detailed analysis
- **FR-018**: System MUST detect and warn when BVH causes performance degradation (slower than non-BVH)
- **FR-019**: System MUST maintain acceptable performance overhead from metrics collection (under 5% frame time increase)
- **FR-020**: System MUST function correctly with or without BVH enabled, adapting displayed metrics accordingly

### Key Entities

- **Frame Metrics**: Real-time performance data collected during each render frame, including timing information, ray counts, and intersection test counts. Resets at frame boundaries to provide per-frame statistics.

- **BVH Statistics**: Tree structure information collected during BVH construction, including node counts, tree depth, leaf node distribution, and build time. Remains constant until scene changes or BVH is rebuilt.

- **Traversal Counters**: Incremental counters tracking BVH traversal operations during ray intersection tests, including nodes visited, nodes skipped, and bounding box tests performed. Accumulated per frame.

- **Performance Section**: Display area in HUD showing real-time metrics including FPS, frame time, BVH status, and optionally detailed statistics. Positioned in left side below existing control information.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: HUD displays current FPS, frame rendering time in milliseconds, and BVH status within 16ms frame budget
- **SC-002**: Toggling BVH with 'B' key updates displayed metrics within one frame (visible within 100ms)
- **SC-003**: Complex scenes (50+ objects) render at least 10 times faster with BVH enabled compared to disabled, as shown in FPS metrics
- **SC-004**: Simple scenes (5 or fewer objects) show BVH overhead not exceeding 2x slower compared to non-BVH rendering
- **SC-005**: BVH efficiency metric accurately reflects intersection test reduction (should show 80%+ reduction for complex scenes)
- **SC-006**: Detailed statistics mode ('I' key) displays all BVH metrics with complete information visible on screen
- **SC-007**: Terminal outputs BVH build statistics (node count, depth, objects per leaf, build time) immediately after construction completes
- **SC-008**: Metrics collection overhead increases frame time by less than 5% compared to rendering without metrics
- **SC-009**: Performance warning appears in terminal when BVH causes degradation (render time ratio > 1.2x)
- **SC-010**: System operates continuously for at least 1000 frames without metric counter overflow or display corruption
- **SC-011**: Users can visually confirm BVH is active and improving performance within 30 seconds of launching application with a complex scene
- **SC-012**: Average tests per ray decreases by at least 90% in complex scenes (100+ objects) when BVH is enabled, demonstrating effective spatial culling

## Assumptions *(mandatory)*

1. **Existing Metrics Infrastructure**: The codebase already has `metrics.h` header with performance tracking structures defined, including fields for FPS, render time, ray counts, and BVH statistics. Implementation needs to activate and integrate this existing system rather than design from scratch.

2. **HUD Extension Capability**: The existing HUD system (`hud_render.c`) can accommodate additional display sections without major architectural changes. Performance section can be added below current control information in left-side panel.

3. **Timer Availability**: System has access to high-resolution timers (microsecond precision) through standard library functions like `gettimeofday()` for accurate frame time measurement, which is already referenced in `metrics.h`.

4. **BVH Toggle Mechanism**: The 'B' key binding for BVH toggle is already implemented and functioning correctly. Metrics system only needs to observe and display the current state.

5. **Detail Mode Key Available**: The 'I' key for toggling detailed information is already implemented in the application. Feature will reuse this existing control.

6. **Terminal Output Acceptable**: Development workflow includes terminal access where printf output for build statistics and warnings is visible and useful. 42 School rules permit printf for debugging output.

7. **Single-Threaded Rendering**: Frame rendering is single-threaded, so metric counters do not require thread-safety mechanisms. All counter increments happen sequentially within one frame.

8. **Fixed Resolution**: Window resolution is stable during metrics collection. Metrics display updates if window is resized but frame-to-frame resolution is constant.

9. **Test Scenes Available**: Development environment has access to both simple scenes (3-5 objects) and complex scenes (50+ objects) for validation testing of BVH effectiveness.

10. **Norminette Compliance**: All code follows 42 School coding standards including 25-line function limit, so metrics functions must be decomposed into small, focused operations.

11. **No External Libraries**: Implementation uses only standard C library and allowed 42 functions (printf, write, etc.). No third-party profiling or metrics libraries are available.

12. **Memory Constraints**: Metrics structures are lightweight (counters and timers only) and do not require dynamic memory allocation. All metrics data fits within stack or static storage.
