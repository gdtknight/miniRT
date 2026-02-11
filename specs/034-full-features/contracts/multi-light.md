# Contract: Multi-Light System

## 데이터 구조

```c
# define MAX_LIGHTS 16

/* scene 구조체 변경 */
t_light  lights[MAX_LIGHTS];
int      light_count;
int      selected_light;    /* 키보드 조작 대상 인덱스 */
```

## 파서 변경

```c
/* parse_light()에서 중복 체크 제거, 배열에 추가 */
t_parse_result  parse_light(char *line, t_scene *scene);
```

### 전제조건
- `scene->light_count < MAX_LIGHTS`

### 후제조건
- `scene->lights[scene->light_count]`에 새 광원 저장
- `scene->light_count++`
- `light_count >= MAX_LIGHTS`이면 PARSE_ERR_OVERFLOW 반환

### 파싱 완료 후 검증
- `scene->light_count == 0`이면 파싱 에러 반환 (L 최소 1개 필수, FR-010)

## 조명 계산 변경

```c
/* 기존: 단일 광원 */
apply_lighting(scene, hit)  // scene->light 직접 사용

/* 변경: 다중 광원 루프 */
apply_lighting(scene, hit)  // scene->lights[i] 순회
```

### 파이프라인

```
for i = 0 to light_count - 1:
    light = &scene->lights[i]
    light_dir = normalize(light->position - hit->point)
    shadow_factor = calculate_shadow_factor(scene, hit, light->position)
    diffuse = max(0, dot(normal, light_dir))
    specular = calculate_specular(light_dir, normal, view_dir) * 0.5
    factor = (diffuse + specular) * (1 - shadow_factor)

    r += hit->color.r/255 * factor * light->brightness * light->color.r/255
    g += hit->color.g/255 * factor * light->brightness * light->color.g/255
    b += hit->color.b/255 * factor * light->brightness * light->color.b/255

final.r = clamp((ambient_r + r) * 255, 0, 255)
final.g = clamp((ambient_g + g) * 255, 0, 255)
final.b = clamp((ambient_b + b) * 255, 0, 255)
```

## 성능 — 소프트 섀도 동적 조절

다중 광원 시 소프트 섀도 샘플 수가 선형 증가하여 성능 병목 발생.

```
effective_samples = max(1, base_samples / light_count)
```

| 광원 수 | base=16 시 effective | 픽셀당 총 섀도 레이 |
|---------|---------------------|-------------------|
| 1 | 16 | 16 |
| 2-3 | 5-8 | 10-24 |
| 4-7 | 2-4 | 8-28 |
| 8+ | 1 (하드 섀도) | 8-16 |

총 섀도 레이 수를 base_samples 근방으로 유지하여 인터랙티브 성능 보장.

## 키보드 조작

- `=` 키: `selected_light = (selected_light + 1) % light_count`
- 광원 이동 키 (`[] ;' ,.`): `scene->lights[selected_light]`에 적용
- HUD에 현재 선택된 광원 인덱스 표시
