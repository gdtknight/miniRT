# Implementation Plan: Camera Yaw Rotation & Bonus Features

**Branch**: `034-full-features` | **Date**: 2026-02-10 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `/specs/034-full-features/spec.md`

## Summary

필수 요구사항 미충족 2건(카메라 yaw 회전, Makefile bonus rule)과 보너스 기능 4건(체커보드, 다중 광원, 원뿔, 범프맵), 잔존 이슈 2건(BVH total_nodes, BVH 실패 경로)을 구현한다. Specular reflection(Phong)은 이미 구현되어 있으므로 제외.

## Technical Context

**Language/Version**: C (42 Norm v4.1 compliant)
**Primary Dependencies**: MiniLibX, libft, math library (-lm)
**Storage**: N/A (in-memory rendering)
**Testing**: 수동 시각 검증 + norminette
**Target Platform**: macOS (Darwin) / Linux
**Project Type**: Single project (raytracer)
**Performance Goals**: 기존 디바운스 파이프라인 재사용 (033에서 구현 완료, 본 기능은 신규 성능 목표 없음)
**Constraints**: 25줄/함수, 5변수/함수, 5함수/파일, 4파라미터/함수
**Scale/Scope**: ~50 소스 파일, ~5000 LOC

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

| Principle | Status | Notes |
|-----------|--------|-------|
| I. Norminette Compliance | PASS | 모든 신규 코드 Norm 준수 설계 |
| II. Memory Safety | PASS | 범프맵 XPM 로드/해제 경로 설계, 프로그램 종료 시 cleanup |
| III. Allowed Functions | PASS | mlx_xpm_file_to_image는 MiniLibX 함수 |
| IV. Modular Architecture | PASS | 텍스처/원뿔 별도 디렉토리, 5함수/파일 제한 준수 |
| V. Visual Correctness | PASS | 원뿔+체커보드+범프맵 = 보너스 렌더링 |
| VI. Stability | PASS | BVH 실패 경로 명시화 포함, NULL 가드 |
| VII. Window Management | PASS | 신규 키 추가만, 기존 이벤트 루프 유지 |

## Project Structure

### Documentation (this feature)

```text
specs/034-full-features/
├── plan.md
├── research.md
├── data-model.md
├── quickstart.md
├── contracts/
│   ├── cone-intersection.md
│   ├── texture-pipeline.md
│   ├── multi-light.md
│   └── keymap.md
└── tasks.md
```

### Source Code (신규/수정 파일)

```text
includes/
├── objects.h              # OBJ_CONE, t_cone_data, checker/bump/bump_path 필드
├── minirt.h               # lights[] 배열, light_count, selected_light, SCENE_HAS_LIGHT 삭제
├── ray.h                  # t_hit에 t_object *obj 추가
├── texture.h              # 신규: t_bump_map, 체커보드/범프맵 함수 선언
├── window_internal.h      # KEY_1, KEY_3, KEY_EQUAL 추가
└── spatial.h              # total_nodes 삭제 반영

src/
├── ray/
│   ├── intersect_cone_body.c    # 신규: 원뿔 측면 교차
│   ├── intersect_cone_cap.c     # 신규: 원뿔 밑면 교차
│   └── intersect_object.c       # OBJ_CONE 케이스 추가
├── parser/
│   ├── parse_cone.c             # 신규: 원뿔 파서
│   ├── parse_bonus_options.c    # 신규: checker:/bump: 파싱 (순서 자유, 미인식 토큰 에러)
│   ├── parse_elements.c         # L 중복 체크 제거, 배열 추가, L≥1 검증
│   └── parser_dispatch.c        # "co" 디스패치 추가
├── texture/
│   ├── checkerboard.c           # 신규: 체커보드 패턴 (구/평면/원기둥/원뿔)
│   ├── bump_map_load.c          # 신규: XPM 로드/해제
│   └── bump_map_perturb.c       # 신규: 법선 교란
├── lighting/
│   ├── lighting.c               # apply_lighting + specular + factor (3/5 → hook 여유)
│   └── lighting_utils.c         # 신규: clamp_color, fast_pow32 이동 (기존 5/5 → 분할)
├── window/
│   ├── window_camera.c          # handle_camera_yaw 추가
│   ├── window_key_handlers.c    # KEY_1/3/EQUAL 핸들링
│   ├── window_objects.c         # selected_light 기반 이동
│   └── window_resize.c          # 원뿔 리사이즈 추가
├── spatial/
│   ├── bvh_build_core.c         # total_nodes 삭제
│   ├── bvh_lifecycle.c          # total_nodes 삭제
│   └── bvh_init.c               # 실패 경로 경고 추가
├── hud/
│   └── hud_render_*.c           # 원뿔 정보, 광원 인덱스 표시
├── keyguide/
│   └── keyguide_render*.c       # 1/3 Yaw, = Next Light 추가
└── Makefile                     # bonus rule + 신규 파일 추가
```

## Implementation Phases

### Phase 1: Camera Yaw + Makefile bonus (P1 — 필수 보완)
1. `window_internal.h`: KEY_1, KEY_3 정의 (macOS + Linux)
2. `window_camera.c`: `handle_camera_yaw()` — Y축 기준 5도 회전 (기존 `rotate_dir()` 재사용)
3. `window_key_handlers.c`: `handle_camera_keys()`에 KEY_1/KEY_3 분기 추가
4. `keyguide_render.c`: Camera 섹션에 "1/3 - Yaw" 추가
5. `Makefile`: `bonus: all` rule + `.PHONY` 추가 (보너스 기능은 기본 빌드에 통합, 별도 바이너리 없음 — FR-005)

### Phase 2: 잔존 이슈 수정 + 공유 인프라
1. `bvh_build_core.c`: `bvh->total_nodes = 1;` 라인 삭제
2. `bvh_lifecycle.c`: `total_nodes` 초기화 삭제
3. `spatial.h` (또는 해당 헤더): `total_nodes` 필드 삭제
4. `bvh_vis_stats.c`: `total_nodes` 참조를 트리 순회 카운트로 대체
5. `bvh_init.c`: `scene_build_bvh()` 실패 경로에 ft_printf 경고 추가
6. `ray.h`: `t_hit`에 `t_object *obj` 필드 추가 — 체커보드/범프맵 접근용 (data-model.md §6)
7. 교차 함수(`intersect_sphere.c`, `intersect_plane.c`, `intersect_cyl_new.c` 등): hit 기록 시 `hit->obj = obj` 설정
8. `lighting.c` → `lighting.c` + `lighting_utils.c` 분할: `clamp_color()`, `fast_pow32()`를 `lighting_utils.c`로 이동 (5/5 → 3/5, 다중 광원 루프 + 텍스처 hook 여유 확보) — research.md §7

### Phase 3: Multi-Light 시스템
1. `minirt.h`: `t_light light` → `t_light lights[MAX_LIGHTS]` + `light_count` + `selected_light`. `SCENE_HAS_LIGHT` 플래그 삭제
2. `parse_elements.c`: `parse_light()` — `scene_has_light()`/`SCENE_HAS_LIGHT` 중복 체크 제거, `light_count` 기반 배열 추가 로직으로 대체. 파싱 완료 후 `light_count == 0`이면 에러 (FR-010). `light_count >= MAX_LIGHTS`이면 오버플로우 에러 (FR-010a)
3. `lighting.c`: `apply_lighting()` — 다중 광원 루프 (Phase 2에서 lighting_utils.c 분할로 여유 확보됨)
4. `shadow_calc.c`/`shadow_test.c`: 광원별 독립 그림자
5. `window_internal.h`: KEY_EQUAL 정의
6. `window_key_handlers.c`: `=` 키 → `select_next_light()`
7. `window_objects.c`: `handle_light_move()` — `lights[selected_light]` 사용
8. 기존 코드의 모든 `scene->light.` 참조를 `scene->lights[0].` 또는 루프로 변경

### Phase 4: Cone Object
1. `objects.h`: `OBJ_CONE` enum, `t_cone_data` struct, union 멤버 추가
2. `intersect_cone_body.c` (신규): 원뿔 측면 이차방정식 교차
3. `intersect_cone_cap.c` (신규): 원뿔 밑면 disk 교차
4. `intersect_object.c`: `OBJ_CONE` 케이스 디스패치
5. `parse_cone.c` (신규): "co" 파서
6. `parser_dispatch.c`: "co" 디스패치 추가
7. `window_resize.c`: 원뿔 리사이즈 (radius + height)
8. `window_rotate.c`: 원뿔 회전 (axis 변경)
9. BVH AABB 계산: 원뿔용 바운딩 박스

### Phase 5: Checkerboard Pattern
1. `objects.h`: `t_object`에 `checker_color`, `has_checker` 필드 추가
2. `texture.h` (신규): 체커보드 함수 선언
3. `checkerboard.c` (신규): 오브젝트별 UV 계산 + 패턴 판별 (구/평면/원기둥/원뿔 — FR-007)
4. `parse_bonus_options.c` (신규): `checker:R,G,B` 파싱 (순서 자유 — FR-009a, 미인식 토큰 에러 — FR-009b)
5. `lighting.c`: 조명 계산 전 체커보드 색상 교체 hook

### Phase 6: Bump Map Textures
1. `texture.h`: `t_bump_map` struct, 범프맵 함수 선언
2. `objects.h`: `t_object`에 `t_bump_map *bump_map` + `char *bump_path` 필드 추가 (Phase 1에서 이미 추가됨 — 확인)
3. `bump_map_load.c` (신규): XPM 로드/해제 + `load_all_bump_maps(scene, mlx)` 일괄 로드 함수
4. `bump_map_perturb.c` (신규): UV 계산 + 법선 교란
5. `parse_bonus_options.c`: `bump:filepath.xpm` 파싱 — **filepath만 저장** (`obj->bump_path = ft_strdup(path)`, `obj->bump_map = NULL`). XPM 로드는 하지 않음 (지연 로딩 — research.md §5)
6. 렌더 초기화 (`mlx_init` 후): `load_all_bump_maps(scene, mlx)` 호출 — 모든 오브젝트 순회하며 `bump_path`가 있으면 XPM 로드. 실패 시 "Error\n" + 종료
7. `lighting.c`: 조명 계산 전 법선 교란 hook
8. 메모리 해제: 프로그램 종료 시 범프맵 이미지 destroy + `bump_path` free

### Phase 7: HUD/Keyguide + Polish
1. HUD: 원뿔 정보 표시, 선택된 광원 인덱스 표시
2. Keyguide: 신규 키 안내 추가
3. norminette 전체 검증
4. 테스트 씬 파일 작성
5. 메모리 누수 검증

## Complexity Tracking

해당 없음 — Constitution Check 위반 없음
