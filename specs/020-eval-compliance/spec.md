# Feature Specification: Evaluation Compliance Fixes

**Feature Branch**: `020-eval-compliance`
**Created**: 2026-01-27
**Status**: Draft
**Input**: User description: "Analyze miniRT.md and miniRT_eval.md to identify gaps and fixes needed for evaluation compliance"

## User Scenarios & Testing *(mandatory)*

### User Story 1 - File Extension Validation (Priority: P1)

An evaluator provides a file without the `.rt` extension (e.g., `scene.txt` or no extension). The program must reject it with a proper error message.

**Why this priority**: This is a pass/fail gate in the evaluation (Section 2). If the program accepts non-.rt files, the entire defense fails with grade 0.

**Independent Test**: Run `./miniRT scene.txt` and verify the program outputs "Error\n" followed by a descriptive message and exits cleanly.

**Acceptance Scenarios**:

1. **Given** a valid scene file named `scene.txt`, **When** the user runs `./miniRT scene.txt`, **Then** the program outputs "Error\n" followed by an error message about invalid file extension and exits with non-zero status.
2. **Given** a file with no extension named `scene`, **When** the user runs `./miniRT scene`, **Then** the program outputs the same error and exits cleanly.
3. **Given** a valid scene file named `scene.rt`, **When** the user runs `./miniRT scene.rt`, **Then** the program parses the file normally.

---

### User Story 2 - Object Resizing (Priority: P1)

An evaluator wants to verify that sphere diameter and cylinder width/height can be modified. The subject states: "Your program must be able to resize the unique properties of objects."

**Why this priority**: This is a mandatory requirement explicitly stated in the subject. Failure to support it could result in lost points during defense.

**Independent Test**: Load a scene with a sphere and cylinder, use designated keys to resize each, and verify the rendered image updates accordingly.

**Acceptance Scenarios**:

1. **Given** a scene with a sphere selected, **When** the user presses the resize-increase key, **Then** the sphere's diameter increases and the scene re-renders showing a larger sphere.
2. **Given** a scene with a cylinder selected, **When** the user presses the diameter-increase key, **Then** the cylinder's width increases visibly.
3. **Given** a scene with a cylinder selected, **When** the user presses the height-increase key, **Then** the cylinder's height increases visibly.
4. **Given** any resizable object, **When** the user presses the resize-decrease key, **Then** the dimension decreases but never goes below a minimum threshold.

---

### User Story 3 - Object Rotation (Priority: P1)

An evaluator tests rotation of cylinders and planes (subject: "apply translation and rotation transformations to objects"). The evaluation specifically tests a 90-degree rotation of a cylinder.

**Why this priority**: Evaluation Section 5 explicitly tests cylinder rotation. Without this, the Translations and Rotations section scores 0.

**Independent Test**: Load a scene with a vertical cylinder, rotate it 90 degrees around the z-axis, verify it displays horizontally.

**Acceptance Scenarios**:

1. **Given** a cylinder extending along the y-axis, **When** the user applies a rotation around the z-axis, **Then** the cylinder's orientation changes and renders without glitching.
2. **Given** a plane with a vertical normal, **When** the user rotates it, **Then** the plane's orientation changes accordingly.
3. **Given** a sphere, **When** the user attempts rotation, **Then** nothing happens (spheres cannot be rotated per subject).
4. **Given** a light, **When** the user attempts rotation, **Then** nothing happens (lights cannot be rotated per subject).

---

### User Story 4 - Window Resize Consistency (Priority: P1)

An evaluator resizes the window and checks that content remains consistent (Section 3: "When you change the window resolution, the window's content must remain consistent").

**Why this priority**: Part of the Technical Display section (eval Section 3) which is all-or-nothing — if any test in this section fails, the entire section scores 0.

**Independent Test**: Run the program, resize the window by dragging edges, verify the rendered image remains consistent.

**Acceptance Scenarios**:

1. **Given** the program is running, **When** the evaluator resizes the window, **Then** the displayed content remains consistent without corruption or crash.
2. **Given** the window is minimized, **When** the evaluator restores it, **Then** the content is properly displayed.
3. **Given** the window is partially hidden by another window, **When** it is brought back to focus, **Then** the content is fully intact.

---

### User Story 5 - Cylinder Inside Intersection (Priority: P2)

The subject states: "all possible intersections and the insides of the objects must be handled correctly." The camera or a ray originating inside a cylinder must render correctly.

**Why this priority**: Could cause visible glitching in intersecting object scenes (Section 6).

**Independent Test**: Place the camera inside a cylinder and verify the interior surface renders correctly.

**Acceptance Scenarios**:

1. **Given** the camera is inside a cylinder, **When** the scene renders, **Then** the cylinder interior is visible without visual artifacts.
2. **Given** two intersecting objects (sphere overlapping cylinder), **When** rendered, **Then** the intersection boundary displays correctly without glitching.

---

### Edge Cases

- What happens when the user provides a filename ending with `.rt` but preceded by dots (e.g., `..rt`)?
- How does the program handle resizing an object to extremely small dimensions?
- What happens when rotating a cylinder whose axis becomes degenerate (zero vector)? → 정규화 불가 시 회전 미적용, 이전 축 벡터 유지 (FR-012)
- How does window resize behave when a render is in progress? → 내부 버퍼 고정, crash 없음 (FR-007, FR-013)
- What happens if the user tries to resize a plane? → 키 입력 무시 (FR-011)
- 렌더링 진행 중 오브젝트 변환 키를 연속 입력하면? → crash/segfault 없이 처리, 현재 렌더 완료 후 변환 반영 또는 즉시 반영 (FR-013). 검증: 복잡한 씬(오브젝트 10개 이상) 렌더 중 resize/rotation 키 반복 입력.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: Program MUST validate that the input filename ends with the `.rt` extension before attempting to parse.
- **FR-002**: Program MUST output "Error\n" followed by a descriptive message when the file extension is invalid.
- **FR-003**: Program MUST allow interactive resizing of sphere diameter via keyboard input.
- **FR-004**: Program MUST allow interactive resizing of cylinder diameter and height via keyboard input.
- **FR-005**: Program MUST allow interactive rotation of cylinders and planes via keyboard input. Rotation operates on world coordinate axes (X, Y, Z), each axis controlled by a separate key pair.
- **FR-006**: Program MUST NOT apply rotation to spheres or lights.
- **FR-007**: Program MUST maintain a fixed window size. OS가 윈도우 드래그 resize를 허용하더라도 내부 이미지 버퍼 크기는 초기 해상도(WINDOW_WIDTH x WINDOW_HEIGHT)로 고정하며, 기존 렌더링 결과를 유지한다. Expose 이벤트(창 가림/복원/최소화 복구) 시 기존 이미지 버퍼를 재출력한다. 어떤 경우에도 빈 화면, 깨진 픽셀, 부분 렌더링이 나타나면 안 된다. (참고: MiniLibX는 non-resizable 윈도우 속성을 직접 제어하기 어려우므로, 드래그 자체는 OS가 허용할 수 있으나 프로그램 내부는 이를 무시한다.)
- **FR-008**: Program MUST correctly render the interior of cylinders when the ray origin is inside the cylinder body. 검증 기준: (1) 내부에서 바라본 실린더 벽면이 해당 오브젝트 색상으로 표시된다, (2) 검은 면이나 무한 원근 아티팩트가 나타나지 않는다, (3) 교차하는 두 오브젝트 경계에서 z-fighting이나 깜빡임이 없다. 검증 시나리오: 카메라를 큰 실린더(지름 50, 높이 100) 내부 중심에 배치하고 사방을 렌더링하여 벽면/캡이 정상 표시되는지 확인. 추가로 구(지름 20)를 실린더 내부에 겹치게 배치하여 교차 경계를 확인.
- **FR-009**: Resize and rotation operations MUST trigger a re-render of the scene to reflect changes visually.
- **FR-010**: Resizing MUST enforce minimum dimension thresholds to prevent zero or negative sizes.
- **FR-011**: Resize 키 입력 시 대상 오브젝트가 plane인 경우 입력을 무시한다 (에러 메시지 없음, 동작 없음).
- **FR-012**: Rotation 연산 후 오브젝트 축 벡터가 퇴화(zero vector)하지 않도록 결과 벡터를 정규화해야 한다. 정규화 불가능한 경우(길이 ≈ 0) 회전을 적용하지 않고 이전 상태를 유지한다.
- **FR-013**: 렌더링 진행 중 오브젝트 변환(resize/rotation/translation) 키 입력이 발생해도 crash 없이 안전하게 처리해야 한다.

### Key Entities

- **Object**: Geometric entity (sphere/plane/cylinder) with position, orientation, dimensions, and color. Dimensions must be mutable at runtime for sphere (diameter) and cylinder (diameter, height).
- **Scene**: Collection of objects, camera, light, and ambient lighting. Must support translation, rotation, and resize transformations on its elements.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Mandatory Part 1~9 (executable name, config file, display, basic shapes, translations/rotations, multi-objects, camera, brightness 1/2, brightness 2/2) 모두 "Yes" 판정을 받는다.
- **SC-002**: Program rejects 100% of non-.rt files with the correct "Error\n" output format.
- **SC-003**: Users can interactively resize sphere diameter and cylinder dimensions, with visual feedback after each operation.
- **SC-004**: Users can interactively rotate cylinders and planes, with the orientation change visible in the rendered scene.
- **SC-005**: Window content remains visually consistent after minimize/restore, overlap with other windows, and OS-level resize attempts (fixed size 정책에 따라 내부 버퍼 불변, 콘텐츠 깨짐 없음).
- **SC-006**: No crashes, segfaults, or memory leaks occur during any evaluation scenario.

## Assumptions

- The `.rt` extension check is a simple case-sensitive string suffix match. (근거: miniRT.md "scene description file with the `.rt` extension")
- Object resizing key 매핑: J/K (지름 증감), N/M (높이 증감). 기존 오브젝트 이동 키(RTFGVB)와 충돌 없음. (결정: research.md R1)
- Object rotation key 매핑: U/O (X축), Y/P (Y축), LEFT/RIGHT (Z축). 기존 카메라 회전(EC)과 충돌 없음. (결정: research.md R1)
- Cylinder inside intersection: body hit 시 ray direction · normal > 0이면 법선 반전. 캡과 동일 패턴. (결정: research.md R4)
- Minimum resize threshold: 반지름/half_height 기준 0.1 (지름/높이 기준 0.2). (결정: research.md R5)

## Clarifications

### Session 2026-01-27

- Q: 오브젝트 회전 축 방식? → A: 월드 좌표 축 고정 (X/Y/Z 각각 별도 키로 회전)
- Q: Window resize 처리 전략? → A: 고정 윈도우 크기 (resize 비허용, expose 이벤트로 기존 버퍼 재출력)
- Q: Plane 선택 시 resize 키 동작? → A: 키 입력 무시 (에러 없음, 동작 없음)

## Scope Boundaries

### In Scope

- .rt file extension validation
- Object resizing (sphere diameter, cylinder diameter/height)
- Object rotation (cylinder axis, plane normal)
- Window resize handling
- Cylinder inside intersection correctness

### Already Implemented (verification only, no code changes needed)

아래는 eval Mandatory Part 1~9에 해당하지만 코드 확인 결과 이미 구현된 항목들이다. 별도 수정 없이 평가 시 정상 동작을 검증만 하면 된다.

- Camera translation (WASDQZ keys) — 근거: `src/window/window_camera.c:28-53`
- Camera rotation (EC keys) — 근거: `src/window/window_camera.c:65-92`
- Object translation (RTFGVB keys) — 근거: `src/window/window_objects.c:53-75`
- Light translation (Insert/Delete/Home/End/PgUp/PgDn keys) — 근거: `src/window/window_objects.c:87-110`
- Basic 3 shapes rendering (sphere, plane, cylinder) — 근거: `src/ray/intersect_object.c`, `src/ray/intersect_cyl_new.c`
- ESC / red cross window close — 근거: `src/window/window_events.c:55-56`, `src/window/window_init.c:50`
- Error output "Error\n" format — 근거: `src/utils/error.c:50, 69`
- Memory cleanup chain — 근거: `src/utils/cleanup.c:18-34`
- Ambient + diffuse lighting + hard shadows — 근거: `src/lighting/lighting.c`, `src/lighting/shadow_test.c`

> Note: Specular reflection(bonus)은 mandatory 평가 범위 밖이므로 이 목록에서 제외. 별도 bonus 평가 시 해당.

### Out of Scope

- Bonus features (checkerboard, cone, bump mapping, colored/multi-spot lights)
- New object types
- Camera FOV modification
- Scene file format changes
- Light rotation (explicitly forbidden by subject)
