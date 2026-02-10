# Feature Specification: 032-fix-review-issues

**Branch**: `032-fix-review-issues` | **Date**: 2026-02-10
**Parent**: `031-perf-bottleneck-optimization` (Round 2 코드 리뷰 후속)

## 개요

031 피처의 코드 리뷰에서 발견된 4건의 이슈를 수정합니다.
기능 변경 없이 안정성·메모리 안전성·방어적 코딩만 보강합니다.

## 요구사항

### R1 [심각]: BVH 재빌드 시 malloc 실패 NULL 역참조 방지

- **파일**: `src/spatial/bvh_init.c`
- **현상**: `scene_build_bvh()`에서 `plane_refs.indices` 또는 `refs` malloc 실패 시 `fill_separated_refs()`가 무조건 호출되어 NULL 역참조
- **수정**: malloc 실패 시 `fill_separated_refs()` 호출 전에 early return
- **검증**: malloc 실패 경로에서 크래시 없음

### R2 [높음]: BVH 재빌드 시 plane_refs.indices 메모리 누수 방지

- **파일**: `src/spatial/bvh_init.c`
- **현상**: 재빌드 시 기존 `plane_refs.indices`를 free하지 않고 새로 대입
- **수정**: 새 할당 전 기존 `plane_refs.indices` free
- **검증**: 재빌드 반복 시 메모리 누수 없음

### R3 [중간]: 카메라 FOV 경계값 제한

- **파일**: `src/parser/parse_elements.c`
- **현상**: FOV 0은 시야각 없음, 180은 tan(90°)로 수치 폭발
- **수정**: `in_range(fov, 0, 180)` → 경계값 제외 (0 < fov < 180)
- **검증**: FOV 0, 180 입력 시 PARSE_ERR_RANGE 반환
- **정책 결정**: Subject 원문 `[0,180]`에서 `(0,180)`으로 축소.
  - FOV=0: `tan(0)=0` → 모든 ray 동일 방향 → 렌더링 불가 (V조 시각적 정확성 위반)
  - FOV=180: `tan(π/2)≈1.633e16` → 수치 특이점 → NaN 전파 가능 (VI조 크래시 방지 위반)
  - `fov`는 `int` 타입이므로 `in_range(fov, 1, 179)` == `0 < fov < 180`
  - 상세 수학적 근거: [research.md](./research.md) R3절 참조

### R4 [중간/잠재]: set_shadow_samples() LUT 재생성

- **파일**: `src/lighting/shadow_config.c`
- **현상**: 샘플 수 변경 시 offset_lut 크기 미갱신 → 힙 오버리드 가능
- **수정**: `set_shadow_samples()` 내에서 기존 LUT free + 재생성
- **검증**:
  - 샘플 수 증가 후 shadow ray 정상 동작
  - Fallback 경로 smoke test: `init_shadow_config()` 내 `init_shadow_offset_lut()` 호출을 임시 주석 처리 → `offset_lut = NULL` 강제 → 빌드 → 렌더링 → 그림자 정상 동작 확인 → 리버트

## 비기능 요구사항

- 42 Norm v4.1 준수 (25줄/함수, 5변수/함수, 4파라미터/함수, 5함수/파일)
- 렌더링 결과 변경 없음
- 기존 벤치마크 성능 변화 없음

## 범위 외

- 새로운 기능 추가 없음
- 기존 API 시그니처 변경 없음 (R4는 내부 동작만 보강)
