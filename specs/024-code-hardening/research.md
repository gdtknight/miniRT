# Research: Code Hardening

**Feature**: 024-code-hardening
**Date**: 2026-01-31

## Overview

본 기능은 코드 리뷰에서 식별된 9개 결함에 대한 수정으로, 새로운 기술 조사가 필요하지 않습니다. 각 수정 사항의 기술적 배경과 구현 방향을 정리합니다.

---

## R1: BVH 교차 검사 - 미초기화 변수

### 문제
`bvh_leaf_intersect()`에서 `temp_hit.distance`가 초기화되지 않은 상태로 `intersect_ref()`에 전달됩니다. 내부 교차 함수가 `hit->distance`를 상한값으로 사용하므로 garbage 값에 의존한 판정이 발생합니다.

### 결정
`temp_hit.distance = hit->distance` 초기화

### 근거
- 호출자가 설정한 클리핑 거리를 유지
- INFINITY 고정값은 그림자 레이 등에서 의도치 않은 동작 유발 가능

### 대안 검토
| 대안 | 장점 | 단점 | 결론 |
|------|------|------|------|
| temp_hit.distance = INFINITY | 간단 | 호출자 의도 무시 | 기각 |
| temp_hit.distance = hit->distance | 호출자 의도 보존 | 없음 | **채택** |

---

## R2: AABB 슬랩 테스트 - Division by Zero

### 문제
`aabb_intersect()`에서 `ray.direction.{x,y,z}`가 0일 때 0으로 나누기 발생. NaN은 모든 비교에서 false를 반환하여 예측 불가 동작.

### 결정
방향 성분이 0에 가까울 때 (|d| < 1e-8) 별도 분기로 처리

### 근거
- 방향이 0이면 ray는 해당 축과 평행
- origin이 slab 외부면 교차 없음, 내부면 무한히 통과
- 1e-8 threshold는 수치 안정성 확보

### 구현 패턴
```c
if (fabs(dir) < 1e-8)
{
    if (origin < box_min || origin > box_max)
    {
        *tmin = 1e30;   // 교차 없음 표시
        *tmax = -1e30;
    }
    return ;  // tmin/tmax 유지 (slab 내부)
}
inv_d = 1.0 / dir;
// ... 기존 로직
```

---

## R3: 카메라 Gimbal Lock

### 문제
`camera->direction`이 world up (0,1,0)과 평행할 때 cross product가 영벡터가 되어 normalize 시 NaN 발생.

### 결정
dot product 임계값 (0.999) 초과 시 대체 up 벡터 (0,0,1) 사용

### 근거
- 0.999 = cos(2.56°), 대부분의 근사 수직 케이스 커버
- (0,0,1)은 수직 방향과 직교하므로 유효한 right 벡터 생성

### 구현 패턴
```c
world_up = (t_vec3){0, 1, 0};
dot = fabs(vec3_dot(camera->direction, world_up));
if (dot > 0.999)
    world_up = (t_vec3){0, 0, 1};
calc->right = vec3_normalize(vec3_cross(camera->direction, world_up));
```

---

## R4: 프로그레시브 타일 - 하드코딩 너비

### 문제
`progressive_next_tile()`에서 tiles_x 계산에 800이 하드코딩되어 실제 윈도우 크기와 불일치.

### 결정
`t_progressive_state`에 width/height 필드 추가

### 근거
- 초기화 시 실제 크기 저장
- 런타임에 일관된 타일 계산

### 구현 패턴
```c
// t_progressive_state 수정
typedef struct s_progressive_state
{
    int     tile_size;
    int     total_tiles;
    int     current_tile;
    int     tiles_completed;
    int     enabled;
    int     width;   // 추가
    int     height;  // 추가
}   t_progressive_state;

// progressive_init()에서 저장
prog->width = width;
prog->height = height;

// progressive_next_tile()에서 사용
tiles_x = (prog->width + prog->tile_size - 1) / prog->tile_size;
```

---

## R5: 그림자 바이어스 - 고정 노멀

### 문제
`calc_shadow_samples()`에서 (0,1,0) 고정 노멀 사용. 실제 표면 각도와 무관한 바이어스 적용.

### 결정
함수 시그니처에 `t_vec3 surface_normal` 파라미터 추가

### 근거
- 기존 `calculate_shadow_bias()` 함수는 이미 노멀을 받음
- 호출 체인에서 실제 hit.normal 전달 필요

### 영향 범위
- `calc_shadow_samples()` 시그니처 변경
- `calculate_shadow_factor()` 시그니처 변경
- 호출 지점 업데이트 (lighting.c 등)

---

## R6: I/O 오류 - EOF 미구분

### 문제
`read()` 반환값 -1(오류)과 0(EOF)을 동일하게 처리. I/O 오류가 성공으로 보고될 수 있음.

### 결정
`t_line_reader`에 `io_error` 플래그 추가

### 근거
- 명확한 오류 상태 구분
- 파싱 루프에서 확인 가능

### 구현 패턴
```c
// t_line_reader 수정
typedef struct s_line_reader
{
    int     fd;
    char    buffer[LINE_READER_BUFFER_SIZE];
    int     buf_pos;
    int     buf_len;
    int     line_num;
    int     line_too_long;
    int     io_error;  // 추가
}   t_line_reader;

// refill_buffer()에서 설정
if (reader->buf_len < 0)
{
    reader->io_error = 1;
    reader->buf_len = 0;
}
```

---

## R7: 정수 파싱 오버플로

### 문제
`parse_int_digits()`에서 `result * 10 + digit` 오버플로 미검사. INT_MAX 초과 시 UB.

### 결정
곱셈 전 오버플로 사전 검사

### 근거
- C 표준에서 signed overflow는 UB
- 사전 검사로 안전한 오류 처리

### 구현 패턴
```c
if (*result > (INT_MAX - digit) / 10)
    return (0);  // 오버플로 감지
*result = *result * 10 + digit;
```

---

## R8: Object List Capacity 오버플로

### 문제
`object_list_grow()`에서 `capacity * 2` 오버플로 미검사.

### 결정
`capacity > INT_MAX / 2` 검사 추가

### 근거
- 실제 발생 가능성 낮으나 방어적 코딩
- 오버플로 시 graceful 실패

### 구현 패턴
```c
if (list->capacity > INT_MAX / 2)
    return (0);
new_capacity = list->capacity * 2;
```

---

## R9: 픽셀 좌표 범위 검사

### 문제
`mlx_img_put_pixel()`/`mlx_img_get_pixel()`에서 x, y 범위 미검사.

### 결정
함수 내 범위 검사 추가

### 근거
- spec의 Assumptions에서 "함수 내 검사 추가"로 결정됨
- 성능 영향 미미

### 구현 패턴
```c
void mlx_img_put_pixel(t_mlx_img *img, int x, int y, int color)
{
    char    *dst;

    if (!img || !img->data)
        return ;
    if (x < 0 || x >= img->width || y < 0 || y >= img->height)
        return ;
    dst = img->data + (y * img->size_line + x * (img->bpp / 8));
    *(unsigned int *)dst = color;
}
```

**참고**: `t_mlx_img` 구조체에 width/height 필드가 없으면 추가 필요.

---

## 결론

모든 NEEDS CLARIFICATION 항목이 해결되었습니다. 새로운 라이브러리나 패턴 도입 없이 기존 코드의 국소적 수정으로 구현 가능합니다.
