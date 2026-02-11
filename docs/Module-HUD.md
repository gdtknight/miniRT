# Module: HUD

HUD(Head-Up Display) 오버레이와 키가이드를 담당하는 모듈입니다.

---

## 소스 파일

### HUD (`src/hud/`)

| 파일 | 역할 |
|------|------|
| `hud_init.c` | HUD 초기화 및 페이지 수 계산 |
| `hud_render.c` | HUD 렌더링 진입점 |
| `hud_scene.c` | 씬 정보 (카메라, 조명, 오브젝트 수) |
| `hud_objects.c` | 오브젝트 목록 페이지네이션 |
| `hud_obj_render.c` | 개별 오브젝트 정보 렌더링 |
| `hud_performance.c` | 성능 메트릭 (FPS, 렌더 시간, BVH 통계) |
| `hud_text.c` | HUD dirty 마킹, vec3 포맷 출력 |
| `hud_format.c` | 숫자/벡터 포맷팅 |
| `hud_format_helpers.c` | 포맷 헬퍼 |
| `hud_format_simple.c` | 간단한 포맷 유틸리티 |
| `hud_color.c` | HUD 배경/텍스트 색상 |
| `hud_color_extract.c` | 색상 추출 유틸리티 |
| `hud_transparency.c` | 반투명 배경 렌더링 |
| `hud_toggle.c` | HUD 토글 (H 키) |
| `hud_navigation.c` | 페이지 네비게이션 |

### Keyguide (`src/keyguide/`)

| 파일 | 역할 |
|------|------|
| `keyguide_init.c` | 키가이드 초기화 |
| `keyguide_render.c` | 키가이드 오버레이 렌더링 |
| `keyguide_render_extra.c` | 추가 키 정보 렌더링 |
| `keyguide_cleanup.c` | 키가이드 리소스 해제 |

---

## HUD 렌더링 구조

```
hud_render(render)
 ├── [visible 아님] → return
 ├── hud_render_background()     // 반투명 배경
 ├── hud_render_scene_info()     // 씬 정보 (카메라, 조명)
 ├── hud_render_objects()        // 오브젝트 목록 (현재 페이지)
 │    └── render_object_by_index()
 │         ├── render_sphere_obj()
 │         ├── render_plane_obj()
 │         └── render_cylinder_obj()
 └── hud_render_performance()    // 성능 메트릭
```

---

## HUD 상태

```c
typedef struct s_hud_state
{
    int     visible;          // 표시 여부
    int     current_page;     // 현재 페이지
    int     objects_per_page; // 페이지당 오브젝트 수
    int     total_pages;      // 전체 페이지 수
    int     dirty;            // 재렌더링 필요 여부
}   t_hud_state;
```

---

## 표시 정보

### 씬 정보
- 카메라 위치, 방향, FOV
- 조명 위치, 밝기
- 오브젝트 수 (sphere/plane/cylinder 별)

### 오브젝트 목록
- 타입, ID, 위치, 크기/반지름
- 선택된 오브젝트 강조 표시
- 페이지네이션 (오브젝트 수가 많을 때)

### 성능 메트릭
- FPS
- 프레임 렌더 시간 (ms)
- 레이 수, 교차 테스트 수
- BVH 노드 방문 수, 스킵률

---

## 키가이드

키가이드는 화면에 현재 사용 가능한 키 바인딩을 오버레이로 표시합니다.

- H 키로 HUD와 함께 토글
- 카메라, 오브젝트, 광원 조작 키 목록 표시
- 현재 선택된 오브젝트에 따라 관련 키 강조

---

## 페이지네이션

오브젝트 수가 `objects_per_page`를 초과하면 페이지 단위로 분할합니다:

```
전체 오브젝트: 40개
페이지당: 8개
전체 페이지: 5
현재 페이지: 1/5

Up / Down 키로 페이지 이동
```
