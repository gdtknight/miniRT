# Tasks: Camera Yaw Rotation & Bonus Features

**Input**: Design documents from `/specs/034-full-features/`
**Prerequisites**: plan.md (required), spec.md (required), research.md, data-model.md, contracts/

**Tests**: 수동 시각 검증 + norminette (spec 기준)

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US4)
- Include exact file paths in descriptions

---

## Phase 1: Shared Infrastructure

**Purpose**: Header 확장 및 BVH 잔존 이슈 수정. 모든 user story의 전제 조건.

**⚠️ CRITICAL**: 이 Phase 완료 전에는 user story 작업 불가

- [ ] T001 `includes/objects.h` 확장 — `OBJ_CONE` enum 추가, `t_cone_data` struct 정의 (`center`, `axis`, `radius`, `radius_sq`, `half_height`), `union u_object_data`에 `cone` 멤버 추가, `t_object`에 `checker_color` (`t_color`), `has_checker` (`int`), `bump_path` (`char *`, 파싱 시 filepath 저장), `bump_map` (`struct s_bump_map *`, mlx_init 후 지연 로드) 필드 추가. data-model.md §1-3 참조
- [ ] T002 [P] `includes/texture.h` 신규 생성 — `t_bump_map` struct 정의 (`img`, `data`, `width`, `height`, `bpp`, `size_line`, `endian`), `checkerboard_color()`, `bump_map_load()`, `bump_map_destroy()`, `bump_perturb_normal()` 함수 선언. data-model.md §4, contracts/texture-pipeline.md 참조
- [ ] T003 [P] `includes/window_internal.h` — macOS/Linux 분기에 `KEY_1` (18/49), `KEY_3` (20/51), `KEY_EQUAL` (24/61) 정의 추가. contracts/keymap.md 참조
- [ ] T004 [P] `includes/spatial.h` — `t_bvh` struct에서 `total_nodes` 필드 삭제. data-model.md §7
- [ ] T005 [P] `src/spatial/bvh_build_core.c` — `bvh->total_nodes = 1;` 라인 삭제 (하드코딩 제거)
- [ ] T006 [P] `src/spatial/bvh_lifecycle.c` — `total_nodes` 초기화 코드 삭제
- [ ] T007 `src/spatial/bvh_vis_stats.c` — `total_nodes` 참조를 트리 순회 기반 동적 카운트로 대체 (T004-T006 완료 후)
- [ ] T008 [P] `src/spatial/bvh_init.c` — `scene_build_bvh()` 실패 경로에 `ft_printf("Warning: BVH rebuild failed\n")` 경고 추가. 기존 BVH 유지하는 설계를 명시적으로 확정. research.md §6-3
- [ ] T008a `includes/ray.h` + 교차 함수 — `t_hit`에 `t_object *obj` 필드 추가 (data-model.md §6). `intersect_sphere.c`, `intersect_plane.c`, `intersect_cyl_new.c`의 hit 기록 시 `hit->obj = obj` 설정. 체커보드/범프맵 접근에 필수
- [ ] T008b `src/lighting/lighting.c` + `src/lighting/lighting_utils.c` — lighting.c 분할: `clamp_color()`, `fast_pow32()`를 신규 `lighting_utils.c`로 이동 (기존 5/5 → 3/5). 다중 광원 루프 + 텍스처 hook 수용 공간 확보. research.md §7

**Checkpoint**: 헤더 확장, BVH 정리, t_hit.obj, lighting 분할 완료. `make re`로 컴파일 검증.

---

## Phase 2: US1 — Camera Yaw Rotation (Priority: P1)

**Goal**: 숫자키 1/3으로 카메라 좌우(yaw) 회전 기능 추가

**Independent Test**: 씬 로드 → 키 1/3 → 화면 좌/우 회전 확인

- [ ] T009 [US1] `src/window/window_camera.c` — `handle_camera_yaw(t_render *render, int keycode)` 함수 추가. Y축 `(0,1,0)` 기준으로 기존 `rotate_dir()` 함수 재사용. KEY_1 = 반시계 5도, KEY_3 = 시계 5도. 회전 후 방향 벡터 정규화 유지. contracts/keymap.md 참조
- [ ] T010 [US1] `src/window/window_key_handlers.c` — `handle_camera_keys()` 함수에 KEY_1/KEY_3 분기 추가하여 `handle_camera_yaw()` 호출. (T009 완료 후)
- [ ] T011 [US1] `src/keyguide/keyguide_render.c` 또는 해당 파일 — Camera 섹션에 `"1/3 - Yaw"` 문자열 추가

**Checkpoint**: Camera yaw 동작 확인 — `scenes/test_yaw.rt`로 검증. S키 리셋 시 yaw 포함 초기화 확인.

---

## Phase 3: US4 — Colored & Multi-spot Lights (Priority: P2)

**Goal**: 다중 광원 지원 + 광원별 독립 그림자 + `=` 키로 광원 전환

**Independent Test**: 다색 광원 2개 이상 씬 → 혼합 조명 + 독립 그림자 확인

- [ ] T012 [US4] `includes/minirt.h` — `t_scene` struct 변경: `t_light light` → `t_light lights[MAX_LIGHTS]`, `int light_count`, `int selected_light` 필드 추가. `#define MAX_LIGHTS 16` 정의. data-model.md §5, contracts/multi-light.md
- [ ] T013 [US4] `src/parser/parse_elements.c` + `includes/minirt.h` — `parse_light()` 함수에서 `scene_has_light()`/`SCENE_HAS_LIGHT` 중복 체크 제거, `light_count` 기반 배열 추가 로직으로 대체. `light_count >= MAX_LIGHTS`이면 오버플로우 에러 반환. `minirt.h`에서 `SCENE_HAS_LIGHT` (0x04) 플래그 삭제. 파싱 완료 후 `light_count == 0`이면 에러 (FR-010). (T012 완료 후)
- [ ] T014 [US4] `scene->light.` 참조 일괄 변경 — 대상 파일: `src/lighting/lighting.c`, `src/parser/parse_elements.c`, `src/window/window_objects.c`, `src/hud/hud_scene.c`. `scene->light.` → `scene->lights[i].` (루프) 또는 `scene->lights[scene->selected_light].` (조작). 파서/HUD에서의 단일 광원 참조도 모두 수정. (T012 완료 후)
- [ ] T015 [US4] `src/lighting/lighting.c` — `apply_lighting()` 함수를 다중 광원 루프로 변경. `for i = 0 to light_count-1` 순회하며 각 광원의 diffuse + specular 기여를 누적. 최종 RGB 클램핑 [0,255]. 소프트 섀도 샘플 동적 조절: `effective_samples = max(1, base_samples / light_count)`. contracts/multi-light.md 파이프라인/성능 참조. (T008b, T014 완료 후)
- [ ] T016 [US4] `src/lighting/shadow_test.c` — 그림자 계산 함수가 광원 위치를 파라미터로 받도록 변경 (기존 `scene->light.position` 직접 참조 대신). `apply_lighting()` 루프에서 각 광원별로 호출. (T015와 함께 수정)
- [ ] T017 [US4] `src/window/window_key_handlers.c` — KEY_EQUAL 분기 추가. `selected_light = (selected_light + 1) % light_count` 순환 로직. contracts/keymap.md
- [ ] T018 [US4] `src/window/window_objects.c` — `handle_light_move()` 함수에서 `scene->light.` 대신 `scene->lights[scene->selected_light].` 사용하도록 변경
- [ ] T019 [US4] `src/keyguide/` 해당 파일 — Light 섹션에 `"= - Next Light"` 문자열 추가

**Checkpoint**: `scenes/test_multilight.rt`로 3색 광원 혼합, 독립 그림자, `=` 키 전환 검증.

---

## Phase 4: US5 — Cone Object (Priority: P2)

**Goal**: 원뿔(co) 오브젝트의 파싱, 교차, 렌더링, 조작 구현

**Independent Test**: 원뿔이 포함된 씬 → 올바른 형태, 그림자, 이동/회전/리사이즈 확인

- [ ] T020 [US5] `src/ray/intersect_cone_body.c` 신규 — 원뿔 측면 교차 함수 `intersect_cone_body()`. apex 기준 이차방정식(`a`, `b`, `c` 계수 계산), 판별식, 높이 범위 검증(`-2h ≤ m ≤ 0`). 법선: 축 투영점→교차점 벡터 + 테이퍼 보정. 내부 교차 시 법선 뒤집기. contracts/cone-intersection.md 참조
- [ ] T021 [P] [US5] `src/ray/intersect_cone_cap.c` 신규 — 원뿔 밑면(base) disk 교차 함수 `intersect_cone_cap()`. 기존 원기둥 cap 교차 패턴 재사용. 꼭짓점은 점이므로 base cap만 구현. 법선 = -axis 방향
- [ ] T022 [US5] `src/ray/intersect_object.c` — `intersect_object()` switch/if 분기에 `OBJ_CONE` 케이스 추가. `intersect_cone_new(ray, obj, hit)` 호출 (cylinder 패턴과 일관 — body/cap은 static, 통합 함수에서 color+hit->obj 설정). (T020, T021, T008a 완료 후)
- [ ] T023 [US5] `src/parser/parse_cone.c` 신규 — `parse_cone()` 함수. 형식: `co [center] [axis] [diameter] [height] [R,G,B]`. center=t_vec3, axis=정규화 벡터([-1,1] 검증), diameter>0→radius=d/2, height>0→half_height=h/2, color=[0,255]. `radius_sq = radius * radius` 캐시 계산
- [ ] T024 [US5] `src/parser/parser_dispatch.c` — 디스패치 테이블/분기에 `"co"` → `parse_cone()` 추가. (T023 완료 후)
- [ ] T025 [US5] BVH AABB 계산 — 원뿔용 바운딩 박스 계산 함수 추가. center ± half_height*axis 범위 + radius 오프셋으로 AABB 계산. 기존 원기둥 AABB 로직 참조하여 해당 파일에 추가
- [ ] T026 [P] [US5] `src/window/window_resize.c` — 원뿔 리사이즈 핸들러 추가. radius(지름)와 height를 독립적으로 변경. 기존 원기둥 리사이즈 패턴 참조
- [ ] T027 [P] [US5] `src/window/window_rotate.c` — 원뿔 회전 핸들러 추가. axis 벡터 변경. 기존 원기둥 회전 패턴 참조

**Checkpoint**: `scenes/test_cone.rt`로 원뿔 형태, 이동/회전/리사이즈, 그림자 검증.

---

## Phase 5: US3 — Checkerboard Pattern (Priority: P2)

**Goal**: 오브젝트 표면에 체커보드(바둑판) 패턴 적용

**Independent Test**: 평면/구/원기둥에 `checker:R,G,B` 옵션 → 바둑판 패턴 확인

- [ ] T028 [US3] `src/texture/checkerboard.c` 신규 — `checkerboard_color(t_object *obj, t_hit *hit)` 함수. 오브젝트별 UV 계산: 평면=법선에서 tangent/bitangent 기저 벡터 구한 뒤 로컬 좌표 투영 `floor(u/scale)+floor(v/scale)`, 구=구면좌표 `floor(u*freq)+floor(v*freq)`, 원기둥/원뿔=`floor(theta*freq)+floor(h*freq)`. `CHECKER_SCALE = 2.0`. 짝수=`obj->color`, 홀수=`obj->checker_color` 반환. contracts/texture-pipeline.md 참조
- [ ] T029 [US3] `src/parser/parse_bonus_options.c` 신규 — 오브젝트 파싱 후 남은 토큰에서 `checker:R,G,B` 패턴 감지 및 파싱. `obj->has_checker = 1`, `obj->checker_color` 설정. 잘못된 형식이면 PARSE_ERR 반환
- [ ] T030 [US3] 기존 파서 통합 — `parse_sphere()`, `parse_plane()`, `parse_cylinder()`, `parse_cone()`에서 표준 파라미터 파싱 후 `parse_bonus_options()` 호출 추가. Norm 25줄 제한 주의. (T029 완료 후)
- [ ] T031 [US3] `src/lighting/lighting.c` — `apply_lighting()` 함수 진입부에 체커보드 hook 추가: `if (hit->obj && hit->obj->has_checker) hit->color = checkerboard_color(hit->obj, hit)`. 조명 계산 전에 색상 교체. `hit->obj`는 T008a에서 추가된 필드. contracts/texture-pipeline.md 통합 위치 참조. (T028, T008a 완료 후)

**Checkpoint**: `scenes/test_checker.rt`로 평면(흑백), 구(적황), 원기둥(청백), 원뿔(녹황) 체커보드 검증.

---

## Phase 6: US6 — Bump Map Textures (Priority: P3)

**Goal**: XPM 범프맵으로 오브젝트 표면에 요철 효과 적용

**Independent Test**: 구에 `bump:brick.xpm` 옵션 → 울퉁불퉁한 표면 확인

- [ ] T032 [US6] `src/texture/bump_map_load.c` 신규 — `bump_map_load(void *mlx, char *filepath)`: `mlx_xpm_file_to_image()`로 XPM 로드, `mlx_get_data_addr()`로 픽셀 데이터 접근. 실패 시 NULL 반환. `bump_map_destroy(void *mlx, t_bump_map *bmap)`: 이미지 destroy + free. contracts/texture-pipeline.md 참조
- [ ] T033 [US6] `src/texture/bump_map_perturb.c` 신규 — `bump_perturb_normal(t_object *obj, t_hit *hit)`: UV 계산(오브젝트별) → 텍스처 샘플 `h(u,v)` 그레이스케일 → 기울기 `du`, `dv` 계산 → tangent/bitangent 벡터 → `perturbed = normalize(normal + strength*(du*T + dv*B))`. `strength = 1.0`. 원본 `hit->normal` 미변경, 새 벡터 반환. contracts/texture-pipeline.md 범프 알고리즘 참조
- [ ] T034 [US6] `src/parser/parse_bonus_options.c` 확장 — 기존 checker 파싱에 `bump:filepath.xpm` 패턴 추가. **지연 로딩**: filepath만 저장 (`obj->bump_path = ft_strdup(path)`, `obj->bump_map = NULL`). XPM 로드는 하지 않음 (mlx_init 이전이므로 불가). research.md §5, contracts/texture-pipeline.md 로딩 타이밍 참조
- [ ] T034a [US6] 렌더 초기화 — `mlx_init()` 완료 후 `load_all_bump_maps(scene, mlx)` 호출 추가. 모든 오브젝트 순회하며 `bump_path != NULL`이면 `bump_map_load()` 호출. 로드 실패 시 "Error\n" + 종료. contracts/texture-pipeline.md 로딩 타이밍 참조. (T032, T034 완료 후)
- [ ] T035 [US6] `src/lighting/lighting.c` — 체커보드 hook 다음에 범프맵 hook 추가: `if (hit->obj && hit->obj->bump_map) normal = bump_perturb_normal(hit->obj, hit)`. 교란된 법선을 이후 조명 계산에 사용. (T033 완료 후)
- [ ] T036 [US6] 프로그램 종료 시 메모리 해제 — 모든 오브젝트의 `bump_map`이 NULL이 아니면 `bump_map_destroy()` 호출 + `bump_path` free. 기존 cleanup 경로에 추가

**Checkpoint**: `scenes/test_bump.rt`로 구 표면 요철 효과 검증. `scenes/test_combined.rt`로 checker+bump 복합 검증. 없는 XPM 파일 → "Error\n" 메시지 검증.

---

## Phase 7: US2 + Polish & Cross-Cutting

**Purpose**: Makefile bonus rule, HUD/Keyguide 업데이트, 최종 검증

- [ ] T037 [US2] `Makefile` — `bonus: all` rule 추가 + `.PHONY`에 `bonus` 추가. 신규 소스 파일 전체 등록: `intersect_cone_body.c`, `intersect_cone_cap.c`, `parse_cone.c`, `parse_bonus_options.c`, `checkerboard.c`, `bump_map_load.c`, `bump_map_perturb.c` (각 Phase 구현 시 점진적으로 추가하되, 최종 검증)
- [ ] T038 [P] HUD 업데이트 — `src/hud/hud_render_*.c`에 원뿔 정보 표시 (type=CONE, center, axis, radius, height) 추가. 선택된 광원 인덱스 `Light [i/n]` 형태 표시 추가
- [ ] T038a [P] `textures/brick.xpm` 준비 — 범프맵 테스트용 XPM 텍스처 파일 생성 또는 배치. 최소 32x32 그레이스케일 이미지. `scenes/test_bump.rt` 및 `scenes/test_combined.rt`에서 참조
- [ ] T039 [P] 테스트 씬 파일 작성 — quickstart.md의 7개 테스트 씬을 `scenes/` 디렉토리에 생성: `test_yaw.rt`, `test_checker.rt`, `test_multilight.rt`, `test_cone.rt`, `test_bump.rt`, `test_combined.rt` (checker+bump 복합)
- [ ] T040 norminette 전체 검증 — `norminette src/ includes/` 실행, 에러 0건 확인. 모든 신규/수정 파일이 42 Norm v4.1 준수
- [ ] T041 메모리 누수 검증 — 프로그램 종료 시 범프맵 이미지, BVH, 오브젝트 리스트 등 모든 동적 메모리 해제 확인

**Checkpoint**: `make bonus` 빌드 성공. quickstart.md 수동 검증 체크리스트 전항목 통과.

---

## Dependencies & Execution Order

### Phase Dependencies

- **Phase 1 (Infrastructure)**: No dependencies — 즉시 시작 가능
- **Phase 2 (US1 Camera Yaw)**: Phase 1의 T003 완료 필요 (KEY_1/KEY_3 정의)
- **Phase 3 (US4 Multi-Light)**: Phase 1의 T001, T008b 완료 필요 (헤더 확장 + lighting 분할)
- **Phase 4 (US5 Cone)**: Phase 1의 T001, T008a 완료 필요 (OBJ_CONE + t_hit.obj)
- **Phase 5 (US3 Checkerboard)**: Phase 1의 T001, T008a 완료 필요 (checker 필드 + hit->obj) + Phase 3 권장 (lighting.c 수정 순서)
- **Phase 6 (US6 Bump Map)**: Phase 1의 T001-T002, T008a 완료 필요 + Phase 5의 T029 완료 필요 (parse_bonus_options.c 공유)
- **Phase 7 (Polish)**: 모든 user story Phase 완료 후

### Within-Phase Dependencies

- T009 → T010 (handle_camera_yaw 정의 → 핸들러에서 호출)
- T012 → T013, T014 (헤더 변경 → 파서/전역 참조 수정)
- T014 → T015, T016, T017, T018 (전역 참조 수정 → 조명/그림자/키/이동)
- T020, T021 → T022 (body+cap 교차 → 통합 디스패치)
- T023 → T024 (파서 → 디스패치 등록)
- T008a → T031, T035 (hit->obj 필드 → 체커보드/범프맵 hook)
- T008b → T015 (lighting 분할 → 다중 광원 루프)
- T028 → T031 (체커보드 함수 → 조명 hook)
- T029 → T030 (파서 함수 → 기존 파서 통합)
- T032, T034 → T034a (범프 로드 함수 + 파서 → 렌더 초기화에서 일괄 로드)
- T033 → T035 (범프 교란 → 조명 hook)

### Parallel Opportunities

- Phase 1: T002, T003, T004, T005, T006, T008, T008a, T008b는 서로 독립, 병렬 가능
- Phase 2와 Phase 3/4: Phase 1 완료 후 병렬 진행 가능
- Phase 4: T020과 T021 병렬 가능, T026과 T027 병렬 가능
- Phase 7: T038과 T039 병렬 가능

### Recommended Execution Order

1. Phase 1 전체 → 컴파일 검증
2. Phase 2 (US1) → yaw 동작 검증
3. Phase 3 (US4) → 다중 광원 검증
4. Phase 4 (US5) → 원뿔 검증
5. Phase 5 (US3) → 체커보드 검증
6. Phase 6 (US6) → 범프맵 검증
7. Phase 7 (Polish) → 최종 검증

---

## Summary

| Phase | User Story | Priority | Tasks | 설명 |
|-------|-----------|----------|-------|------|
| 1 | — | — | T001-T008b (10) | 헤더 확장 + BVH 잔존 이슈 + t_hit.obj + lighting 분할 |
| 2 | US1 | P1 | T009-T011 (3) | Camera Yaw 회전 |
| 3 | US4 | P2 | T012-T019 (8) | 다중 광원 시스템 |
| 4 | US5 | P2 | T020-T027 (8) | 원뿔 오브젝트 |
| 5 | US3 | P2 | T028-T031 (4) | 체커보드 패턴 |
| 6 | US6 | P3 | T032-T036 (6) | 범프맵 텍스처 (지연 로딩) |
| 7 | US2 | P1 | T037-T041 (6) | Makefile + Polish + XPM 테스트 파일 |
| **Total** | | | **45 tasks** | |
