# miniRT 코드베이스 정리 제안서

**작성일**: 2026-02-08  
**분석 범위**: `src/`, `includes/`, `Makefile`  
**목적**: Dead code, 중복 코드, 의존성 정리 필요 부분 식별 및 조치 방안 수립

---

## 1. Executive Summary

### 1.1 분석 결과 요약

| 카테고리 | 상태 | 이슈 수 | 조치 가능 |
|----------|------|---------|-----------|
| Dead Code | ✅ 양호 | 0 | - |
| 중복 코드 | ⚠️ 개선 필요 | 3 | 1건 |
| Makefile | ✅ 양호 | 0 | - |
| 헤더 의존성 | ⚠️ 정리 필요 | 2 | 2건 |

### 1.2 권장 조치 요약

| # | 조치 | Constitution 준수 | 권장 |
|---|------|-------------------|------|
| 1 | `get_type_count()` 통합 | ✅ PASS | ⭐ 권장 |
| 2 | `hud_text.h` 중복 선언 제거 | ✅ PASS | ⭐ 권장 |
| 3 | `hud_objects.h` 정리 | ✅ PASS | 선택 |

---

## 2. Project Constitution 참조

### 2.1 42 Norm v4.1 핵심 제약

| 제약 | 값 | 검증 방법 |
|------|---|----------|
| 함수 길이 | **25줄** max | `norminette` |
| 변수 개수 | **5개**/함수 max | `norminette` |
| 인자 개수 | **4개**/함수 max | `norminette` |
| 함수 개수 | **5개**/파일 max | `norminette` |
| 라인 폭 | **80컬럼** max | `norminette` |

### 2.2 Constitution Check 7원칙

| 원칙 | 설명 |
|------|------|
| I. Norminette Compliance | Norm v4.1 100% 준수 |
| II. Memory Safety | 메모리 누수 금지, heap 정리 필수 |
| III. Allowed Functions | 허용 함수만 사용 |
| IV. Modular Architecture | 기존 모듈 구조 유지 |
| V. Visual Correctness | 렌더링 결과 동일 (RGB ±1) |
| VI. Stability | segfault, crash 금지 |
| VII. Window Management | ESC, 창 닫기 정상 동작 |

### 2.3 평가 기준 (miniRT_eval.md)

- **Norm 오류 → 0점**
- **메모리 누수 → 0점**
- **금지 함수 사용 → 0점**
- **Crash/Segfault → 0점**
- **relink 발생 → 비기능 판정**

---

## 3. 상세 분석 결과

### 3.1 Dead Code 분석

#### 3.1.1 헤더 선언 (includes/)

**결과**: ✅ 모든 선언에 대응하는 구현 존재

- 22개 헤더 파일 분석 완료
- 미구현 선언 없음
- 025-dead-code-removal에서 이미 정리됨

#### 3.1.2 Static 함수 (src/)

**결과**: ✅ 모든 static 함수가 동일 파일 내에서 호출됨

| 파일 | Static 함수 | 호출 위치 | 상태 |
|------|-------------|-----------|------|
| `hud_color.c` | `blend_component()` | `blend_colors()` 3회 | ✅ |
| `hud_performance.c` | `render_perf_header()` | `hud_render_performance()` | ✅ |
| `hud_performance.c` | `copy_str()` | `concat_and_print()` | ✅ |
| `hud_performance.c` | `concat_and_print()` | `render_perf_basic()` | ✅ |
| `hud_performance.c` | `render_perf_basic()` | `hud_render_performance()` | ✅ |
| `bvh_traverse.c` | `ray_goes_positive()` | `traverse_children()` | ✅ |
| `bvh_traverse.c` | `bvh_leaf_intersect()` | `bvh_node_intersect()` | ✅ |
| `bvh_traverse.c` | `traverse_children()` | `bvh_node_intersect()` | ✅ |
| `parse_cylinder.c` | `get_cylinder_count()` | `parse_cylinder()` | ✅ |

#### 3.1.3 Makefile SRCS

**결과**: ✅ 모든 87개 소스 파일 존재 확인

- 누락된 파일 없음
- 고아 파일 없음 (SRCS에 없는 .c 파일 없음)

---

### 3.2 중복 코드 분석

#### 3.2.1 🔴 [HIGH] Object Type Counting 중복

**위치**:
- `src/parser/parse_objects.c:25-38` - `get_type_count()`
- `src/parser/parse_cylinder.c:24-37` - `get_cylinder_count()`

**중복 코드 비교**:

```c
// parse_objects.c:25-38 - 범용 버전
static int	get_type_count(t_scene *scene, t_object_type type)
{
	int	i;
	int	count;

	count = 0;
	i = 0;
	while (i < scene->objects.count)
	{
		if (scene->objects.items[i].type == type)
			count++;
		i++;
	}
	return (count);
}

// parse_cylinder.c:24-37 - 실린더 전용 (중복)
static int	get_cylinder_count(t_scene *scene)
{
	int	i;
	int	count;

	count = 0;
	i = 0;
	while (i < scene->objects.count)
	{
		if (scene->objects.items[i].type == OBJ_CYLINDER)
			count++;
		i++;
	}
	return (count);
}
```

**문제점**:
- 완전히 동일한 로직, 15줄 중복
- `get_cylinder_count(scene)` ≡ `get_type_count(scene, OBJ_CYLINDER)`

**사용처**:
| 파일 | 호출 | 용도 |
|------|------|------|
| `parse_objects.c:93` | `get_type_count(scene, OBJ_SPHERE)` | sphere ID 생성 |
| `parse_objects.c:149` | `get_type_count(scene, OBJ_PLANE)` | plane ID 생성 |
| `parse_cylinder.c:127` | `get_cylinder_count(scene)` | cylinder ID 생성 |

---

#### 3.2.2 🟡 [MEDIUM] Radius Squared 캐싱 패턴 중복

**위치** (6개소):
| 파일 | 라인 | 코드 |
|------|------|------|
| `window_resize.c` | 37-39 | `obj->radius_sq = obj->radius * obj->radius;` |
| `window_resize.c` | 63-64 | (cylinder radius) |
| `window_resize.c` | 72 | (cylinder radius) |
| `parse_objects.c` | 59-60 | sphere radius_sq |
| `parse_cylinder.c` | 65-66 | cylinder radius_sq |

**현재 패턴**:
```c
obj->data.sphere.radius = diameter / 2.0;
obj->data.sphere.radius_sq = obj->data.sphere.radius * obj->data.sphere.radius;
```

**권장 조치**: 매크로 도입 (선택적)
```c
#define SET_RADIUS(obj, rad) \
    do { (obj)->radius = (rad); (obj)->radius_sq = (rad) * (rad); } while(0)
```

**Constitution 검토**:
- Norm v4.1: 매크로는 리터럴/상수 값에만 사용 권장
- 이 매크로는 코드 동작을 포함하므로 **권장하지 않음**

**결론**: ❌ **조치 보류** - Norm 정신에 맞지 않음

---

#### 3.2.3 🟡 [MEDIUM] String Copy 유틸리티 분산

**위치**:
- `src/hud/hud_performance.c:37` - `copy_str()`
- `src/hud/hud_text.c` - `copy_str_to_buf()`

**현황**:
- 유사한 문자열 복사 함수가 여러 파일에 static으로 존재
- 각 파일이 5함수 제한에 가까움

**결론**: ❌ **조치 보류** - 파일 분리 시 Makefile 수정 필요, 영향도 대비 이득 적음

---

### 3.3 헤더 의존성 분석

#### 3.3.1 🟡 [MEDIUM] 중복 선언: `hud_mark_dirty()`

**선언 위치**:
| 파일 | 라인 | 선언 |
|------|------|------|
| `includes/hud.h` | 217 | `void hud_mark_dirty(t_render *render);` |
| `includes/hud_text.h` | 18 | `void hud_mark_dirty(t_render *render);` |

**사용처 분석**:
| 파일 | `hud.h` include | `hud_text.h` include |
|------|-----------------|----------------------|
| `hud_scene.c` | ✅ line 14 | ✅ line 15 |
| `hud_objects.c` | ✅ line 14 | ✅ line 15 |
| `hud_render.c` | ✅ line 14 | ✅ line 15 |

**결론**: ✅ **조치 가능** - `hud_text.h`에서 제거해도 `hud.h` 통해 접근 가능

---

#### 3.3.2 🟡 [MEDIUM] 중복 선언: `hud_render_objects()`

**선언 위치**:
| 파일 | 라인 | 선언 |
|------|------|------|
| `includes/hud.h` | 324 | `void hud_render_objects(t_render *render, int *y);` |
| `includes/hud_objects.h` | 18 | `void hud_render_objects(t_render *render, int *y);` |

**사용처 분석**:
| 파일 | `hud.h` include | `hud_objects.h` include |
|------|-----------------|-------------------------|
| `hud_render.c` | ✅ line 14 | ✅ line 17 |

**결론**: ✅ **조치 가능** - `hud_objects.h`에서 제거 또는 파일 전체 삭제 가능

---

## 4. 조치 1: get_type_count() 통합

### 4.1 현재 파일별 함수 구성

| 파일 | 함수 수 | 함수 목록 |
|------|---------|-----------|
| `parse_objects.c` | 5 | `get_type_count`, `parse_sphere_data`, `parse_sphere`, `parse_plane_data`, `parse_plane` |
| `parse_cylinder.c` | 5 | `get_cylinder_count`, `parse_cylinder_dims`, `parse_cyl_vectors`, `parse_cylinder`, (1 여유) |
| `parser_utils.c` | 3 | `line_reader_init`, `line_reader_get_line_num`, `validate_extension` |

### 4.2 함수 관계도

```
parse_sphere() ─────┬──▶ get_type_count(scene, OBJ_SPHERE)
                    └──▶ format_id()
                    
parse_plane() ──────┬──▶ get_type_count(scene, OBJ_PLANE)
                    └──▶ format_id()
                    
parse_cylinder() ───┬──▶ get_cylinder_count(scene)  ← 중복!
                    └──▶ format_id()
```

### 4.3 제안 조치

**단계 1**: `get_type_count()`를 `parser_utils.c`로 이동 (static → public)

**단계 2**: `parser.h`에 선언 추가
```c
int	get_type_count(t_scene *scene, t_object_type type);
```

**단계 3**: `parse_objects.c`에서 static 함수 제거

**단계 4**: `parse_cylinder.c`에서 `get_cylinder_count()` 제거, `get_type_count()` 호출로 변경
```c
// Before
format_id(obj.id, 8, "cy-", get_cylinder_count(scene) + 1);

// After
format_id(obj.id, 8, "cy-", get_type_count(scene, OBJ_CYLINDER) + 1);
```

### 4.4 Constitution 검증

| 항목 | Before | After | 상태 |
|------|--------|-------|------|
| `parser_utils.c` 함수 수 | 3 | 4 | ✅ PASS (≤5) |
| `parse_objects.c` 함수 수 | 5 | 4 | ✅ PASS (≤5) |
| `parse_cylinder.c` 함수 수 | 5 | 4 | ✅ PASS (≤5) |
| 함수 인자 수 | - | 2 | ✅ PASS (≤4) |
| Makefile 수정 | - | 불필요 | ✅ PASS |

### 4.5 영향 분석

| 항목 | 영향 |
|------|------|
| 파일 응집도 | ⚠️ 약간 감소 - 카운트 로직이 파싱 파일에서 분리 |
| 코드 중복 | ✅ 해소 - 15줄 중복 제거 |
| 의존성 | ✅ 변화 없음 - `parser.h` 통해 이미 연결 |
| 빌드 | ✅ 변화 없음 - Makefile 수정 불필요 |
| 테스트 | ✅ 동일 동작 - 로직 변경 없음 |

### 4.6 수정 대상 파일

| 파일 | 변경 내용 |
|------|-----------|
| `src/parser/parser_utils.c` | `get_type_count()` 추가 (public) |
| `src/parser/parse_objects.c` | `get_type_count()` static 제거 |
| `src/parser/parse_cylinder.c` | `get_cylinder_count()` 삭제, 호출부 수정 |
| `includes/parser.h` | `get_type_count()` 선언 추가 |

---

## 5. 조치 2: hud_text.h 중복 선언 제거

### 5.1 현재 상태

**hud_text.h (전체)**:
```c
#ifndef HUD_TEXT_H
# define HUD_TEXT_H

# include "minirt.h"

void	hud_mark_dirty(t_render *render);           // ← 중복 (hud.h:217)
void	hud_render_background_row(t_render *render, int y);
void	hud_render_background(t_render *render);
void	format_and_print_vec3(t_render *render, int *y,
			char *label, t_vec3 vec);
int		copy_str_to_buf(char *dst, char *src);

#endif
```

### 5.2 제안 조치

`hud_mark_dirty()` 선언 제거

**수정 후**:
```c
#ifndef HUD_TEXT_H
# define HUD_TEXT_H

# include "minirt.h"

void	hud_render_background_row(t_render *render, int y);
void	hud_render_background(t_render *render);
void	format_and_print_vec3(t_render *render, int *y,
			char *label, t_vec3 vec);
int		copy_str_to_buf(char *dst, char *src);

#endif
```

### 5.3 Constitution 검증

| 항목 | 상태 | 근거 |
|------|------|------|
| 컴파일 | ✅ PASS | 모든 사용처가 `hud.h`도 include |
| 기능 | ✅ PASS | 선언만 제거, 구현 및 호출 영향 없음 |
| Norm | ✅ PASS | 코드 감소 |

### 5.4 영향 분석

**사용처 확인**:
| 파일 | `hud.h` | `hud_text.h` | 영향 |
|------|---------|--------------|------|
| `hud_scene.c` | ✅ | ✅ | 없음 - `hud.h` 통해 접근 |
| `hud_objects.c` | ✅ | ✅ | 없음 - `hud.h` 통해 접근 |
| `hud_render.c` | ✅ | ✅ | 없음 - `hud.h` 통해 접근 |

---

## 6. 조치 3: hud_objects.h 정리 (선택)

### 6.1 현재 상태

**hud_objects.h (전체)**:
```c
#ifndef HUD_OBJECTS_H
# define HUD_OBJECTS_H

# include "minirt.h"

void	hud_render_objects(t_render *render, int *y);  // ← 중복 (hud.h:324)

#endif
```

### 6.2 옵션

| 옵션 | 내용 | 장점 | 단점 |
|------|------|------|------|
| A | 선언만 제거 | 최소 변경 | 빈 헤더 파일 남음 |
| B | 파일 전체 삭제 | 깔끔함 | Makefile 등 수정 필요 여부 확인 |
| C | 현행 유지 | 변경 없음 | 중복 유지 |

### 6.3 권장: 옵션 A (선언만 제거)

**이유**:
- 파일 삭제 시 다른 영향 확인 필요
- 향후 HUD objects 관련 함수 추가 시 사용 가능
- 최소 침습적 변경

### 6.4 수정 후

```c
#ifndef HUD_OBJECTS_H
# define HUD_OBJECTS_H

# include "minirt.h"

/* hud_render_objects is declared in hud.h */

#endif
```

또는 빈 헤더로 유지:

```c
#ifndef HUD_OBJECTS_H
# define HUD_OBJECTS_H

# include "minirt.h"

#endif
```

---

## 7. 수정하지 않는 항목

### 7.1 Radius Squared 패턴

**이유**: Norm v4.1에서 매크로는 리터럴/상수에만 권장. 동작 포함 매크로는 정신에 맞지 않음.

### 7.2 String Copy 유틸리티

**이유**: 
- 각 파일 5함수 제한에 가까움
- 통합 시 새 파일 필요 → Makefile 수정
- 이득 대비 작업량 과다

---

## 8. 실행 계획

### Phase 1: 조치 1 (get_type_count 통합)

```
1. parser_utils.c에 get_type_count() 추가
2. parser.h에 선언 추가
3. parse_objects.c에서 static get_type_count() 제거
4. parse_cylinder.c에서 get_cylinder_count() 제거 및 호출부 수정
5. make re && norminette 검증
6. 기능 테스트 (sphere, plane, cylinder 파싱)
```

### Phase 2: 조치 2 (hud_text.h 정리)

```
1. hud_text.h에서 hud_mark_dirty() 선언 제거
2. make re && norminette 검증
3. HUD 기능 테스트
```

### Phase 3: 조치 3 (hud_objects.h 정리) - 선택

```
1. hud_objects.h에서 hud_render_objects() 선언 제거
2. make re && norminette 검증
3. HUD 기능 테스트
```

---

## 9. 검증 체크리스트

### 9.1 빌드 검증

- [ ] `make clean && make` - 경고 0
- [ ] `norminette src/ includes/` - 오류 0

### 9.2 기능 검증

- [ ] 정상 씬 파일 파싱 (sphere, plane, cylinder 포함)
- [ ] 오브젝트 ID 정상 생성 (sp-1, pl-1, cy-1 형식)
- [ ] HUD 정상 렌더링
- [ ] HUD dirty flag 정상 동작

### 9.3 회귀 테스트

- [ ] `scenes/` 디렉토리 전체 씬 파일 로드
- [ ] 렌더링 결과 시각적 확인

---

## 10. 리스크 분석

| 리스크 | 확률 | 영향 | 완화 |
|--------|------|------|------|
| 컴파일 오류 | Low | 빌드 실패 | 단계별 검증 |
| 기능 오류 | Low | ID 생성 오류 | 기능 테스트 |
| Norm 위반 | Low | 평가 실패 | norminette 검증 |

---

## 11. 결론

### 11.1 권장 조치

| 우선순위 | 조치 | 효과 | 난이도 |
|----------|------|------|--------|
| 1 | get_type_count() 통합 | 15줄 중복 제거 | 낮음 |
| 2 | hud_text.h 중복 제거 | 헤더 정리 | 매우 낮음 |
| 3 | hud_objects.h 정리 | 헤더 정리 | 매우 낮음 |

### 11.2 예상 결과

- **코드 감소**: ~20줄
- **파일 수 변경**: 없음
- **Constitution 위반**: 없음
- **기능 변경**: 없음

### 11.3 실행 시점

031-perf-bottleneck-optimization 구현 **전** 또는 **후** 모두 가능
- 독립적인 리팩토링이므로 충돌 없음

---

## Appendix A: 파일별 함수 수 현황

| 디렉토리 | 파일 | 함수 수 | 여유 |
|----------|------|---------|------|
| parser/ | parse_objects.c | 5 | 0 |
| parser/ | parse_cylinder.c | 5 | 0 |
| parser/ | parser_utils.c | 3 | 2 |
| parser/ | parse_elements.c | 3 | 2 |
| parser/ | parse_number.c | 2 | 3 |
| parser/ | parse_number_utils.c | 3 | 2 |
| parser/ | parse_error.c | 4 | 1 |
| parser/ | parse_error_msg.c | 2 | 3 |
| parser/ | parse_token.c | 1 | 4 |
| parser/ | parse_validation_strict.c | 4 | 1 |
| parser/ | parser.c | 2 | 3 |
| parser/ | parser_dispatch.c | 1 | 4 |
| parser/ | parse_line_reader.c | 1 | 4 |

---

## Appendix B: 중복 코드 상세

### B.1 get_type_count vs get_cylinder_count

**차이점**: 없음 (타입 파라미터 하드코딩 여부만 다름)

| 항목 | get_type_count | get_cylinder_count |
|------|----------------|-------------------|
| 위치 | parse_objects.c:25 | parse_cylinder.c:24 |
| 인자 | scene, type | scene |
| 반환 | int | int |
| 로직 | type 비교 | OBJ_CYLINDER 비교 |
| 줄 수 | 14줄 | 14줄 |

---

## Appendix C: 헤더 의존성 그래프

```
minirt.h
    └── hud.h
            ├── hud_mark_dirty() ◄─── 중복
            └── hud_render_objects() ◄─── 중복

    └── hud_text.h
            └── hud_mark_dirty() ◄─── 중복 (제거 대상)

    └── hud_objects.h
            └── hud_render_objects() ◄─── 중복 (제거 대상)
```

---

*End of Report*
