# miniRT Optimization Plan

작성일: 2026-02-05
기준 브랜치: `develop` (027-baseline-benchmark 병합 후)

통합 대상 문서:
- `optimization-research-report.md` — 이론 분석 (P1~P9)
- `optimization-unified-report.md` — 코드 기준 현황 정리
- `optimization-metrics-setup-report.md` — 계측 인프라 설계
- `optimization-metrics-review-report.md` — 계측 정확성 검토
- `optimization-metrics-collection-plan.md` — 측정 절차 정의
- `benchmark-baseline.md` — 실측 데이터 + 신규 발견

---

## 1. 현재 상태 요약

### 1.1 완료된 기반 작업

| 항목 | 상태 |
|------|------|
| 버퍼 렌더링 + dirty flag | 완료 |
| Debounce + low-quality preview | 완료 |
| Primary ray BVH 적용 | 완료 |
| Dead code / 헤더 정리 (025) | 완료 |
| HUD/Keyguide 리소스 정리 (026) | 완료 |
| Metrics 카운터 hot-path 연결 (027) | 완료 |
| Primary/shadow intersect 분리 (027) | 완료 |
| BVH skip rate 계측 (027) | 완료 |
| Pixel timing 프레임 단위 reset (027) | 완료 |

### 1.2 남은 병목

실측(benchmark-baseline.md)과 이론 분석(research-report.md)에서 확인된 병목:

| 병목 | 근거 | 출처 |
|------|------|------|
| BVH miss 폴백 이중 탐색 | `trace_ray()`에서 BVH miss 시 `check_all_objects()` 재실행. BVH efficiency 0% 원인 | benchmark §4.3 (실측 발견) |
| Shadow ray brute-force | `check_object_shadow()` 선형 순회. 전체 비용의 ~90%+ 추정 | research P1, benchmark §4.4 |
| Shadow offset 삼각함수 반복 | 16 samples × sqrt/cos/sin per pixel | research P4 |
| Camera basis 매 픽셀 재계산 | `init_camera_calc()` 1,296,000회/frame 동일 계산 | research P2 |
| Specular pow(x, 32) | 범용 pow() ~125 cycles vs 반복 제곱 ~25 cycles | research P3 |
| Shadow vec3_magnitude 이중 sqrt | magnitude + normalize에서 sqrt 2회 | research P7 |
| Sphere sqrt 미캐싱 | discriminant sqrt 2회 계산 | research P8 |
| BVH inv_dir 매 노드 재계산 | 1/ray_dir 축별 매 노드 반복 | research P5 |
| BVH 양쪽 자식 항상 탐색 | near/far ordering 미적용 | research P6 |
| BVH rebuild 매 프레임 | debounce active 중에도 rebuild | research P9 |
| 실린더 교차 비용 비선형 | S4(18 mixed)가 S2(20 sphere)보다 14.8x 느림 | benchmark §4.2 |

---

## 2. 최적화 항목 목록

연구 보고서의 P1~P9에 벤치마크에서 발견된 P0를 추가.

| ID | 항목 | 위험도 | 수정 파일 수 | 효과 근거 |
|----|------|--------|------------|-----------|
| **P0** | BVH miss 폴백 제거 | LOW | 1 | 실측: 모든 씬 이중 탐색, BVH eff 0% |
| **P1** | Shadow ray BVH + any-hit | LOW | 2 | 이론: shadow 75% 감소 |
| **P2** | Camera basis 프레임 캐싱 | LOW | 3 | 이론: ~259M cycles/frame (2%) |
| **P3** | Specular pow32 경량화 | LOW | 1 | 이론: ~130M cycles/frame (1%) |
| **P4** | Shadow offset LUT | LOW | 3 | 이론: ~5.2B cycles/frame (40%) |
| **P5** | BVH inv_dir precompute | LOW | 3 | 이론: ~930 cycles/ray |
| **P6** | BVH child ordering | MEDIUM | 2 | 이론: 30-50% traversal 감소 |
| **P7** | Shadow magnitude/normalize 통합 | LOW | 1-2 | 이론: ~800 cycles/pixel |
| **P8** | Sphere sqrt 캐싱 | LOW | 1 | 이론: sqrt 호출 절반 |
| **P9** | BVH rebuild debounce 연동 | LOW | 1 | 이론: 연속 입력 시 ~90% rebuild 감소 |

---

## 3. 실행 순서

이론 추정(research-report)과 실측(benchmark-baseline) 결과를 종합하여 재정렬.
원칙: **위험도 LOW 우선, 효과 큰 순, 독립 변경 우선**.

### Phase 0: 버그성 비효율 제거

즉시 수정 가능하고 측정 정확성에 직접 영향.

| 순서 | ID | 항목 | 변경 내용 | 수정 파일 |
|------|-----|------|-----------|-----------|
| 0-1 | P0 | BVH miss 폴백 제거 | `trace_ray()`에서 BVH 활성 시 `check_all_objects` 폴백 삭제 | `trace.c` |

**검증**: 재측정 후 intersect_tests가 `rays × objects` 이하인지 확인.

### Phase A: 안전한 수학 최적화

수학적 동치 변환. 부동소수점 반올림 차이만 존재 (RGB ±1 허용).

| 순서 | ID | 항목 | 변경 내용 | 수정 파일 |
|------|-----|------|-----------|-----------|
| A-1 | P3 | Specular pow32 | `pow(spec, 32.0)` → 반복 제곱 5회 | `lighting.c` |
| A-2 | P8 | Sphere sqrt 캐싱 | discriminant sqrt 결과 로컬 변수 저장 | `intersect_object.c` |
| A-3 | P7 | Shadow magnitude 통합 | `vec3_magnitude` + `vec3_normalize` → 1회 sqrt 공유 | `shadow_test.c` (+`vector_ops.c` 선택) |

### Phase B: Shadow 경로 최적화

전체 비용의 대부분을 차지하는 shadow 경로 집중 개선.

| 순서 | ID | 항목 | 변경 내용 | 수정 파일 |
|------|-----|------|-----------|-----------|
| B-1 | P4 | Shadow offset LUT | 오프셋 테이블 1회 precompute, 샘플 루프에서 참조 | `shadow.h`, `shadow_config.c`, `shadow_calc.c` |
| B-2 | P1 | Shadow ray BVH | `check_object_shadow()` → `bvh_intersect_any()` 경유 + early exit | `shadow_test.c`, `bvh_traverse.c` |

### Phase C: Camera + BVH traversal 개선

Primary + shadow 양쪽에 효과.

| 순서 | ID | 항목 | 변경 내용 | 수정 파일 |
|------|-----|------|-----------|-----------|
| C-1 | P2 | Camera basis 캐싱 | `t_camera.cache` 필드, 프레임 시작 시 1회 계산 | `minirt.h`, `camera.c`, `window_camera.c` |
| C-2 | P5 | BVH inv_dir precompute | `t_ray.inv_dir` 필드, traversal 진입 시 1회 계산 | `ray.h`, `aabb.c`, `bvh_traverse.c` |
| C-3 | P6 | BVH child ordering | near/far child t_entry 비교, hit 시 t_max 갱신 | `bvh_traverse.c`, `aabb.c` |

### Phase D: 기타

| 순서 | ID | 항목 | 변경 내용 | 수정 파일 |
|------|-----|------|-----------|-----------|
| D-1 | P9 | BVH rebuild debounce | `DEBOUNCE_ACTIVE` 중 rebuild 지연 | `window_loop.c` |

---

## 4. Phase 간 측정 계획

`optimization-metrics-collection-plan.md` 및 `benchmark-baseline.md` Appendix B 템플릿 준수.

| 시점 | 기록 위치 | 내용 |
|------|-----------|------|
| Phase 0 완료 후 | benchmark-results.md Round 1 | P0 폴백 제거 효과. **baseline 재측정 포함** (분리 metrics 최초 실측) |
| Phase A 완료 후 | benchmark-results.md Round 2 | P3+P8+P7 수학 최적화 누적 효과 |
| B-1 (P4) 후 | benchmark-results.md Round 3 | Shadow LUT 단독 효과 |
| B-2 (P1) 후 | benchmark-results.md Round 4 | Shadow BVH 단독 효과 |
| Phase C 완료 후 | benchmark-results.md Round 5 | Camera + BVH traversal 누적 효과 |

측정 조건:
- 시나리오 S1~S4 (collection-plan §4)
- 5회 실행, 중앙값
- 개선율 = `(baseline - optimized) / baseline × 100%`

---

## 5. 42 Norm 제약 대응

| 제약 | 영향받는 항목 | 대응 |
|------|-------------|------|
| 함수 25줄 | P1 (bvh_intersect_any), P6 (child ordering) | 헬퍼 함수 분리 |
| 변수 5개 | P5 (inv_dir 추가) | 구조체 내장 (`t_ray.inv_dir`) |
| 인자 4개 | P1 (metrics 전달) | scene 캐스트 `((t_scene *)scene)->metrics` |
| 파일 5함수 | P1 (bvh_traverse.c 함수 추가) | 파일 분리 또는 기존 함수 대체 |

---

## 6. 위험 관리

| ID | 위험 | 영향 | 대응 |
|----|------|------|------|
| P0 | BVH가 모든 오브젝트를 커버하지 않는 경우 | 일부 오브젝트 렌더 누락 | BVH 빌드가 전체 objects.items를 포함하는지 확인 후 적용 |
| P1 | shadow any-hit BVH 경로에서 distance 상한 미적용 | 광원 뒤 오브젝트를 차폐물로 오판 | `hit.distance = light_distance` 설정 유지 |
| P6 | child ordering으로 traversal 결과 변경 | 최근접 hit 누락 | t_max pruning만 적용, hit 판정 로직 변경 없음 |
| 공통 | 부동소수점 반올림 차이 | RGB ±1 이내 차이 | 적용 전후 동일 씬 픽셀 비교 검증 |

---

## 7. 문서 체계

| 문서 | 역할 | 상태 |
|------|------|------|
| `optimization-research-report.md` | P1~P9 이론 분석 상세 | 아카이브 (참조용) |
| `optimization-unified-report.md` | 과거 문서 통합 + 코드 현황 | 아카이브 (참조용) |
| `optimization-metrics-setup-report.md` | 계측 인프라 설계 | 아카이브 (Phase 1 완료) |
| `optimization-metrics-review-report.md` | 계측 정확성 검토 → 3건 수정 | 아카이브 (3건 반영 완료) |
| `optimization-metrics-collection-plan.md` | 측정 절차/지표 정의 | **활성** (모든 측정에 준수) |
| `benchmark-baseline.md` | Baseline 데이터 + A/B 템플릿 | **활성** (재측정 대기) |
| **`optimization-plan.md`** (본 문서) | 통합 실행 계획 | **활성** (이 문서 기준으로 진행) |
| `benchmark-results.md` | 최적화별 A/B 결과 누적 | 미생성 (Round 1부터 생성) |
