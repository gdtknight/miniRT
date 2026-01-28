# Research: Evaluation Compliance Fixes

## R1: Key Mapping for Resize/Rotation

**Decision**: J/K (지름), N/M (높이), U/O (X축 회전), Y/P (Y축 회전), LEFT/RIGHT (Z축 회전)

**Rationale**:
- 기존 사용 키 충돌 회피: WASDQZEC(카메라), RTFGVB(오브젝트 이동), H/I/TAB/[/](HUD)
- 키보드 물리 배치 고려: resize(JK/NM)와 rotation(UO/YP)이 우측 영역에 집중되어 조작 편의성 유지
- LEFT/RIGHT 화살표는 UP/DOWN과 일관된 방향키 그룹 활용

**Alternatives considered**:
- 숫자키(1-6): 직관성 낮음, 실수 가능성
- Shift+기존키: MiniLibX는 Shift modifier와 keycode를 별도 이벤트(KeyPress/KeyRelease)로 전달하여 조합 키 처리가 복잡하고, 현재 코드의 SHIFT_HELD 플래그 관리가 이미 존재하므로 추가 조합을 도입하면 상태 관리 부담 증가

## R2: BVH Rebuild Strategy

**Decision**: Lazy rebuild — RENDER_BVH_DIRTY 플래그 + 렌더 직전 1회 rebuild

**Rationale**:
- 현재 코드에서 `scene_build_bvh()`는 내부적으로 기존 트리를 destroy 후 재생성
- 매 키 입력마다 rebuild하면 debounce 중 불필요한 중복 rebuild 발생
- 렌더 직전 1회로 통합하면 debounce 150ms 동안의 모든 변환을 한 번에 반영

**Alternatives considered**:
- 즉시 rebuild (매 입력마다): 불필요한 overhead, debounce와 충돌
- BVH 비활성화: 성능 저하, 대규모 씬에서 문제

## R3: Expose Event Handling

**Decision**: mlx_hook event 12 등록, mlx_put_image_to_window 호출만 수행

**Rationale**:
- 현재 expose 이벤트 미등록 → 창 가림/복원 시 빈 화면 가능성
- expose handler에서 기존 이미지 버퍼를 재출력하면 추가 렌더 없이 일관성 유지
- MiniLibX의 mlx_put_image_to_window는 기존 이미지 데이터를 그대로 전송

**Alternatives considered**:
- RENDER_DIRTY 플래그 세팅 후 재렌더: 불필요한 연산, 복원 시 지연 발생
- 없음 (현상 유지): FR-007 미충족

## R4: Cylinder Inside Normal Correction

**Decision**: body intersection에서 ray direction · normal > 0이면 normal 반전

**Rationale**:
- 캡 교차(intersect_cyl_cap_new)에서 이미 동일 패턴으로 법선 반전 구현됨 (intersect_cyl_new.c:61-63)
- 구 교차(intersect_object.c)에서도 두 번째 root 선택으로 내부 교차 처리
- body에만 누락된 패턴을 동일하게 적용하면 일관성 확보

**Alternatives considered**:
- 별도 inside_hit 플래그: 추가 상태 관리 필요, 기존 코드와 불일치
- 무시: FR-008 미충족, eval Section 6 교차 테스트 실패 리스크

## R5: Minimum Resize Threshold

**Decision**: 0.1 (반지름/half_height 기준)

**Rationale**:
- 이동 step이 1.0이므로 resize step도 1.0이 자연스러움
- 0.1은 10회 감소로 도달하는 하한 → 실수로 0이 되는 것 방지
- 지름 기준 0.2, 높이 기준 0.2 → 화면에서 여전히 렌더링 가능한 최소 크기

**Alternatives considered**:
- 0.01: 너무 작아서 화면에서 사실상 보이지 않음
- 1.0: 제한이 너무 높아 자유도 부족
