# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.4.0] - 2026-03

### Refactored
- **Input module split**: `input_dispatch.c`, `input_camera.c`, `input_objects.c`, `input_resize.c`, `input_rotate.c` moved from `src/window/` to `src/input/`
- **Metrics module split**: `metrics_*.c` moved from `src/render/` to `src/metrics/`
- **File renames**: `trace.c` → `render_trace.c`, `camera.c` → `render_camera.c`, `intersect_cyl_new.c` → `intersect_cylinder.c`, `vector.c` → `vec3.c`, `bounds.c` → `aabb_bounds.c`
- **Dead code removal**: Removed pixel_timing module, unused `handle_key_release`, dead HUD files
- **Main refactoring**: Extracted `init_scene` and `load_bump_maps` helpers

### Fixed
- **Linux cleanup**: Added `mlx_destroy_display` call for proper X11 cleanup
- **Shadow sampling**: Fixed ceiling division for per-light sample adjustment
- **Safety guards**: NaN guards in utils/bvh_vis, buffer overflow fixes in HUD, zero-normal safety in cylinder/cone intersection

### Docs
- Updated all docs/ module pages for file renames and module splits
- Updated all wiki/ pages: key bindings, debounce FSM, resolution, multi-light, cone, bonus options

## [2.3.0] - 2026-02

### Added
- **Object Resize**: Y/U 키로 반지름, N/M 키로 원기둥 높이 조절
- **Object Rotation**: I/J (X축), O/K (Y축), P/L (Z축) Rodrigues 회전
- **LQ Preview Throttle**: 디바운스 중 50ms 간격 low quality 프리뷰

### Changed
- **Debounce FSM Redesign**: 4-상태 머신 IDLE → ACTIVE (150ms) → FINAL → COOLDOWN (350ms)으로 단순화. PREVIEW 상태 제거
- **Keymap Overhaul**: 광원 이동 키를 `[`/`]`/`;`/`'`/`,`/`.`으로 변경, 오브젝트 선택을 TAB으로 통일, HUD 페이지를 Up/Down으로 변경
- **Render Flags**: `RENDER_SHOW_INFO`, `RENDER_SHIFT_HELD` 제거

### Removed
- **Dead Code Cleanup**: I 키 정보 토글 핸들러, shift 키 핸들링, 미사용 키 정의 (Insert, Home, PgUp, Delete, End, PgDn, Left, Right) 제거

## [2.2.0] - 2026-02

### Added
- **Plane BVH Separation**: Plane을 BVH 트리에서 분리하여 bounded 오브젝트만으로 트리 구축
- **Shadow BVH Any-Hit**: Shadow ray에 early-exit BVH 순회 적용
- **Shadow Offset LUT**: cos/sin/sqrt 사전 계산으로 shadow sampling 가속
- **Math Optimizations**: `fast_pow32`, sphere sqrt 캐싱, shadow magnitude 통합
- **inv_dir Precompute**: 레이 생성 시 `1/direction` 사전 계산 (AABB 교차 최적화)
- **Camera Basis Caching**: 카메라 basis 벡터(right, up) 프레임당 1회 계산 후 캐시
- **BVH Child Ordering**: 레이 방향 기준 가까운 자식 우선 순회
- **Metrics Print on Startup**: 시작 시 성능 메트릭 자동 출력

### Changed
- **Shadow BVH Threshold**: 20 → 5로 하향 (더 작은 씬에서도 BVH 활용)

### Fixed
- **Shadow offset LUT**: 샘플 수 변경 시 LUT 재생성
- **Parser FOV**: FOV 범위를 exclusive (0, 180)으로 수정
- **BVH malloc guard**: `scene_build_bvh`에서 malloc 실패 시 안전 처리
- **plane_refs leak**: BVH 재구축 시 plane_refs 메모리 누수 수정
- **Parser line buffer**: `finalize_line`에서 null-terminate 보장
- **Parser failure cleanup**: parse 실패 시 미처리 라인 메모리 해제

### Performance
- S4 (혼합 18obj): **27.2s → 6.1s (77.7% 개선)**
- S3 (50 spheres): 3.4s → 1.4s (57.4% 개선)
- Shadow tests (S4): 369M → 83.7M (-77.4%)
- BVH skip rate (S4): 32.9% → 82.0%

## [2.1.0] - 2026-01-15

### Added
- **BVH Acceleration**: 기본 활성화, 복잡한 씬에서 2~10배 렌더링 가속
- **Debounced Rendering**: 150ms 디바운스 타이머 기반 렌더링 최적화
- **BVH Tree Visualization**: `--bvh-vis` 플래그로 BVH 트리 구조 콘솔 출력
- **Unified Object Identifiers**: 일관된 오브젝트 식별자 체계 (sp-1, pl-1, cy-1, co-1)
- `RENDER_RENDERING` 비트 플래그로 정확한 렌더 진행 추적

### Changed
- 100% Norminette 준수 달성
- 헤더 인클루드 구조 개선

### Fixed
- BVH 활성화 시 검정 화면 문제
- 렌더 취소 로직 (실제 렌더링 중일 때만 취소)

### Performance
- BVH: 복잡한 씬에서 2~10배 가속
- 디바운스: 입력 중 불필요 렌더 90%+ 감소

## [2.0.0] - 2026-01

### Added
- 전면 코드 리팩토링 및 모듈화
- 통합 오브젝트 구조체 (`t_object` union)
- 문서 구조화

## [1.2.0] - 2025-12

### Added
- Soft Shadows (스토캐스틱 다중 샘플링)
- Specular Reflection (Phong 모델)
- Adaptive Shadow Bias

## [1.1.0] - 2025-12

### Added
- HUD 씬 정보 표시
- 렌더링 최적화

## [1.0.1] - 2025-12

### Fixed
- 마이너 버그 수정

## [1.0.0] - 2025-12

### Added
- 초기 릴리스
- 기본 레이 트레이싱 (구, 평면, 원기둥)
- Phong 조명 (Ambient + Diffuse)
- .rt 씬 파일 파서
- 카메라/오브젝트 인터랙티브 조작
- MiniLibX 윈도우 시스템
