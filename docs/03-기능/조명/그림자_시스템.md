# 그림자 시스템 (Shadow System)

## 개요

miniRT의 그림자 시스템은 현실적인 그림자 렌더링을 제공합니다. 기존의 하드 그림자(hard shadow)에서 소프트 그림자(soft shadow), 그림자 감쇠(shadow attenuation), 적응형 바이어스(adaptive bias)를 지원하도록 확장되었습니다.

## 주요 기능

### 1. 적응형 그림자 바이어스 (Adaptive Shadow Bias)

그림자 아크네(shadow acne) 현상을 방지하기 위한 동적 바이어스 계산 시스템입니다.

**원리**:
- 표면 법선과 광원 방향 사이의 각도를 계산
- 각도가 클수록 (평행에 가까울수록) 더 큰 바이어스 적용
- 각도가 작을수록 (수직에 가까울수록) 기본 바이어스 사용

**공식**:
```
bias = base_bias × (1.0 + (1.0 - |dot(normal, light_dir)|) × 2.0)
```

**함수**: `calculate_shadow_bias()`
- **매개변수**:
  - `normal`: 표면 법선 벡터
  - `light_dir`: 광원 방향 벡터
  - `base_bias`: 기본 바이어스 값 (0.001)
- **반환값**: 조정된 바이어스 값

**사용 예제**:
```c
t_vec3 normal = {0.0, 1.0, 0.0};
t_vec3 light_dir = vec3_normalize(to_light);
double bias = calculate_shadow_bias(normal, light_dir, 0.001);
```

### 2. 소프트 그림자 (Soft Shadows)

여러 개의 그림자 광선을 사용하여 부드러운 그림자 경계를 생성합니다.

**원리**:
- 광원 위치 주변에서 여러 샘플 포인트 생성
- 각 샘플 포인트에서 그림자 광선 발사
- 차폐된 광선의 비율로 그림자 강도 계산
- 샘플 수가 많을수록 부드러운 그림자

**그림자 팩터 (Shadow Factor)**:
- 0.0 = 완전히 밝음 (그림자 없음)
- 1.0 = 완전히 어두움 (완전한 그림자)
- 0.0 ~ 1.0 = 부분 그림자 (반영부)

**함수**: `calculate_shadow_factor()`
- **매개변수**:
  - `scene`: 장면 데이터
  - `point`: 테스트할 점
  - `light_pos`: 광원 위치
  - `config`: 그림자 설정
- **반환값**: 그림자 팩터 (0.0 ~ 1.0)

**구현**:
```c
double shadow_factor = calculate_shadow_factor(scene, hit->point, 
                                                light_pos, &config);
diffuse = diffuse × (1.0 - shadow_factor);
```

### 3. 그림자 감쇠 (Shadow Attenuation)

거리에 따른 그림자 부드러움을 제공합니다.

**원리**:
- 광원으로부터의 거리를 계산
- 거리가 멀수록 그림자가 부드러워짐
- 역제곱 감쇠 곡선 사용

**공식**:
```
attenuation = 1.0 / (1.0 + (distance/max_distance)² × softness)
```

**함수**: `calculate_shadow_attenuation()`
- **매개변수**:
  - `distance`: 광원으로부터의 거리
  - `max_distance`: 최대 감쇠 거리
  - `softness`: 부드러움 계수
- **반환값**: 감쇠 팩터 (0.0 ~ 1.0)

### 4. 그림자 테스트 (Shadow Testing)

단일 광선으로 그림자 여부를 판단합니다.

**함수**: `is_in_shadow()`
- **매개변수**:
  - `scene`: 장면 데이터
  - `point`: 테스트할 점
  - `light_pos`: 광원 위치
  - `bias`: 그림자 바이어스
- **반환값**: 1 (그림자) 또는 0 (밝음)

**구현 세부사항**:
- 적응형 바이어스 적용
- 모든 구체(sphere)와 평면(plane) 확인
- 광원까지의 거리 내에서만 차폐 확인

## 데이터 구조

### t_shadow_config

그림자 렌더링 설정을 관리하는 구조체입니다.

```c
typedef struct s_shadow_config
{
    int     samples;        // 그림자 샘플 수 (1 = 하드 그림자)
    double  softness;       // 그림자 부드러움 (0.0 - 1.0)
    double  bias_scale;     // 바이어스 배율
    int     enable_ao;      // 앰비언트 오클루전 활성화 (미구현)
}   t_shadow_config;
```

**기본값**:
- `samples`: 4 (적당한 품질과 성능 균형)
- `softness`: 0.1 (약간의 부드러움)
- `bias_scale`: 2.0 (그림자 아크네 방지)
- `enable_ao`: 0 (비활성화)

## 설정 관리

### 초기화

```c
t_shadow_config config = init_shadow_config();
```

기본 설정으로 그림자 구성을 초기화합니다.

### 검증

```c
if (validate_shadow_config(&config))
{
    // 설정이 유효함
}
```

다음 조건을 확인합니다:
- `samples >= 1`
- `0.0 <= softness <= 1.0`
- `bias_scale >= 0.0`

### 샘플 수 설정

```c
set_shadow_samples(&config, 16);  // 16개 샘플 (고품질)
```

샘플 수가 많을수록:
- ✅ 더 부드러운 그림자
- ✅ 더 현실적인 결과
- ❌ 더 느린 렌더링

권장 샘플 수:
- **1**: 하드 그림자 (가장 빠름)
- **4**: 약간의 소프트 그림자 (균형)
- **9**: 중간 품질
- **16**: 고품질 소프트 그림자

### 부드러움 설정

```c
set_shadow_softness(&config, 0.5);  // 중간 부드러움
```

부드러움 값의 의미:
- **0.0**: 매우 샤프한 그림자
- **0.1**: 약간 부드러운 그림자 (기본값)
- **0.5**: 중간 부드러움
- **1.0**: 매우 부드러운 그림자

## 조명 통합

그림자 시스템은 `apply_lighting()` 함수에 통합되어 있습니다.

### 렌더링 파이프라인

```
1. 표면 법선과 광원 방향 계산
2. 람베르트 확산 조명 계산
3. 그림자 팩터 계산 (소프트 그림자)
4. 그림자 팩터를 확산광에 적용
5. 최종 색상 = 주변광 + 확산광 × 광원 밝기
```

### 코드 흐름

```c
// 1. 광원 방향 계산
t_vec3 light_dir = vec3_normalize(vec3_subtract(light_pos, point));

// 2. 확산광 계산
double diffuse = vec3_dot(normal, light_dir);
if (diffuse < 0) diffuse = 0;

// 3. 그림자 팩터 계산
double shadow_factor = calculate_shadow_factor(scene, point, 
                                                light_pos, &config);

// 4. 그림자 적용
diffuse = diffuse × (1.0 - shadow_factor);

// 5. 최종 색상
result = color × (ambient + diffuse × brightness);
```

## 성능 고려사항

### 샘플 수의 영향

| 샘플 수 | 품질 | 상대 성능 |
|---------|------|-----------|
| 1       | 하드 그림자 | 100% (기준) |
| 4       | 약간 부드러움 | ~25% |
| 9       | 중간 품질 | ~11% |
| 16      | 고품질 | ~6% |

### 최적화 팁

1. **동적 샘플 수**: 중요한 영역에만 높은 샘플 수 사용
2. **거리 기반 LOD**: 먼 물체는 적은 샘플 사용
3. **캐싱**: 정적 장면에서 그림자 재사용
4. **조기 종료**: 완전히 차폐된 경우 나머지 샘플 생략

## 사용 예제

### 기본 사용법

```c
// 장면 초기화 시 그림자 설정 초기화
scene->shadow_config = init_shadow_config();

// 조명 계산에서 자동으로 그림자 적용
t_color result = apply_lighting(scene, hit);
```

### 커스텀 설정

```c
// 고품질 그림자 설정
set_shadow_samples(&scene->shadow_config, 16);
set_shadow_softness(&scene->shadow_config, 0.3);

// 렌더링
t_color result = apply_lighting(scene, hit);
```

### 하드 그림자로 전환

```c
// 성능 최적화를 위해 하드 그림자 사용
set_shadow_samples(&scene->shadow_config, 1);
```

## 문제 해결

### 그림자 아크네 (Shadow Acne)

**증상**: 물체 표면에 점박이 패턴 발생

**해결책**:
1. 바이어스 스케일 증가: `config.bias_scale = 3.0`
2. 적응형 바이어스가 자동으로 처리

### 피터 패닝 (Peter Panning)

**증상**: 물체가 그림자에서 떠 있는 것처럼 보임

**해결책**:
1. 바이어스 스케일 감소: `config.bias_scale = 1.5`
2. 샘플 수 증가로 부드러운 전환

### 그림자가 너무 부드러움

**해결책**:
1. 부드러움 감소: `set_shadow_softness(&config, 0.05)`
2. 샘플 수 감소: `set_shadow_samples(&config, 4)`

### 성능 문제

**해결책**:
1. 샘플 수 감소: `set_shadow_samples(&config, 1)`
2. 중요한 물체에만 소프트 그림자 적용
3. 최적화된 교차 테스트 알고리즘 사용

## 향후 개선 사항

1. **앰비언트 오클루전 (Ambient Occlusion)**
   - 구석과 틈새의 어두워짐 효과
   - 깊이감 향상

2. **다중 광원 그림자**
   - 여러 광원의 그림자 상호작용
   - 복합 그림자 효과

3. **투명 그림자**
   - 투명/반투명 물체를 통한 그림자
   - 색상 투과

4. **그림자 매핑**
   - 하드웨어 가속 그림자
   - 더 빠른 렌더링

5. **적응형 샘플링**
   - 그림자 경계에서만 높은 샘플링
   - 평평한 영역은 낮은 샘플링

## 참고 자료

- Ray Tracing in One Weekend (Peter Shirley)
- Physically Based Rendering (PBRT)
- Real-Time Rendering, 4th Edition
- 42 miniRT subject

## 기술 사양

- **파일**: 
  - `src/lighting/shadow_calc.c` - 그림자 계산
  - `src/lighting/shadow_config.c` - 설정 관리
  - `src/lighting/shadow_test.c` - 그림자 테스트
  - `includes/shadow.h` - 헤더 파일

- **의존성**:
  - `vec3.h` - 벡터 연산
  - `ray.h` - 광선 구조체
  - `minirt.h` - 장면 데이터

- **테스트**: `tests/test_shadow_calc.c`, `tests/test_shadow_config.c`

- **노미네트 준수**: 모든 파일 100% 통과

---

**버전**: 1.0.0  
**작성일**: 2025-12-17  
**작성자**: miniRT team
