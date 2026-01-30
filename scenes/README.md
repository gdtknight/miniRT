# miniRT 테스트 장면 파일 가이드 (정리본)

**작성일**: 2026-01-30  
**목적**: 테스트 장면을 목적별로 통합 정리

---

## 📁 디렉토리 구조

```
scenes/
  valid/    # 정상 렌더링/기능 테스트
  perf/     # 성능/스트레스 테스트
  invalid/  # 오류/파서 실패 케이스
  archive/  # 이전 문서 아카이브
```

---

## ✅ valid/ (정상 케이스)

### 사용자 스토리(US) 기반
- `valid_us01_basic.rt` / `valid_us01_sphere_only.rt` / `valid_us01_multiple.rt`
- `valid_us02_parse.rt`
- `valid_us03_lighting_ambient.rt` / `valid_us03_lighting_diffuse.rt` / `valid_us03_shadows.rt`
- `valid_us04_intersect_sphere.rt` / `valid_us04_intersect_plane.rt`
- `valid_us04_intersect_cylinder.rt` / `valid_us04_intersect_all.rt`
- `valid_us05_window.rt`
- `valid_us06_transformations.rt`
- `valid_us07_sphere_sizes.rt` / `valid_us07_cylinder_sizes.rt`

### 추가 시각/기능 테스트
- `valid_smoke_simple.rt` / `valid_scene_complex.rt`
- `valid_scene_comprehensive.rt` / `valid_scene_overlapping.rt`
- `valid_camera_angle.rt`
- `valid_camera_fov_narrow.rt` / `valid_camera_fov_wide.rt`
- `valid_light_ambient_dark.rt` / `valid_light_ambient_bright.rt`
- `valid_light_inside_object.rt`
- `valid_shadow_soft.rt` (보너스 성격)
- `valid_intersect_inside.rt`
- `valid_transform_rotation.rt` / `valid_transform_resize.rt`

---

## 📈 perf/ (성능/부하 테스트)

- `perf_spheres_20.rt`
- `perf_spheres_50.rt`
- `perf_all_objects.rt` (객체 수 최대)
- `perf_timing.rt`

---

## ❌ invalid/ (오류/파서 실패 케이스)

- 범위/값 오류: `invalid_*_out_of_range.rt`, `invalid_negative_radius.rt`
- 중복/누락: `invalid_ambient_duplicate.rt`, `invalid_missing_sphere.rt`, `invalid_missing_ambient.rt`
- 형식 오류: `invalid_format.rt`, `invalid_syntax.rt`
- 기타: `invalid_camera_bad.rt`, `invalid_light_bad.rt`, `invalid_parse_mid.rt`, `invalid_parse_last.rt`

---

## 🚀 실행 예시

### 정상 케이스 일괄 테스트
```bash
for scene in scenes/valid/*.rt scenes/perf/*.rt; do
  echo "Testing: $scene"
  ./miniRT "$scene" || echo "  ✗ Failed: $scene"
done
```

### 오류 케이스 테스트
```bash
for scene in scenes/invalid/*.rt; do
  echo "Testing: $scene"
  ./miniRT "$scene" 2>&1 | grep -q "Error" && echo "  ✓ Error handled" || echo "  ✗ Missing error"
done
```

---

## 🗂 archive/

`NEW_SCENES_SUMMARY.md`, `TEST_SCENES_README.md`는 내용 중복으로 `archive/`로 이동함.
