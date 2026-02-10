# 성능 중심 코드 리뷰 리포트

작성일: 2026-02-08
대상: `src/`, `includes/`, `Makefile`
기준: 성능 최우선, 그 다음 코드 효율성

---

## 1. 요약

현 코드베이스는 기능적으로는 구조가 안정적이나, **성능 관점에서 치명적인 병목이 상시 활성화**되어 있다. 특히 다음 3개는 성능 최우선 기준에서 즉시 해결 대상이다.

1) BVH 트래버스에서 `t_max`가 현재 hit 거리로 갱신되지 않음 → pruning 상실
2) per-pixel timing 측정이 상시 활성화 → 렌더링 오버헤드 과다
3) 프레임마다 metrics 출력 → I/O 병목

이 세 항목을 먼저 해결하지 않으면 다른 최적화 효과는 대부분 상쇄된다.

---

## 2. 고우선 순위 이슈 (성능 영향 큼)

### 2.1 BVH `t_max` 갱신 부재로 pruning 무력화

**현상**
- BVH traversal에서 AABB 테스트가 항상 `t_max = 1000000.0` 고정값을 사용
- 가장 가까운 hit을 찾더라도 far child의 AABB 테스트가 동일 범위로 반복됨

**영향**
- child ordering (P6)의 효과가 크게 감소
- BVH pruning이 실질적으로 동작하지 않아 깊은 트리에서 성능이 급락 가능

**관련 코드**
- `src/spatial/bvh_traverse.c`

**권장 방향**
- `bvh_node_intersect()`의 `t_max`를 `hit->distance`로 초기화/갱신하여 pruning이 가능하도록 해야 함

---

### 2.2 per-pixel timing 측정 상시 활성화

**현상**
- 모든 픽셀에 대해 `get_time_ns()` 2회 + `pixel_timing_add_sample()` 호출
- full-quality 렌더 경로에서 항상 실행됨

**영향**
- 픽셀당 오버헤드가 지속 발생
- 실제 렌더링 성능을 왜곡함 (벤치마크 결과 신뢰도 하락)

**관련 코드**
- `src/render/render.c`
- `src/render/pixel_timing.c`

**권장 방향**
- 측정은 디버그/벤치 모드로 분리
- 기본 렌더 경로에서는 측정 비활성화

---

### 2.3 프레임마다 metrics 출력 (stdout I/O 병목)

**현상**
- `metrics_print_summary()`가 모든 렌더 종료 시 호출

**영향**
- 출력 I/O 비용이 고정적으로 발생
- 대형 씬일수록 오버헤드 비중이 커짐

**관련 코드**
- `src/window/window_loop.c`
- `src/render/metrics_calc.c`

**권장 방향**
- 출력은 벤치/디버그 모드에서만 활성화

---

## 3. 중간 우선순위 이슈 (성능 + 안정성)

### 3.1 Shadow 샘플 수 변경 시 LUT 재생성 누락

**현상**
- `set_shadow_samples()`는 `samples` 값만 변경
- `offset_lut` 재할당/재생성 없음

**영향**
- 샘플 수 증가 시 `offset_lut[index]` 접근이 범위를 벗어날 위험
- 성능 이전에 메모리 안정성 문제

**관련 코드**
- `src/lighting/shadow_config.c`
- `src/lighting/shadow_calc.c`

**권장 방향**
- `set_shadow_samples()`에서 LUT 재생성 또는 `offset_lut` NULL 처리 후 fallback 경로 유도

---

## 4. 저우선 순위 이슈 (효율/정확성)

### 4.1 BVH total_nodes 갱신 미흡

**현상**
- `bvh_build()`에서 `total_nodes`가 1로 고정

**영향**
- 시각화/통계 정확도 저하
- 성능 직접 영향은 제한적

**관련 코드**
- `src/spatial/bvh_build_core.c`

---

### 4.2 Plane AABB가 매우 큰 상수

**현상**
- 평면 AABB는 ±1e6 고정

**영향**
- 평면이 많은 씬에서 BVH pruning 효율 저하
- 성능 손실 가능 (특히 많은 planar geometry)

**관련 코드**
- `src/spatial/bounds.c`

---

## 5. 빌드 성능 관점 (Makefile)

**관측**
- `CFLAGS`에 `-O2`/`-O3` 등 최적화 플래그가 없음

**영향**
- 릴리스 빌드 성능이 불필요하게 낮을 가능성

**관련 코드**
- `Makefile`

**권장 방향**
- 릴리스 전용 프로파일 도입 (예: `CFLAGS_RELEASE = -O2 -DNDEBUG`)

---

## 6. 최우선 개선 순서 제안

1. BVH traversal `t_max` 갱신 (pruning 복원)
2. per-pixel timing 측정 분리
3. metrics 출력 분리
4. shadow LUT 재생성 문제 해결
5. 나머지 정밀 개선

---

## 7. 결론

현재 코드베이스는 기능적 완성도는 높으나, 성능 최우선 기준으로는 **측정/출력 오버헤드와 BVH pruning 미작동이 핵심 병목**이다. 이 두 영역을 해결하지 않으면 다른 미세 최적화는 체감 효과가 거의 없다. 우선순위는 명확하며, 수정 범위를 크게 늘리지 않고도 성능 개선 폭이 클 것으로 판단된다.

