# Research: 025-dead-code-removal

## R1: render_state.h 타입 참조 범위

**질문**: render_state.h의 타입(t_render_state, t_quality_mode, t_interaction_state, t_progressive_state, t_tile_rect)이 삭제 대상 파일 외부에서 참조되는가?

**결론**: 모든 타입이 완전히 격리됨. render_state.h 전체 삭제 가능.

**근거**:
- `t_render_state`: render_state.c, render_quality.c에서만 사용
- `t_quality_mode`: render_state.h 내부 + render_quality.c에서만 사용
- `t_interaction_state`: render_state.h 내부 + render_state.c, render_quality.c에서만 사용
- `t_progressive_state`: render_state.h 내부 + render_progressive.c에서만 사용
- `t_tile_rect`: render_progressive.h + render_progressive.c에서만 사용
- render_state.h 자체를 include하는 파일: render_quality.h, render_progressive.h (둘 다 삭제 대상)

**대안 검토**: 타입만 보존하고 함수만 삭제하는 옵션 → 불필요. 참조 없음 확인됨.

---

## R2: render_quality.h / render_progressive.h 삭제 가능 여부

**질문**: 이 헤더들이 삭제 대상 .c 파일 외부에서 include되는가?

**결론**: 둘 다 삭제 가능.

**근거**:
- `render_quality.h`: render_quality.c, render_state.c에서만 include (둘 다 삭제 대상)
- `render_progressive.h`: render_progressive.c에서만 include (삭제 대상)

---

## R3: print_error 전환 전략

**질문**: 구체적 에러 메시지 유지 vs 일반 에러 코드 전환

**결론**: 대안 A (일반 코드) 채택

**근거**:
- `ERR_PARSE_MISSING` 코드가 이미 존재하며 "Missing required element in scene file" 메시지 제공
- validate_scene의 4개 케이스 모두 "필수 요소 누락" 범주이므로 의미적으로 정확
- 42 Norm 25줄 제한 내에서 구현 가능 (comma operator 패턴)
- 에러 코드 추가(대안 C)는 enum 확장 + 메시지 배열 수정 필요하여 과잉

**대안 검토**:
- 대안 B (error_context_print): validate_scene에 t_error_context 매개변수 추가 필요 → 시그니처 변경 → 호출자 변경 연쇄. 과잉.
- 대안 C (전용 코드): ERR_MISSING_AMBIENT 등 4개 코드 추가. 기능상 이점 낮음 대비 변경 범위 과다.

---

## R4: cleanup_all 치환 시 변수 제한

**질문**: close_window에서 scene 포인터 저장 시 Norm 5변수 제한 초과 여부

**결론**: 안전. 현재 변수 1개(render), 추가 1개(scene) = 2개. 제한 내.

---

## R5: Makefile 동기화

**질문**: 삭제 대상 .c 파일 8개의 Makefile SRCS 행 위치

**결론**:
- `$(SRC_DIR)/spatial/aabb_shapes.c` → Makefile line 105 부근
- `$(SRC_DIR)/render/render_state.c` → Makefile line 98
- `$(SRC_DIR)/render/render_quality.c` → Makefile line 99
- `$(SRC_DIR)/render/render_progressive.c` → Makefile line 100
- `$(SRC_DIR)/utils/format_object_id.c` → Makefile line 116
- `$(SRC_DIR)/utils/ft_atof.c` → Makefile line 117
- `$(SRC_DIR)/parser/parse_validation.c` → Makefile line 77
- `$(SRC_DIR)/utils/cleanup.c` → Makefile line 114
