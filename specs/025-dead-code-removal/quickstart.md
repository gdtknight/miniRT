# Quickstart: 025-dead-code-removal

## 작업 요약

miniRT 코드베이스에서 검증된 dead code를 제거하는 리팩토링 작업.
새로운 기능 없음. 삭제/치환만 수행.

## 실행 순서

### Phase 1: 무참조 파일/선언 삭제

1. 즉시 삭제 (10개 = FR-001: 9 + FR-002: render_state.h): overlay.h, format_object_id.h, render_quality.h, render_progressive.h, render_state.h, aabb_shapes.c, render_state.c, render_quality.c, render_progressive.c, format_object_id.c
2. 헤더 정리: objects.h (typedef 3개), parser.h (선언 2개), window_internal.h (선언 8개), keyguide.h (선언 1개)
3. 함수 삭제: keyguide_render_background (keyguide_render.c)
4. Makefile 동기화: 5개 .c 행 제거 (Phase 1분)

### Phase 2: 의존성 분리

5. `in_range` 이동: parse_validation.c → parse_validation_strict.c
6. parse_validation.c 삭제 + Makefile
7. ft_atof.c 삭제 + minirt.h 선언 제거 + Makefile

### Phase 3: 래퍼/에러 경로 치환

8. cleanup_all → render_destroy + scene_destroy (window_lifecycle.c)
9. cleanup.c 삭제 + minirt.h 선언 제거 + Makefile
10. print_error → error_print 전환 (parser.c 7곳)
11. print_error 함수/선언 삭제 (error.c, error.h)

### Phase 4: 검증

12. `make re`
13. `norminette src/ includes/`
14. 정상 씬 렌더링 확인
15. 비정상 씬 18개 에러 출력 확인

## 핵심 주의사항

- `render_destroy(render)` 호출 전에 `render->scene`을 지역 변수에 저장 (FR-006)
- `print_error`는 0 반환, `error_print`는 1 반환 → comma operator `(error_print(code), 0)` 패턴 사용 (FR-007)
- `in_range`는 parser.h에 이미 선언 존재 → 헤더 변경 불필요 (FR-005)

## 검증 명령

```bash
make re
norminette src/ includes/
./miniRT scenes/valid/basic.rt
for f in tests/scenes/invalid/*.rt; do ./miniRT "$f" 2>&1; done
```
