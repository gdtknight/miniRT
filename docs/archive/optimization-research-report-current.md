# miniRT Optimization Research Report (Current Codebase)

작성일: 2026-02-08
대상 코드: 현재 작업 트리 기준
목표: 현 코드 베이스 기준 추가 최적화 여지 식별 및 적용 우선순위 제안

---

## 1. 요약

현재 코드 베이스는 P0, P2, P3, P4, P5, P6, P7, P8, P1 일부(Shadow BVH any-hit)까지 반영된 상태로 보인다. 주요 핫패스는 다음 두 영역으로 집중된다.

- Shadow 경로 (soft shadow 샘플 loop + any-hit BVH traversal)
- Pixel loop 내부의 반복/측정 비용 (per-pixel timing, per-sample setup)

추가 최적화는 크게 3개 축으로 나뉜다.

1) Shadow any-hit traversal 효율 개선
2) 작은 씬에서 BVH 오버헤드 회피
3) 측정/디버그용 오버헤드 분리

---

## 2. 현 코드 기준 관측 포인트

### 2.1 렌더 루프
- 렌더 루프는 프레임마다 BVH dirty 체크 후 렌더 수행.
- per-pixel timing은 full-quality에서 모든 픽셀에 대해 타이밍 샘플을 수집한다.
- HUD 출력은 매 프레임 렌더 후 수행.

관측 파일:
- src/window/window_loop.c
- src/render/render.c

### 2.2 Shadow 경로
- soft shadow는 샘플 수만큼 반복하며 각 샘플에서 shadow ray 생성
- shadow offset LUT가 존재하며, 샘플별 삼각함수 호출은 LUT로 대체됨
- shadow ray는 BVH any-hit 경로를 사용하며, BVH traversal은 AABB + 재귀 호출

관측 파일:
- src/lighting/shadow_calc.c
- src/lighting/shadow_test.c
- src/spatial/bvh_any_hit.c

### 2.3 BVH traversal
- primary ray는 P6 child ordering을 적용
- shadow any-hit는 child ordering을 적용하지 않고 left -> right 고정
- AABB 교차는 inv_dir 기반으로 계산

관측 파일:
- src/spatial/bvh_traverse.c
- src/spatial/bvh_any_hit.c
- src/spatial/aabb.c

### 2.4 카메라 캐시
- create_camera_ray()가 cache 기반으로 동작
- 캐시 무효화는 카메라 이동/피치/리셋/파싱 시점에 수행

관측 파일:
- src/render/camera.c
- src/window/window_camera.c
- src/parser/parse_elements.c

---

## 3. 추가 최적화 후보 (현 코드 기준)

### 3.1 Shadow any-hit에도 child ordering 적용 (우선순위: High)

현상:
- primary BVH는 near/far ordering 적용
- shadow any-hit는 left -> right 고정 순회
- any-hit의 early exit은 자식 순서에 매우 민감

개선안:
- node_any_hit()에서 split_axis와 ray direction을 이용해 near/far 순서로 호출

예상 효과:
- shadow any-hit에서 평균 방문 노드 수 감소
- shadow 테스트가 많은 씬(S4 등)에서 유의미한 개선 가능

대상:
- src/spatial/bvh_any_hit.c

리스크:
- 기능 리스크 낮음 (순서 변경은 any-hit의 논리적 정합성 유지)

---

### 3.2 작은 씬에서는 shadow BVH 비활성화 (우선순위: High)

현상:
- 객체 수가 적은 씬에서는 BVH traversal 오버헤드가 brute-force보다 비쌀 수 있음
- S4(18 objects) 같은 씬에서 BVH 적용이 성능을 악화시킬 가능성 존재

개선안:
- scene->objects.count 기준 threshold 도입
- 예: count <= 20이면 brute-force shadow, 그 이상에서 any-hit

예상 효과:
- 작은 씬 성능 악화 방지
- 큰 씬은 기존 BVH 경로 유지

대상:
- src/lighting/shadow_test.c

리스크:
- threshold 튜닝 필요
- 씬 구성에 따라 최적 threshold가 달라질 수 있음

---

### 3.3 Shadow 전용 AABB 경량화 (우선순위: Medium)

현상:
- aabb_intersect()는 범용 구현이며 t_axis_check 구조체 초기화가 3회 발생
- any-hit에서는 t_max만 필요하고 hit distance 갱신이 목적이 아님

개선안:
- any-hit 전용 AABB 함수 추가
  - tmin/tmax 업데이트만 수행
  - 구조체 없이 로컬 변수로 단순 계산
- bvh_any_hit.c에서만 사용

예상 효과:
- shadow any-hit에서 AABB 비용 축소
- 특히 샘플 수가 많은 씬에서 누적 효과

대상:
- src/spatial/aabb.c
- src/spatial/bvh_any_hit.c

리스크:
- 중복 구현으로 유지보수 비용 증가

---

### 3.4 any-hit traversal의 반복 비용 감소 (우선순위: Medium)

현상:
- node_any_hit()가 매 노드마다 t_min/t_max를 새로 세팅
- 재귀 호출 자체가 비용

개선안:
- t_min/t_max를 함수 인자로 전달하는 방식으로 재귀 깊이 비용 축소
- 또는 반복 스택(명시적 배열) 사용으로 재귀 제거

예상 효과:
- shadow any-hit의 오버헤드 감소

대상:
- src/spatial/bvh_any_hit.c

리스크:
- 구현 복잡도 증가
- norm 제약(함수 길이/변수 개수) 주의

---

### 3.5 per-pixel timing 수집 비용 분리 (우선순위: Medium)

현상:
- full-quality 렌더에서 모든 픽셀에 대해 get_time_ns() 호출 2회
- 성능 측정 목적 외에는 오버헤드로 작동

개선안:
- 디버그/벤치마크 모드에서만 per-pixel timing 활성화
- 기본 렌더에서는 timing 비활성화

예상 효과:
- 픽셀당 오버헤드 감소
- 실제 렌더 성능 개선

대상:
- src/render/render.c
- src/pixel_timing/*

리스크:
- 측정 모드 토글 경로 필요

---

### 3.6 shadow bias 계산/샘플 셋업 축소 (우선순위: Low)

현상:
- calculate_shadow_bias()는 샘플 루프 바깥에서 1회만 계산 (이미 최적)
- 샘플 loop에서는 매번 vec3_add/normalize 수행

개선안:
- sample_light_pos의 계산을 LUT 기반으로 더 단순화
- vec3_add/vec3_multiply 조합 최소화

예상 효과:
- 미세 최적화 수준

대상:
- src/lighting/shadow_calc.c

리스크:
- 복잡도 대비 체감 효과 낮음

---

## 4. 최적화 우선순위 제안

1) Shadow any-hit child ordering 적용
2) 작은 씬에서 shadow BVH 비활성화 (threshold)
3) shadow any-hit 전용 AABB 경량화
4) any-hit traversal 비용 축소(반복 스택/전달형 t_min/t_max)
5) per-pixel timing 측정 분리
6) shadow 샘플 내 미세 최적화

---

## 5. 검증/측정 제안

- 테스트 시나리오: S1~S4
- 각 최적화 단계별 5회 실행 중앙값 기록
- Shadow tests / BVH nodes_visited / skip_rate 같이 기록
- threshold 적용 시 object count별 교차 테스트 변화 비교

---

## 6. 결론

현재 코드 베이스는 핵심 최적화 대부분이 반영된 상태이나, shadow any-hit 경로에서 추가적인 효율 개선 여지가 남아 있다. 특히 child ordering 미적용과 작은 씬에서의 BVH 오버헤드는 즉시 개선 가능한 고효율 후보로 판단된다. 이후 단계는 any-hit AABB 경량화와 측정 오버헤드 분리로 이어지는 것이 합리적이다.

