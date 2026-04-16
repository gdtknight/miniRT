# 심화: BVH 구축과 순회

> 이 문서는 누구를 위한 것인가
> - miniRT 의 BVH가 어떤 순서로 만들어지고 어떻게 순회되는지 코드 단위로 보고 싶은 사람
> - 평면 분리, any-hit, near/far 순서 같은 설계 결정의 이유를 확인하고 싶은 사람

배경 지식은 [개념: AABB와 공간분할](개념-AABB와-공간분할) 부터 읽고 오세요.

## 1. 자료구조

```c
// includes/spatial/spatial.h:37
typedef struct s_bvh_node
{
    t_aabb              bounds;
    struct s_bvh_node  *left;
    struct s_bvh_node  *right;
    t_object_ref       *objects;    // 리프만 사용, 내부노드는 NULL
    int                 object_count;
    int                 depth;
    int                 split_axis; // 0=x, 1=y, 2=z
}   t_bvh_node;

// includes/spatial/spatial.h:56
typedef struct s_bvh
{
    t_bvh_node      *root;
    int              enabled;
    int              visualize;
    t_plane_refs     plane_refs;    // BVH 외부에 따로 보관하는 평면 인덱스
}   t_bvh;
```

리프 노드는 `object_count > 0` 로 식별하며 실제 객체 인덱스 배열을 들고 있습니다. 내부 노드는 `objects == NULL`, `object_count == 0`, `split_axis` 가 유효.

## 2. 빌드 엔트리 — build_scene_bvh

`src/spatial/bvh/bvh_init.c:133`:

```c
void build_scene_bvh(t_scene *scene)
{
    if (!(scene->flags & SCENE_BVH_ENABLED) || scene->objects.count == 0) return;
    if (!scene->bvh) scene->bvh = bvh_create();
    pc = count_planes(scene);             // 평면 개수
    bc = scene->objects.count - pc;        // 나머지(바운드 가능) 개수
    alloc_bvh_refs(scene, &refs, pc, bc);  // 두 배열 할당
    fill_separated_refs(scene, refs);      // 인덱스 분리
    if (bc > 0)
        bvh_build(scene->bvh, refs, bc, scene);
    free(refs);
}
```

### 평면을 분리하는 이유

평면의 AABB는 세 축으로 무한합니다. 만약 BVH에 넣으면 모든 부모 박스가 무한이 되어 가지치기가 불가능합니다. 그래서 `plane_refs` 로 따로 빼고, 렌더 시에는 BVH 결과 + 평면 리스트 결과를 비교해 가장 가까운 히트를 취합니다 (`src/render/render_trace.c:68`).

## 3. 재귀 빌드 — bvh_build_recursive

`src/spatial/bvh/bvh_build_core.c:68`:

```c
if (count <= 2 || depth > 20)
    return (create_leaf_node(objects, count, scene));
bounds = compute_bounds(objects, count, scene);
axis   = choose_split_axis(bounds);
params.split = calculate_split_position(bounds, axis);
sp.mid = partition_objects(&params);
return (create_split_node(&sp));
```

### 리프 조건

- 객체 2개 이하 → 분할 의미 없음
- depth 20 초과 → 무한 재귀 방지

### 축 선택

`choose_split_axis()` (`bvh_build_split.c:23`) 은 AABB의 세 변 길이를 비교해 **가장 긴 축**을 고릅니다. 기하학적으로 그 방향이 물체가 제일 넓게 퍼져있다는 신호라 균형 잡힌 분할 가능성이 큽니다.

### 분할 위치

`calculate_split_position()` 는 단순히 AABB 중간값 `(min + max) / 2` 를 반환 (`bvh_build_split.c:46`). Surface Area Heuristic (SAH) 같은 정교한 기법은 사용하지 않는 대신 빌드가 빠릅니다.

### 파티션 — `partition_objects`

`bvh_build_partition.c:59`. 객체 배열을 in-place로 재배치해, 앞쪽엔 split 보다 작은 중심들을, 뒤쪽엔 큰 중심들을 모읍니다. 전통적인 Lomuto partition 변형:

```c
left_count = 0;
for (i = 0..count):
    center = get_object_center(objects[i], scene);
    if (center[axis] < split):
        swap(objects[left_count], objects[i]);
        left_count++;
return (left_count);
```

모든 객체가 같은 축값을 공유해 한쪽이 비어버리면 `count/2` 를 반환하는 fallback으로 노드가 항상 자식을 갖도록 보장 (`bvh_build_partition.c:82`).

### split 노드 생성

`create_split_node()` (`bvh_build_split.c:62`) 가 좌/우 자식을 재귀 빌드한 뒤, 둘 중 하나라도 실패하면 전체 서브트리를 해제하고 NULL을 반환하는 에러 처리를 포함합니다.

## 4. 바운드 계산

### 객체 한 개

`src/spatial/aabb/aabb_bounds.c:86` 의 `aabb_for_object()` 는 타입별로 분기:

- 구: `center ± (r, r, r)`
- 실린더/원뿔: `center ± r + fabs(axis * half_height)` 를 각 축별로
- 평면: `(-1e6, -1e6, -1e6) ~ (1e6, 1e6, 1e6)` (실제로는 plane_refs로 분리되어 이 fallback은 거의 쓰이지 않음)

### 집합 바운드

`compute_bounds()` (`bvh_build_partition.c:26`) 는 첫 객체 바운드로 초기화 후 차례로 `aabb_merge()` 로 합칩니다.

## 5. 순회 — bvh_intersect (closest-hit)

카메라 광선의 경우 "가장 가까운 교차점"을 구해야 합니다. `src/spatial/bvh/bvh_traverse.c:127` 의 `bvh_node_intersect()`:

```
1. AABB 테스트 (aabb_intersect). 실패면 skip, metrics.bvh_skip++
2. 리프면 bvh_leaf_intersect 로 모든 객체 교차 테스트
3. 내부면 traverse_children
```

### 근/원 자식 순서

`traverse_children()` 은 광선이 split_axis 방향으로 양의 방향이면 left → right, 음이면 right → left 로 방문합니다 (`bvh_traverse.c:98`). 가까운 쪽을 먼저 맞춰서 `hit.distance` 를 줄이면 먼쪽 서브트리의 AABB 테스트가 쉽게 실패해 조기 가지치기가 가능합니다.

### 리프 테스트

`bvh_leaf_intersect()` (`bvh_traverse.c:49`): 각 객체 인덱스를 풀어 `intersect_object()` 호출, 더 가까운 히트만 유지.

## 6. 순회 — bvh_intersect_any (any-hit)

그림자 광선은 "광원까지의 경로 어딘가에 막히기만 하면" 된다. 첫 교차에서 바로 리턴하면 속도가 극적으로 올라갑니다.

`src/spatial/bvh/bvh_any_hit.c:118` 의 `bvh_intersect_any()` → `node_any_hit()`:

```
1. AABB 테스트
2. 리프: leaf_any_hit — 첫 intersect 성공시 1 리턴
3. 내부: get_near_far → near 먼저 재귀, 히트면 즉시 return 1
```

`hit.distance` 를 최소화하려 하지 않고, `max_dist` (광원까지 거리) 보다 짧은 교차가 **단 하나만** 발견되면 됩니다.

셰이더에서 `shadow_is_occluded()` 는 객체 수가 `SHADOW_BVH_THRESHOLD(=5)` 보다 크면 BVH any-hit을 쓰고, 작으면 브루트 포스로 모든 객체를 훑습니다 (`src/lighting/shadow/shadow_occlusion.c:127`).

## 7. 리빌드 시점

객체를 움직이면 바운드도 바뀌므로 BVH를 다시 만들어야 합니다. 입력 핸들러가 `RENDER_BVH_DIRTY` 플래그를 세우면 다음 `render_loop` 에서 `build_scene_bvh()` 가 다시 돌아갑니다. `render_loop.c:27` 참조.

## 관련 문서

- [개념: AABB와 공간분할](개념-AABB와-공간분할)
- [광선 교차 및 공간가속](광선-교차-및-공간가속)
- [심화: 광선-객체 교차 수학](심화-광선-객체-교차수학)
- [성능 최적화](성능-최적화)
