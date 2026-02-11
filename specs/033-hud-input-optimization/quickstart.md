# Quickstart: HUD / Input / Rendering Optimization

**Feature**: 033-hud-input-optimization | **Date**: 2026-02-10

---

## 빌드

```bash
# 클린 빌드
make fclean && make

# norminette 검증
norminette src/ includes/
```

## 테스트 씬

```bash
# 기본 테스트 (오브젝트 다수)
./miniRT scenes/valid/perf_all_objects.rt

# 간단한 씬 (빠른 렌더)
./miniRT scenes/valid/three_spheres.rt
```

## 수동 테스트 시나리오

### 1. 디바운스 상태 머신 테스트

| 시나리오 | 입력 | 기대 동작 |
|----------|------|----------|
| 단일 탭 | W 1회 | 즉시 LQ → 150ms 후 FQ |
| 빠른 연속 탭 | W 3회 (<150ms 간격) | LQ 여러 번(스로틀) → FQ 1회 |
| 느린 연속 탭 | W 3회 (300ms 간격) | 각 탭마다 LQ + FQ 사이클 (쿨다운이 ACTIVE 복귀 보장) |
| Key-hold | W 꾹 누름 | LQ ~20fps → 키 뗀 후 FQ 1회 |
| FQ 직후 탭 | W 누름 → FQ 완료 직후 W 누름 | COOLDOWN에서 ACTIVE 복귀 → LQ → FQ |

### 2. 데드 코드 제거 테스트

| 시나리오 | 입력 | 기대 동작 |
|----------|------|----------|
| KEY_I 제거 | I 키 | 아무 동작 없음 (렌더 트리거 없음) |
| HUD 정상 동작 | H 키 | HUD 표시/숨김 정상 |

### 3. pixel_timing 게이트 테스트

| 시나리오 | 조건 | 기대 동작 |
|----------|------|----------|
| 플래그 꺼짐 (기본) | RENDER_ENABLE_PIXEL_TIMING 미설정 | stdout에 pixel timing 출력 없음 |
| 플래그 켜짐 | RENDER_ENABLE_PIXEL_TIMING 설정 | 기존과 동일한 pixel timing 출력 |

### 4. 키맵 테스트 (Option A + 키보드 그리드)

| 기능 | 키 입력 | 기대 동작 |
|------|---------|----------|
| Light X 이동 | `[` / `]` | Light X- / X+ |
| Light Y 이동 | `;` / `'` | Light Y- / Y+ |
| Light Z 이동 | `,` / `.` | Light Z- / Z+ |
| Obj Resize X | `Y` / `U` | 오브젝트 X축 리사이즈 |
| Obj Resize Y | `N` / `M` | 오브젝트 Y축 리사이즈 (기존 유지) |
| Obj Rotate X | `I` / `J` | 오브젝트 X축 회전 |
| Obj Rotate Y | `O` / `K` | 오브젝트 Y축 회전 |
| Obj Rotate Z | `P` / `L` | 오브젝트 Z축 회전 |
| 오브젝트 선택 | TAB / SHIFT+TAB | 다음/이전 선택 |
| HUD 페이지 | SHIFT+, / SHIFT+. | HUD 페이지 Up/Down |

### 5. 키가이드 테스트

| 시나리오 | 기대 동작 |
|----------|----------|
| 키가이드 표시 | Light, Resize, Rotate, HUD 키 모두 표시 |
| "Display:" 섹션 | 제거됨 (I-Info 없음) |
| 선택 키 표시 | `TAB - Select` 표시 |

## 회귀 테스트

```bash
# 모든 유효 씬 렌더링 확인
for scene in scenes/valid/*.rt; do
    echo "Testing: $scene"
    timeout 30 ./miniRT "$scene" &
    sleep 2
    kill %1 2>/dev/null
done
```

- 모든 씬에서 정상 렌더링 확인
- ESC 키 정상 종료 확인
- 윈도우 닫기 정상 종료 확인
