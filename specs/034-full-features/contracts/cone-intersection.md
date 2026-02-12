# Contract: Cone Intersection

## 함수 시그니처

```c
/* 원뿔 body(측면) 교차 — static, cylinder 패턴과 동일 (color 미포함) */
static int  intersect_cone_body(t_ray *ray, t_cone_data *cone, t_hit *hit);

/* 원뿔 cap(밑면) 교차 — static, cylinder 패턴과 동일 */
static int  intersect_cone_cap(t_ray *ray, t_cone_data *cone, t_hit *hit);

/* 원뿔 통합 교차 (body + cap) — public, color는 여기서 적용 */
int  intersect_cone_new(t_ray *ray, t_object *obj, t_hit *hit);
```

> cylinder 패턴과 일관: body/cap은 static으로 geometry만 처리, 통합 함수에서 color + hit->obj 설정.

## 전제조건 (Preconditions)

- `ray`는 유효한 정규화된 방향 벡터
- `cone->axis`는 정규화된 단위 벡터
- `cone->radius > 0`, `cone->half_height > 0`
- `hit->distance`는 호출 전 INFINITY 또는 이전 최소 거리

## 후제조건 (Postconditions)

- 교차 시: `hit->hit = true`, `hit->distance` 갱신, `hit->point` 설정, `hit->normal` 정규화, `hit->color` 설정
- 미교차 시: `hit` 변경 없음, 반환값 0
- 내부 교차: 음의 근은 건너뛰고 양의 근 사용, 법선 뒤집기

## 원뿔 Body 이차방정식

center `C`, axis `v`, half_height `h`, base_radius `r`
- apex = `C + v * h`
- 기울기 `k = r / (2h)`
- `D = ray.origin - apex`
- `a = dot(d,d) - (1+k²) * dot(d,v)²`
- `b = 2 * (dot(D,d) - (1+k²) * dot(D,v) * dot(d,v))`
- `c = dot(D,D) - (1+k²) * dot(D,v)²`

높이 범위 검증: `m = dot(D,v) + t*dot(d,v)`, `-2h ≤ m ≤ 0` (apex 기준)

## 법선 계산

### Body 법선 (정확한 수식)

교차점 `P`, apex `A = C + v*h`, 투영 `m = dot(P - A, v)`:

```
n = normalize((P - A) - m * (1 + k²) * v)
```

여기서 `k = r / (2h)`. 원기둥 법선 `normalize(P - axis_pt)`와 달리 gradient 보정항 `(1 + k²)`가 필수. 이 보정 없이 원기둥 법선을 그대로 사용하면 조명이 틀어짐.

내부 교차 시 법선 뒤집기: `n = -n`

### Cap 법선

- Base cap: `-axis` 방향 (밑면 바깥쪽)
- 내부 교차 시: `axis` 방향
