# Module: Lighting

Phong 조명 모델과 그림자 시스템을 담당하는 모듈입니다.

---

## 소스 파일

| 파일 | 역할 |
|------|------|
| `lighting.c` | `apply_lighting` — Phong 모델 적용 (ambient + diffuse + specular) |
| `shadow_calc.c` | `calculate_shadow_factor` — 소프트 섀도우 팩터 계산 |
| `shadow_test.c` | `is_in_shadow` — 단일 shadow ray 차폐 테스트 |
| `shadow_attenuation.c` | `calculate_shadow_attenuation` — 거리 기반 감쇠 |
| `shadow_config.c` | `init_shadow_config` — 그림자 설정 초기화 + offset LUT |

---

## Phong 조명 모델

`apply_lighting(scene, hit)` 실행 흐름:

```
1. ambient = ambient.ratio × ambient.color × object.color
2. light_dir = normalize(light.position - hit.point)
3. shadow_factor = calculate_shadow_factor(...)
4. diffuse = max(0, dot(normal, light_dir)) × brightness × (1 - shadow_factor)
5. reflect_dir = 2 × dot(normal, light_dir) × normal - light_dir
6. specular = pow(max(0, dot(reflect_dir, view_dir)), 32) × brightness × (1 - shadow_factor)
7. final_color = clamp(ambient + diffuse + specular, 0, 255)
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
      ├── offset = offset_lut[i]  // 사전 계산된 LUT 참조
      ├── sample_pos = light_pos + offset × softness
      └── is_in_shadow(scene, biased_point, sample_pos, bias)
           ├── BVH any-hit (objects >= threshold)
           │    └── bvh_intersect_any()
           └── brute-force (objects < threshold)
                └── 모든 오브젝트 순회
```

### Shadow Offset LUT

`init_shadow_offset_lut()`에서 cos/sin/sqrt을 1회 사전 계산:

```c
for (i = 0; i < samples; i++)
{
    angle = 2π × i / samples;
    radius = sqrt((i + 1.0) / samples);
    lut[i] = (t_vec3){cos(angle) × radius, sin(angle) × radius, 0};
}
```

- 매 shadow sample에서 삼각함수 호출 제거
- `scene_destroy()` 시 LUT 메모리 해제
- 샘플 수 변경 시 LUT 재생성

---

## Adaptive Shadow Bias

```c
double calculate_shadow_bias(t_vec3 normal, t_vec3 light_dir, double base_bias)
```

표면 각도에 따라 bias를 조절하여 shadow acne(자기 교차 아티팩트)를 방지합니다:
- 법선과 광원 방향의 각도가 클수록(grazing angle) bias 증가
- 기본 bias: `bias_scale × base_bias`

---

## Shadow BVH 분기

`is_in_shadow()`에서 오브젝트 수에 따라 분기:

| 조건 | 경로 |
|------|------|
| objects >= `SHADOW_BVH_THRESHOLD` (5) | `bvh_intersect_any()` (any-hit, early exit) |
| objects < 5 | brute-force 순회 |

Any-hit는 첫 번째 차폐 발견 시 즉시 종료하므로, 가장 가까운 교차를 찾을 필요가 없어 일반 BVH 순회보다 빠릅니다.
