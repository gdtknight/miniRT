# Quickstart: 032-fix-review-issues

**Feature**: 032-fix-review-issues
**Date**: 2026-02-10

## 개요

031 코드 리뷰에서 발견된 4건의 안정성 이슈를 수정합니다.

| # | 심각도 | 이슈 | 파일 |
|---|--------|------|------|
| R1 | 심각 | malloc 실패 시 NULL 역참조 | bvh_init.c |
| R2 | 높음 | BVH 재빌드 시 plane_refs 누수 | bvh_init.c |
| R3 | 중간 | FOV 0/180 경계값 | parse_elements.c |
| R4 | 중간/잠재 | set_shadow_samples LUT 미갱신 | shadow_config.c |

## 빠른 시작

### 1. 빌드 및 검증

```bash
# 브랜치 확인
git checkout 032-fix-review-issues

# 빌드
make re

# Norminette (수정 대상 파일만)
norminette src/spatial/bvh_init.c src/parser/parse_elements.c src/lighting/shadow_config.c
```

### 2. FOV 경계값 테스트

기대: FOV=0, 180 → stderr에 `Error` 출력 후 종료 (PARSE_ERR_RANGE 경로)

```bash
# FOV=0 → "Error" 출력 확인 (PARSE_ERR_RANGE)
echo "A 0.2 255,255,255
C 0,0,0 0,0,1 0
L 0,10,0 0.7 255,255,255
sp 0,0,5 2 255,0,0" > /tmp/test_fov0.rt
./miniRT /tmp/test_fov0.rt 2>&1 | head -1
# 기대 출력: "Error" (프로그램 즉시 종료)

# FOV=180 → "Error" 출력 확인 (PARSE_ERR_RANGE)
echo "A 0.2 255,255,255
C 0,0,0 0,0,1 180
L 0,10,0 0.7 255,255,255
sp 0,0,5 2 255,0,0" > /tmp/test_fov180.rt
./miniRT /tmp/test_fov180.rt 2>&1 | head -1
# 기대 출력: "Error" (프로그램 즉시 종료)

# FOV=1, FOV=179 → 정상 렌더 확인 (윈도우 열림)
echo "A 0.2 255,255,255
C 0,0,0 0,0,1 1
L 0,10,0 0.7 255,255,255
sp 0,0,5 2 255,0,0" > /tmp/test_fov1.rt
./miniRT /tmp/test_fov1.rt
```

### 3. 성능 회귀 확인

```bash
# 기존 벤치마크 (이전 결과와 비교)
./miniRT scenes/perf/perf_timing.rt        # 기대: ~241ms
./miniRT scenes/perf/perf_all_objects.rt   # 기대: ~6,067ms
```

### 4. 메모리 누수 확인

```bash
# R2 검증: BVH 재빌드 반복 후 누수 없음
leaks -atExit -- ./miniRT scenes/perf/perf_spheres_20.rt
# → 카메라 이동(방향키) 여러 번 수행 후 ESC 종료
```

### 5. 실패 경로 smoke test

정상 빌드에서는 재현 불가능한 실패 경로를 **임시 코드 패치 → 실행 → 리버트** 방식으로 검증합니다. 최종 커밋에는 포함하지 않습니다.

**R1/R2: alloc_bvh_refs() 실패 시 크래시 없음 확인**

주의: cleanup 코드 **이후**에 삽입해야 실제 실패 상태를 정확히 재현합니다.

```bash
# 1. alloc_bvh_refs() 내 cleanup 3줄 이후에 임시 코드 삽입
#    (src/spatial/bvh_init.c의 alloc_bvh_refs 함수)
#    `scene->bvh->plane_refs.indices = NULL;` 다음 줄에:
#      *refs = NULL;
#      return (0);
# 2. 빌드 및 실행
make re && ./miniRT scenes/perf/perf_all_objects.rt
# 3. 확인: 크래시 없이 brute-force 렌더 정상 동작 (모든 오브젝트 표시)
#    근거: bvh->root=NULL → trace_ray()가 check_all_objects() fallback 사용
# 4. 리버트
git checkout src/spatial/bvh_init.c && make re
```

**R4: offset_lut NULL fallback 정상 동작 확인**

```bash
# 1. init_shadow_config()에서 init_shadow_offset_lut(&config); 을 임시 주석 처리
#    (src/lighting/shadow_config.c의 init_shadow_config 함수)
# 2. 빌드 및 실행
make re && ./miniRT scenes/perf/perf_timing.rt
# 3. 확인: 그림자 렌더링 정상 (generate_shadow_sample_offset fallback 사용)
# 4. 리버트
git checkout src/lighting/shadow_config.c && make re
```

## 참고 문서

- [spec.md](./spec.md) — 요구사항 명세
- [plan.md](./plan.md) — 구현 계획 (Norm 제약 분석 포함)
- [research.md](./research.md) — 설계 근거 및 대안 분석
- [data-model.md](./data-model.md) — 구조체/함수 변경 정리
