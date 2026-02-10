# Data Model: Evaluation Compliance Fixes

## 변경되는 데이터 구조

### 1. Render State Flags (window.h)

기존 플래그에 추가:

| Flag | Value | Description |
|------|-------|-------------|
| RENDER_DIRTY | 0x01 | 기존: 재렌더 필요 |
| RENDER_RENDERING | 0x02 | 기존: 렌더링 진행 중 |
| RENDER_LOW_QUALITY | 0x04 | 기존: 저품질 프리뷰 |
| RENDER_SHIFT_HELD | 0x08 | 기존: Shift 키 상태 |
| RENDER_SHOW_INFO | 0x10 | 기존: 정보 오버레이 |
| **RENDER_BVH_DIRTY** | **0x20** | **신규: BVH 재빌드 필요** |

### 2. Key Constants (window_internal.h)

신규 키 상수 추가:

| Constant | macOS | Linux | 용도 |
|----------|-------|-------|------|
| KEY_J | 38 | 106 | 지름 증가 |
| KEY_K | 40 | 107 | 지름 감소 |
| KEY_N | 45 | 110 | 높이 증가 |
| KEY_M | 46 | 109 | 높이 감소 |
| KEY_U | 32 | 117 | X축 회전+ |
| KEY_O | 31 | 111 | X축 회전- |
| KEY_Y | 16 | 121 | Y축 회전+ |
| KEY_P | 35 | 112 | Y축 회전- |
| KEY_LEFT | 123 | 65361 | Z축 회전+ |
| KEY_RIGHT | 124 | 65363 | Z축 회전- |

### 3. 기존 구조체 변경 없음

- `t_object`, `t_sphere_data`, `t_cylinder_data`, `t_plane_data`: 변경 없음
- 리사이즈는 기존 `radius`, `radius_sq`, `half_height` 필드를 직접 수정
- 회전은 기존 `axis`(cylinder), `normal`(plane) 벡터를 직접 수정

## 상태 전이

### Resize Flow

```
키 입력 (J/K/N/M)
  → 선택된 오브젝트 타입 확인
  → sphere: radius ± step, radius_sq 재계산
  → cylinder 지름: radius ± step, radius_sq 재계산
  → cylinder 높이: half_height ± step
  → plane: 무시 (FR-011)
  → 최소값 가드 (radius/half_height < 0.1이면 무시)
  → RENDER_BVH_DIRTY 세팅
  → debounce_on_input() 호출
```

### Rotation Flow

```
키 입력 (U/O/Y/P/LEFT/RIGHT)
  → 선택된 오브젝트 타입 확인
  → sphere: 무시 (FR-006)
  → cylinder: axis 벡터에 rotation 적용
  → plane: normal 벡터에 rotation 적용
  → 결과 벡터 정규화
  → 정규화 불가(길이 ≈ 0): 회전 무시, 이전 벡터 유지 (FR-012)
  → RENDER_BVH_DIRTY 세팅
  → debounce_on_input() 호출
```

### BVH Rebuild Flow

```
렌더 루프 (render_loop)
  → RENDER_BVH_DIRTY 확인
  → 참이면: scene_build_bvh(scene), 플래그 클리어
  → 렌더 진행
```

## Rotation 수학

Rodrigues' rotation formula (기존 카메라 회전과 동일):

```
v_rot = v * cos(θ) + (k × v) * sin(θ) + k * (k · v) * (1 - cos(θ))
```

- v: 회전 대상 벡터 (cylinder axis 또는 plane normal)
- k: 회전 축 단위 벡터 (X: {1,0,0}, Y: {0,1,0}, Z: {0,0,1})
- θ: 회전 각도 (±5° = ±π/36 rad)
