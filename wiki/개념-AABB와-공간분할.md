# 개념: AABB와 공간분할

> 이 문서는 누구를 위한 것인가
> - "왜 BVH가 필요한가?"를 한 번도 배운 적 없는 사람
> - slab test 한 줄짜리 공식이 어디서 튀어나왔는지 알고 싶은 사람
> - 가속 구조 없이 짠 레이트레이서가 왜 느린지 몸으로 느끼고 싶은 사람

## 1. 순수 O(N) 탐색의 한계

광선 하나가 씬에 있는 모든 물체와 교차 테스트를 하면, 물체가 N개일 때 픽셀당 N번 계산이 필요합니다.

```
해상도 W × H 픽셀,  물체 N개
→ 총 연산: W × H × N
→ 1920×1080, 물체 1000개 = 약 20억 번 교차 테스트
```

그래서 "광선 근처에 있는 물체만" 테스트하는 공간 자료구조가 필요합니다.

## 2. AABB: 가장 싼 충돌 판정 껍질

**AABB (Axis-Aligned Bounding Box)** 는 세 축에 평행한 변으로 물체를 감싼 상자입니다. 두 점 `min`, `max` 만으로 표현 가능:

```c
// includes/spatial/spatial.h:24
typedef struct s_aabb
{
    t_vec3  min;
    t_vec3  max;
}   t_aabb;
```

```
         max
          ●
         ┌┤───┐
        /│   /│
       ┌─│──┐ │
       │ ●--│-┘ 
       │/   │/
       └────┘
       ●
       min
```

복잡한 도형(구, 실린더)을 직접 교차하는 것보다 **훨씬 싸게** "이 영역에 들어올 수 있는가?" 만 먼저 묻는 것이 핵심 아이디어입니다. miniRT의 AABB 계산은 `src/spatial/aabb/aabb_bounds.c` 에 있습니다.

## 3. Slab Test: 왜 저 한 줄로 판정이 되나

AABB는 세 쌍의 "평행 평면(slab)" 교집합과 같습니다.

```
x축 slab:  x = min.x  와  x = max.x  사이
y축 slab:  y = min.y  와  y = max.y  사이
z축 slab:  z = min.z  와  z = max.z  사이

광선이 세 slab을 모두 통과하는 구간의 교집합이 비어있지 않으면 상자에 맞는다.
```

광선 $P(t) = O + t \vec{d}$ 가 x축 slab을 통과하는 시각은:

$$t_0 = \frac{\text{min.x} - O_x}{d_x}, \quad t_1 = \frac{\text{max.x} - O_x}{d_x}$$

`d_x` 가 음수면 `t0`과 `t1`의 의미가 뒤집히므로 swap 합니다. 이것이 `src/spatial/aabb/aabb.c:25` 의 `update_bounds()` 함수가 하는 일.

세 축의 `[t0, t1]` 을 모두 교집합 낸 뒤, `t_max >= t_min && t_max > 0` 이면 광선이 상자를 통과한다는 뜻입니다 (`src/spatial/aabb/aabb.c:88`).

### `inv_dir` 최적화

나눗셈은 느립니다. 광선을 만들 때 미리 $1/d_x$ 를 계산해두면, slab 테스트에서 곱셈으로 해결:

```c
// src/spatial/aabb/aabb.c:52
t0 = (ac->box_min - ac->ray_origin) * ac->inv_dir;
```

`src/render/render_camera.c:66` 에서 광선마다 `inv_dir` 을 초기화합니다. 방향 성분이 0이어도 `1e-15` 를 더해 0 나누기를 피합니다.

## 4. 그래도 N번은 한다 — BVH의 아이디어

각 물체에 AABB만 씌워도 여전히 N개를 모두 테스트해야 합니다. 이걸 **트리**로 묶으면 로그 스케일로 줄어듭니다.

**BVH (Bounding Volume Hierarchy)**: 모든 물체를 포함하는 큰 AABB를 루트로 두고, 그 안을 두 그룹으로 나눠 자식 노드를 만들고, 재귀적으로 반복.

```
          ┌───────────────┐
          │   [큰 AABB]    │ ← 루트 (모든 물체 포함)
          │               │
          │ ┌───┐  ┌─────┐ │
          │ │ L │  │  R  │ │
          │ └───┘  └─────┘ │
          └───────────────┘
             ↓          ↓
        자식 노드     자식 노드
          (왼쪽 절반)   (오른쪽 절반)
```

광선을 테스트할 때:
1. 루트 AABB와 교차?
2. 안 맞으면 서브트리 전체 **스킵** (큰 이득!)
3. 맞으면 좌/우 자식 재귀.
4. 리프 노드에 도달하면 안에 들어있는 몇 개의 실제 물체와만 교차.

평균 복잡도는 **O(log N)**.

## 5. miniRT의 BVH 구체적 선택

- **분할 기준**: 가장 긴 축의 중간(midpoint split). `src/spatial/bvh/bvh_build_split.c:23` 의 `choose_split_axis()` 가 bounds에서 가장 긴 축을 고르고, `calculate_split_position()` 이 중간값을 반환.
- **리프 기준**: 물체가 2개 이하이거나 depth가 20 초과일 때 더 쪼개지 않음 (`src/spatial/bvh/bvh_build_core.c:76`).
- **평면 제외**: 평면은 AABB가 무한대라 BVH에 넣으면 모든 박스가 커져서 의미가 없어집니다. 그래서 **BVH 외부**에 따로 리스트로 보관합니다 (`t_bvh.plane_refs`, `src/spatial/bvh/bvh_init.c:72`).
- **any-hit 모드**: 그림자 광선처럼 "뭔가에 맞기만 하면 된다"를 위한 전용 함수 `bvh_intersect_any()` 가 있습니다. 첫 교차에서 즉시 리턴해서 시간을 아낍니다 (`src/spatial/bvh/bvh_any_hit.c:118`).

## 6. 시각화

`--bvh-vis` 플래그로 실행하면 콘솔에 트리가 출력됩니다.

```
$ ./miniRT scene.rt --bvh-vis
├── internal [depth=0] [...bounds...]
│   ├── internal [depth=1] ...
│   │   ├── leaf [depth=2] (sp-1, sp-2)
│   │   └── leaf ...
│   └── ...
└── ...
```

구현은 `src/spatial/debug/bvhd_print.c`. 자세한 흐름은 [심화: BVH 구축과 순회](심화-BVH-구축과-순회) 참고.

## 관련 문서

- [개념: 벡터와 기하](개념-벡터와-기하)
- [심화: BVH 구축과 순회](심화-BVH-구축과-순회)
- [광선 교차 및 공간가속](광선-교차-및-공간가속)
- [성능 최적화](성능-최적화)
