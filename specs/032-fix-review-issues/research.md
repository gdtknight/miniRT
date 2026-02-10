# Research: 032-fix-review-issues

**Date**: 2026-02-10

## R1: malloc 실패 시 NULL 역참조

### 현상 분석

`scene_build_bvh()` (bvh_init.c:111-118)에서:
- `plane_refs.indices = malloc(sizeof(int) * pc)` — pc > 0일 때 실패 가능
- `refs = malloc(sizeof(t_object_ref) * bc)` — bc > 0일 때 실패 가능
- `fill_separated_refs(scene, refs)` — 두 할당 결과 무관하게 무조건 호출

`fill_separated_refs()` 내부:
- `scene->bvh->plane_refs.indices[pi++] = i` → indices가 NULL이면 크래시
- `refs[bi++].index = i` → refs가 NULL이면 크래시

### 설계 결정

**Decision**: 할당+검증을 `alloc_bvh_refs()` helper로 추출

**Rationale**:
- `scene_build_bvh()`가 이미 24줄 → 인라인 가드 추가 시 25줄 초과
- Helper 추출로 두 함수 모두 Norm 제한 내 유지
- 파일 함수 수: 4 → 5 (한계이나 초과 아님)

**Alternatives considered**:
1. `fill_separated_refs()` 내부에서 NULL 체크 → 의미론적으로 부적절 (silent skip)
2. scene_build_bvh에서 인라인 가드 → 25줄 초과
3. 별도 파일로 분리 → 과도한 분할

## R2: plane_refs.indices 재빌드 누수

### 현상 분석

재빌드 트리거:
- `window_objects.c:74` — 오브젝트 조작
- `window_resize.c:100` — 리사이즈
- `window_rotate.c:136` — 회전

`scene_build_bvh()` 재호출 시:
- `scene->bvh`는 이미 존재 (bvh_create 스킵)
- line 112: `scene->bvh->plane_refs.indices = NULL` — 기존 포인터 해제 없이 덮어씀
- `bvh_build()` 내부의 `bvh_node_destroy()`는 트리 노드만 해제, plane_refs는 별도

### 설계 결정

**Decision**: `alloc_bvh_refs()` 진입 시점에서 `free(scene->bvh->plane_refs.indices)` 호출

**Rationale**:
- R1의 helper 함수에 자연스럽게 포함
- `free(NULL)`은 C 표준에서 no-op 보장 → 초기 빌드 시에도 안전
- 단일 지점에서 해제·할당 관리

## R3: FOV 경계값

### 현상 분석

`parse_camera()` in `parse_elements.c`: `in_range(fov, 0, 180)` — `>=`/`<=` 비교
- FOV=0: `tan(0) = 0` → 모든 ray 동일 방향, 단색 화면
- FOV=180: `tan(π/2)` ≈ 1.633e16 → 수치 불안정 (IEEE 754 double의 M_PI/2 ≠ 정확한 π/2)

42 subject 원문: "FOV : Horizontal field of view in degrees in range [0,180]"

### 설계 결정

**Decision**: `in_range(fov, 1, 179)` (exclusive bounds via integer type)

**Rationale**:
- `fov`는 `int` 타입 → `1 ≤ fov ≤ 179` == `0 < fov < 180`
- FOV=0, FOV=180은 수학적으로 무의미한 값
- Subject의 `[0,180]`은 입력 형식 범위이지 유효 범위가 아님
- 코드 변경: 상수 2개만 변경, 줄 수 동일

**수학적 재현 근거** (camera.c `update_camera_cache` 기준):

| FOV | `fov * 0.5 * π / 180` | `tan(...)` = fov_scale | 렌더 결과 |
|-----|----------------------|----------------------|-----------|
| 0 | 0.0 | **0.0** | ray direction = camera direction 고정. 모든 픽셀이 동일 ray → 단색 화면 |
| 1 | 0.00873 | 0.00873 | 극히 좁은 시야. 정상 렌더 |
| 90 | 0.7854 | 1.0 | 표준 시야. 정상 렌더 |
| 179 | 1.5621 | 572.96 | 매우 넓은 시야. 정상 렌더 (극단적 왜곡) |
| 180 | π/2 = 1.5708 | **1.633e16** | fov_scale이 ~10^16. pixel_pos 성분이 10^16 스케일. `vec3_normalize` 시 direction 성분이 무시되어 투영 무의미 |

- FOV=0: `fov_scale=0` → `pixel_pos = camera->direction + 0*right + 0*up` → 1296000개 ray 모두 동일 방향. "렌더링"이라 할 수 없는 단색 출력
- FOV=180: `fov_scale≈1.633e16` → `pixel_pos` 의 right/up 성분이 direction 대비 ~10^16배. `vec3_normalize` 후 direction 기여가 소실. 수학적 특이점

**파서 거부가 안전한 이유**:
- 42 subject VI조: "The program MUST NOT crash under any circumstances" → FOV=180에서 NaN 전파 시 위반 가능
- 42 subject V조: "visual correctness" → FOV=0의 단색 출력은 시각적 정확성 요건에 부합 불가
- `in_range(fov, 1, 179)`는 subject `[0,180]` 문구의 엄밀한 축소이나, 수학적 특이점을 제거하여 VI조 안정성을 보장
- Subject 문구와의 불일치 리스크가 잔존하므로, 평가 시 이 결정을 설명할 수 있어야 함

**Alternatives considered (상세 비교)**:

| 대안 | 장점 | 단점 | 결정 |
|------|------|------|------|
| A. 파서에서 거부 `(0,180)` | 입력 단계에서 차단, 코드 변경 최소 | Subject `[0,180]` 문구와 불일치 가능 | **채택** |
| B. camera.c에서 clamp | Subject 문구 준수, 파서 통과 | 렌더 단에서 값을 변조하는 것은 silent behavior 변경 |  기각 |
| C. camera.c에서 fov_scale 안전 범위 제한 | 가장 유연 | 복잡도 증가, clamp 범위 선정이 자의적 | 기각 |
| D. 그대로 유지 | 변경 없음, Subject 문자 준수 | FOV=180 시 수치 불안정, 평가 시 "의미 없는 렌더링" 감점 가능 | 기각 |

**최종 근거**: 대안 A는 단일 상수 변경으로 가장 단순하며, FOV=0/180이 수학적 특이점(tan(0)=0, tan(π/2)≈∞)이라는 사실은 재현 가능한 객관적 근거. Subject `[0,180]` 문구와의 불일치 리스크를 인지하되, VI조(크래시 방지) + V조(시각 정확성) 준수를 우선함.

## R4: set_shadow_samples() LUT 미갱신

### 현상 분석

`set_shadow_samples()` in `shadow_config.c`:
```c
void set_shadow_samples(t_shadow_config *config, int samples)
{
    if (config && samples >= 1)
        config->samples = samples;  // offset_lut 크기 유지
}
```

`init_shadow_offset_lut()`: `malloc(sizeof(t_vec3) * config->samples)` — 초기 samples(16) 크기
`sample_shadow_ray()`: `offset_lut[index]` — index가 samples-1까지 → 증가 시 힙 오버리드

현재 호출처: 테스트 코드만 (`tests/test_shadow_config.c`), 프로덕션 미사용

### 설계 결정

**Decision**: `set_shadow_samples()` 내에서 기존 LUT free + `init_shadow_offset_lut()` 호출

**Rationale**:
- `init_shadow_offset_lut()`은 같은 파일 내 static → 추가 선언 불필요
- malloc 실패 시 `offset_lut = NULL` → `sample_shadow_ray()`의 fallback 경로 자동 사용
- 함수 수 변경 없음 (기존 함수 수정만), 8줄 → Norm 준수
- Public API 방어: 향후 호출 시 안전성 보장

**Alternatives considered**:
1. API 제거 (dead code) → 헤더에 선언 있으므로 계약 위반
2. 주석으로 경고만 → 근본 해결 아님
3. LUT 없이 항상 `generate_shadow_sample_offset()` → 성능 저하 (LUT 목적 무효화)
