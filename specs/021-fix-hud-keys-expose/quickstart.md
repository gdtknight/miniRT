# Quickstart: Fix HUD Key Guide & Expose Restore

**Date**: 2026-01-28

## 구현 순서

### Step 1: KEYGUIDE_HEIGHT 상수 변경
- `includes/keyguide.h`에서 `KEYGUIDE_HEIGHT` 400 → 500

### Step 2: keyguide_render_extra.c 생성
- `src/keyguide/keyguide_render_extra.c` 신규 파일
- Resize 섹션: "Resize:" 헤더 + "J/K - Diameter", "N/M - Height"
- Rotation 섹션: "Rotation:" 헤더 + "U/O - Rot X", "Y/P - Rot Y", "←/→ - Rot Z"

### Step 3: keyguide_render.c 수정
- `keyguide_render()`에서 `keyguide_render_content2()` 호출 후 `keyguide_render_extra()` 호출 추가

### Step 4: handle_expose 수정
- `src/window/window_events.c`의 `handle_expose()`에서 씬 버퍼 출력 후 `hud.visible` 체크하여 `hud_render()` + `keyguide_render()` 호출

### Step 5: Makefile 업데이트
- `keyguide_render_extra.c`를 소스 목록에 추가

### Step 6: Norminette 검증
- 변경된 모든 파일에 `norminette` 실행
- 오류 시 즉시 수정

### Step 7: 불필요 코드 정리
- 변경 과정에서 발견된 미사용 코드/변수 제거

### Step 8: 수동 검증
- SC-001: HUD ON → 창 가림 → 복원 → HUD 표시 확인
- SC-001: HUD OFF → 창 가림 → 복원 → HUD 미표시 확인
- SC-002: 키가이드에 Resize/Rotation 키 10개 표시 확인
- SC-003: expose 전후 프레임 카운터 동일 확인
- SC-004: 레이아웃 겹침/잘림 없음 확인

## 빌드 & 테스트

```bash
make re
./miniRT scenes/basic.rt
# H키로 HUD 표시 → 키가이드 확인 → 다른 창으로 가림 → 복원 → 확인
```
