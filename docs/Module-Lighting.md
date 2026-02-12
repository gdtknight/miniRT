# Module: Lighting

Phong 조명 모델과 그림자 시스템을 담당하는 모듈입니다.

---

## 소스 파일

| 파일 | 역할 |
|------|------|
| `lighting.c` | `apply_lighting` — 다중 광원 Phong 모델 적용 + 체커보드/범프맵 hook |
| `lighting_utils.c` | `clamp_color`, `fast_pow32` — 조명 유틸리티 |
| `shadow_calc.c` | `calculate_shadow_factor` — 소프트 섀도우 팩터 계산 |
| `shadow_test.c` | `is_in_shadow` — 단일 shadow ray 차폐 테스트 |
| `shadow_config.c` | `init_shadow_config` — 그림자 설정 초기화 + offset LUT |

---

## Phong 조명 모델

`apply_lighting(scene, hit)` 실행 흐름:

```
1. 체커보드 hook: has_checker → checkerboard_color()로 hit->color 교체
2. 범프맵 hook: bump_map → bump_perturb_normal()로 hit->normal 교란
3. view_dir = normalize(camera.position - hit.point)
4. for each light in lights[0..light_count-1]:
   a. light_dir = normalize(light.position - hit.point)
   b. diffuse = max(0, dot(normal, light_dir))
   c. shadow_factor = calculate_shadow_factor(...)
      (내부에서 samples / light_count 로 광원 수 비례 조절)
   d. specular = pow(max(0, dot(reflect_dir, view_dir)), 32) × 0.5
   e. acc += (diffuse + specular) × (1 - shadow_factor) × brightness × light_color
5. final_color = clamp(object.color × (ambient + acc), 0, 255)
```

### Specular 최적화

`pow(x, 32.0)` 대신 `fast_pow32(x)` 사용:
```
x² → x⁴ → x⁸ → x¹⁶ → x³²  (곱셈 5회, pow 호출 제거)
```

---

## 소프트 섀도우

### 알고리즘

1. Hit point에서 광원 방향으로 여러 shadow ray 발사
2. 각 ray에 offset을 적용하여 광원 주변을 샘플링
3. 차폐된 ray 비율 = shadow factor (0.0: 완전 조명, 1.0: 완전 그림자)

```
calculate_shadow_factor(scene, query, light_pos, config)
 ├── calculate_shadow_bias(normal, light_dir, base_bias)
 └── for i in [0, samples):
      ├── offset_lut != NULL → offset = offset_lut[i] × radius
      │    └── offset_lut == NULL → generate_shadow_sample_offset() 폴백
      ├── sample_pos = light_pos + offset
      └── is_in_shadow(scene, biased_point, sample_pos, bias)
           ├── BVH any-hit (objects > threshold)
           │    └── bvh_intersect_any()
           └── brute-force (objects <= threshold)
                └── 모든 오브젝트 순회
```

### Shadow Offset LUT

`init_shadow_offset_lut()`에서 grid-based stratified 오프셋을 1회 사전 계산:

```c
grid_size = (int)sqrt(samples);
for (i = 0; i < samples; i++)
{
    angle = 2π × (i % grid_size) / grid_size;
    r = (i / grid_size + 0.5) / grid_size;
    lut[i] = (t_vec3){r × cos(angle), r × sin(angle), 0};
}
```

- Grid 기반 층화 샘플링으로 균일한 분포 보장
- 매 shadow sample에서 삼각함수 호출 제거
- `scene_destroy()` 시 LUT 메모리 해제
- `set_shadow_samples()` 호출 시 LUT 재생성

---

## Adaptive Shadow Bias

```c
double calculate_shadow_bias(t_vec3 normal, t_vec3 light_dir, double base_bias)
```

표면 각도에 따라 bias를 조절하여 shadow acne(자기 교차 아티팩트)를 방지합니다:
- 법선과 광원 방향의 각도가 클수록(grazing angle) bias 증가
- 계산식: `bias = base_bias × (1.0 + angle_factor × 2.0)`
- `angle_factor = 1.0 - |dot(normal, light_dir)|`
- 호출 시 `base_bias = 0.001` 고정

---

## Shadow BVH 분기

`is_in_shadow()`에서 BVH 활성화 여부와 오브젝트 수에 따라 분기:

```
is_in_shadow(scene, point, light_pos, bias)
 ├── BVH 활성화 && objects > SHADOW_BVH_THRESHOLD (5)
 │    ├── bvh_intersect_any() → hit → return 1 (early exit)
 │    └── check_plane_shadow() → plane 별도 순회
 └── 그 외
      └── check_object_shadow() → brute-force 전체 순회
```

| 조건 | 경로 |
|------|------|
| BVH 활성화 + objects > 5 | `bvh_intersect_any()` + `check_plane_shadow()` |
| 그 외 | `check_object_shadow()` brute-force 순회 |

- Any-hit는 첫 번째 차폐 발견 시 즉시 종료하므로 일반 BVH 순회보다 빠름
- BVH 경로에서는 plane이 BVH에 포함되지 않으므로 `check_plane_shadow()`로 별도 테스트
