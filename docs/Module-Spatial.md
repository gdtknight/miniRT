# Module: Spatial (BVH)

BVH(Bounding Volume Hierarchy) 공간 가속 구조를 담당하는 모듈입니다.

---

## 소스 파일

| 파일 | 역할 |
|------|------|
| `bvh_init.c` | `scene_build_bvh` — 씬 BVH 구축 진입점, plane 분리 |
| `bvh_lifecycle.c` | `bvh_create`, `bvh_destroy` — BVH 생명주기 |
| `bvh_build_core.c` | `bvh_build_recursive` — 재귀 트리 구축 |
| `bvh_build_partition.c` | 오브젝트 파티셔닝 |
| `bvh_build_split.c` | split axis 선택 및 split position 계산 |
| `bvh_traverse.c` | `bvh_intersect`, `bvh_node_intersect` — 트리 순회 |
| `bvh_any_hit.c` | `bvh_intersect_any` — shadow용 any-hit 순회 |
| `aabb.c` | AABB 교차 판정 (`aabb_intersect`) |
| `aabb_basic.c` | AABB 생성, 병합, surface area |
| `bounds.c` | 오브젝트별 AABB 계산 (`get_object_bounds`) |

---

## BVH 구축 알고리즘

### 1. Plane 분리

`scene_build_bvh()`에서 plane을 BVH 트리에서 제외합니다:

- Plane의 AABB는 `[-1e6, 1e6]³`으로 극도로 넓어 BVH 트리 품질을 저하
- Plane 인덱스를 `t_plane_refs`에 별도 저장
- Bounded 오브젝트(sphere, cylinder)만 BVH 트리 구축

### 2. Median Split

```
bvh_build_recursive(objects, count, scene, depth)
 ├── count <= leaf threshold → create_leaf_node()
 ├── compute_bounds()         // 전체 오브젝트 AABB 합산
 ├── choose_split_axis()      // 가장 긴 축 선택
 ├── calculate_split_position() // 중앙값 기준 분할점
 ├── partition_objects()      // 분할점 기준 좌/우 분리
 └── create_split_node()
      ├── bvh_build_recursive(left, ...)
      └── bvh_build_recursive(right, ...)
```

- **Split axis**: AABB의 가장 긴 축 (x/y/z)
- **Split position**: 중앙값 (median) 기준
- **Leaf threshold**: 오브젝트 수 ≤ 임계값이면 리프 노드 생성

### 3. AABB 계산

| 오브젝트 | AABB |
|---------|------|
| Sphere | center ± radius |
| Cylinder | center ± (radius, half_height, radius) + 축 보정 |
| Plane | 제외 (별도 관리) |

---

## AABB 교차 판정

```c
int aabb_intersect(t_aabb box, t_ray ray, double *t_min, double *t_max)
```

Slab method 사용:
1. 각 축(x, y, z)별 `t_near`, `t_far` 계산
2. `inv_dir` (사전 계산)로 나눗셈 대체
3. 모든 축의 `[t_near, t_far]` 교집합이 비어있지 않으면 교차

---

## 트리 순회

### Primary ray (`bvh_intersect`)

```
bvh_node_intersect(node, ray, hit, scene)
 ├── aabb_intersect() → miss → return 0
 ├── leaf → intersect_object() per object
 └── internal node
      ├── ordered traversal (ray 방향 기준 가까운 자식 먼저)
      ├── first child intersect
      ├── t_max pruning: second child AABB.tmin > current hit.distance → skip
      └── second child intersect
```

- **Ordered traversal**: `ray.direction[split_axis]`가 양수면 left 먼저, 음수면 right 먼저
- **t_max pruning**: 이미 찾은 hit보다 먼 노드는 탐색하지 않음

### Shadow ray (`bvh_intersect_any`)

```
bvh_intersect_any(bvh, ray, max_dist, scene)
 ├── BVH 순회
 ├── 첫 번째 hit 발견 시 즉시 return 1 (early exit)
 └── plane 별도 루프
```

- Any-hit 모드: 그림자 판정은 "차폐 여부"만 필요하므로 가장 가까운 교차가 아닌 아무 교차에서 즉시 종료
- **Shadow BVH threshold** (`SHADOW_BVH_THRESHOLD = 5`): 오브젝트 5개 이상이면 BVH 사용

---

## Plane 분리 전략 효과

S4 씬 (7sp + 7cy + 3pl) 기준:

| 지표 | plane 포함 BVH | plane 분리 BVH |
|------|---------------|---------------|
| BVH skip rate | 32.9% | **82.0%** |
| Shadow tests | 369.6M | **83.7M** |
| Frame time | 25.6s | **6.1s** |

Plane의 `[-1e6, 1e6]³` AABB가 BVH 트리의 모든 분할을 무력화했기 때문에, 분리만으로 극적인 개선이 발생합니다.

---

## BVH 시각화

`--bvh-vis` 플래그로 BVH 트리 구조를 콘솔에 출력합니다 (`src/bvh_vis/`).

```bash
./miniRT --bvh-vis scenes/valid/valid_scene_comprehensive.rt
```

출력 예시:
```
BVH Tree (depth=0, axis=X)
├── [L] depth=1, axis=Y, bounds=[...]
│   ├── [L] LEAF: sp0, sp1
│   └── [R] LEAF: sp2
└── [R] depth=1, axis=Z
    └── LEAF: cy0, cy1
```
