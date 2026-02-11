# Module: Render

렌더링 루프, 카메라 레이 생성, 메트릭 수집을 담당하는 모듈입니다.

---

## 소스 파일

| 파일 | 역할 |
|------|------|
| `render.c` | 렌더 루프 (`render_scene_to_buffer`) |
| `trace.c` | `trace_ray` — BVH/brute-force 분기 및 조명 적용 |
| `camera.c` | 카메라 레이 생성 (`create_camera_ray`) |
| `metrics_frame.c` | 프레임 시간 측정 (`metrics_start_frame`, `metrics_end_frame`) |
| `metrics_counters.c` | 레이/교차/BVH 카운터 갱신 |
| `metrics_calc.c` | FPS, BVH 효율, 테스트/레이 평균 계산 |
| `metrics_shadow.c` | shadow 교차 테스트 카운터 |
| `pixel_timing.c` | 픽셀 단위 시간 샘플링 (`pixel_timing_add_sample`) |
| `pixel_timing_print.c` | 픽셀 타이밍 통계 출력 (min/max/avg/p95/p99) |
| `render_debounce.c` | 디바운스 상태 머신 |
| `render_debounce_timer.c` | 디바운스 타이머 유틸리티 |

---

## 렌더링 루프

`render_loop()`은 MiniLibX의 `mlx_loop_hook`에 등록되어 매 프레임 호출됩니다.

```
render_loop(render)
 ├── debounce_update()              // FSM 상태 전이
 ├── rebuild_bvh_if_dirty()         // BVH 재구축 (필요 시)
 ├── [RENDER_DIRTY] execute_render_pass()
 │    ├── metrics_start_frame()
 │    ├── render_scene_to_buffer(scene, render)
 │    │    ├── [full quality] render_pixel() per pixel
 │    │    │    ├── create_camera_ray()
 │    │    │    ├── get_time_ns()         // 시작 시간
 │    │    │    ├── trace_ray()
 │    │    │    ├── get_time_ns()         // 종료 시간
 │    │    │    ├── pixel_timing_add_sample()
 │    │    │    └── put_pixel_to_buffer()
 │    │    └── [low quality] render_low_quality()
 │    │         ├── create_camera_ray()
 │    │         ├── trace_ray()
 │    │         └── draw_pixel_block()   // 블록 단위 채우기
 │    ├── metrics_end_frame()
 │    └── mlx_put_image_to_window()
 ├── [rendered] keyguide.dirty = 1
 └── [HUD visible && (dirty || rendered)]
      ├── hud_render()
      └── keyguide_render()
```

---

## trace_ray 흐름

```c
t_color trace_ray(t_scene *scene, t_ray *ray)
```

1. `metrics_add_ray()` — 레이 카운터 증가
2. **교차 검사**:
   - BVH 활성화: `bvh_intersect()` + plane 별도 루프
   - BVH 비활성화: 전체 오브젝트 순회 (brute-force)
3. Hit 발생 시 `apply_lighting()` → 색상 반환
4. Miss 시 검정색 `(0, 0, 0)` 반환

---

## 카메라 레이 생성

`create_camera_ray(camera, x, y)`:

1. `t_camera_cache`가 유효하면 캐싱된 basis 벡터 사용
2. 무효하면 `camera->direction`에서 right/up 벡터 계산 후 캐시
3. 뷰포트 좌표 `(x, y)` → 정규화 → basis 벡터 조합 → 레이 방향
4. `inv_dir` (1/direction) 사전 계산 (AABB 교차 최적화)

---

## 품질 모드

| 모드 | 조건 | 설명 |
|------|------|------|
| Full quality | `RENDER_LOW_QUALITY` 플래그 없음 | 픽셀 개별 렌더링, timing 측정 |
| Low quality | `RENDER_LOW_QUALITY` 플래그 있음 | 블록 단위 렌더링, timing 미측정 |

인터랙션(키 입력) 시 low quality로 전환하여 빠른 피드백을 제공하고, 디바운스 타이머 후 full quality로 재렌더링합니다.

---

## 디바운스 메커니즘

4-상태 FSM:

```
IDLE ─(입력)─→ ACTIVE ─(150ms 만료)─→ FINAL ─(FQ 렌더 완료)─→ COOLDOWN ─(350ms)─→ IDLE
                 ↑ ↺                     │                                          │
                 │ (재입력: 타이머 리셋)   │                                          │
                 │                       └──────────(재입력 시)──────────────────────┘
                 └──────────────────────────(재입력 시)──────────────────────────────┘
```

- **IDLE**: 대기 상태
- **ACTIVE**: 디바운스 타이머 시작 (150ms). 추가 입력 시 타이머 리셋하여 대기 연장
- **FINAL**: FQ 렌더 대기 상태. ACTIVE→FINAL 전이 시 `RENDER_LOW_QUALITY` 해제 + `RENDER_DIRTY` 설정
- **COOLDOWN**: FQ 렌더 완료 후 350ms 쿨다운. 재입력 시 ACTIVE로 복귀

LQ preview: `debounce_on_input()` 호출 시 상태와 무관하게 `debounce_check_preview_throttle()`로 50ms 간격 제한하여 LQ 프리뷰 트리거

---

## 메트릭 수집

`t_metrics` 구조체에 수집하는 지표:

| 카테고리 | 지표 | 설명 |
|---------|------|------|
| Frame | `render_time_us` | 프레임 렌더링 시간 (μs) |
| | `fps` | 초당 프레임 수 |
| Ray | `rays_traced` | 추적된 레이 수 |
| | `intersect_tests` | primary 교차 테스트 수 |
| | `shadow_intersect_tests` | shadow 교차 테스트 수 |
| BVH | `nodes_visited` | 방문한 BVH 노드 수 |
| | `tests_skipped` | AABB 스킵된 테스트 수 |

메트릭은 HUD에 실시간 표시됩니다. `RENDER_ENABLE_METRICS_PRINT` 플래그 설정 시 콘솔에도 출력됩니다.
