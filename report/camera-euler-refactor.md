# 카메라 오일러 각도 리팩토링

> 상태: **구현 완료** (2026-02-12)

## 1. 문제 정의

### 변경 전 구현 (Rodrigues + get_camera_right)

v5 코드 리뷰 수정에서 `rotate_dir`(Rodrigues 3번째 항 누락)을 완전한 `rodrigues_rotate`로
교체하고, gimbal lock 방지를 위해 `get_camera_right`(fallback axis)를 추가한 상태였다.

```c
// window_camera.c — 변경 전
static t_vec3  get_camera_right(t_vec3 dir)
{
    t_vec3  up;
    up = (t_vec3){0, 1, 0};
    if (fabs(vec3_dot(dir, up)) > 0.99)
        up = (t_vec3){0, 0, 1};
    return (vec3_normalize(vec3_cross(dir, up)));
}

// handle_camera_pitch에서:
right = get_camera_right(render->scene->camera.direction);
new_dir = rodrigues_rotate(render->scene->camera.direction, right, angle);
```

### 남아 있던 문제 3가지

**문제 1: pitch+yaw 조합 시 부동소수점 누적 오차**

매 회전마다 `rodrigues_rotate` → `vec3_normalize` 반복. direction 벡터가 매번 삼각함수
연산 후 재정규화되므로, 수백 회 회전 후 미세 오차 누적 가능.

**문제 2: 360° pitch 회전 시 불연속점**

pitch가 90° 초과하면 `get_camera_right`의 `cross(direction, world_up)`에서 right 벡터
부호가 반전. 그 후 `rodrigues_rotate(direction, right, angle)`의 right가 뒤집혀
pitch 방향이 역전되는 현상 발생.

**문제 3: camera cache의 right 벡터 반전 (렌더링 좌우 반전)**

`src/render/camera.c`의 `update_camera_cache`가 `cross(direction, world_up)`으로
레이 생성용 right 벡터를 계산. pitch > 90°에서 `cos(pitch)` 부호 반전으로 right가
뒤집혀 **렌더링 이미지가 좌우 반전**되는 치명적 버그:

```
pitch = 85°, yaw = 0:
  direction = (0, 0.996,  0.087)
  cross(dir, (0,1,0)) = (-0.087, 0, 0) → 정규화 → (-1, 0, 0)

pitch = 95°, yaw = 0:
  direction = (0, 0.996, -0.087)
  cross(dir, (0,1,0)) = ( 0.087, 0, 0) → 정규화 → (+1, 0, 0) ← 반전!
```

---

## 2. 해결 방안: 오일러 각도 기반 방향 재구성

### 핵심 아이디어

pitch/yaw 누적 각도를 `t_camera` 구조체에 저장하고, 삼각함수로 direction 벡터와
camera basis를 직접 재구성한다.

```c
camera->pitch += delta;
camera->yaw   += delta;

// direction 재구성
direction.x = cos(pitch) * sin(yaw);
direction.y = sin(pitch);
direction.z = cos(pitch) * cos(yaw);

// right 벡터: yaw에서 직접 계산 (pitch 무관, 반전 없음)
right = (-cos(yaw), 0, sin(yaw));
```

### 장점

- `get_camera_right` + `rodrigues_rotate` 호출 제거 → 간소화
- pitch/yaw 독립 → 조합해도 각도 보존, 누적 오차 없음
- right 벡터가 pitch에 무관 → 360° 전 영역에서 연속적
- `dot(direction, right) = 0` 항상 보장 (수학적 증명 아래)

### right ⊥ direction 증명

```
right = (-cos(y), 0, sin(y))
direction = (cos(p)·sin(y), sin(p), cos(p)·cos(y))

dot = cos(p)·sin(y)·(-cos(y)) + sin(p)·0 + cos(p)·cos(y)·sin(y)
    = -cos(p)·sin(y)·cos(y) + cos(p)·sin(y)·cos(y)
    = 0  ∀ pitch, yaw  ✓
```

---

## 3. 수정 파일 목록

| 파일 | 변경 내용 |
|------|----------|
| `includes/minirt.h` | `t_camera` 구조체에 `pitch`, `yaw` 필드 추가 |
| `src/parser/parse_elements.c` | `init_camera_state` 헬퍼 추출, pitch/yaw 역산 |
| `src/window/window_camera.c` | Euler 기반 전면 수정 (5함수 교체) |
| `src/render/camera.c` | `update_camera_cache` — Euler 기반 right 벡터로 교체 |

---

## 4. 상세 변경 사항

### 4.1 `includes/minirt.h` — 구조체 수정

```c
// 변경 후
typedef struct s_camera
{
    t_vec3          position;
    t_vec3          direction;
    t_vec3          initial_position;
    t_vec3          initial_direction;
    double          fov;
    double          pitch;              // 누적 상하 각도 (rad)
    double          yaw;                // 누적 좌우 각도 (rad)
    t_camera_cache  cache;
}   t_camera;
```

- `pitch`: 상하 각도 (rad). 양수=위, 음수=아래. 360° 제한 없음.
- `yaw`: 좌우 각도 (rad). `atan2(x, z)` 기준.

### 4.2 `src/parser/parse_elements.c` — 초기화

Norm 25줄 제한으로 `parse_camera`에서 초기화 부분을 `init_camera_state`로 추출:

```c
static void  init_camera_state(t_scene *scene)
{
    scene->camera.initial_position = scene->camera.position;
    scene->camera.initial_direction = scene->camera.direction;
    scene->camera.pitch = asin(scene->camera.direction.y);
    scene->camera.yaw = atan2(scene->camera.direction.x,
            scene->camera.direction.z);
    scene->camera.cache.valid = 0;
}

// parse_camera() 끝부분:
    init_camera_state(scene);
    scene_set_flag(scene, SCENE_HAS_CAMERA);
    return (PARSE_OK);
```

**역산 근거**:
```
direction.x = cos(pitch) · sin(yaw)  →  yaw   = atan2(x, z)
direction.y = sin(pitch)              →  pitch = asin(y)
direction.z = cos(pitch) · cos(yaw)
```

- `asin` 입력 범위: direction은 정규화됨 → `|y| ≤ 1` 보장.
- `atan2(0, 0)` = 0 (수직 방향 시) → 안전.

파일 함수 수: 4 → 5 (`init_camera_state` 추가). Norm 최대, 준수.

### 4.3 `src/window/window_camera.c` — 핵심 수정

파일 구성 변경:
```
변경 전:                         변경 후:
1. get_camera_right (static)  →  1. camera_rebuild_dir (static)
2. handle_camera_move          →  2. handle_camera_move (right를 yaw에서 직접)
3. handle_camera_pitch         →  3. handle_camera_pitch (pitch += step)
4. handle_camera_reset         →  4. handle_camera_reset (pitch/yaw 역산)
5. handle_camera_yaw           →  5. handle_camera_yaw (yaw += step)
```

함수 수: 5 → 5. **Norm 준수**.

---

#### 4.3.1 `camera_rebuild_dir` (신규, static)

```c
static void  camera_rebuild_dir(t_camera *cam)
{
    double  cos_p;

    cam->pitch = fmod(cam->pitch, 2.0 * M_PI);
    cam->yaw = fmod(cam->yaw, 2.0 * M_PI);
    cos_p = cos(cam->pitch);
    cam->direction.x = cos_p * sin(cam->yaw);
    cam->direction.y = sin(cam->pitch);
    cam->direction.z = cos_p * cos(cam->yaw);
    cam->direction = vec3_normalize(cam->direction);
    cam->cache.valid = 0;
}
```

- 변수: `cos_p` (1개) → Norm OK
- 라인: 10줄 → Norm OK
- `fmod` 래핑으로 장시간 사용 시 부동소수점 정밀도 보존
- `vec3_normalize`는 안전장치 (cos²+sin² = 1이지만 부동소수점 보정)

---

#### 4.3.2 `handle_camera_pitch` (수정)

```c
void  handle_camera_pitch(t_render *render, int keycode)
{
    double  step;

    step = 5.0 * M_PI / 180.0;
    if (keycode == KEY_C)
        step = -step;
    else if (keycode != KEY_E)
        return ;
    render->scene->camera.pitch += step;
    camera_rebuild_dir(&render->scene->camera);
}
```

- 변수: `step` (1개) → Norm OK
- 라인: 8줄 → Norm OK
- `cross`, `rodrigues_rotate` 호출 완전 제거

---

#### 4.3.3 `handle_camera_yaw` (수정)

```c
void  handle_camera_yaw(t_render *render, int keycode)
{
    double  step;

    step = 5.0 * M_PI / 180.0;
    if (keycode == KEY_1)
        step = -step;
    else if (keycode != KEY_3)
        return ;
    render->scene->camera.yaw += step;
    camera_rebuild_dir(&render->scene->camera);
}
```

- 구조 동일, `rodrigues_rotate` → `yaw += step` + `rebuild`

---

#### 4.3.4 `handle_camera_move` (수정)

right 벡터 계산을 `get_camera_right()` → yaw에서 직접 계산으로 교체:

```c
    // 변경 전:
    right = get_camera_right(render->scene->camera.direction);

    // 변경 후:
    right.x = -cos(render->scene->camera.yaw);
    right.y = 0;
    right.z = sin(render->scene->camera.yaw);
```

**검증** — pitch=0일 때 기존 cross 결과와 동일:
```
yaw=0:   right = (-cos(0), 0, sin(0))     = (-1, 0, 0)
         cross((0,0,1), (0,1,0))           = (-1, 0, 0) ✓

yaw=π/2: right = (-cos(π/2), 0, sin(π/2)) = ( 0, 0, 1)
         cross((1,0,0), (0,1,0))           = ( 0, 0, 1) ✓
```

변수 수: `right`, `move`, `step` (3개) → Norm OK. 라인: 21줄 → Norm OK.

---

#### 4.3.5 `handle_camera_reset` (수정)

```c
void  handle_camera_reset(t_render *render)
{
    t_camera  *cam;

    cam = &render->scene->camera;
    cam->position = cam->initial_position;
    cam->direction = cam->initial_direction;
    cam->pitch = asin(cam->initial_direction.y);
    cam->yaw = atan2(cam->initial_direction.x, cam->initial_direction.z);
    cam->cache.valid = 0;
}
```

- direction 직접 복원 (initial_direction은 원본 그대로 보관)
- pitch/yaw를 역산하여 Euler 상태 동기화

---

### 4.4 `src/render/camera.c` — `update_camera_cache` 수정

> **초기 분석에서 "변경 없음"으로 판단했으나, 검증 과정에서 치명적 버그 발견.**

**변경 전** — `cross(direction, world_up)` 기반:
```c
static void  update_camera_cache(t_camera *camera)
{
    t_vec3  world_up;

    if (camera->cache.valid)
        return ;
    camera->cache.aspect_ratio = ASPECT_RATIO_NUM / ASPECT_RATIO_DEN;
    camera->cache.fov_scale = tan(camera->fov * 0.5 * M_PI / 180.0);
    world_up = (t_vec3){0, 1, 0};
    if (fabs(vec3_dot(camera->direction, world_up)) > 0.999)
        world_up = (t_vec3){0, 0, 1};
    camera->cache.right = vec3_normalize(vec3_cross(camera->direction,
                world_up));
    camera->cache.up = vec3_normalize(vec3_cross(camera->cache.right,
                camera->direction));
    camera->cache.valid = 1;
}
```

**문제**: `cross(direction, world_up)` 결과에 `cos(pitch)` 인수가 포함되어,
pitch > 90°에서 `cos(pitch)` 부호 반전 시 right 벡터가 뒤집힘:

```
direction = (cos(p)·sin(y), sin(p), cos(p)·cos(y))

cross(direction, (0,1,0)):
  x = sin(p)·0 - cos(p)·cos(y)·1 = -cos(p)·cos(y)
  y = 0
  z = cos(p)·sin(y)·1 - sin(p)·0 =  cos(p)·sin(y)

정규화 → right ∝ cos(p)·(-cos(y), 0, sin(y))

cos(p) > 0: right = (-cos(y), 0,  sin(y))   ← 정상
cos(p) < 0: right = ( cos(y), 0, -sin(y))   ← 반전!
```

이로 인해:
- pitch 85° → 95° 전환 시 렌더링 좌우 반전
- up 벡터도 `cross(right, direction)`이므로 동시에 상하 반전
- 최종 결과: pitch 90° 경계에서 화면 180° 뒤집힘

**변경 후** — yaw에서 직접 계산:
```c
static void  update_camera_cache(t_camera *camera)
{
    if (camera->cache.valid)
        return ;
    camera->cache.aspect_ratio = ASPECT_RATIO_NUM / ASPECT_RATIO_DEN;
    camera->cache.fov_scale = tan(camera->fov * 0.5 * M_PI / 180.0);
    camera->cache.right.x = -cos(camera->yaw);
    camera->cache.right.y = 0;
    camera->cache.right.z = sin(camera->yaw);
    camera->cache.up = vec3_normalize(vec3_cross(camera->cache.right,
                camera->direction));
    camera->cache.valid = 1;
}
```

**수정 효과**:
- right = `(-cos(yaw), 0, sin(yaw))` — pitch에 무관, 부호 반전 없음
- `world_up` 변수 및 gimbal lock guard (`fabs(dot) > 0.999`) 완전 제거
- up = `cross(right, direction)` — right가 연속적이므로 up도 연속적
- 변수 0개 → Norm 개선
- 라인 11줄 → Norm OK

**up 벡터 연속성 검증**:
```
pitch=0°:   up = cross((-1,0,0), (0,0,1))   = (0, 1,  0)  ← 위
pitch=90°:  up = cross((-1,0,0), (0,1,0))   = (0, 0, -1)  ← 뒤
pitch=135°: up = cross((-1,0,0), (0,0.707,-0.707)) = (0, -0.707, -0.707)
pitch=180°: up = cross((-1,0,0), (0,0,-1))  = (0, -1, 0)  ← 아래 (뒤집힘)
```

각도 증가에 따라 up 벡터가 연속적으로 회전. 불연속점 없음. ✓

---

## 5. Norm 준수 검증

### window_camera.c

| 함수 | 라인 수 | 변수 수 | 파라미터 수 |
|------|--------|--------|------------|
| camera_rebuild_dir (static) | 10 | 1 (cos_p) | 1 (cam) |
| handle_camera_move | 21 | 3 (right, move, step) | 2 (render, keycode) |
| handle_camera_pitch | 8 | 1 (step) | 2 (render, keycode) |
| handle_camera_reset | 8 | 1 (cam) | 1 (render) |
| handle_camera_yaw | 8 | 1 (step) | 2 (render, keycode) |

- 함수 수: 5개 (**Norm 최대, 준수**)
- 모든 함수 25줄 이하 ✓
- 모든 함수 변수 5개 이하 ✓
- 모든 함수 파라미터 4개 이하 ✓

### parse_elements.c

| 함수 | 라인 수 | 변수 수 | 파라미터 수 |
|------|--------|--------|------------|
| parse_ambient | 18 | 3 | 2 |
| parse_camera_vecs (static) | 15 | 1 | 2 |
| init_camera_state (static) | 7 | 0 | 1 |
| parse_camera | 20 | 3 | 2 |
| parse_light | 18 | 3 | 2 |

- 함수 수: 5개 (**Norm 최대, 준수**)
- 모든 함수 25줄 이하 ✓

### camera.c

| 함수 | 라인 수 | 변수 수 | 파라미터 수 |
|------|--------|--------|------------|
| update_camera_cache (static) | 11 | 0 | 1 |
| create_camera_ray | 12 | 3 | 3 |

- 함수 수: 2개 ✓
- `world_up` 변수 제거로 Norm 개선

---

## 6. 엣지케이스

| 케이스 | direction | pitch/yaw | 동작 |
|--------|-----------|-----------|------|
| 정면 | (0, 0, 1) | pitch=0, yaw=0 | 기본값 ✓ |
| 수직 위 | (0, 1, 0) | pitch=π/2, yaw=0 | direction 정확 ✓ |
| 수직 아래 | (0, -1, 0) | pitch=-π/2, yaw=0 | 동일 ✓ |
| 뒤돌아봄 | (0, 0, -1) | pitch=0, yaw=π | atan2(0,-1)=π ✓ |
| 대각선 | (0.5, 0.5, 0.707) | pitch=π/6, yaw≈0.615 | asin(0.5)=π/6 ✓ |
| 360° yaw | yaw += 5° × 72 | fmod로 래핑 | 연속 회전 ✓ |
| **360° pitch** | E 72회 | fmod로 래핑 | **렌더 반전 없이 연속 회전** ✓ |
| **pitch 90° 경계** | pitch 85° → 95° | cos(p) 부호 전환 | **right/up 연속 ✓** |
| **pitch 180°** | pitch=π | direction=(0,0,-1) | 뒤돌아봄, 화면 뒤집힘 (정상) ✓ |
| 초기 direction (0,1,0) | pitch=π/2 | yaw=atan2(0,0)=0 | 안전 ✓ |

---

## 7. 테스트 계획

### 기능 테스트

1. **기본 pitch**: E 키 → 위로 5°, C 키 → 아래로 5°
2. **기본 yaw**: 3 키 → 우로 5°, 1 키 → 좌로 5°
3. **360° pitch**: E 키 72회 연속 → 한 바퀴 돌아서 원래 방향
4. **360° yaw**: 3 키 72회 연속 → 한 바퀴 돌아서 원래 방향
5. **pitch+yaw 조합**: pitch 30° → yaw 90° → pitch 각도 유지 확인
6. **pitch 90° 통과**: E 키로 90° 넘기기 → 렌더링 좌우 반전 없이 연속 회전
7. **pitch 180° 통과**: E 키로 180° → 뒤돌아봄 상태, 화면 자연스럽게 뒤집힘
8. **카메라 이동**: pitch 45° 상태에서 W/A/D → 올바른 방향 이동
9. **카메라 이동 (수직)**: pitch 90° 상태에서 A/D → 수평 이동 유지
10. **리셋**: S 키 → 초기 위치/방향/각도 복원

### 빌드 검증

```bash
make fclean && make
norminette src/window/window_camera.c src/parser/parse_elements.c \
           src/render/camera.c includes/minirt.h
```

### 씬 파일 테스트

```
# 정면
C  0,0,-15  0,0,1  70

# 대각선
C  0,5,-15  0.3,-0.2,0.9  70

# 수직 부근
C  0,0,0  0,0.99,0.14  70
```
