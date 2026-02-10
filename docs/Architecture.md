# Architecture

miniRT의 전체 아키텍처와 렌더링 파이프라인을 설명합니다.

---

## 전체 실행 흐름

```
main()
 ├── parse_args()           # 인자 파싱 (파일명, --bvh-vis)
 ├── init_scene()            # 씬 구조체 초기화
 ├── parse_scene()           # .rt 파일 파싱 → 씬 데이터 구축
 ├── scene_build_bvh()       # BVH 트리 구축 (plane 분리 포함)
 ├── init_window()           # MiniLibX 윈도우/이미지/HUD 초기화
 └── mlx_loop()              # 이벤트 루프 진입
      └── render_loop()      # 매 프레임 콜백
           ├── metrics_start_frame()
           ├── render_scene_to_buffer()
           │    ├── create_camera_ray()
           │    ├── trace_ray()
           │    └── put_pixel_to_buffer()
           ├── mlx_put_image_to_window()
           ├── metrics_end_frame()
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
 │    └── bvh_node_intersect() → aabb_intersect() → intersect_object()
 └── BVH 비활성화 → brute-force (모든 오브젝트 순회)
 └── hit 발생 → apply_lighting()
```

- BVH 트리를 ordered traversal (가까운 자식 먼저)로 순회
- Plane은 BVH에서 분리되어 별도 선형 테스트
- `t_max` pruning으로 불필요한 노드 탐색 제거

### 3. 조명 계산 (`apply_lighting`)

각 hit point에 대해 Phong 모델 적용:
1. Ambient: 균일 환경광
2. Shadow factor: 소프트 섀도우 (스토캐스틱 샘플링)
3. Diffuse: Lambert 반사 (`dot(N, L)`)
4. Specular: Phong 반사 (`pow(dot(R, V), 32)`)

### 4. 품질 모드

- **Full quality**: 모든 픽셀 개별 렌더링 + pixel timing 측정
- **Low quality**: 인터랙션 중 블록 단위 렌더링 (빠른 프리뷰)

### 5. 디바운스

입력 이벤트 발생 시 즉시 low quality 렌더링 후, 150ms 디바운스 타이머 경과 후 full quality 렌더링을 수행합니다.

상태 머신: `IDLE → ACTIVE → PREVIEW → FINAL`

---

## 소스 파일 분포

| 모듈 | 파일 수 | 역할 |
|------|--------|------|
| parser/ | 14 | .rt 파일 파싱 |
| render/ | 11 | 렌더 루프, 카메라, 메트릭 |
| spatial/ | 8 | BVH 구축/순회, AABB |
| lighting/ | 5 | Phong 조명, 그림자 |
| hud/ | 14 | HUD 오버레이 |
| window/ | 14 | 윈도우, 키 이벤트 |
| ray/ | 2 | 교차 판정 |
| math/ | 2 | 벡터 연산 |
| bvh_vis/ | 7 | BVH 시각화 |
| scene/ | 3 | 씬 관리 |
| keyguide/ | 4 | 키가이드 |
| utils/ | 3 | 에러, 타이머 |
