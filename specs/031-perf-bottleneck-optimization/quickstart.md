# Quickstart: Performance Bottleneck Optimization

**Feature**: 031-perf-bottleneck-optimization
**Date**: 2026-02-06 (Updated: 2026-02-10)

## 개요

이 피처는 miniRT 렌더러의 성능 병목을 단계적으로 해결합니다.

### Round 1 (✅ 구현 완료)
- **P5**: BVH inv_dir 사전 계산 (나눗셈 → 곱셈)
- **P2**: Camera basis 캐싱 (픽셀당 → 프레임당)
- **P6**: BVH child ordering (near/far 순서 최적화, t_max pruning 부분 동작)
- **P1**: Shadow ray BVH 가속 (brute-force → BVH any-hit)

### Round 2 (✅ 구현 완료)
- **PA**: Plane BVH 분리 (무한 오브젝트 제외로 BVH 품질 회복)
- **PB**: Shadow BVH threshold 20→5 하향 (소규모 씬 brute-force 유지)

## 빠른 시작

### 1. 빌드 및 테스트

```bash
# 브랜치 확인
git checkout 031-perf-bottleneck-optimization

# 빌드
make re

# 벤치마크 실행 (metrics 출력이 활성화된 상태)
./miniRT scenes/perf/perf_all_objects.rt
# → stdout에 Render Metrics 출력됨 (ESC로 종료)

# 다른 벤치마크 씬
./miniRT scenes/perf/perf_timing.rt        # 3 objects
./miniRT scenes/perf/perf_spheres_20.rt    # 20 objects
./miniRT scenes/perf/perf_spheres_50.rt    # 50 objects
```

### 2. Metrics 출력 예시

```
=== Render Metrics ===
Frame time:        941.7 ms
FPS:               1.0619
Rays traced:       1296000
Primary tests:     343289
Shadow tests:      15364205
Primary tests/ray: 0.3
BVH nodes visited: 5220172
BVH skip rate:     59.0%
Objects:           20
=======================
```

### 3. 코드 품질 확인

```bash
# Norminette
norminette src/spatial/ src/render/ src/lighting/ includes/

# 메모리 누수 (GUI 실행 후 ESC로 종료)
leaks -atExit -- ./miniRT scenes/perf/perf_spheres_20.rt
```

## 현재 상태 (Round 2 결과)

| Scene | Objects | Round 1 | Round 2 | 변화 |
|-------|---------|---------|---------|------|
| perf_timing | 3 | 241ms | **241ms** | 0% |
| perf_spheres_20 | 20 | 942ms | **1,219ms** | +29.4% |
| perf_spheres_50 | 50 | 1,416ms | **1,432ms** | +1.1% |
| perf_all_objects | 18 | 25,192ms | **6,067ms** | **-75.9%** |

**핵심 성과**: Plane AABB 오염 해결 + Shadow threshold 하향으로 perf_all_objects 75.9% 개선.

## Round 2 구현 가이드 (PA: Plane BVH 분리)

### 핵심 변경

**1. `includes/spatial.h`**: `t_plane_refs` 구조체 추가, `t_bvh`에 `plane_refs` 필드 추가

**2. `src/spatial/bvh_init.c`**: `scene_build_bvh()`에서 plane 필터링
- OBJ_PLANE 타입 오브젝트를 `plane_refs`에 분리
- Bounded 오브젝트만으로 BVH 구축

**3. `src/render/trace.c`**: Primary ray에서 plane 별도 intersection
```c
// BVH로 bounded 오브젝트 검사
bvh_hit = bvh_intersect(scene->bvh, *ray, &hit, scene);
// Plane 별도 검사
plane_hit = intersect_planes(scene->bvh, *ray, &hit, scene);
```

**4. `src/lighting/shadow_test.c`**: Shadow ray에서 plane 별도 검사 + threshold 제거

## 참고 문서

- [spec.md](./spec.md) - 요구사항 명세
- [research.md](./research.md) - 기술 조사 (Round 1 + Round 2)
- [data-model.md](./data-model.md) - 구조체 변경 (Round 1 구현완료 + Round 2 설계)
- [tasks.md](./tasks.md) - 실행 태스크 (Round 1 완료 + Round 2 신규)
- [plan.md](./plan.md) - 구현 계획
