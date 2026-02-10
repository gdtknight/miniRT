# Implementation Plan: Performance Bottleneck Optimization

**Branch**: `031-perf-bottleneck-optimization` | **Date**: 2026-02-06 (Updated: 2026-02-10) | **Spec**: [spec.md](./spec.md)
**Input**: Feature specification from `/specs/031-perf-bottleneck-optimization/spec.md`

## Summary

miniRT 렌더러의 핵심 성능 병목을 단계적으로 해결한다.

**Round 1 (완료)**: P5(inv_dir), P2(camera cache), P6(child ordering), P1(shadow BVH any-hit)
**Round 2 (신규)**: PA(Plane BVH 분리), PB(Shadow BVH Threshold 조정) — 벤치마크 분석에서 발견된 추가 병목

## Technical Context

**Language/Version**: C (42 Norm v4.1 compliant)
**Primary Dependencies**: MiniLibX, libft, math library (-lm)
**Storage**: N/A (in-memory rendering)
**Testing**: Manual benchmark (S1~S4 scenes) + norminette + leaks check
**Target Platform**: macOS (Darwin)
**Project Type**: Single project
**Performance Goals**: perf_all_objects 프레임 타임 25s → 목표 20~40% 개선 (보수적 추정). Plane 분리 효과가 크면 추가 개선 기대
**Constraints**: 42 Norm v4.1 (25줄/함수, 5변수/함수, 4인자/함수, 5함수/파일), RGB ±1 정확성, 메모리 누수 없음
**Scale/Scope**: 3~50 오브젝트 씬, 1296000 primary rays (1080x1200)

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

| Principle | Status | Notes |
|-----------|--------|-------|
| I. 42 Norminette | PASS | 모든 변경 파일 norminette 통과 필수 |
| II. Memory Safety | PASS | plane 리스트 별도 할당 시 free 경로 확보 |
| III. Allowed Functions | PASS | 추가 외부 함수 불필요 (malloc/free만 사용) |
| IV. Modular Architecture | PASS | `src/spatial/` 내 파일 추가/수정 |
| V. Visual Correctness | PASS | Plane 분리 후 렌더링 결과 동일해야 함 |
| VI. Stability | PASS | NULL 체크, bounds 검증 |
| VII. Window Management | N/A | 윈도우 관련 변경 없음 |

## Project Structure

### Documentation (this feature)

```text
specs/031-perf-bottleneck-optimization/
├── plan.md              # This file
├── research.md          # Phase 0 output (Updated for Round 2)
├── data-model.md        # Phase 1 output (Updated for Round 2)
├── quickstart.md        # Phase 1 output
└── tasks.md             # Phase 2 output
```

### Source Code (repository root)

```text
src/
├── spatial/
│   ├── aabb.c               # AABB intersection (P5 적용 완료)
│   ├── aabb_basic.c          # AABB 기본 연산
│   ├── bounds.c              # 오브젝트 AABB 계산 (PA: plane 분리 대상)
│   ├── bvh_build_core.c      # BVH 재귀 빌드 (P6 적용 완료)
│   ├── bvh_build_split.c     # 축 선택, 분할 위치
│   ├── bvh_build_partition.c # 오브젝트 파티션
│   ├── bvh_init.c            # BVH 초기화 (PA: plane 필터링 대상)
│   ├── bvh_traverse.c        # Primary ray 순회 (PA: plane 별도 테스트)
│   ├── bvh_any_hit.c         # Shadow ray 순회 (PA: plane 별도 테스트)
│   └── bvh_lifecycle.c       # BVH 생성/소멸
├── render/
│   ├── camera.c              # 레이 생성 (P5, P2 적용 완료)
│   └── render.c              # Primary ray 루프 (PA: plane 테스트 통합)
├── lighting/
│   ├── shadow_test.c         # Shadow 테스트 (P1 적용 완료, PB 대상)
│   └── shadow_calc.c         # Shadow 계산
└── window/
    └── window_camera.c       # 카메라 이벤트 (P2 적용 완료)

includes/
├── spatial.h                 # BVH 구조체 (PA: plane 리스트 추가)
├── ray.h                     # Ray 구조체 (P5 적용 완료)
└── minirt.h                  # Scene, Camera 구조체 (P2 적용 완료)
```

**Structure Decision**: 기존 `src/spatial/` 모듈 내에서 수정. 신규 파일 불필요 (기존 파일 수정으로 충분).

---

## Round 1: 완료된 최적화 (P1+P2+P5+P6)

> 아래 항목들은 이미 구현 완료 상태이며, 기록 목적으로 포함.

| ID | 항목 | 상태 | 결과 |
|----|------|------|------|
| P5 | inv_dir precompute | ✅ 완료 | AABB 나눗셈→곱셈 변환 |
| P2 | Camera basis caching | ✅ 완료 | 프레임당 1회 계산 |
| P6 | BVH child ordering | ✅ 완료 | near/far ordering 적용 |
| P1 | Shadow BVH any-hit | ✅ 완료 | `bvh_intersect_any()` 구현 |

### Round 1 벤치마크 결과 (2026-02-10)

| Scene | Objects | Frame Time | FPS | Shadow Tests | BVH Skip |
|-------|---------|-----------|------|-------------|----------|
| `perf_timing` | 3 | 241ms | 4.15 | 634K | 93.7% |
| `perf_spheres_20` | 20 | 942ms | 1.06 | 15.4M | 59.0% |
| `perf_spheres_50` | 50 | 1,416ms | 0.71 | 2.2M | 46.6% |
| `perf_all_objects` | 18 | **25,192ms** | 0.04 | **369.6M** | 33.0% |

### Round 1 발견 사항

1. **Plane이 BVH 트리를 오염**: 모든 plane이 `[-1e6, 1e6]³` AABB → BVH skip rate 93.7%→33.0% 급락
2. **SHADOW_BVH_THRESHOLD=20**: 18 오브젝트 씬에서 BVH 미사용 → brute-force 369.6M shadow tests
3. **perf_all_objects가 perf_spheres_50보다 18배 느림**: 오브젝트 수 적음에도 plane 때문에 극심한 성능 저하

---

## Round 2: 신규 최적화 (PA+PB) — 설계 완료, 미구현

> **상태**: 아래 설계는 벤치마크 분석에서 도출되었으며, 코드에는 아직 반영되지 않았습니다. 구현은 tasks.md Phase 8-11에서 진행 예정.

### PA: Plane BVH 분리 (최우선)

#### 문제 분석

현재 `bounds_for_plane()`은 plane의 geometry 정보를 완전 무시하고 `[-1e6, 1e6]³` 고정 AABB를 반환:

```c
// src/spatial/bounds.c (현재)
static t_aabb bounds_for_plane(t_plane_data *p)
{
    double large = 1000000.0;
    (void)p;  // plane 데이터 완전히 무시
    return (aabb_create(
        (t_vec3){-large, -large, -large},
        (t_vec3){large, large, large}));
}
```

**연쇄적 영향**:
1. `compute_bounds()` → plane 포함 시 merged AABB가 `[-1e6, 1e6]³`로 확장
2. `choose_split_axis()` → extent `2e6`으로 동일, 축 선택 무의미
3. `calculate_split_position()` → split 위치가 0 부근, 실제 오브젝트 분포 무관
4. `partition_objects()` → plane center(=point)와 split 비교 → 한쪽 쏠림 → degenerate fallback `count/2`
5. **BVH 트리 전체 품질 저하**: skip rate 93.7% → 33.0%

#### 해법: Plane을 BVH에서 제외하고 별도 순회

```
Before:  BVH[sphere, plane, cylinder, plane, sphere, ...]  → 오염된 트리
After:   BVH[sphere, cylinder, sphere, ...]  +  별도 planes[]  → 깨끗한 트리
```

**핵심 원리**: Plane은 무한 오브젝트이므로 AABB로 근사할 수 없음. 대신:
- Bounded 오브젝트(sphere, cylinder)만 BVH에 포함
- Plane은 항상 ray와 교차 가능하므로 별도로 전수 검사 (dot product 1회, 매우 저렴)
- BVH 트리 품질 회복으로 전체 성능 대폭 개선

#### 구현 설계

**1. Scene 구조체에 plane 인덱스 리스트 추가**:

```c
// includes/spatial.h (또는 minirt.h)
typedef struct s_plane_refs
{
    int    *indices;      // plane 오브젝트 인덱스 배열
    int    count;         // plane 개수
}   t_plane_refs;
```

**2. BVH 빌드 시 plane 필터링** (`src/spatial/bvh_init.c`):

```c
// scene_build_bvh() 수정:
// 1. objects를 순회하며 plane/non-plane 분류
// 2. non-plane만으로 BVH 구축
// 3. plane 인덱스를 별도 저장
```

**3. Primary ray 순회** (`src/spatial/bvh_traverse.c` 또는 render 루프):

```c
// BVH 순회로 bounded 오브젝트 검사
hit = bvh_intersect(bvh, ray, ...);
// + plane 별도 순회 (항상)
hit = intersect_planes(planes, ray, hit);
```

**4. Shadow ray 순회** (`src/spatial/bvh_any_hit.c` 또는 shadow_test.c):

```c
// BVH로 bounded 오브젝트 검사
if (bvh_intersect_any(bvh, ray, max_dist, scene))
    return (1);
// + plane 별도 순회
if (check_plane_shadow(planes, ray, max_dist, scene))
    return (1);
```

#### 수정 대상 파일

| 파일 | 변경 내용 |
|------|----------|
| `includes/spatial.h` | `t_plane_refs` 구조체 추가, `t_bvh`에 `plane_refs` 필드 추가 |
| `src/spatial/bvh_init.c` | `scene_build_bvh()`: plane 필터링 로직 |
| `src/spatial/bvh_traverse.c` | plane 별도 intersection 함수 추가 |
| `src/spatial/bvh_any_hit.c` | plane 별도 shadow 테스트 함수 추가 |
| `src/spatial/bounds.c` | `bounds_for_plane()` 제거 가능 (더 이상 불필요) |
| `Makefile` | 변경 불필요 (기존 파일 수정) |

#### 기대 효과

- BVH가 bounded 오브젝트만으로 구축 → 트리 품질 대폭 향상
- Skip rate: 33% → 70~80%+ 예상
- Plane intersection 비용: 오브젝트당 dot product 1회 (무시 가능)
- perf_all_objects: 14개 bounded 오브젝트 + 4개 plane 별도

---

### PB: Shadow BVH Threshold 조정

#### 문제 분석

```c
// src/lighting/shadow_test.c (현재)
#define SHADOW_BVH_THRESHOLD 20

int is_in_shadow(...)
{
    if (scene->objects.count > SHADOW_BVH_THRESHOLD)
        return (bvh_intersect_any(...));    // BVH 사용
    return (check_object_shadow(...));       // brute-force
}
```

`perf_all_objects`는 18개 오브젝트 → threshold(20) 미만 → **BVH 미사용**:
- 16 shadow samples × 1,296,000 pixels × 18 objects = **373M tests** (실측 369.6M)
- PA 적용 후에도 threshold가 bounded 오브젝트 수(14개)를 기준으로 판단하면 여전히 brute-force 가능

#### 해법

**PA 적용 후 threshold 로직 재설계**:

1. **Threshold 하향**: `SHADOW_BVH_THRESHOLD`를 20 → 4~6으로 변경
2. **또는 threshold 제거**: PA 적용 후 BVH에는 bounded 오브젝트만 포함되므로, BVH가 존재하면 항상 사용
3. **Plane은 threshold 무관**: plane은 별도 순회이므로 threshold 판단에서 제외

#### 구현 설계

```c
// src/lighting/shadow_test.c (수정 후)
int is_in_shadow(...)
{
    int    in_shadow;

    in_shadow = 0;
    // 1. BVH로 bounded 오브젝트 검사 (항상 사용, threshold 제거)
    if (scene->bvh && scene->bvh->enabled && scene->bvh->root)
        in_shadow = bvh_intersect_any(scene->bvh, shadow_ray, mag, scene);
    // 2. Plane 별도 검사 (BVH 결과와 무관하게)
    if (!in_shadow)
        in_shadow = check_plane_shadow(scene, &shadow_ray, mag);
    return (in_shadow);
}
```

#### 기대 효과

- Shadow tests: 369.6M → ~30-50M (90% 감소 예상)
- PA와 결합 시 BVH 품질 향상으로 skip rate 증가 → 추가 감소

---

## PA + PB 조합 기대 효과

> **Note**: 아래 수치는 낙관적 추정치. 실제 개선율은 soft shadow 16 samples, metrics 출력 오버헤드, plane intersection 비용 등에 의해 제한될 수 있음. 벤치마크로 실측 검증 필수.

| 지표 | 현재 (Round 1) | 보수적 예상 | 낙관적 예상 |
|------|---------------|------------|------------|
| Frame time (all_objects) | 25,192ms | 15,000~20,000ms (20~40% 개선) | 5,000~10,000ms |
| Shadow tests (all_objects) | 369.6M | 100~200M (50% 감소) | 30~50M (90% 감소) |
| BVH skip rate (all_objects) | 33.0% | 50~60% | 70~80% |
| Frame time (spheres_20) | 942ms | 800~900ms | 500~700ms |
| Frame time (spheres_50) | 1,416ms | 1,100~1,300ms | 800~1,000ms |

---

## 의존성 및 실행 순서

### 전체 실행 흐름

```text
[Round 1 - 완료]
Phase 1 (Pre-check)        ✅
Phase 2 (P5: inv_dir)      ✅
Phase 3 (P2: camera cache) ✅
Phase 4 (P6: child ordering) ✅
Phase 5 (P1: shadow BVH)   ✅
Phase 6 (Validation)       ⚠️ T036, T037 미완료

[Round 2 - 신규]
Phase 8 (PA: Plane BVH 분리)
    ↓
Phase 9 (PB: Shadow threshold 조정) — PA와 동시 또는 직후
    ↓
Phase 10 (Round 2 Validation)
    ↓
Phase 11 (Final Benchmark)
```

### PA → PB 의존성

- **PA (Plane 분리)**가 먼저 완료되어야 PB의 threshold 로직이 의미 있음
- PA 완료 후 BVH에 bounded 오브젝트만 남으므로 threshold 판단 기준이 변경됨
- PB는 PA의 `is_in_shadow()` 수정과 함께 구현 가능 (같은 Phase로 통합 가능)

---

## 리스크 및 대응

| 리스크 | 영향 | 대응 |
|--------|------|------|
| Plane 분리 후 intersection 누락 | 렌더링 오류 (plane이 안 보임) | 모든 씬에서 plane 렌더링 확인 |
| Plane shadow 누락 | 그림자 아티팩트 (plane에 그림자 없음) | perf_all_objects에서 plane 그림자 검증 |
| Threshold 제거 시 소규모 씬 오버헤드 | 3개 오브젝트에서 BVH 오버헤드 | 벤치마크로 확인 (perf_timing 비교) |
| Norm 제한으로 함수 분리 필요 | 파일당 5함수 초과 | 기존 파일 내 static 함수 재구성 |
| plane_refs 메모리 누수 | 프로그램 종료 시 미해제 | cleanup 경로에 free 추가 |

## Complexity Tracking

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| N/A | N/A | N/A |

현재 설계에 constitution 위반 사항 없음.
