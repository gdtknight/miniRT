# Feature Specification: Performance Bottleneck Optimization

**Feature Branch**: `031-perf-bottleneck-optimization`
**Created**: 2026-02-06 (Updated: 2026-02-10)
**Status**: Round 1 + Round 2 구현 완료
**Input**: Apply core performance bottleneck optimizations from optimization-plan.md + benchmark-driven discoveries

### 범위 요약

| Round | 항목 | 상태 |
|-------|------|------|
| Round 1 | P5(inv_dir), P2(camera cache), P6(child ordering), P1(shadow BVH) | ✅ 구현 완료 |
| Round 2 | PA(Plane BVH 분리), PB(Shadow threshold 하향) | ✅ 구현 완료 |

## Clarifications

### Session 2026-02-06

- Q: Shadow BVH Metrics 분리 전략 → A: `intersect_tests`를 `primary_intersect_tests`와 `shadow_intersect_tests`로 분리 **(검증용, 본 스펙 범위 외 - 별도 작업으로 분리 권장)**
- Q: Camera Dirty Flag 트리거 조건 → A: position, direction, FOV 변경 시 모두 dirty flag 설정
- Q: inv_dir Division by Zero 처리 → A: 별도 처리 없이 IEEE 754 infinity 자연 생성 허용
- Q: BVH Child Ordering 축 결정 방식 → A: BVH 노드의 분할 축(split axis) 기준으로 레이 방향 부호 확인
- Q: Shadow Any-Hit 반환 방식 → A: boolean만 반환 (차폐 여부만), 광원 거리(max_dist)는 함수 인자로 전달. 시그니처: `bool bvh_intersect_any(t_bvh *bvh, t_ray ray, double max_dist, void *scene)`

---

## 배경 및 목적

`docs/optimization-plan.md`에서 식별된 핵심 성능 병목을 **단계적으로** 해결하여 렌더링 성능을 개선한다.

- **Round 1** (✅ 구현 완료): P1/P2/P5/P6 — 4개 항목 적용
- **Round 2** (⬜ 설계 완료, 미구현): PA/PB — Plane BVH 분리 + Shadow threshold 제거

### Round 1 대상 병목 항목 (✅ 구현 완료)

| ID | 항목 | 적용 전 | 예상 효과 |
|----|------|---------|----------|
| P1 | Shadow ray BVH + any-hit | 선형 순회 | shadow 75% 감소 |
| P2 | Camera basis 프레임 캐싱 | 픽셀당 재계산 | ~259M cycles/frame 절감 |
| P5 | BVH inv_dir precompute | 노드당 재계산 | ~930 cycles/ray 절감 |
| P6 | BVH child ordering | 고정 순서 | 30-50% traversal 감소 |

### 스코프 제외 항목

| ID | 항목 | 제외 사유 |
|----|------|----------|
| P7 | Shadow magnitude/normalize 통합 | ✅ 이미 029-math-optimizations에서 적용 완료 |
| P8 | Sphere sqrt 캐싱 | ✅ 이미 029-math-optimizations에서 적용 완료 |

---

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Shadow Ray 성능 개선 (항목: P1)

레이트레이서 사용자로서, 복잡한 씬에서도 부드러운 프레임레이트를 경험하고 싶다. 현재 그림자 계산이 전체 비용의 90%를 차지하므로 BVH 가속이 필요하다.

**Why this priority**: Shadow ray가 전체 렌더링 비용의 대부분을 차지. 단일 항목 중 가장 큰 성능 개선 기대.

**Independent Test**: 100개 이상의 오브젝트가 있는 씬에서 그림자 렌더링 시간을 측정하여 개선율 확인 가능.

**Acceptance Scenarios**:

1. **Given** S4 시나리오(18개 혼합 오브젝트), **When** 그림자 렌더링 수행, **Then** shadow ray 교차 테스트 횟수가 baseline 대비 50% 이상 감소
2. **Given** BVH가 활성화된 씬, **When** 첫 번째 차폐물 발견, **Then** 즉시 그림자 판정 종료 (any-hit early exit)
3. **Given** 광원 뒤에 오브젝트가 있는 경우, **When** 그림자 테스트 수행, **Then** 해당 오브젝트를 차폐물로 오판하지 않음 (distance 상한 적용)

---

### User Story 2 - Camera Basis 캐싱 (항목: P2)

레이트레이서 사용자로서, 카메라 이동 없이 정적 뷰에서 불필요한 계산이 발생하지 않길 원한다.

**Why this priority**: 프레임당 1,296,000회 동일 계산 제거. 비교적 안전한 수학적 동치 변환.

**Independent Test**: 정적 카메라 상태에서 `init_camera_calc()` 호출 횟수를 측정하여 프레임당 1회로 감소 확인.

**Acceptance Scenarios**:

1. **Given** 카메라 위치/방향 변경 없음, **When** 프레임 렌더링, **Then** camera basis 계산 1회만 수행
2. **Given** 카메라 파라미터 변경, **When** 다음 프레임 렌더링, **Then** camera basis 자동 재계산 (dirty flag)
3. **Given** 1920x1080 해상도, **When** 프레임 렌더링, **Then** 렌더링 결과가 캐싱 적용 전과 픽셀 단위로 동일

---

### User Story 3 - BVH Traversal 최적화 (항목: P5+P6)

레이트레이서 사용자로서, BVH 순회 효율성이 개선되어 복잡한 씬에서도 빠른 렌더링을 원한다.

**Why this priority**: P5(inv_dir)와 P6(child ordering)은 함께 적용 시 시너지 효과. 모든 레이(primary + shadow)에 적용됨.

> **구현 상태**: ✅ Round 1에서 구현 완료. P6 child ordering은 near/far 결정 및 순회 순서 최적화 적용. t_max pruning은 `traverse_children()`에서 `hit->distance` 전파를 통해 부분적으로 동작 — near child hit 시 갱신된 distance가 far child에 전달되나, 초기 t_max가 `INFINITY`에서 시작하므로 첫 hit 이전까지는 pruning 효과 제한적.

**Independent Test**: BVH 노드 방문 횟수와 교차 테스트 횟수를 metrics로 측정하여 개선율 확인.

**Acceptance Scenarios**:

1. **Given** t_ray 구조체, **When** 레이 생성, **Then** inv_dir(1/direction) 필드가 1회 계산되어 저장
2. **Given** BVH 내부 노드 방문, **When** 자식 노드 순회, **Then** 레이 방향에 가까운 자식을 먼저 방문 (near/far ordering)
3. **Given** 가까운 자식에서 hit 발견, **When** t_max 갱신, **Then** 먼 자식의 불필요한 순회 생략
4. **Given** 동일 씬 렌더링, **When** 최적화 전후 비교, **Then** 평균 노드 방문 횟수 **20% 이상** 감소

---

### User Story 4 - Plane BVH 분리 (항목: PA, Round 2) ⬜ 미구현

레이트레이서 사용자로서, plane이 포함된 혼합 씬에서도 BVH 가속 효과를 유지하고 싶다. 현재 plane의 거대 AABB(`[-1e6, 1e6]³`)가 BVH 트리 품질을 오염시켜 skip rate가 급락한다.

**Why this priority**: perf_all_objects(18 objects, 4 planes)에서 BVH skip rate 93.7%→33.0%, frame time 25초. Plane 분리 시 BVH 트리 품질 회복 기대.

**Acceptance Scenarios**:

1. **Given** plane이 포함된 씬, **When** BVH 구축, **Then** plane은 BVH 트리에 포함되지 않고 별도 리스트로 관리
2. **Given** BVH 순회 완료 후, **When** plane intersection 수행, **Then** 모든 plane이 정상적으로 렌더링됨 (누락 없음)
3. **Given** plane이 있는 씬, **When** shadow ray 테스트, **Then** plane에 의한 그림자가 정상 렌더링됨
4. **Given** plane이 없는 씬, **When** 렌더링, **Then** 기존 동작과 동일 (regression 없음)

---

### User Story 5 - Shadow BVH Threshold 제거 (항목: PB, Round 2) ⬜ 미구현

레이트레이서 사용자로서, 오브젝트 수와 무관하게 shadow ray가 항상 BVH 가속을 받길 원한다. 현재 `SHADOW_BVH_THRESHOLD=20` 때문에 18개 오브젝트 씬에서 BVH 미사용.

**Acceptance Scenarios**:

1. **Given** BVH가 존재하는 씬, **When** shadow ray 테스트, **Then** 오브젝트 수와 무관하게 항상 BVH 사용
2. **Given** 3개 오브젝트 씬, **When** shadow ray 테스트, **Then** BVH 사용 시 성능 저하 없음 (perf_timing ≤ 250ms)

---

### Edge Cases

- **Division by zero**: inv_dir 계산 시 direction 성분이 0인 경우 → IEEE 754 infinity 자연 생성 허용 (별도 분기 처리 없음, slab method와 호환)
- **Degenerate BVH**: 노드가 하나뿐인 BVH에서 child ordering 적용 시 → 정상 동작 확인
- **Camera at origin**: 카메라 위치가 (0,0,0)일 때 basis 계산 → 정상 동작 확인
- **Grazing angle rays**: 레이가 surface에 거의 평행할 때 → 수치 안정성 유지
- **Plane 없는 씬**: PA 적용 후 plane_refs.count=0인 경우 → plane 순회 skip, 기존 동작 동일
- **Plane만 있는 씬**: bounded 오브젝트 0개 → BVH 트리 비어있음, plane만 별도 순회

---

## Requirements *(mandatory)*

### Functional Requirements

#### P1: Shadow Ray BVH
- **FR-001**: Shadow ray는 BVH를 통해 가속되어야 한다
- **FR-002**: 첫 번째 차폐물 발견 시 즉시 종료해야 한다 (any-hit). 기존 패턴과 일관된 시그니처로 별도 함수 구현:
  ```c
  bool bvh_intersect_any(t_bvh *bvh, t_ray ray, double max_dist, void *scene);
  ```
- **FR-003**: 광원까지의 거리(max_dist)를 인자로 전달하여 광원 뒤 오브젝트를 무시해야 한다

#### P2: Camera Basis Caching
- **FR-004**: Camera basis (right, up, forward 벡터)는 프레임당 1회만 계산되어야 한다
- **FR-005**: 카메라 파라미터(position, direction, FOV) 변경 시 dirty flag를 통해 재계산을 트리거해야 한다
- **FR-006**: 캐싱된 값은 `t_camera` 구조체 내에 저장되어야 한다

#### P5: BVH inv_dir Precompute
- **FR-007**: `t_ray` 구조체에 `inv_dir` 필드를 추가해야 한다
- **FR-008**: 레이 생성 시점에 `inv_dir = 1.0 / direction`을 1회 계산해야 한다
- **FR-009**: AABB 교차 테스트에서 나눗셈 대신 곱셈을 사용해야 한다

#### P6: BVH Child Ordering
- **FR-010**: BVH 순회 시 노드의 분할 축(split axis)과 레이 방향 부호를 비교하여 near/far 자식을 결정해야 한다
- **FR-011**: near 자식을 먼저 방문하고, hit 시 t_max를 갱신해야 한다
- **FR-012**: far 자식의 AABB가 현재 t_max보다 멀면 순회를 생략해야 한다

### Key Entities

- **t_ray**: 레이 구조체. `inv_dir` 필드 추가 (P5)
- **t_camera**: 카메라 구조체. `cache` 필드 추가 (P2)
- **t_bvh_node**: BVH 노드. child ordering 로직 변경 (P6)

---

## 검증 리스크

### Metrics 정합성 확인 필요

P1(shadow BVH)와 P5/P6(BVH traversal)은 기존 metrics 수치 정의와 정합성을 별도 검증해야 함:

- **intersect_tests 분리 (본 스펙 범위 외)**: `intersect_tests`를 `primary_intersect_tests`와 `shadow_intersect_tests`로 분리하는 것이 측정 정확도에 유리하나, 별도 작업으로 분리 권장
- **nodes_visited**: P5/P6 적용 후 early exit으로 인한 skip 횟수가 정확히 반영되는지 확인
- **검증 방법**: `docs/optimization-metrics-collection-plan.md` 기준 S1~S4 시나리오로 전후 비교

> **Note**: Metrics 분리는 본 스펙의 필수 범위가 아님. 기존 `intersect_tests` 카운터로 전체 교차 테스트 수를 측정하되, shadow BVH 적용 효과는 전체 렌더링 시간 감소로 검증

---

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: S4 시나리오에서 전체 렌더링 시간 **20% 이상** 감소 (P1이 주요 기여)
- **SC-002**: Shadow ray 교차 테스트 횟수 50% 이상 감소 (P1)
- **SC-003**: Camera basis 계산 횟수 프레임당 1회로 감소 (P2)
- **SC-004**: BVH 노드 방문 횟수 20% 이상 감소 (P5 + P6)
- **SC-005**: 렌더링 결과 픽셀 차이 ±1 이내 (수치 정확성 유지)
- **SC-006**: 42 Norm v4.1 완전 준수 (norminette 통과)

> **Note**: SC-001 목표를 50%에서 20%로 하향 조정. Shadow brute-force(P1)가 지배적 병목이나, any-hit 적용 후에도 BVH 순회 자체 비용이 남아있어 전체 50% 감소는 비현실적.

### 검증 방법

- 벤치마크 시나리오 S1~S4 적용
- 5회 실행 중앙값 비교
- 개선율 = `(baseline - optimized) / baseline × 100%`
- 참조: `docs/optimization-metrics-collection-plan.md`

---

## Assumptions

- BVH는 모든 오브젝트를 커버한다 (폴백 경로 불필요)
- IEEE 754 부동소수점 표준을 따른다 (infinity, NaN 처리)
- 싱글 스레드 렌더링 환경 (멀티스레딩은 별도 피처)
- 기존 metrics 인프라(027 브랜치)가 정상 동작한다

---

## Constraints

- 42 Norm v4.1 준수 (함수 25줄, 변수 5개, 인자 4개 제한)
- 외부 라이브러리 추가 금지
- RGB ±1 이내 렌더링 결과 유지
