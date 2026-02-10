# Research: Performance Bottleneck Optimization

**Feature**: 031-perf-bottleneck-optimization
**Date**: 2026-02-06 (Updated: 2026-02-10)

> **Note**: Round 1 섹션(P1/P2/P5/P6)은 연구 시점의 "before" 상태를 기록합니다. 이 항목들은 모두 **구현 완료**되었으며, 현재 코드에는 아래 설계가 반영되어 있습니다.

## P1: Shadow Ray BVH + Any-Hit ✅ 구현 완료

### 연구 시점 분석 (구현 전)

**Shadow Testing (brute-force)**
- 파일: `src/lighting/shadow_test.c` (lines 30-45)
- `check_object_shadow()`: 모든 오브젝트를 순차적으로 순회
- BVH를 사용하지 않음 → 주요 성능 병목

**기존 BVH Traversal (closest-hit)**
- 파일: `src/spatial/bvh_traverse.c`
- `bvh_intersect()` (line 154): BVH 활성화 시 `bvh_node_intersect()` 호출
- `bvh_node_intersect()` (line 124): 재귀적 AABB 컬링, 가장 가까운 hit 반환

### 설계 결정

**Decision**: `bvh_intersect_any()` 별도 함수 구현

**Rationale**:
- Any-hit은 closest-hit과 다른 종료 조건 (첫 교차 발견 시 즉시 반환)
- 기존 `bvh_intersect()` 수정 시 closest-hit 로직 복잡해짐
- 42 Norm 5함수/파일 제한으로 별도 파일 필요

**Alternatives Considered**:
1. `bvh_intersect()` 플래그 인자 추가 → Norm 인자 4개 제한 위반
2. 기존 함수에 early exit 조건 추가 → closest-hit 로직 손상 위험

### 구현 계획

```c
// includes/spatial.h
bool bvh_intersect_any(t_bvh *bvh, t_ray ray, double max_dist, void *scene);

// src/spatial/bvh_any_hit.c (새 파일)
static bool bvh_node_intersect_any(t_bvh_node *node, t_ray ray, 
                                    double max_dist, void *scene);
static bool bvh_leaf_intersect_any(t_bvh_node *node, t_ray ray,
                                    double max_dist, void *scene);
```

---

## P2: Camera Basis Caching ✅ 구현 완료

### 연구 시점 분석 (구현 전)

**`t_camera` 구조체** (`includes/minirt.h`, lines 68-75):
```c
typedef struct s_camera {
    t_vec3  position;
    t_vec3  direction;
    t_vec3  initial_position;
    t_vec3  initial_direction;
    double  fov;
} t_camera;
```
→ basis 벡터(right, up) 미저장

**`init_camera_calc()`** (`src/render/camera.c`, lines 28-39):
- aspect_ratio, fov_scale, right, up 벡터 계산
- 매 픽셀마다 `create_camera_ray()` 내에서 호출됨

**카메라 수정 위치** (`src/window/window_camera.c`):
- `handle_camera_move()` (lines 27-52): position 변경
- `handle_camera_pitch()` (lines 63-90): direction 변경

### 설계 결정

**Decision**: `t_camera_cache` 구조체 추가 + dirty flag

**Rationale**:
- 카메라 파라미터 변경 시에만 basis 재계산
- 정적 뷰에서 1,296,000회 → 1회로 감소
- dirty flag로 자동 갱신 트리거

**구현 계획**:

```c
// includes/minirt.h
typedef struct s_camera_cache {
    t_vec3  right;
    t_vec3  up;
    double  aspect_ratio;
    double  fov_scale;
    int     valid;          /* 0=invalid, 1=valid */
} t_camera_cache;

typedef struct s_camera {
    t_vec3          position;
    t_vec3          direction;
    t_vec3          initial_position;
    t_vec3          initial_direction;
    double          fov;
    t_camera_cache  cache;  // 추가
} t_camera;
```

**수정 파일**:
- `includes/minirt.h`: 구조체 확장
- `src/render/camera.c`: `update_camera_cache()`, `create_camera_ray()` 수정
- `src/window/window_camera.c`: 카메라 변경 시 `cache.valid = 0`

---

## P5: BVH inv_dir Precompute ✅ 구현 완료

### 연구 시점 분석 (구현 전)

**`t_ray` 구조체** (`includes/ray.h`, lines 22-26):
```c
typedef struct s_ray {
    t_vec3  origin;
    t_vec3  direction;
} t_ray;
```
→ `inv_dir` 필드 없음

**AABB 교차** (`src/spatial/aabb.c`, lines 48-67):
- 매 노드마다 `inv_d = 1.0 / ray_dir` 계산 (나눗셈 3회)
- BVH depth가 깊을수록 반복 계산 증가

### 설계 결정

**Decision**: `t_ray.inv_dir` 필드 추가, 레이 생성 시 1회 계산

**Rationale**:
- 나눗셈(~25 cycles) → 곱셈(~5 cycles)으로 대체
- IEEE 754 infinity 자연 생성으로 division by zero 처리

**구현 계획**:

```c
// includes/ray.h
typedef struct s_ray {
    t_vec3  origin;
    t_vec3  direction;
    t_vec3  inv_dir;  // 추가
} t_ray;
```

**수정 파일**:
- `includes/ray.h`: 구조체 확장
- `src/render/camera.c`: `create_camera_ray()`에서 inv_dir 계산
- `src/lighting/shadow_test.c`: shadow ray 생성 시 inv_dir 계산
- `src/spatial/aabb.c`: 나눗셈 → 곱셈으로 변경

---

## P6: BVH Child Ordering ✅ 구현 완료

### 연구 시점 분석 (구현 전)

**`t_bvh_node` 구조체** (`includes/spatial.h`, lines 37-45):
```c
typedef struct s_bvh_node {
    t_aabb              bounds;
    struct s_bvh_node   *left;
    struct s_bvh_node   *right;
    t_object_ref        *objects;
    int                 object_count;
    int                 depth;
} t_bvh_node;
```
→ `split_axis` 필드 없음

**BVH 빌드** (`src/spatial/bvh_build_split.c`):
- `select_split_axis()`: 가장 긴 축 선택
- 축 정보가 노드에 저장되지 않음

**현재 순회** (`src/spatial/bvh_traverse.c`):
- `traverse_children()` (lines 96-111): 항상 left → right 순서

### 설계 결정

**Decision**: `t_bvh_node.split_axis` 필드 추가 + 레이 방향 기반 near/far 결정

**Rationale**:
- 레이 방향이 양수면 left가 near, 음수면 right가 near
- near child에서 hit 발견 시 t_max 갱신으로 far child pruning

**구현 계획**:

```c
// includes/spatial.h
typedef struct s_bvh_node {
    t_aabb              bounds;
    struct s_bvh_node   *left;
    struct s_bvh_node   *right;
    t_object_ref        *objects;
    int                 object_count;
    int                 depth;
    int                 split_axis;  // 추가: 0=X, 1=Y, 2=Z
} t_bvh_node;
```

**수정 파일**:
- `includes/spatial.h`: 구조체 확장
- `src/spatial/bvh_build_core.c`: 노드 생성 시 split_axis 저장
- `src/spatial/bvh_traverse.c`: `traverse_children()` 수정

---

## 의존성 및 순서

```
P5 (inv_dir) ─┬─> P1 (shadow BVH) ─> shadow_test.c 수정
              │
              └─> P6 (child ordering) ─> bvh_traverse.c 수정

P2 (camera cache) ─> camera.c, window_camera.c 수정 (독립적)
```

**권장 구현 순서**:
1. P5 (inv_dir) - 다른 최적화의 기반
2. P2 (camera cache) - 독립적, 안전
3. P6 (child ordering) - P5 필요
4. P1 (shadow BVH) - P5, P6 완료 후

---

## 리스크 및 대응

| 리스크 | 영향 | 대응 |
|--------|------|------|
| inv_dir infinity 전파 | AABB 결과 오류 | slab method 검증, 테스트 케이스 추가 |
| split_axis 저장 메모리 | 노드당 4바이트 증가 | 무시 가능 (~수백 노드) |
| camera cache 동기화 | 렌더링 오류 | dirty flag 철저히 설정 |
| any-hit distance 오판 | 그림자 아티팩트 | max_dist 검증 테스트 |

---

## Round 2 추가 연구 (2026-02-10) — 설계 단계, 미구현

벤치마크 분석에서 발견된 추가 병목에 대한 연구.

> **상태**: 아래 PA/PB 항목은 연구 및 설계 완료 상태이며, 코드에는 아직 반영되지 않았습니다. 구현은 tasks.md Phase 8-9에서 진행 예정.

## PA: Plane BVH 분리 ⬜ 미구현

### 현재 구현 분석

**Plane AABB 계산** (`src/spatial/bounds.c`, lines 52-61):
```c
static t_aabb bounds_for_plane(t_plane_data *p)
{
    double large = 1000000.0;
    (void)p;  // plane 데이터 완전히 무시
    return (aabb_create(
        (t_vec3){-large, -large, -large},
        (t_vec3){large, large, large}));
}
```

**문제의 연쇄 효과**:

1. `compute_bounds()` (`bvh_build_partition.c`): plane 1개만 포함되면 전체 merged AABB가 `[-1e6, 1e6]³`로 확장
2. `choose_split_axis()` (`bvh_build_split.c`): 3축 extent 모두 `2e6` → 축 선택이 의미 없음 (항상 첫 번째 축 또는 미세 차이)
3. `calculate_split_position()`: midpoint가 ~0.0 부근 → 실제 오브젝트 분포와 무관
4. `partition_objects()`: plane의 center(=point)로 비교 → 모든 plane이 한쪽으로 쏠림 → degenerate fallback `count/2`
5. AABB intersection: `[-1e6, 1e6]³` 박스는 거의 모든 ray와 교차 → skip 불가

**벤치마크 증거**:

| Scene | Planes | BVH Skip Rate | Frame Time |
|-------|--------|---------------|------------|
| perf_timing | 0 | 93.7% | 241ms |
| perf_spheres_20 | 0 | 59.0% | 942ms |
| perf_all_objects | **4** | **33.0%** | **25,192ms** |

### 대안 평가

| 대안 | 장점 | 단점 | 결론 |
|------|------|------|------|
| A: Plane BVH 분리 | 트리 품질 완전 회복, 깔끔한 설계 | 순회 코드 수정 필요 | **채택** |
| B: Thin-slab AABB | BVH 구조 변경 최소화 | 기울어진 plane에서 효과 제한, 여전히 2축 1e6 | 기각 |
| C: Plane-aware split | plane을 split 계산에서 제외 | 구현 복잡, BVH 일관성 깨짐 | 기각 |

**Decision**: 대안 A 채택 — Plane을 BVH에서 완전히 제외하고 별도 순회

**Rationale**:
- Plane은 수학적으로 무한 → AABB 근사 자체가 원리적 한계
- Plane intersection은 dot product 1회로 매우 저렴 → BVH 가속 불필요
- BVH 트리에서 plane을 제거하면 나머지 bounded 오브젝트의 트리 품질이 자동 회복
- Thin-slab 대안은 축 정렬 plane(0,1,0 등)에만 효과적, 기울어진 plane에서는 여전히 거대 AABB

### 구현 요소 조사

**Plane intersection 비용 분석**:
- `intersect_plane()`: normal과 ray direction의 dot product → 거의 zero-cost
- 씬당 plane 수: 일반적으로 1~5개 → 전수 검사해도 무시 가능

**BVH 구조 변경 범위**:
- `t_bvh`에 plane 인덱스 리스트 추가 → 구조체 확장
- `scene_build_bvh()`에서 plane 필터링 → `bvh_init.c` 수정
- `bvh_intersect()`, `bvh_intersect_any()` 호출자에서 plane 별도 테스트 → 순회 코드 수정

**Norm 준수 확인**:
- `bvh_init.c`: 현재 3함수 → plane 필터링 추가해도 5함수 이내 가능
- `bvh_traverse.c`: 현재 5함수 → plane 테스트를 별도 파일 또는 호출자에 배치 필요
- `bvh_any_hit.c`: 현재 4함수 → plane shadow 테스트 추가 시 5함수 도달

---

## PB: Shadow BVH Threshold 조정 ⬜ 미구현

### 현재 구현 분석

**Threshold 정의** (`includes/shadow.h`, line 18):
```c
#define SHADOW_BVH_THRESHOLD 20
```

**사용 위치** (`src/lighting/shadow_test.c`, lines 79-82):
```c
if (scene->bvh && scene->bvh->enabled && scene->bvh->root
    && scene->objects.count > SHADOW_BVH_THRESHOLD)
    return (bvh_intersect_any(scene->bvh, shadow_ray, mag, scene));
return (check_object_shadow(scene, &shadow_ray, &shadow_hit));
```

**설계 의도**: 소규모 씬에서 BVH 오버헤드가 brute-force보다 클 수 있어 threshold 도입.

**문제**: 18개 오브젝트 씬(`perf_all_objects`)에서:
- `objects.count(18) <= SHADOW_BVH_THRESHOLD(20)` → BVH 미사용
- 16 shadow samples × 1,296,000 pixels × 18 objects = 373M tests
- BVH 사용 시 early exit + AABB pruning으로 90%+ 감소 가능

### 대안 평가

| 대안 | 장점 | 단점 | 결론 |
|------|------|------|------|
| A: Threshold 제거 | 단순, 항상 BVH 사용 | 3~5개 오브젝트에서 미세 오버헤드 | **채택** (PA와 결합 시) |
| B: Threshold 하향 (4~6) | 소규모 최적화 유지 | 여전히 edge case 존재 | 차선 |
| C: 동적 threshold | 오브젝트 수에 따라 자동 조정 | 구현 복잡, Norm 제한 | 기각 |

**Decision**: PA 적용 후 threshold 제거 (대안 A)

**Rationale**:
- PA 적용 후 BVH에는 bounded 오브젝트만 포함 → BVH 품질 향상 → 소규모 씬에서도 BVH가 효율적
- Threshold 로직이 PA의 plane 분리와 충돌할 수 있음 (plane 수를 포함/제외 여부 모호)
- BVH가 존재하면 항상 사용하는 것이 단순하고 일관적

### perf_timing (3 오브젝트) 오버헤드 검증

- 3개 sphere만 있는 씬 → BVH 트리 깊이 1~2
- BVH 오버헤드: AABB intersection 2~3회 추가 (~100ns)
- 전체 프레임 대비 무시 가능 (<0.01%)
- **결론**: threshold 제거로 인한 소규모 씬 성능 저하 없음

---

## PA+PB 통합 의존성

```
PA (Plane BVH 분리) ──> PB (Threshold 제거)
                        └─> is_in_shadow() 통합 수정
```

**PA가 PB의 전제조건**:
- PA 없이 threshold만 제거하면 → plane이 BVH를 오염시킨 상태로 BVH 사용 → 오히려 성능 저하 가능
- PA 적용 후 threshold 제거 → BVH 품질 향상 + 항상 BVH 사용 → 최적 성능
