# Feature Specification: Fix HUD Key Guide & Expose Restore

**Feature Branch**: `021-fix-hud-keys-expose`
**Created**: 2026-01-28
**Status**: Draft
**Input**: User description: "HUD에 새로 추가된 키들도 함께 표시. 화면 창 복구 했을 때 scene은 제대로 표시가 되지만 HUD가 다시 표시되지 않는 현상 수정."

## User Scenarios & Testing *(mandatory)*

### User Story 1 - HUD Not Restored After Window Expose (Priority: P1)

프로그램을 다른 창으로 가린 후 복원하면, 씬 이미지는 정상 표시되지만 HUD와 키 가이드 오버레이가 사라진다. 사용자는 HUD를 다시 보려면 H 키로 토글을 두 번 해야 한다.

**Why this priority**: 평가 Section 3 (Window Stability)에서 "콘텐츠가 일관되게 유지되어야 한다"고 요구한다. HUD가 사라지는 것은 콘텐츠 불일치로 판정될 수 있다.

**Independent Test**: 프로그램 실행 → HUD 표시 상태(H키) → 다른 창으로 가림 → 복원 → HUD와 키 가이드가 그대로 표시되는지 확인.

**Acceptance Scenarios**:

1. **Given** HUD가 표시된 상태에서, **When** 창을 다른 창으로 가린 후 복원하면, **Then** HUD와 키 가이드가 이전 상태 그대로 표시된다.
2. **Given** HUD가 숨김 상태에서, **When** 창을 복원하면, **Then** HUD 없이 씬만 표시된다 (이전 상태 유지).
3. **Given** 프로그램이 최소화된 상태에서, **When** 복원하면, **Then** HUD가 이전 상태 그대로 표시된다.

---

### User Story 2 - Display New Keys in Key Guide (Priority: P1)

020-eval-compliance에서 추가된 resize/rotation 키(J/K/N/M/U/O/Y/P/LEFT/RIGHT)가 키 가이드에 표시되지 않아 사용자가 새 기능의 존재를 알 수 없다.

**Why this priority**: 평가 시 조작 방법을 한눈에 파악할 수 없으면 평가관이 기능 미구현으로 오인할 수 있다.

**Independent Test**: 프로그램 실행 → H키로 HUD 표시 → 키 가이드 영역에 resize/rotation 키 라벨이 표시되는지 확인.

**Acceptance Scenarios**:

1. **Given** HUD가 표시된 상태에서, **When** 키 가이드를 확인하면, **Then** resize 키 라벨(J/K, N/M)이 표시된다.
2. **Given** HUD가 표시된 상태에서, **When** 키 가이드를 확인하면, **Then** rotation 키 라벨(U/O, Y/P, ←/→)이 표시된다.
3. **Given** 키 가이드에 신규 키가 추가되었을 때, **When** 전체 키 가이드를 확인하면, **Then** 기존 키 라벨과 함께 읽기 쉽게 그룹화되어 표시된다.

---

### Edge Cases

- HUD가 꺼진 상태에서 expose 이벤트가 발생하면 HUD를 렌더링하지 않아야 한다.
- 키 가이드에 새 항목을 추가했을 때 기존 레이아웃이 겹치거나 잘리지 않아야 한다.
- 렌더링 진행 중 expose 이벤트가 발생해도 crash 없이 처리되어야 한다.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: 창 복원(expose 이벤트) 시 HUD가 표시 상태였다면, 씬 이미지 버퍼를 화면에 출력한 뒤 HUD 오버레이와 키 가이드 오버레이를 순차적으로 다시 그린다. 새로운 레이트레이싱 연산은 수행하지 않는다.
- **FR-002**: 창 복원 시 HUD가 숨김 상태였다면 씬 이미지 버퍼만 화면에 재출력해야 한다 (HUD를 새로 켜지 않는다).
- **FR-003**: 키 가이드에 resize 키를 표시해야 한다: "J/K - Diameter", "N/M - Height".
- **FR-004**: 키 가이드에 rotation 키를 표시해야 한다: "U/O - Rot X", "Y/P - Rot Y", "←/→ - Rot Z".
- **FR-005**: 신규 키 라벨은 기존 키 가이드 레이아웃에 "Resize" 섹션(J/K, N/M)과 "Rotation" 섹션(U/O, Y/P, ←/→)으로 그룹화하여 추가한다. 각 섹션은 헤더 라벨로 구분하고, 기존 섹션(Navigation, Camera, Objects, Display) 뒤에 배치한다.
- **FR-006**: expose 핸들러는 렌더 플래그를 변경하지 않으며, 레이트레이싱 패스를 트리거하지 않는다. 기존 버퍼 출력과 오버레이 재출력만 수행한다.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: 창 가림/복원 후 HUD와 키 가이드가 100% 이전 상태로 표시된다.
- **SC-002**: 키 가이드에 resize 4키(J/K/N/M)와 rotation 6키(U/O/Y/P/←/→)가 모두 표시된다.
- **SC-003**: expose 복원 시 프레임 카운터가 증가하지 않으며, 렌더링 소요 시간이 발생하지 않는다 (기존 버퍼 재출력만 수행됨을 확인).
- **SC-004**: 기존 키 가이드 항목이 잘리거나 겹치지 않으며, Resize/Rotation 섹션이 헤더로 구분되어 표시된다.

## Assumptions

- 키 가이드 라벨 텍스트는 영문 약어를 사용한다 (현재 키 가이드와 동일한 형식).
- expose 핸들러는 현재 씬 이미지 버퍼만 재출력하며, HUD/keyguide는 별도 오버레이로 렌더링된다.
- 키 가이드 컨테이너 높이 상수 조정으로 신규 항목 공간을 확보한다 (기존 섹션 레이아웃은 변경하지 않음).

## Scope Boundaries

### In Scope

- Expose 핸들러에서 HUD/키 가이드 재출력
- 키 가이드에 resize/rotation 키 라벨 추가

### Out of Scope

- 키 가이드 레이아웃 전면 재설계
- HUD 콘텐츠(오브젝트 정보, 성능 지표 등) 변경
- 새로운 HUD 페이지나 기능 추가
