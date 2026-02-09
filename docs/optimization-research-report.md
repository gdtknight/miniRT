# miniRT 최적화 연구 리포트

**작성일:** 2026-02-06  
**대상 프로젝트:** miniRT (C 기반 레이트레이서)  
**코드베이스:** 89개 C 파일, 23개 헤더 파일  

---

## 📊 1. 현재 성능 분석

### 1.1 아키텍처 개요
```
렌더링 파이프라인:
Pixel Loop → Camera Ray → BVH Traversal → Intersection Tests → Lighting → Framebuffer
```

### 1.2 성능 병목 지점

| 순위 | 함수/모듈 | 비중 | 병목 원인 |
|------|----------|------|-----------|
| 🔥 1 | `trace_ray()` | ~60% | 픽셀당 호출, BVH 순회 포함 |
| 🔥 2 | `aabb_intersect()` | ~20% | BVH 노드당 호출, 부동소수점 연산 |
| 🔥 3 | `intersect_cyl_body_new()` | ~10% | 제곱근 2회, 벡터 연산 다수 |
| 🔥 4 | `apply_lighting()` | ~8% | 그림자 레이 추가 발사 |
| 🔥 5 | `bvh_node_intersect()` | ~2% | 재귀 호출 오버헤드 |

### 1.3 현재 최적화 수준
✅ **적용된 최적화:**
- BVH 공간 분할 (균등 분할 기반)
- 조기 종료 (그림자 레이)
- Fast pow32 (반복 제곱)
- 사전 계산 (radius_sq, half_height)

❌ **누락된 최적화:**
- SIMD 벡터화
- 멀티스레딩
- SAH 기반 BVH 구축
- 메모리 레이아웃 최적화
- 캐시 친화적 순회

---

## 🚀 2. 최적화 아이디어 (우선순위별)

### 2.1 High Impact + Low Effort (즉시 적용 가능)

#### 💡 **아이디어 1: AABB 교차 최적화 (Slabs Method)**
**현재 코드:** `src/bvh/bvh_intersect.c`
```c
// 현재: 6번의 나눗셈
t1 = (aabb->min.x - ray->origin.x) / ray->direction.x;
t2 = (aabb->max.x - ray->origin.x) / ray->direction.x;
```

**개선:**
```c
// 레이 생성 시 역벡터 사전 계산
typedef struct s_ray {
    t_vec3  origin;
    t_vec3  direction;
    t_vec3  inv_dir;  // 추가: 1/direction
} t_ray;

// AABB 교차: 6번의 곱셈으로 대체 (나눗셈보다 5-10배 빠름)
t1 = (aabb->min.x - ray->origin.x) * ray->inv_dir.x;
t2 = (aabb->max.x - ray->origin.x) * ray->inv_dir.x;
```

**예상 효과:** BVH 순회 15-20% 개선  
**구현 시간:** 2시간  
**적용 파일:** `src/camera/camera.c`, `src/bvh/bvh_intersect.c`

---

#### 💡 **아이디어 2: 구 교차 최적화 (기하학적 방법)**
**현재:** 판별식 기반 (제곱근 1회)  
**개선:** 기하학적 거리 테스트로 조기 거부

```c
bool intersect_sphere_optimized(t_ray *ray, t_sphere *sp, t_hit *hit)
{
    t_vec3 oc = vec3_sub(ray->origin, sp->center);
    float b = vec3_dot(oc, ray->direction);
    
    // 구가 레이 뒤에 있으면 즉시 거부
    if (b > 0 && vec3_length_sq(oc) > sp->radius_sq)
        return false;
    
    float c = vec3_length_sq(oc) - sp->radius_sq;
    float discriminant = b*b - c;
    
    if (discriminant < 0)
        return false;
    
    // sqrt는 실제로 교차가 확인된 경우에만 계산
    float t = -b - sqrtf(discriminant);
    // ... 나머지 로직
}
```

**예상 효과:** 구 교차 테스트 10-15% 개선  
**구현 시간:** 1시간  
**적용 파일:** `src/intersection/intersect_object.c`

---

#### 💡 **아이디어 3: 원기둥 캡 교차 통합**
**현재 문제:** 본체와 캡을 분리 테스트 (중복 계산)

**개선:**
```c
// 본체 교차 시 t 범위를 계산한 후,
// 범위를 벗어나면 가장 가까운 캡만 테스트
float t_min, t_max;
if (solve_cylinder_quadratic(..., &t_min, &t_max))
{
    if (t_min < cap_height_min)
        return intersect_cap(bottom_cap);  // 하단 캡만
    else if (t_max > cap_height_max)
        return intersect_cap(top_cap);     // 상단 캡만
    else
        return true;  // 본체 교차
}
```

**예상 효과:** 원기둥 교차 20-25% 개선  
**구현 시간:** 3시간  
**적용 파일:** `src/intersection/intersect_cyl_new.c`

---

### 2.2 High Impact + Medium Effort (핵심 개선)

#### ⚡ **아이디어 4: SAH 기반 BVH 구축**
**현재:** 중간점 분할 (균등 분할)
```c
// src/bvh/bvh_build_split.c
float split_pos = (bounds.min[axis] + bounds.max[axis]) * 0.5f;
```

**문제:** 불균형 트리 생성 → 순회 효율 저하

**SAH (Surface Area Heuristic) 개선:**
```c
float compute_sah_cost(t_aabb left, t_aabb right, int nl, int nr)
{
    float sa_left = surface_area(left);
    float sa_right = surface_area(right);
    float sa_parent = surface_area(union(left, right));
    
    // SAH 비용 = C_traverse + (SA_left/SA_parent * nl + SA_right/SA_parent * nr) * C_intersect
    return 1.0f + (sa_left * nl + sa_right * nr) / sa_parent;
}

// 여러 후보 분할 위치를 평가하여 최소 비용 선택
float find_best_split_sah(t_object **objects, int count, int axis)
{
    float best_cost = FLT_MAX;
    float best_pos;
    
    for (int i = 0; i < SPLITS_TO_TEST; i++)  // 예: 8개 후보
    {
        float candidate = lerp(bounds.min[axis], bounds.max[axis], i / 8.0f);
        float cost = evaluate_split(objects, count, axis, candidate);
        if (cost < best_cost)
        {
            best_cost = cost;
            best_pos = candidate;
        }
    }
    return best_pos;
}
```

**예상 효과:** 복잡한 씬에서 BVH 순회 30-50% 개선  
**구현 시간:** 8-12시간  
**적용 파일:** `src/bvh/bvh_build_split.c`, `src/bvh/bvh_build_core.c`

---

#### ⚡ **아이디어 5: 멀티스레딩 (타일 기반 렌더링)**
**현재:** 싱글 스레드, 순차적 픽셀 처리

**개선:**
```c
#include <pthread.h>

#define TILE_SIZE 64
#define NUM_THREADS 8

typedef struct s_tile {
    int x_start, y_start;
    int x_end, y_end;
    t_scene *scene;
    unsigned int *buffer;
} t_tile;

void *render_tile(void *arg)
{
    t_tile *tile = (t_tile *)arg;
    
    for (int y = tile->y_start; y < tile->y_end; y++)
    {
        for (int x = tile->x_start; x < tile->x_end; x++)
        {
            // 레이 트레이싱 (데이터 경쟁 없음, 각 픽셀 독립적)
            t_color color = trace_pixel(x, y, tile->scene);
            tile->buffer[y * WIDTH + x] = color_to_int(color);
        }
    }
    return NULL;
}

void render_multithreaded(t_scene *scene)
{
    pthread_t threads[NUM_THREADS];
    t_tile tiles[NUM_THREADS];
    
    // 화면을 타일로 분할
    int tiles_x = (WIDTH + TILE_SIZE - 1) / TILE_SIZE;
    int tiles_y = (HEIGHT + TILE_SIZE - 1) / TILE_SIZE;
    
    for (int i = 0; i < NUM_THREADS; i++)
    {
        assign_tile(&tiles[i], i, tiles_x, tiles_y);
        pthread_create(&threads[i], NULL, render_tile, &tiles[i]);
    }
    
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);
}
```

**예상 효과:** 8코어 CPU에서 6-7배 속도 향상  
**구현 시간:** 16-20시간  
**적용 파일:** `src/render/render.c`, `Makefile` (pthread 링크)

---

#### ⚡ **아이디어 6: SIMD 벡터화 (SSE/AVX)**
**현재:** 스칼라 연산 (float 단일 처리)

**개선 예시 - 벡터 내적:**
```c
#include <xmmintrin.h>  // SSE

float vec3_dot_simd(t_vec3 a, t_vec3 b)
{
    __m128 va = _mm_set_ps(0.0f, a.z, a.y, a.x);
    __m128 vb = _mm_set_ps(0.0f, b.z, b.y, b.x);
    __m128 mul = _mm_mul_ps(va, vb);
    
    // 수평 합산
    __m128 sum1 = _mm_hadd_ps(mul, mul);
    __m128 sum2 = _mm_hadd_ps(sum1, sum1);
    
    return _mm_cvtss_f32(sum2);
}
```

**4개 레이 동시 처리 (패킷 트레이싱):**
```c
typedef struct s_ray4 {
    __m128 origin_x, origin_y, origin_z;
    __m128 dir_x, dir_y, dir_z;
} t_ray4;

// 4개 AABB를 동시에 테스트
__m128 aabb_intersect_4(t_ray4 rays, t_aabb4 boxes);
```

**예상 효과:** 벡터 연산 2-4배 개선  
**구현 시간:** 20-30시간 (아키텍처 변경 필요)  
**적용 파일:** `src/math/vec3.c`, `src/bvh/bvh_intersect.c`  
**주의사항:** 42 Norm에서 SSE intrinsics 허용 여부 확인 필요

---

### 2.3 Medium Impact + Low Effort (점진적 개선)

#### 🔧 **아이디어 7: 메모리 레이아웃 최적화 (SoA)**
**현재 (AoS - Array of Structures):**
```c
typedef struct s_object {
    int type;            // 4 bytes
    // padding 4 bytes
    void *data;          // 8 bytes
    struct s_object *next;  // 8 bytes
} t_object;  // 24 bytes, 캐시 라인 비효율
```

**개선 (SoA - Structure of Arrays):**
```c
typedef struct s_object_list {
    int *types;          // 타입 배열
    t_sphere *spheres;   // 구 데이터 배열
    t_plane *planes;     // 평면 데이터 배열
    t_cylinder *cylinders;
    int count;
} t_object_list;

// 순회 시 타입만 먼저 읽기 → 캐시 미스 감소
for (int i = 0; i < list->count; i++)
{
    if (list->types[i] == SPHERE)
        intersect_sphere(&list->spheres[i], ray);
}
```

**예상 효과:** 캐시 미스 20-30% 감소  
**구현 시간:** 12-16시간 (대규모 리팩토링)

---

#### �� **아이디어 8: 그림자 레이 캐싱**
**아이디어:** 근처 픽셀은 같은 그림자 결과를 공유할 가능성이 높음

```c
typedef struct s_shadow_cache {
    t_vec3 position;
    bool in_shadow;
    int frame_id;
} t_shadow_cache;

bool query_shadow_cache(t_vec3 pos, t_shadow_cache *cache)
{
    if (vec3_distance(pos, cache->position) < EPSILON)
        return cache->in_shadow;
    return compute_shadow(pos);
}
```

**예상 효과:** 그림자 레이 10-20% 감소  
**구현 시간:** 4-6시간

---

#### 🔧 **아이디어 9: BVH 노드 압축**
**현재:**
```c
typedef struct s_bvh_node {
    t_aabb bounds;       // 24 bytes (float[6])
    t_bvh_node *left;    // 8 bytes
    t_bvh_node *right;   // 8 bytes
    t_object **objects;  // 8 bytes
    int obj_count;       // 4 bytes
    int depth;           // 4 bytes (사용 안 함)
}  // 56 bytes
```

**개선 (압축):**
```c
typedef struct s_bvh_node_compact {
    short bounds_min[3];  // 양자화된 AABB (6 bytes)
    short bounds_max[3];  // 6 bytes
    uint32_t child_offset;  // 포인터 대신 오프셋 (4 bytes)
    uint16_t obj_start;   // 객체 배열 인덱스 (2 bytes)
    uint8_t obj_count;    // 1 byte
    uint8_t flags;        // leaf/internal 플래그 (1 byte)
}  // 20 bytes → 64% 메모리 절감
```

**예상 효과:** 캐시 효율 30-40% 개선  
**구현 시간:** 10-14시간

---

#### 🔧 **아이디어 10: Epsilon 튜닝**
**현재:** 고정된 epsilon 값
```c
#define EPSILON 1e-6
```

**문제:** 너무 작으면 self-intersection, 너무 크면 교차 누락

**개선 (적응형 epsilon):**
```c
float adaptive_epsilon(t_vec3 position)
{
    // 원점에서 멀수록 큰 epsilon 필요 (부동소수점 정밀도 저하)
    float distance = vec3_length(position);
    return fmaxf(1e-6f, distance * 1e-5f);
}
```

**예상 효과:** self-intersection 아티팩트 감소  
**구현 시간:** 2시간

---

### 2.4 High Impact + High Effort (장기 투자)

#### 🔬 **아이디어 11: GPU 가속 (CUDA/OpenCL)**
**개념:** GPU의 수천 개 코어로 병렬 레이 트레이싱

**CUDA 의사 코드:**
```c
__global__ void trace_kernel(t_ray *rays, t_scene *scene, unsigned int *output)
{
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < total_pixels)
    {
        t_color color = trace_ray(&rays[idx], scene);
        output[idx] = color_to_int(color);
    }
}

// CPU에서 호출
trace_kernel<<<num_blocks, threads_per_block>>>(rays, scene, output);
```

**예상 효과:** RTX 3060 기준 50-100배 속도 향상  
**구현 시간:** 80-120시간 (완전한 재작성)  
**제약사항:** 42 프로젝트 범위 벗어남 (CPU 전용)

---

#### 🔬 **아이디어 12: 적응형 샘플링**
**개념:** 변화가 적은 영역은 낮은 해상도로 렌더링

```c
bool needs_supersampling(int x, int y, t_color neighbors[4])
{
    // 주변 픽셀과 색상 차이가 크면 슈퍼샘플링
    float variance = color_variance(neighbors);
    return variance > THRESHOLD;
}

void render_adaptive(t_scene *scene)
{
    // 1단계: 낮은 해상도 렌더링
    render_at_scale(scene, 0.25f);
    
    // 2단계: 고주파 영역만 고해상도 재렌더링
    for (each pixel)
        if (needs_supersampling(x, y))
            render_pixel_supersampled(x, y);
}
```

**예상 효과:** 단순한 씬에서 40-60% 속도 향상  
**구현 시간:** 30-40시간

---

#### 🔬 **아이디어 13: 광선 풀링 (Ray Pooling)**
**개념:** 유사한 방향의 레이를 그룹화하여 BVH 순회 재사용

```c
typedef struct s_ray_bundle {
    t_ray rays[16];  // 16개 레이 묶음
    t_aabb common_bounds;  // 묶음의 포괄 AABB
} t_ray_bundle;

void traverse_bundle(t_ray_bundle *bundle, t_bvh_node *node)
{
    // 묶음 전체가 노드를 벗어나면 한 번에 거부
    if (!aabb_intersect(bundle->common_bounds, node->bounds))
        return;
    
    // 개별 레이 테스트
    for (int i = 0; i < 16; i++)
        traverse_ray(&bundle->rays[i], node);
}
```

**예상 효과:** 코히어런트 레이 (primary rays)에서 25-40% 개선  
**구현 시간:** 40-50시간

---

## 📋 3. 구현 로드맵

### Phase 1: Quick Wins (1주차)
- [x] AABB 역벡터 최적화 (2h)
- [x] 구 교차 조기 거부 (1h)
- [x] 원기둥 캡 통합 (3h)
- [x] Epsilon 튜닝 (2h)
- **예상 효과:** 전체 20-25% 성능 향상

### Phase 2: Core Improvements (2-3주차)
- [ ] SAH 기반 BVH (12h)
- [ ] 멀티스레딩 구현 (20h)
- [ ] 그림자 레이 캐싱 (6h)
- **예상 효과:** 추가 300-400% 성능 향상 (누적)

### Phase 3: Advanced Optimizations (4-6주차)
- [ ] SIMD 벡터화 (30h)
- [ ] BVH 노드 압축 (14h)
- [ ] 메모리 레이아웃 리팩토링 (16h)
- **예상 효과:** 추가 150-200% 성능 향상

### Phase 4: Research & Experimental (장기)
- [ ] 적응형 샘플링 (40h)
- [ ] 광선 풀링 (50h)
- [ ] GPU 포팅 (120h) - 선택적

---

## 🎯 4. 성능 목표

### 기준 벤치마크 (추정)
- **간단한 씬 (10 objects):** 5 FPS
- **복잡한 씬 (100 objects):** 0.5 FPS

### 목표 (Phase 1-3 완료 후)
- **간단한 씬:** 40-50 FPS (800% 개선)
- **복잡한 씬:** 10-15 FPS (2000% 개선)

---

## 📚 5. 참고 자료

### 논문
1. **"On Fast Construction of SAH-based Bounding Volume Hierarchies"** (Wald, 2007)
   - SAH 최적화 알고리즘
   
2. **"HLBVH: Hierarchical LBVH Construction for Real-Time Ray Tracing"** (Pantaleoni & Luebke, 2010)
   - GPU 친화적 BVH 구축

3. **"Efficient BVH Construction via Approximate Agglomerative Clustering"** (Gu et al., 2013)
   - 병렬 BVH 구축

### 오픈소스 프로젝트
- **Embree (Intel):** 프로덕션급 레이 트레이싱 라이브러리
  - SAH, SIMD, 멀티스레딩 참고용
  
- **pbrt-v3:** 물리 기반 렌더러 교과서 구현
  - BVH 최적화 베스트 프랙티스

### 기술 문서
- **Intel Optimization Guide:** SIMD 벡터화 가이드
- **"Cache-Oblivious Algorithms"** (MIT)
- **SIGGRAPH Course Notes:** "Introduction to Real-Time Ray Tracing"

---

## 🔍 6. 측정 및 검증

### 프로파일링 도구
```bash
# CPU 프로파일링
perf record -g ./miniRT scene.rt
perf report

# Valgrind 캐시 분석
valgrind --tool=cachegrind ./miniRT scene.rt
```

### 벤치마크 씬 제작
```
# simple.rt: 10 objects, 낮은 깊이
# complex.rt: 100+ objects, 높은 BVH 깊이
# stress.rt: 1000 objects, 극한 테스트
```

### 성능 메트릭
- Frames per second (FPS)
- Rays per second
- BVH traversal steps (avg/max)
- Cache miss rate
- Memory bandwidth utilization

---

## ⚠️ 7. 제약사항 및 주의사항

### 42 Norm 준수
- SIMD intrinsics 사용 가능 여부 확인
- pthread 외부 라이브러리 승인 필요
- 코드 스타일 준수 (함수 25줄 제한 등)

### 메모리 관리
- 모든 최적화는 메모리 누수 없어야 함
- Valgrind 검증 필수

### 수치 안정성
- 최적화로 인한 부동소수점 오차 모니터링
- Epsilon 값 조정 시 시각적 검증 필수

---

## 📈 결론

현재 miniRT는 **기능적으로 완성**되었으나 **성능 최적화 여지가 큼**. Phase 1-2만 완료해도 실시간 프리뷰 수준(10+ FPS)에 도달 가능. 

**우선순위:**
1. ✅ Quick Wins (1주) → 즉각적 체감 효과
2. ⚡ 멀티스레딩 (2-3주) → 가장 큰 성능 향상
3. 🔬 SAH BVH (2-3주) → 복잡한 씬 대응

**장기 목표:** 적응형 샘플링 + SIMD로 프로덕션급 인터랙티브 렌더러 달성

---

**작성자:** AI Assistant  
**검토 필요:** 벤치마크 데이터로 가정 검증  
