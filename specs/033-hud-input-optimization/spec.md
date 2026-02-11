# Feature Specification: HUD / Input / Rendering Optimization

**Feature Branch**: `033-hud-input-optimization`
**Created**: 2026-02-10
**Status**: Draft
**Input**: `report/hud-input-optimization-report.md` 분석 결과 기반

## User Scenarios & Testing *(mandatory)*

### User Story 1 - 디바운스 상태 머신 재설계 (Priority: P1)

사용자가 키를 연속 탭(press-release-press-release)할 때, 매번 FQ(Full Quality) 렌더가 트리거되어 UI가 멈추는 문제를 해결한다. 새 상태 머신은 IDLE → ACTIVE → FINAL → COOLDOWN 흐름으로 재설계하여 입력을 병합하고, FQ 렌더 횟수를 최소화한다.

**Why this priority**: 현재 이산 탭마다 PREVIEW + FINAL 이중 렌더가 발생하여 가장 큰 체감 성능 문제. 상태 머신 재설계가 다른 모든 입력 최적화의 기반.

**Independent Test**: 키를 300ms 간격으로 5회 연속 탭 후 FQ 렌더 횟수가 1-2회인지 확인.

**Acceptance Scenarios**:

1. **Given** IDLE 상태, **When** 첫 입력, **Then** ACTIVE 전환 + 즉시 LQ 프리뷰 렌더 + 타이머(150ms) 시작
2. **Given** ACTIVE 상태 + 타이머 미만료, **When** 추가 입력, **Then** 타이머 리셋 + LQ 프리뷰(스로틀 50ms)
3. **Given** ACTIVE 상태 + 타이머 만료, **When** debounce_update 호출, **Then** FINAL 전환 + FQ 렌더 1회
4. **Given** FQ 완료, **When** 상태 전이, **Then** COOLDOWN(350ms) 진입
5. **Given** COOLDOWN 상태, **When** 새 입력, **Then** ACTIVE 복귀 + LQ 프리뷰 + 타이머 리셋
6. **Given** COOLDOWN 상태 + 350ms 무입력, **When** debounce_update 호출, **Then** IDLE 전환

---

### User Story 2 - 데드 코드 제거 (Priority: P1)

사용하지 않는 코드를 제거하여 유지보수성을 높이고 불필요한 렌더 트리거를 방지한다.

**Why this priority**: KEY_I 핸들러가 불필요한 FQ 렌더를 트리거하며, 데드 코드가 코드 이해를 방해함. 안전한 삭제만으로 즉각 개선 가능.

**Independent Test**: `I` 키 입력 시 렌더가 트리거되지 않고, 빌드가 정상 통과하는지 확인.

**Acceptance Scenarios**:

1. **Given** 프로그램 실행 중, **When** `I` 키 입력, **Then** 아무 동작 없음 (핸들러 제거됨)
2. **Given** HUD 렌더링, **When** hud_render() 호출, **Then** hud_render_background 호출 없음 (함수 제거됨)
3. **Given** 빌드, **When** `make`, **Then** 경고 없이 컴파일 성공

---

### User Story 3 - pixel_timing 게이트 추가 (Priority: P2)

`RENDER_ENABLE_PIXEL_TIMING` 플래그가 꺼져 있을 때 pixel_timing reset/calculate/print 호출을 생략하여 불필요한 연산과 stdout 노이즈를 제거한다.

**Why this priority**: 매 FQ 렌더마다 빈 데이터에 대한 정렬/통계/출력이 실행됨. 단순 조건문 추가로 해결 가능.

**Independent Test**: `RENDER_ENABLE_PIXEL_TIMING` 미설정 상태에서 FQ 렌더 후 stdout에 pixel timing 출력이 없는지 확인.

**Acceptance Scenarios**:

1. **Given** RENDER_ENABLE_PIXEL_TIMING 미설정, **When** FQ 렌더, **Then** pixel_timing_reset/calculate/print 미호출
2. **Given** RENDER_ENABLE_PIXEL_TIMING 설정, **When** FQ 렌더, **Then** 기존과 동일하게 동작

---

### User Story 4 - 키맵 변경 및 키가이드 업데이트 (Priority: P2)

macOS 랩탑에서 사용 불가능한 Insert/Delete/Home/End/PgUp/PgDn 키를 구두점 키로 대체하고, Resize/Rotate 키를 키보드 그리드 레이아웃(Y,U/N,M + I,J/O,K/P,L)으로 재배치하며, 키가이드 표시를 업데이트한다.

**Why this priority**: macOS 사용자가 Light 이동 기능에 접근 불가. 키가이드에 누락된 기능이 다수. Resize/Rotate 키 배치가 비직관적.

**Independent Test**: macOS 랩탑에서 Light X/Y/Z 이동이 가능하고, Resize(Y,U/N,M)와 Rotate(I,J/O,K/P,L) 동작하며, 키가이드에 표시되는지 확인.

**Acceptance Scenarios**:

1. **Given** 키맵 Option A 적용, **When** `[`/`]` 입력, **Then** Light X- / X+ 이동
2. **Given** 키맵 변경, **When** `Y`/`U` 입력, **Then** Obj Resize X- / X+ 동작
3. **Given** 키맵 변경, **When** `I`/`J`/`O`/`K`/`P`/`L` 입력, **Then** Obj Rotate X/Y/Z 동작
4. **Given** 키맵 변경, **When** 키가이드 표시, **Then** Light 이동, Resize, Rotate, HUD 페이지 키 모두 표시
5. **Given** 키맵 변경, **When** Tab/Shift+Tab 입력, **Then** 오브젝트 선택 동작

---

### User Story 5 - 키가이드 점진적 리드로 (Priority: P3)

키가이드 텍스트를 정적/동적 섹션으로 분리하여 매 프레임 전체 재출력을 방지한다.

**Why this priority**: 활발한 입력 시 매 프레임 `mlx_string_put()` 전체 호출이 체감 지연 유발. P1/P2 완료 후 추가 최적화.

**Independent Test**: 키가이드 표시 상태에서 입력 시 정적 라벨이 재렌더되지 않는지 확인.

**Acceptance Scenarios**:

1. **Given** 키가이드 활성, **When** 정적 라벨 변경 없음, **Then** 정적 라벨 mlx_string_put 미호출
2. **Given** 키가이드 활성 + 동적 값 변경, **When** 리드로, **Then** 변경된 값만 업데이트

---

### Edge Cases

- COOLDOWN 중 350ms 경계에서 입력이 도착하는 타이밍 레이스
- LQ 스로틀(50ms)과 키 반복(~30ms) 간격 경합 시 LQ 렌더 빈도
- FQ 렌더 중(블로킹) 큐잉된 이벤트 다수가 FQ 완료 후 일괄 처리될 때 상태 전이
- 키맵 변경 시 기존 씬 파일의 키 충돌 없음 확인
- `debounce_on_input()` 내 `last_preview_time` 체크 시 Norm 로컬 변수 5개 제한

## Requirements *(mandatory)*

### Functional Requirements

#### 디바운스 상태 머신 재설계

- **FR-001**: DEBOUNCE_PREVIEW 상태를 제거하고 DEBOUNCE_COOLDOWN 상태를 추가해야 한다
- **FR-002**: IDLE → ACTIVE 전환 시 즉시 LQ 프리뷰 렌더를 트리거해야 한다 (RENDER_LOW_QUALITY | RENDER_DIRTY)
- **FR-003**: LQ 프리뷰는 최소 간격 50ms(DEBOUNCE_PREVIEW_MIN_INTERVAL_MS)로 스로틀링해야 한다. 실측 후 조정 가능하도록 define으로 관리한다
- **FR-004**: ACTIVE → 타이머(150ms) 만료 시 PREVIEW 없이 바로 FINAL로 전환해야 한다
- **FR-005**: FQ 완료 후 COOLDOWN(350ms) 상태로 진입해야 한다. 350ms는 사람의 의도적 키 재입력 간격(300-400ms) 기준이며, 실측 후 조정 가능하도록 `DEBOUNCE_COOLDOWN_MS` define으로 관리한다
- **FR-006**: COOLDOWN 중 입력 시 ACTIVE로 복귀 + LQ 프리뷰 + 타이머 리셋해야 한다
- **FR-007**: COOLDOWN 350ms 무입력 시 IDLE로 전환해야 한다
- **FR-008**: cancel_requested 관련 사문 코드를 제거해야 한다

#### 데드 코드 제거

- **FR-009**: KEY_I 핸들러 (`window_events.c:39-43`)를 제거해야 한다. `KEY_I` define은 Obj Rotate X-에 재활용하므로 유지
- **FR-010**: RENDER_SHOW_INFO define (`window.h:33`)을 제거해야 한다
- **FR-011**: 키가이드 `"Display:"` 섹션 + `"I - Info"` 텍스트를 제거해야 한다
- **FR-012**: `hud_render_background()`, `hud_render_background_row()` 함수 및 헤더 선언을 제거해야 한다

#### 렌더링 오버헤드 제거

- **FR-013**: pixel_timing_reset/calculate/print를 RENDER_ENABLE_PIXEL_TIMING 플래그로 게이트해야 한다

#### 키맵 및 키가이드

- **FR-014**: Light 이동 키를 macOS 호환 구두점 키(`[]/;'/,.`)로 대체해야 한다
- **FR-014a**: Obj Resize 키를 Y,U/N,M 키보드 그리드 레이아웃으로 재배치해야 한다
- **FR-014b**: Obj Rotate 키를 I,J/O,K/P,L 키보드 그리드 레이아웃으로 재배치해야 한다
- **FR-015**: 키가이드에 Light 이동, Resize, Rotate, HUD 페이지 전환 키를 표시해야 한다
- **FR-016**: 키가이드 점진적 리드로: 정적/동적 섹션 분리

### Key Entities

- **t_debounce_state**: 디바운스 상태 머신. `DEBOUNCE_COOLDOWN` enum 추가, `last_preview_time` 필드 추가, `cancel_requested`/`preview_enabled`/`auto_upgrade` 필드 제거
- **t_debounce_state_enum**: IDLE, ACTIVE, FINAL, COOLDOWN (PREVIEW 제거)
- **t_render**: 렌더 상태. `RENDER_SHOW_INFO` 플래그 제거

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: 이산 탭 5회 연속(300ms 간격) 시 FQ 렌더 2회 이하 (현재 5회)
- **SC-002**: Key-hold 시 LQ 프리뷰 ~20fps + FQ 1회 (현재 FQ 1회만, LQ 피드백 없음)
- **SC-003**: KEY_I 입력 시 렌더 트리거 0회 (현재 1회)
- **SC-004**: RENDER_ENABLE_PIXEL_TIMING 미설정 시 stdout pixel_timing 출력 0건
- **SC-005**: macOS 랩탑에서 Light X/Y/Z 이동 가능 (현재 불가)
- **SC-006**: 42 Norm v4.1 완전 준수 (norminette 통과)
- **SC-007**: 렌더링 결과 변경 없음 (동일 씬, 동일 카메라 → 동일 화면)
