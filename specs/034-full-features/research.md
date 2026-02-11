# Research: 034-full-features

**Date**: 2026-02-10

## 1. Cone Intersection Algorithm

**Decision**: Cylinder 패턴(body quadratic + cap disk) 기반으로 원뿔 교차 구현
**Rationale**: 기존 `intersect_cyl_new.c`의 body+cap 구조를 그대로 재활용 가능. 원뿔은 축을 따라 반지름이 선형 변화하는 2차 곡면이므로 이차방정식 계수만 변경.
**Alternatives**:
- Apex 기준 정의: 수학적으로 더 단순하지만, 기존 원기둥(center 기준)과 불일치하여 이동/회전 코드 재사용이 어려움 → 기각

**원뿔 교차 공식 (center 기준)**:
- 축 방향 `v`, 중심 `C`, half_height `h`, 밑면 반지름 `r`
- 꼭짓점 `apex = C + v * h`, 밑면 중심 `base = C - v * h`
- 임의 높이 `m`에서 반지름: `r_at_m = r * (h - m) / (2h)` (apex에서 0, base에서 r)
- body 교차: 이차방정식 `at² + bt + c = 0` (원기둥과 유사, 테이퍼 보정항 추가)
- cap 교차: 밑면 disk만 (꼭짓점은 점이므로 cap 불필요)

## 2. UV 매핑 전략

**Decision**: hit point에서 on-the-fly로 UV 계산, t_hit 구조체에 UV 필드 추가하지 않음
**Rationale**: UV는 체커보드/범프맵 적용 시에만 필요하고, 모든 교차에 UV를 계산하면 불필요한 오버헤드. 체커보드/범프맵 함수 내부에서 hit point + object data로 직접 계산.
**Alternatives**:
- t_hit에 UV 필드 추가: 매 교차마다 UV 계산 → 불필요한 성능 비용 → 기각

**오브젝트별 UV 계산**:
- 구: `u = 0.5 + atan2(z,x)/(2π)`, `v = 0.5 - asin(y)/π`
- 평면: hit point의 두 축 좌표를 스케일로 나눔
- 원기둥: 측면은 `u = atan2()/2π`, `v = height/total_height`; 캡은 평면과 유사
- 원뿔: 원기둥과 동일한 패턴 (테이퍼 보정 불필요 — UV는 각도+높이 기반)

## 3. 다중 광원 데이터 구조

**Decision**: `t_light light` → `t_light lights[MAX_LIGHTS]` + `int light_count` + `int selected_light`
**Rationale**: 동적 배열(malloc)보다 고정 배열이 42 Norm과 메모리 관리에 적합. 광원 수는 실무적으로 10개 이하.
**Alternatives**:
- 동적 배열 (`t_light *lights`): 유연하지만 malloc/free 관리 복잡 → 기각
- 연결 리스트: 순차 접근에 적합하지만 인덱스 접근 불가 → 기각

**MAX_LIGHTS**: 16 (충분한 여유, 메모리 영향 미미)

## 4. 체커보드 패턴 구현 위치

**Decision**: `apply_lighting()` 진입 직후, 조명 계산 전에 hit->color를 체커보드 패턴으로 교체
**Rationale**: 조명 계산은 color를 입력으로 사용하므로, color 교체가 가장 자연스러운 hook point.
**Alternatives**:
- 교차 시점에서 color 변경: 교차 코드와 텍스처 코드가 결합됨 → 기각
- 별도 후처리: 조명 효과가 체커보드에 적용되지 않음 → 기각

## 5. 범프맵 구현 전략

**Decision**: XPM 파일을 MiniLibX `mlx_xpm_file_to_image()`로 로드, 그레이스케일 높이맵으로 해석, 법선 교란. **지연 로딩(deferred load) 패턴** 사용.
**Rationale**: MiniLibX가 XPM 로딩을 기본 지원. 추가 라이브러리 불필요.
**Alternatives**:
- PNG 사용 (`mlx_png_file_to_image`): MiniLibX에서 지원하지만 subject가 XPM 언급 → XPM 우선
- 수학적 procedural bump: 파일 불필요하지만 subject 요구사항에 부합하지 않음 → 기각
- 파싱 시점에 XPM 로드: `mlx_init()`이 파싱 이후(`init_render_ctx`)에 호출되므로 파싱 시점에 mlx 포인터가 존재하지 않아 **불가능** → 기각

**범프맵 파이프라인** (지연 로딩):
1. 파싱 시 filepath만 저장 (`obj->bump_path = ft_strdup(path)`, `obj->bump_map = NULL`)
2. `mlx_init()` 완료 후, 렌더 초기화 단계에서 모든 오브젝트 순회하며 XPM 로드 (`load_all_bump_maps(scene, mlx)`)
3. 로드 실패 시 "Error\n" + 메시지 출력 후 프로그램 종료
4. 조명 계산 전 UV 계산 → 텍스처 샘플링 → 기울기(gradient) 계산
5. tangent/bitangent 벡터 계산 → 법선 교란 → 교란된 법선으로 조명

## 6. 잔존 이슈 분석

### 6-1. BVH total_nodes 하드코딩
**Decision**: `total_nodes` 필드를 삭제하고, 필요 시 트리 순회로 동적 계산
**Rationale**: `total_nodes`는 bvh_vis (시각화/통계)에서만 읽히고, 하드코딩된 1이므로 어차피 부정확. 유지보다 삭제가 안전.
**Alternatives**:
- 재귀 빌드 시 카운트 증가: Norm 25줄 제한으로 빌드 함수에 로직 추가 어려움 → 기각

### 6-2. HUD 배경 렌더링
**Finding**: 033에서 `hud_render_background()`와 `hud_render_background_row()`가 dead code로 **이미 삭제됨**. 선언도 제거 완료. 잔존 이슈 아님.

### 6-3. BVH 재빌드 실패 경로
**Decision**: `scene_build_bvh()` 실패 시 기존 BVH 트리를 유지하는 것을 명시적 설계로 확정, ft_printf로 경고 출력
**Rationale**: BVH는 최적화 구조이므로, 실패 시 이전 유효한 트리를 유지하는 것이 brute-force 폴백보다 나음. 사일런트 실패 대신 stderr 경고 추가.

## 7. lighting.c 파일 분할

**Decision**: lighting.c (5/5 함수)를 분할하여 다중 광원 루프 + 체커보드/범프맵 hook 수용
**Rationale**: 현재 lighting.c에 clamp_color, fast_pow32, calculate_specular, calc_lighting_factor, apply_lighting = 5함수 (Norm 한도). 함수 추가 불가.
**분할 계획**:
- `lighting_utils.c`: `clamp_color()`, `fast_pow32()` 이동 (범용 유틸리티)
- `lighting.c`: `calculate_specular()`, `calc_lighting_factor()`, `apply_lighting()` 유지 (3/5) → 체커보드/범프맵 hook은 apply_lighting 내부 if문으로 처리, 별도 함수 불필요
**Alternatives**:
- apply_lighting을 분할하여 multi_light_loop 함수 추출: 4파라미터 제한으로 인터페이스 복잡 → 기각 (루프를 apply_lighting 내부에 유지)

## 8. 다중 광원 + 소프트 섀도 성능

**Decision**: 광원 수에 따라 섀도 샘플 수를 동적 조절
**Rationale**: 현재 소프트 섀도 16샘플. 광원 16개면 픽셀당 256 섀도 레이 → 800×600에서 1.2억 섀도 레이, 인터랙티브 조작 불가능.
**완화 전략**: `effective_samples = max(1, base_samples / light_count)`. 광원 1개=16샘플, 4개=4샘플, 8개 이상=하드 섀도(1샘플). 총 섀도 레이 수를 일정하게 유지.
**Alternatives**:
- 항상 하드 섀도 폴백: 단일 광원에서도 소프트 섀도 손실 → 기각
- 샘플 수 고정: 다중 광원 시 성능 폭발 → 기각

## 9. 보너스 옵션 파싱 전략

**Decision**: `checker:`/`bump:` 순서 자유, 미인식 토큰은 파싱 에러
**Rationale**: 순서 고정은 사용자 불편. 오브젝트 라인의 표준 파라미터 파싱 후 남은 토큰을 루프로 순회하며 `checker:` 또는 `bump:` prefix를 검사. 매칭되지 않는 토큰이 있으면 에러 반환.
**Alternatives**:
- 순서 고정 (checker → bump): 파서 구현은 단순하지만 사용자 혼란 유발 → 기각
- 미인식 토큰 무시: 오타 발견 불가, 디버깅 어려움 → 기각

## 10. Makefile bonus Rule

**Decision**: `bonus: all` (기존 `all` 타겟과 동일)
**Rationale**: 보너스 기능을 기존 소스에 통합하므로 별도 컴파일이 불필요. subject는 `bonus` rule 존재만 요구.
