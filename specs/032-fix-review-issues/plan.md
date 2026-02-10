# Implementation Plan: 032-fix-review-issues

**Branch**: `032-fix-review-issues` | **Date**: 2026-02-10 | **Spec**: [spec.md](./spec.md)
**Input**: Feature specification from `/specs/032-fix-review-issues/spec.md`

## Summary

031 피처 코드 리뷰에서 발견된 4건의 안정성·메모리 안전성 이슈를 수정합니다.
기능 변경 없이 방어적 코딩만 보강하며, 수정 범위는 4개 파일에 한정됩니다.

## Technical Context

**Language/Version**: C (42 Norm v4.1 compliant)
**Primary Dependencies**: MiniLibX, libft, math library (-lm)
**Storage**: N/A (in-memory rendering)
**Testing**: norminette, manual benchmark (`./miniRT scenes/perf/*.rt`)
**Target Platform**: macOS (darwin)
**Project Type**: single
**Performance Goals**: 기존 벤치마크 성능 변화 없음
**Constraints**: 25줄/함수, 5변수/함수, 4파라미터/함수, 5함수/파일 (Norm v4.1)
**Scale/Scope**: 4개 파일 수정, 신규 파일 없음

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

| Principle | Status | Notes |
|-----------|--------|-------|
| I. Norminette | ✅ | 모든 수정 25줄/함수 제한 준수 설계 완료 |
| II. Memory Safety | ✅ | R1: NULL 가드 추가, R2: free 누락 수정, R4: LUT 재할당 |
| III. Allowed Functions | ✅ | malloc/free만 사용 (이미 허용) |
| IV. Modular Architecture | ✅ | 기존 모듈 구조 유지, 신규 파일 없음 |
| V. Visual Correctness | ✅ | 렌더링 결과 변경 없음 |
| VI. Stability & Crash Prevention | ✅ | R1/R3이 직접적으로 크래시 방지 |
| VII. Window Management | ✅ | 영향 없음 |

**GATE RESULT: PASS** — 위반 사항 없음.

## Project Structure

### Documentation (this feature)

```text
specs/032-fix-review-issues/
├── plan.md              # This file
├── research.md          # Phase 0: Norm 제약 분석 및 설계 근거
├── data-model.md        # Phase 1: 구조체 변경 (없음, 기존 구조 유지)
└── quickstart.md        # Phase 1: 빌드/검증 가이드
```

### Source Code (수정 대상)

```text
src/
├── spatial/
│   └── bvh_init.c           # R1 + R2: malloc 가드 + 누수 수정
├── parser/
│   └── parse_elements.c     # R3: FOV 경계값 제한
└── lighting/
    └── shadow_config.c      # R4: LUT 재생성
```

## Norm 제약 분석

### bvh_init.c (R1 + R2)

**현재 상태**: 4함수/5 한계, `scene_build_bvh()` 24줄/25 한계

**문제**: R1(malloc 가드) + R2(free 추가) 합치면 scene_build_bvh에 ~5줄 추가 → 25줄 초과

**해결**: 할당+검증 로직을 `alloc_bvh_refs()` static 함수로 추출
- `alloc_bvh_refs()`: ~18줄, 4파라미터 (scene, refs_out, pc, bc)
- 실패 시 cleanup: 이미 할당된 indices 해제 + count=0 롤백
- `scene_build_bvh()`: ~20줄로 축소
- 파일 함수 수: 4 → 5 (한계 도달, 초과 아님)

### parse_elements.c (R3)

**현재 상태**: 4함수/5 한계, `parse_camera()` ~23줄/25 한계

**수정**: `in_range(fov, 0, 180)` → `in_range(fov, 1, 179)` (상수 변경만, 줄 수 동일)

**근거**: fov는 `int` 타입이므로 `1 ≤ fov ≤ 179` == `0 < fov < 180`

### shadow_config.c (R4)

**현재 상태**: 5함수/5 한계 (이미 최대), `set_shadow_samples()` 4줄

**수정**: `set_shadow_samples()` 내부에서 기존 LUT free + `init_shadow_offset_lut()` 호출
- `init_shadow_offset_lut()`은 같은 파일 내 static으로 이미 존재 → 호출 가능
- 수정 후 ~8줄, 함수 수 변경 없음 (기존 함수 수정만)

## 구현 단계

### Phase 1: R1 + R2 — bvh_init.c malloc 가드 + 누수 수정

**1-1. `alloc_bvh_refs()` static 함수 추가**

```c
static int	alloc_bvh_refs(t_scene *scene, t_object_ref **refs, int pc, int bc)
{
	free(scene->bvh->plane_refs.indices);
	scene->bvh->plane_refs.count = 0;
	scene->bvh->plane_refs.indices = NULL;
	if (pc > 0)
		scene->bvh->plane_refs.indices = malloc(sizeof(int) * pc);
	*refs = NULL;
	if (bc > 0)
		*refs = malloc(sizeof(t_object_ref) * bc);
	if (pc > 0 && !scene->bvh->plane_refs.indices)
		return (0);
	if (bc > 0 && !*refs)
	{
		free(scene->bvh->plane_refs.indices);
		scene->bvh->plane_refs.indices = NULL;
		return (0);
	}
	scene->bvh->plane_refs.count = pc;
	return (1);
}
```

- 18줄, 4파라미터, 0변수 → Norm 준수
- R2: `free(scene->bvh->plane_refs.indices)` — 재빌드 시 기존 포인터 해제
- R1: malloc 결과 검증 후 0 반환 → 호출측에서 early return
- **실패 시 cleanup**: count=0 기본값, bc 할당 실패 시 indices도 해제
  - pc 할당 실패: count=0, indices=NULL → 안전한 상태
  - bc 할당 실패: 성공한 indices 해제 + count=0 → 안전한 상태
  - 소비자(`trace.c:74`, `shadow_test.c:67`)가 count=0이면 루프 미진입

**1-2. `scene_build_bvh()` 수정**

```c
void	scene_build_bvh(t_scene *scene)
{
	t_object_ref	*refs;
	int				pc;
	int				bc;

	if (!(scene->flags & SCENE_BVH_ENABLED) || scene->objects.count == 0)
		return ;
	if (!scene->bvh)
		scene->bvh = bvh_create();
	if (!scene->bvh)
		return ;
	pc = count_planes(scene);
	bc = scene->objects.count - pc;
	if (!alloc_bvh_refs(scene, &refs, pc, bc))
	{
		free(refs);
		return ;
	}
	fill_separated_refs(scene, refs);
	if (bc > 0)
		bvh_build(scene->bvh, refs, bc, scene);
	free(refs);
}
```

- 20줄, 3변수, 1파라미터 → Norm 준수
- 할당 실패 시 fill_separated_refs 호출 차단
- `free(refs)`: alloc_bvh_refs 내부에서 `*refs = NULL`로 초기화했으므로 안전
- `if (bc > 0)` 단순화 근거: alloc_bvh_refs 성공(return 1) 시 bc>0이면 refs≠NULL 보장

### Phase 2: R3 — FOV 경계값 제한

**2-1. `parse_elements.c` 수정**

```c
// before
if (!in_range(fov, 0, 180))
// after
if (!in_range(fov, 1, 179))
```

- 상수 변경만, 줄 수 변화 없음
- `fov`는 `int` → `in_range(fov, 1, 179)` == `1 ≤ fov ≤ 179` == `0 < fov < 180`

### Phase 3: R4 — set_shadow_samples() LUT 재생성

**3-1. `shadow_config.c` 수정**

```c
void	set_shadow_samples(t_shadow_config *config, int samples)
{
	if (!config || samples < 1)
		return ;
	free(config->offset_lut);
	config->offset_lut = NULL;
	config->samples = samples;
	init_shadow_offset_lut(config);
}
```

- 8줄, 0변수 → Norm 준수
- 기존 LUT 해제 → samples 갱신 → 새 크기로 LUT 재생성
- `init_shadow_offset_lut()` 내부에서 malloc 실패 시 `offset_lut = NULL` 유지 → fallback 경로 (`generate_shadow_sample_offset`) 자동 사용

## 검증 계획

### 정상 경로

```bash
# 빌드
make re

# Norminette
norminette src/spatial/bvh_init.c src/parser/parse_elements.c src/lighting/shadow_config.c

# 기존 벤치마크 (성능 변화 없음 확인)
./miniRT scenes/perf/perf_timing.rt
./miniRT scenes/perf/perf_all_objects.rt

# FOV 경계값 테스트
# FOV=0 → Error 출력 확인 (PARSE_ERR_RANGE)
# FOV=180 → Error 출력 확인 (PARSE_ERR_RANGE)
# FOV=1, FOV=179 → 정상 렌더 확인
```

### 실패 경로 검증 (smoke test)

실패 경로는 런타임에서 재현이 어렵기 때문에 **임시 코드 패치 → 빌드 → 실행 → 확인 → 리버트** 방식으로 검증합니다. 최종 커밋에는 포함하지 않습니다.

**R1/R2: alloc_bvh_refs() 실패 경로**

주의: 삽입 지점은 반드시 cleanup 코드(free + count=0 + indices=NULL) **이후**여야 합니다.
함수 첫 줄에 삽입하면 cleanup이 스킵되어 실제 실패 상태와 다른 상태를 테스트하게 됩니다.

```bash
# 1. alloc_bvh_refs() 내 cleanup 3줄 이후에 임시 코드 삽입
#    (src/spatial/bvh_init.c의 alloc_bvh_refs 함수)
#    scene->bvh->plane_refs.indices = NULL; 다음 줄에:
#      *refs = NULL;
#      return (0);
#    → cleanup 완료 후 할당 실패를 시뮬레이션
# 2. make re && ./miniRT scenes/perf/perf_all_objects.rt
# 3. 확인: 크래시 없이 brute-force 렌더 정상 동작 (모든 오브젝트 표시)
#    근거: bvh->root=NULL → trace_ray()가 check_all_objects() fallback 사용
# 4. 리버트: git checkout src/spatial/bvh_init.c
```

**R4: offset_lut NULL fallback 경로**

```bash
# 1. init_shadow_config()에서 init_shadow_offset_lut(&config) 호출을 임시 주석 처리
#    → offset_lut = NULL 강제
# 2. make re && ./miniRT scenes/perf/perf_timing.rt
# 3. 확인: 그림자 렌더링 정상 (generate_shadow_sample_offset fallback 사용)
# 4. 리버트: git checkout src/lighting/shadow_config.c
```

## 수정 파일 요약

| 파일 | 이슈 | 변경 내용 | 줄 수 변화 |
|------|------|-----------|------------|
| `src/spatial/bvh_init.c` | R1+R2 | alloc_bvh_refs 추출 (실패 cleanup 포함), scene_build_bvh 리팩터 | +24 (함수 추가), -6 (기존 축소) |
| `src/parser/parse_elements.c` | R3 | in_range 상수 변경 | 0 |
| `src/lighting/shadow_config.c` | R4 | set_shadow_samples LUT 재생성 | +4 |

## Complexity Tracking

> 위반 사항 없음. 모든 수정이 Norm v4.1 제약 내에서 처리됩니다.
