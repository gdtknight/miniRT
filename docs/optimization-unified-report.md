# miniRT Optimization Unified Report (Current Codebase)

작성일: 2026-02-04  
대상: `docs/` 내 optimization 관련 문서 전체 + 현재 `src/`, `includes/`, `Makefile`  
기준 브랜치: `develop` (026-resource-cleanup 병합 포함)

---

## 1. 목적

기존에 분산된 최적화 문서들을 현재 코드 기준으로 재검증하고,
- 이미 반영된 항목
- 더 이상 유효하지 않은 항목
- 아직 남은 실제 병목
을 단일 흐름으로 정리한다.

---

## 2. 검토한 기존 문서

- `docs/reports/RENDERING_OPTIMIZATION_IMPLEMENTATION.md`
- `docs/reports/OPTIMIZATION_SUMMARY.md`
- `docs/reports/OPTIMIZATION_CHANGES.md`
- `docs/rendering-optimization-report.md`
- `docs/optimization-research-report.md`
- `docs/non-rendering-optimization.md`
- `docs/function_call_hierarchy_optimization_report.md`
- `docs/optimization-metrics-guide.md`

---

## 3. 현재 코드 기준 핵심 결론

1. **렌더링 기본 최적화(버퍼 렌더링 + dirty/debounce + 저품질 프리뷰)는 구현 완료 상태**다.
2. **Primary ray는 BVH를 사용하지만, shadow ray는 아직 brute-force**다.
3. **과거 문서 중 일부는 현재 구조와 불일치**한다(삭제된 모듈/구조체, 오래된 파일 경로, 미사용 기능 전제).
4. 가장 큰 추가 개선 여지는 여전히 **shadow 경로 최적화**와 **핫루프 연산 축소**에 있다.

---

## 4. 현재 아키텍처/최적화 상태 (팩트 기반)

### 4.1 렌더 루프/인터랙션

- 렌더 루프: `src/window/window_loop.c`
  - `debounce_update()` 호출
  - `RENDER_BVH_DIRTY` 시 BVH 재빌드
  - `RENDER_DIRTY`일 때만 렌더 패스 실행
- debounce 상태머신: `src/render/render_debounce.c`, `src/render/render_debounce_timer.c`
  - 입력 시 debounce timer 시작/리셋
  - preview 모드에서 `RENDER_LOW_QUALITY` 사용
  - cancel 요청으로 현재 렌더 중단 가능

### 4.2 이미지 버퍼/픽셀 처리

- 픽셀 write/read는 `mlx_img_put_pixel`, `mlx_img_get_pixel` 사용: `src/window/mlx_pixel.c`
- endian/bpp 코덱 통합: `src/window/mlx_pixel_codec.c`
- HUD 픽셀 접근도 동일 코덱 사용: `src/hud/hud_transparency.c`

### 4.3 BVH

- 빌드: `src/spatial/bvh_build_core.c`, `src/spatial/bvh_build_partition.c`, `src/spatial/bvh_build_split.c`
- 트래버스: `src/spatial/bvh_traverse.c`
- primary ray 경로에서 BVH 사용: `src/render/trace.c`
- 변경 시 rebuild 트리거: window 오브젝트 이동/회전/리사이즈 핸들러에서 `RENDER_BVH_DIRTY` 설정

### 4.4 Shadow

- shadow 경로는 여전히 전체 오브젝트 선형 검사: `src/lighting/shadow_test.c`
- soft shadow 샘플 오프셋은 sample마다 `sqrt/cos/sin` 계산: `src/lighting/shadow_calc.c`

### 4.5 계측

- 프레임 타이밍/FPS 수집: `src/render/metrics_frame.c`
- ray 카운트 증가: `src/render/trace.c` (`metrics_add_ray`)
- BVH/교차 상세 카운터 함수는 정의되어 있으나 실제 핫패스 연결은 제한적
  - `metrics_add_intersect_test`, `metrics_add_bvh_node_visit`, `metrics_add_bvh_box_test`, `metrics_add_bvh_skip`

### 4.6 비렌더링 최적화 관련 현재 상태

- 파서는 1-byte read가 아니라 버퍼 기반 라인리더 사용: `src/parser/parse_line_reader.c`
- dead code 제거/헤더 정리가 진행되어 유지보수 비용 감소(025 반영)
- HUD/Keyguide 배경 버퍼 제거로 UI 리소스 단순화(026 반영)

---

## 5. 과거 문서 대비 정정 포인트

1. `render_state/render_quality/render_progressive` 기반 적응 품질 모듈은 현재 코드에서 제거되었다.
2. `overlay.h`, `format_object_id`, `cleanup_all`, `print_error` 등은 최신 코드 기준 제거/치환되었다.
3. `window.c` 단일 파일 기준 서술은 현재 구조(`window_*` 분할 파일)와 맞지 않는다.
4. HUD 배경 이미지 캐시 전제 제안은 최신 코드(배경 버퍼 제거)와 맞지 않으므로, 현재는 텍스트/동적 오버레이 최적화로 방향을 잡아야 한다.

---

## 6. 현재 기준 병목 후보 (우선순위)

## P1 (효과 큼)

### 6.1 Shadow ray에 BVH 적용

- 현재: `is_in_shadow()` -> `check_object_shadow()` 선형 순회
- 제안:
  - shadow ray도 `bvh_intersect()` 경유
  - 광원 거리(`shadow_hit.distance`)를 상한으로 활용해 early reject
- 기대효과: 오브젝트 수 증가 시 가장 큰 체감 개선

### 6.2 카메라 basis/FOV 파생값 프레임 캐싱

- 현재: `create_camera_ray()`가 픽셀마다 `init_camera_calc()` 호출
- 제안: 프레임 시작 시 한 번 계산 후 픽셀 루프에서 재사용
- 기대효과: 고해상도에서 누적 연산 감소

## P2 (효과 중간)

### 6.3 AABB 트래버스 미세 최적화

- 현재: 축별 `safe_slab_axis()` 3회 호출, 매축에서 분기/역수 계산
- 제안:
  - ray 기반 precompute(`inv_dir` 등) 또는 축별 early-break 강화
  - 필요 시 near/far child ordering 도입
- 기대효과: BVH 노드 방문 많은 장면에서 중간 수준 개선

### 6.4 specular `pow(spec, 32.0)` 경량화

- 현재: `src/lighting/lighting.c`
- 제안: 반복 제곱 체인으로 대체
- 기대효과: 픽셀당 소폭, 전체적으로 누적 개선

### 6.5 soft shadow 오프셋 LUT

- 현재: 샘플마다 삼각함수 호출
- 제안: sample count별 오프셋 테이블 precompute
- 기대효과: soft shadow 샘플이 많을수록 유효

## P3 (관측/유지보수 최적화)

### 6.6 metrics 연결 보강

- 현재 정의된 카운터를 실제 핫패스에 연결
  - BVH node visit / box test / intersect test
- 효과: 최적화 우선순위 판단 정확도 상승

### 6.7 BVH rebuild 정책 보완

- 현재: `RENDER_BVH_DIRTY`면 렌더 루프에서 즉시 rebuild
- 제안: debounce active 구간에서 rebuild 지연(또는 스로틀링)
- 효과: 연속 입력 시 불필요한 rebuild 감소

---

## 7. 측정 지표(현실 적용 버전)

최소 필수 KPI:

1. Frame time avg / p95 / p99
2. FPS avg / min
3. Rays traced per frame
4. (추가 후) BVH nodes visited / box tests / intersect tests
5. Shadow time 비중(샘플링 기반)

권장 비교 시나리오:

- 소형 씬(객체 < 20)
- 중형 씬(객체 50~200)
- 밀집 씬(객체 500+)
- soft shadow samples: 1 / 4 / 8

각 시나리오에서 아래 3가지 모드 비교:

- BVH OFF
- BVH ON (현재)
- BVH ON + 제안 적용 버전

---

## 8. 단계별 실행 로드맵

### Phase A (저위험/고효과)

1. Shadow BVH 적용
2. metrics hot-path 연결
3. specular pow 경량화

### Phase B (중위험)

4. camera basis 프레임 캐시
5. soft shadow LUT
6. BVH rebuild 스로틀링

### Phase C (고위험/실험)

7. BVH child ordering 및 traversal 고도화
8. SAH/Incremental BVH 검토

---

## 9. 결론

현재 코드베이스는 초기 문서들이 제시한 1차 최적화(버퍼 렌더링, dirty/debounce, BVH 기본 적용)를 이미 상당 부분 달성했다.  
지금 시점의 실질 우선순위는 **shadow 경로의 BVH 전환**과 **핫루프 상수비용 절감**이며, 이를 뒷받침하는 **계측 연결 강화**가 필요하다.

이 문서를 기준으로, 기존 optimization 문서는 참고 아카이브로 두고 향후 업데이트는 본 단일 리포트를 기준으로 이어가는 것을 권장한다.
