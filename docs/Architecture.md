# Architecture

miniRT의 전체 아키텍처와 렌더링 파이프라인을 설명합니다.

---

## 전체 실행 흐름

```
main()
 ├── parse_args()           # 인자 파싱 (파일명, --bvh-vis)
 ├── init_and_parse()        # scene_create + parse_scene + scene_build_bvh
 ├── init_render_ctx()       # render_create + load_all_bump_maps
 └── mlx_loop()              # 이벤트 루프 진입
      └── render_loop()      # 매 프레임 콜백
           ├── debounce_update()          # 디바운스 FSM 상태 전이
           ├── rebuild_bvh_if_dirty()     # BVH 재구축 (필요 시)
           ├── [RENDER_DIRTY] execute_render_pass()
           │    ├── metrics_start_frame()
           │    ├── render_scene_to_buffer()
           │    │    ├── create_camera_ray()
           │    │    ├── trace_ray()
           │    │    └── put_pixel_to_buffer()
           │    ├── metrics_end_frame()
           │    └── mlx_put_image_to_window()
           ├── hud_render()
           └── keyguide_render()
```

---

## 모듈 의존성

```
           ┌──────────┐
           │  main.c  │
           └────┬─────┘
      ┌─────────┼─────────┐
      v         v         v
 ┌────────┐ ┌───────┐ ┌────────┐
 │ parser │ │ scene │ │ window │
 └───┬────┘ └───┬───┘ └───┬────┘
     │          │         │
     v          v         v
 ┌────────────────────────────┐
 │         render             │
 │  (camera, trace, metrics)  │
 └──────┬──────────┬──────────┘
        │          │
   ┌────v───┐  ┌───v─────┐
   │ spatial │  │lighting │
   │  (BVH)  │  │(shadow) │
   └────┬────┘  └────┬────┘
        │            │
        v            v
   ┌─────────┐  ┌────────┐
   │   ray   │  │  math  │
   │(intersect)│ │(vector)│
   └─────────┘  └────────┘
```

---

## 렌더링 파이프라인

### 1. 카메라 레이 생성

`create_camera_ray()`가 각 픽셀에 대해 카메라 위치에서 뷰포트를 통과하는 레이를 생성합니다. 카메라 basis 벡터(right, up)는 `t_camera_cache`에 캐싱하여 프레임당 1회만 계산합니다.

### 2. 레이 트레이싱 (`trace_ray`)

```
trace_ray(scene, ray)
 ├── BVH 활성화 → bvh_intersect() + plane 별도 테스트
 │    └── bvh_node_intersect() → aabb_intersect() → intersect_object_new()
 └── BVH 비활성화 → brute-force (모든 오브젝트 순회)
 └── hit 발생 → apply_lighting()
```

- BVH 트리를 ordered traversal (가까운 자식 먼저)로 순회
- Plane은 BVH에서 분리되어 별도 선형 테스트
- `t_max` pruning으로 불필요한 노드 탐색 제거

### 3. 조명 계산 (`apply_lighting`)

각 hit point에 대해 Phong 모델 적용:
1. 체커보드 색상 교체 (활성화 시)
2. 범프맵 법선 교란 (활성화 시)
3. 다중 광원 루프: 각 광원별 독립 계산
   - Shadow factor: 소프트 섀도우 (동적 샘플 조절)
   - Diffuse: Lambert 반사 (`dot(N, L)`)
   - Specular: Phong 반사 (`pow(dot(R, V), 32) × 0.5`)
4. Ambient: 균일 환경광 + 광원 기여 합산
5. 최종 RGB 클램핑 [0, 255]

### 4. 품질 모드

- **Full quality**: 모든 픽셀 개별 렌더링 + pixel timing 측정
- **Low quality**: 인터랙션 중 블록 단위 렌더링 (빠른 프리뷰)

### 5. 디바운스

입력 이벤트 발생 시 디바운스 FSM이 LQ preview를 50ms 간격으로 트리거하고, 150ms 입력 없으면 full quality 렌더링을 수행합니다.

상태 머신: `IDLE → ACTIVE (150ms) → FINAL (FQ render) → COOLDOWN (350ms) → IDLE`

---

## 소스 파일 분포

| 모듈 | 파일 수 | 역할 |
|------|--------|------|
| parser/ | 16 | .rt 파일 파싱 (bonus options, cone 포함) |
| render/ | 7 | 렌더 루프, 카메라, 디바운스 |
| spatial/ | 10 | BVH 구축/순회, AABB |
| lighting/ | 5 | Phong 조명, 그림자 (다중 광원) |
| hud/ | 11 | HUD 오버레이 |
| keyguide/ | 3 | 키가이드 |
| input/ | 5 | 키 입력 디스패치, 카메라/오브젝트/광원 조작 |
| window/ | 6 | 윈도우, 이벤트 |
| ray/ | 4 | 교차 판정 (sphere, plane, cylinder, cone) |
| math/ | 2 | 벡터 연산 |
| texture/ | 4 | 체커보드, 범프맵 |
| bvh_vis/ | 7 | BVH 시각화 |
| scene/ | 3 | 씬 관리 |
| metrics/ | 4 | 프레임 시간, 레이/BVH 카운터 |
| utils/ | 3 | 에러, 포맷 헬퍼, 타이머 |
