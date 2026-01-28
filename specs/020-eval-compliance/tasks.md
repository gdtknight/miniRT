# Tasks: Evaluation Compliance Fixes

**Input**: Design documents from `/specs/020-eval-compliance/`
**Prerequisites**: plan.md, spec.md, research.md, data-model.md, quickstart.md

**Tests**: Manual visual verification + test .rt scene files (no automated tests)

**Organization**: Tasks are grouped by user story. US1(확장자), US2(리사이즈), US3(회전)은 P1 우선순위. US4(윈도우), US5(내부교차)는 P1/P2. US2/US3는 Foundational(키 인프라) 완료 후 병렬 가능.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (US1~US5)
- Include exact file paths in descriptions

---

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: 신규 키 상수 및 상태 플래그 추가. 모든 User Story의 전제 조건.

- [X] T001 [P] Add KEY_J, KEY_K, KEY_N, KEY_M key constants (resize keys) to `includes/window_internal.h` — macOS: 38/40/45/46, Linux: 106/107/110/109. `#ifdef __APPLE__` / `#elif defined(__linux__)` 분기 내에 추가. 근거: data-model.md §2, research.md R1.
- [X] T002 [P] Add KEY_U, KEY_O, KEY_Y, KEY_P, KEY_LEFT, KEY_RIGHT key constants (rotation keys) to `includes/window_internal.h` — macOS: 32/31/16/35/123/124, Linux: 117/111/121/112/65361/65363. 근거: data-model.md §2, research.md R1.
- [X] T003 [P] Add `RENDER_BVH_DIRTY` flag (0x20) to `includes/window.h` — 기존 RENDER_SHOW_INFO(0x10) 다음에 `#define RENDER_BVH_DIRTY 0x20` 추가. 근거: data-model.md §1, plan.md D2.

**Checkpoint**: 키 상수와 플래그가 정의됨. 컴파일 확인 (`make`).

---

## Phase 2: Foundational (Key Dispatch Infrastructure)

**Purpose**: 키 입력 dispatch 확장. US2(리사이즈), US3(회전) 모두 이 Phase에 의존.

**⚠️ CRITICAL**: US2, US3 작업은 이 Phase 완료 후에만 시작 가능.

- [X] T004 Extend `handle_transform_keys()` in `src/window/window_key_handlers.c` to dispatch resize keys (KEY_J/K/N/M) to a new `handle_object_resize()` function. 기존 함수 하단에 `else if` 분기 추가. 함수 프로토타입은 `void handle_object_resize(t_render *render, int keycode)`. 근거: plan.md Phase B.
- [X] T005 Extend `handle_transform_keys()` in `src/window/window_key_handlers.c` to dispatch rotation keys (KEY_U/O/Y/P/LEFT/RIGHT) to a new `handle_object_rotate()` function. T004와 동일 패턴. 함수 프로토타입은 `void handle_object_rotate(t_render *render, int keycode)`. 근거: plan.md Phase B.

**Checkpoint**: 키 dispatch 완료. 컴파일은 아직 실패 (핸들러 함수 미구현). 프로토타입만 헤더에 선언하면 링크 에러만 남음.

---

## Phase 3: User Story 1 — File Extension Validation (Priority: P1) 🎯 MVP

**Goal**: `.rt` 확장자가 아닌 파일을 거부하여 eval Section 2 통과.

**Independent Test**: `./miniRT scene.txt 2>&1` → stderr에 "Error\n" + 설명 메시지 출력, 비정상 종료 없음.

### Implementation for User Story 1

- [X] T006 [P] [US1] Add `.rt` extension validation function in `src/parser/parser.c` — `parse_scene()` 함수 시작부에서 `open()` 호출 전에 filename suffix `.rt` 체크. 구현: ft_strlen(filename) >= 3 && strncmp(filename + len - 3, ".rt", 3) == 0 패턴. 실패 시 `print_error("Invalid file extension (expected .rt)")` 호출 후 return (0). 기존 ERR_FILE_EXT 에러 코드가 `src/utils/error.c`에 정의되어 있으므로 활용. 근거: spec.md FR-001/002, plan.md Phase A.

**Checkpoint**: `./miniRT scene.txt` → "Error\n" + 메시지 출력. `./miniRT scene.rt` → 정상 파싱. quickstart.md 확장자 테스트 섹션 기준으로 검증.

---

## Phase 4: User Story 2 — Object Resizing (Priority: P1)

**Goal**: 구 지름, 실린더 지름/높이를 키보드로 증감하여 subject 필수 요구사항 충족.

**Independent Test**: resize_test.rt 로드 → 구 선택 → J/K로 크기 변화 확인. 실린더 선택 → J/K(지름), N/M(높이) 변화 확인.

### Implementation for User Story 2

- [X] T007 [US2] Create `src/window/window_resize.c` with 42 header and `#include` directives. 파일 구조: `handle_object_resize(t_render *render, int keycode)` 메인 함수 + 내부 static helper 함수. Norm 준수 (5함수/파일, 25줄/함수, 4파라미터/함수). 근거: plan.md Structure Decision.
- [X] T008 [US2] Implement sphere resize logic in `src/window/window_resize.c` — KEY_J: radius += 1.0, radius_sq 재계산. KEY_K: radius -= 1.0, radius < 0.1이면 무시 (FR-010, D5). KEY_N/M: 무시 (구는 높이 없음). 선택된 오브젝트가 OBJ_SPHERE인 경우에만 적용. 근거: data-model.md Resize Flow.
- [X] T009 [US2] Implement cylinder resize logic in `src/window/window_resize.c` — KEY_J/K: radius ± 1.0, radius_sq 재계산, radius < 0.1이면 무시. KEY_N/M: half_height ± 1.0, half_height < 0.1이면 무시. 선택된 오브젝트가 OBJ_CYLINDER인 경우에만 적용. 근거: data-model.md Resize Flow.
- [X] T010 [US2] Implement plane ignore in `src/window/window_resize.c` — OBJ_PLANE인 경우 early return (FR-011). 어떤 키도 무시, 에러 메시지 없음.
- [X] T011 [US2] Add BVH dirty flag + debounce trigger in `src/window/window_resize.c` — resize 적용 후 `render_set_flag(render, RENDER_BVH_DIRTY)` + `debounce_on_input(&render->debounce, render)` 호출. 근거: data-model.md Resize Flow, plan.md D2.
- [X] T012 [US2] Add `handle_object_resize` prototype to appropriate header and update Makefile to compile `src/window/window_resize.c`. Makefile의 SRC_WINDOW 변수에 `src/window/window_resize.c` 추가.

**Checkpoint**: resize_test.rt로 quickstart.md 키 매핑 검증 체크리스트(resize 부분) 수행. 구/실린더 크기 변화 확인, 평면 무시 확인, 최소값 가드 확인.

---

## Phase 5: User Story 3 — Object Rotation (Priority: P1)

**Goal**: 실린더/평면을 월드 좌표 축 기준으로 회전하여 eval Section 5 통과.

**Independent Test**: rotation_test.rt 로드 → 실린더 선택 → LEFT/RIGHT 키로 Z축 회전 확인. 18회 입력 = 90° 회전.

### Implementation for User Story 3

- [X] T013 [US3] Implement `handle_object_rotate()` in `src/window/window_objects.c` — 메인 dispatch: keycode → 회전 축(X/Y/Z) + 방향(+/-) 결정. KEY_U/O → X축, KEY_Y/P → Y축, KEY_LEFT/RIGHT → Z축. 회전 각도: ±5° (M_PI / 36.0). 선택된 오브젝트가 OBJ_SPHERE이면 early return (FR-006). 근거: spec.md FR-005/006, data-model.md Rotation Flow.
- [X] T014 [US3] Implement Rodrigues rotation in `src/window/window_objects.c` — static helper `rotate_vector(t_vec3 v, t_vec3 axis, double angle)`: v_rot = v*cos(θ) + (k×v)*sin(θ) + k*(k·v)*(1-cos(θ)). 기존 vec3_cross, vec3_dot, vec3_multiply, vec3_add 함수 활용. 결과 벡터 정규화. 정규화 실패(길이 < EPSILON)시 원본 벡터 반환 (FR-012). 근거: data-model.md Rotation 수학.
- [X] T015 [US3] Apply rotation to cylinder axis and plane normal in `src/window/window_objects.c` — OBJ_CYLINDER: `obj->data.cylinder.axis = rotate_vector(axis, rot_axis, angle)`. OBJ_PLANE: `obj->data.plane.normal = rotate_vector(normal, rot_axis, angle)`. 적용 후 `render_set_flag(render, RENDER_BVH_DIRTY)` + `debounce_on_input()` 호출. 근거: data-model.md Rotation Flow.
- [X] T016 [US3] Verify Norm compliance in `src/window/window_objects.c` — 현재 3함수 + 신규 3함수(handle_object_rotate, rotate_vector, apply rotation) = 6함수. Norm 5함수/파일 초과 시 rotate_vector를 별도 파일(`src/window/window_rotate.c`)로 분리. Makefile 업데이트 필요 여부 확인.

**Checkpoint**: rotation_test.rt로 quickstart.md 키 매핑 검증 체크리스트(rotation 부분) 수행. 실린더 Z축 회전 18회 = 수평 확인. 구 회전 무시 확인. 축 퇴화 검증(FR-012).

---

## Phase 6: User Story 2+3 공통 — BVH Rebuild (Blocking)

**Purpose**: 리사이즈/회전 후 BVH 재빌드로 교차 테스트 정확성 보장.

**⚠️ CRITICAL**: Phase 4, 5 완료 후 수행. 이 Phase 없이는 리사이즈/회전 후 렌더링이 부정확할 수 있음.

- [X] T017 Implement conditional BVH rebuild in render loop in `src/render/render.c` — 렌더 시작 직전에 `if (render_has_flag(render, RENDER_BVH_DIRTY))` 체크. 참이면 `scene_build_bvh(render->scene)` 호출 + `render_clear_flag(render, RENDER_BVH_DIRTY)`. 위치: `render_loop()` 또는 실제 렌더 함수 진입점에서 RENDER_DIRTY 체크 직후. 근거: plan.md D2, data-model.md BVH Rebuild Flow.
- [X] T018 Add BVH dirty flag to existing object translation handlers in `src/window/window_objects.c` — `handle_object_move()` 함수 끝에 `render_set_flag(render, RENDER_BVH_DIRTY)` 추가. 기존 오브젝트 이동 시에도 BVH가 재빌드되어야 교차가 정확함. `handle_light_move()`에도 동일 적용 (라이트 이동은 BVH에 영향 없으나 일관성 위해 — 또는 생략 가능, 라이트는 BVH 노드가 아님).

**Checkpoint**: resize/rotation 후 오브젝트 교차가 정확하게 렌더링되는지 확인. 겹치는 오브젝트를 이동/리사이즈하여 BVH 반영 여부 검증.

---

## Phase 7: User Story 4 — Window Stability (Priority: P1)

**Goal**: expose 이벤트 처리로 창 가림/복원 시 콘텐츠 유지, eval Section 3 통과.

**Independent Test**: 프로그램 실행 → 다른 창으로 가린 후 복원 → 이미지 정상 표시.

### Implementation for User Story 4

- [X] T019 [P] [US4] Implement `handle_expose()` in `src/window/window_events.c` — `int handle_expose(t_render *render)`: `mlx_put_image_to_window(render->mlx.mlx, render->mlx.win, render->mlx.img, 0, 0)` 호출 후 `return (0)`. 새 렌더 트리거 없음 (기존 버퍼 재출력만). Norm 준수 확인: 현재 window_events.c에 3함수, 추가 후 4함수 (5함수 제한 이내). 근거: plan.md D3.
- [X] T020 [US4] Register expose hook in `src/window/window_init.c` — 기존 `mlx_hook` 호출들 근처에 `mlx_hook(render->mlx.win, 12, 1L << 15, handle_expose, render)` 추가. event 12 = Expose (X11) / 동등 이벤트 (macOS). 근거: plan.md D3.

**Checkpoint**: quickstart.md Window Stability 검증 체크리스트 수행. 가림/복원, 최소화/복원, OS resize 드래그 테스트.

---

## Phase 8: User Story 5 — Cylinder Inside Intersection (Priority: P2)

**Goal**: 실린더 body 내부에서 법선이 올바르게 반전되어 내부 렌더링 정상화.

**Independent Test**: inside_test.rt 로드 → 카메라가 실린더 내부에서 벽면이 정상 색상으로 표시.

### Implementation for User Story 5

- [X] T021 [P] [US5] Add normal flip for cylinder body intersection in `src/ray/intersect_cyl_new.c` — `intersect_cyl_body_new()` 함수에서 hit->normal 계산 직후에 `if (vec3_dot(ray->direction, hit->normal) > 0) hit->normal = vec3_multiply(hit->normal, -1.0);` 추가. 캡 교차(intersect_cyl_cap_new, lines 61-63)와 동일 패턴. 근거: plan.md D4, research.md R4.

**Checkpoint**: quickstart.md 실린더 내부 교차 검증 체크리스트 수행. inside_test.rt로 벽면/캡 정상, 교차 경계 z-fighting 없음 확인.

---

## Phase 9: Polish & Integration Verification

**Purpose**: 전체 기능 통합 검증, 테스트 씬 파일 생성, Norm/메모리 최종 확인.

- [X] T022 [P] Create test scene files: `scenes/resize_test.rt`, `scenes/rotation_test.rt`, `scenes/inside_test.rt` per quickstart.md 검증 테스트 씬 섹션. 기존 scenes/ 디렉토리에 추가.
- [X] T023 Run full Norminette check on all modified/new files: `norminette includes/window_internal.h includes/window.h src/parser/parser.c src/window/window_init.c src/window/window_events.c src/window/window_key_handlers.c src/window/window_objects.c src/window/window_resize.c src/ray/intersect_cyl_new.c src/render/render.c`. 모든 파일 0 errors.
- [X] T024 Run memory leak check: `leaks miniRT` (macOS) during interactive operation — resize/rotation/expose 이벤트 반복 후 leak 없음 확인.
- [X] T025 Execute quickstart.md 통합 검증 체크리스트 전체 수행: 키 매핑 검증, Window Stability, 렌더 중 입력 안전(FR-013), 실린더 내부 교차, 키코드 플랫폼 검증.
- [X] T026 Execute quickstart.md 확장자 테스트: `.rt` 오류 메시지가 stderr에 "Error\n" + 설명 형식과 정확히 일치하는지 확인.
- [X] T027 Verify `make re` compiles without warnings under `-Wall -Wextra -Werror`.

**Checkpoint**: 모든 검증 통과. eval Mandatory Part 1~9 시뮬레이션 가능.

---

## Dependencies & Execution Order

### Phase Dependencies

- **Phase 1 (Setup)**: No dependencies — can start immediately
- **Phase 2 (Foundational)**: Depends on Phase 1 — BLOCKS US2, US3
- **Phase 3 (US1)**: Depends on Phase 1 only — **can run parallel with Phase 2**
- **Phase 4 (US2)**: Depends on Phase 2
- **Phase 5 (US3)**: Depends on Phase 2 — **can run parallel with Phase 4**
- **Phase 6 (BVH)**: Depends on Phase 4, 5
- **Phase 7 (US4)**: Depends on Phase 1 only — **can run parallel with Phases 2~6**
- **Phase 8 (US5)**: No Phase dependencies — **can run parallel with everything**
- **Phase 9 (Polish)**: Depends on ALL phases complete

### Dependency Graph

```
Phase 1 (Setup) ──┬── Phase 2 (Keys) ──┬── Phase 4 (US2: Resize) ──┐
                   │                     └── Phase 5 (US3: Rotate) ──┼── Phase 6 (BVH) ──┐
                   ├── Phase 3 (US1: Extension) ─────────────────────┤                    │
                   └── Phase 7 (US4: Window) ────────────────────────┤                    │
Phase 8 (US5: Cylinder) ────────────────────────────────────────────┤                    │
                                                                     └── Phase 9 (Polish) ┘
```

### Parallel Opportunities

- **즉시 병렬**: T001, T002, T003 (Phase 1 내 모든 태스크)
- **Phase 1 완료 후**: Phase 2, Phase 3(US1), Phase 7(US4), Phase 8(US5) 동시 시작 가능
- **Phase 2 완료 후**: Phase 4(US2), Phase 5(US3) 동시 시작 가능
- **서로 다른 파일**: T006(parser.c), T019(window_events.c), T021(intersect_cyl_new.c) 모두 독립

---

## Parallel Example: Maximum Parallelism

```
# After Phase 1 complete, launch 4 streams simultaneously:

Stream 1: Phase 2 (T004, T005) → Phase 4 (T007~T012) → Phase 6 (T017, T018)
Stream 2: Phase 3 (T006) — US1 완료
Stream 3: Phase 7 (T019, T020) — US4 완료
Stream 4: Phase 8 (T021) — US5 완료

# Phase 5 (US3) starts when Phase 2 completes:
Stream 5: Phase 5 (T013~T016) → feeds into Phase 6

# Final: Phase 9 (T022~T027) after all streams complete
```

---

## Implementation Strategy

### MVP First (User Story 1 Only)

1. Complete Phase 1: Setup (T001~T003)
2. Complete Phase 3: US1 — File Extension Validation (T006)
3. **STOP and VALIDATE**: `./miniRT scene.txt` → Error 출력 확인
4. 이것만으로 eval Section 2의 확장자 검증 통과

### Incremental Delivery

1. Phase 1 + Phase 3 (US1) → .rt 검증 통과
2. Phase 2 + Phase 4 (US2) + Phase 6 (BVH) → 리사이즈 동작
3. Phase 5 (US3) → 회전 동작 (eval Section 5 통과)
4. Phase 7 (US4) → 윈도우 안정성 (eval Section 3 통과)
5. Phase 8 (US5) → 내부 교차 보정 (eval Section 6 보강)
6. Phase 9 → 최종 검증

---

## Notes

- [P] tasks = different files, no dependencies
- [Story] label maps task to specific user story for traceability
- Norm constraint: 5 functions per .c file — T016에서 확인 필요 (window_objects.c 초과 시 파일 분리)
- BVH rebuild (Phase 6)는 US2/US3 모두에 필요하므로 공통 Phase로 분리
- FR-013은 별도 구현 불필요 (plan.md D6) — Phase 9에서 스트레스 테스트로 검증만 수행
- 모든 신규 코드는 heap 할당 없음 (스택 연산만) — 메모리 leak 리스크 없음
