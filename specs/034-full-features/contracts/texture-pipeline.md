# Contract: Texture Pipeline (Checkerboard + Bump Map)

## 체커보드 함수

```c
/* 교차점의 체커보드 패턴 색상 결정 */
t_color  checkerboard_color(t_object *obj, t_hit *hit);
```

### 전제조건
- `obj->has_checker == 1`
- `hit->point`는 유효한 교차점

### 후제조건
- 반환값: `obj->color` 또는 `obj->checker_color` (패턴에 따라)

### 패턴 계산 (오브젝트별)

**평면**: 평면 법선에서 tangent/bitangent 기저 벡터를 구하고, hit point를 로컬 좌표로 투영하여 `floor(u/scale) + floor(v/scale)` 짝홀 판별. (월드 x/z 직접 사용은 수평 평면에서만 동작하므로 기각)
**구**: UV 기반 — `floor(u*freq) + floor(v*freq)`
**원기둥/원뿔**: 측면은 `floor(theta*freq) + floor(h*freq)`, 캡은 평면과 유사

스케일 상수: `CHECKER_SCALE = 2.0` (고정)

## 범프맵 함수

```c
/* XPM 파일 로드 */
t_bump_map  *bump_map_load(void *mlx, char *filepath);

/* 범프맵 해제 */
void        bump_map_destroy(void *mlx, t_bump_map *bmap);

/* 교차점 법선을 범프맵으로 교란 */
t_vec3      bump_perturb_normal(t_object *obj, t_hit *hit);
```

### 로딩 타이밍 (지연 로딩)

`mlx_init()`은 파싱 이후(`init_render_ctx`)에 호출되므로, 파싱 시점에 XPM 로드 불가.

1. **파싱 시**: `obj->bump_path = ft_strdup(filepath)`, `obj->bump_map = NULL`
2. **mlx_init 후**: `load_all_bump_maps(scene, mlx)` — 모든 오브젝트 순회하며 로드
3. **로드 실패 시**: "Error\n" + 메시지 출력 후 프로그램 종료

```c
/* mlx_init 완료 후 호출 (render_create 또는 이후) */
int  load_all_bump_maps(t_scene *scene, void *mlx);
```

### bump_map_load 전제조건
- `filepath`는 .xpm 확장자
- `mlx`는 초기화 완료된 MiniLibX 포인터 (**파싱 시점 아님**)

### bump_map_load 후제조건
- 성공: 유효한 `t_bump_map*` 반환
- 실패: NULL 반환 (호출자가 에러 처리)

### bump_perturb_normal 전제조건
- `obj->bump_map != NULL`
- `hit->point`, `hit->normal`은 유효

### bump_perturb_normal 후제조건
- 반환값: 교란된 정규화 법선 벡터
- 원래 `hit->normal` 변경하지 않음

### 범프 알고리즘
1. UV 좌표 계산 (오브젝트 타입별)
2. 텍스처 샘플 `h(u,v)` — 그레이스케일 밝기를 높이로
3. 기울기: `du = h(u+1,v) - h(u,v)`, `dv = h(u,v+1) - h(u,v)`
4. tangent/bitangent 계산
5. `perturbed = normalize(normal + strength * (du*tangent + dv*bitangent))`
6. `strength = 1.0` (고정)

## 조명 파이프라인 통합 위치

```
apply_lighting(scene, hit)
  ├─ obj = hit->obj          /* t_hit에 추가된 오브젝트 포인터 (data-model §6) */
  ├─ [NEW] if (obj && obj->has_checker) hit->color = checkerboard_color(obj, hit)
  ├─ [NEW] if (obj && obj->bump_map) hit->normal = bump_perturb_normal(obj, hit)
  ├─ calc light_dir, view_dir
  ├─ [CHANGED] for each light: calc_lighting_factor + accumulate
  └─ blend ambient + lit color + clamp
```
