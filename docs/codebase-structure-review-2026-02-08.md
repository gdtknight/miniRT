# Codebase Structure Review (src/ + includes/)

작성일: 2026-02-08
범위: `src/`, `includes/` 디렉토리 구조 및 파일 분리 적절성 평가

---

## 1. 요약

현재 코드베이스는 기능별 디렉토리 분리가 전반적으로 잘 되어 있으며, 렌더링/파싱/공간가속(BVH)/UI(HUD/Keyguide) 영역의 경계가 비교적 명확하다. 다만, 다음과 같은 구조적 개선 여지가 있다.

- `includes/`가 단일 flat 구조로 유지되어, 모듈 경계가 헤더 레벨에서 흐려짐
- `window/`와 `render/`의 책임 경계가 부분적으로 겹침 (렌더 루프, 플래그, UI 출력)
- `utils/`의 범용성 범위가 넓고, 일부 기능이 모듈 내부로 이동 가능

종합적으로는 “작동하는 기능 중심 구조”에 가깝고, 장기 유지보수를 고려하면 헤더/모듈 경계 정리가 유효하다.

---

## 2. 디렉토리 분리 현황

### 2.1 src/ 기능별 분리

```
src/
├── bvh_vis/      # BVH 시각화 전용
├── hud/          # HUD UI 렌더링
├── keyguide/     # 키 가이드 UI
├── lighting/     # 조명/그림자
├── math/         # 벡터/수학
├── parser/       # .rt 파서
├── ray/          # 교차/레이 처리
├── render/       # 렌더링 루프, metrics, timing
├── scene/        # 씬 모델/관리
├── spatial/      # BVH/AABB/공간 가속
├── utils/        # 에러/시간/포맷 등 공용
└── window/       # MLX 윈도우, 이벤트, 입력
```

- 기능 단위 분리가 명확함
- BVH 관련 로직(spatial)과 BVH 시각화(bvh_vis)가 분리되어 모듈 책임이 잘 나뉨
- HUD / keyguide가 UI 전용으로 분리되어 가시성 좋음

### 2.2 includes/ 구조

- 모든 헤더가 flat 구조로 배치됨
- 모듈 단위 하위 디렉토리 없음
- 헤더 레벨에서 모듈 경계가 희석되는 구조

---

## 3. 모듈별 평가

### 3.1 spatial/ + ray/

- BVH, AABB, 빌드/트래버스 분리가 적절함
- `bvh_any_hit.c`가 separate 파일로 분리되어 역할이 명확함
- ray 교차는 `src/ray/`로 별도 분리되어 SRP에 부합

**판정**: 구조적으로 적절

### 3.2 lighting/

- shadow config / calc / test가 분리됨
- shadow LUT 및 any-hit 호출 등 고비용 경로가 별도 파일로 분리되어 유지보수 용이

**판정**: 적절

### 3.3 render/ vs window/

- `render/`는 렌더링 알고리즘과 metrics, pixel timing을 담당
- `window/`는 MLX 이벤트/입력/루프 및 HUD 출력에 관여

문제 지점:
- 렌더 루프(`window_loop.c`)에서 metrics 출력, HUD 렌더 제어까지 수행
- render flags/디바운스는 window 레이어에 포함되어 있으나, 렌더 정책과 맞닿음

**판정**: 경계가 기능적으로는 합리적이나 장기적으로는 “render 정책”을 `render/`로 이동시키는 것이 더 명확할 수 있음

### 3.4 parser/

- 파싱, 에러, 유효성 검증, 토큰화가 파일 단위로 잘 분리됨
- `parse_*` 단위 구성은 역할 분리가 명확함

**판정**: 적절

### 3.5 hud/ + keyguide/

- HUD 기능이 세부 파일로 잘 분할됨 (format, render, navigation, performance, scene 등)
- keyguide도 별도 디렉토리로 분리되어 UI 경계가 명확함

**판정**: 적절

### 3.6 utils/

- error, timer, format_helpers가 혼재
- `format_helpers.c`는 parser 쪽에서 사용되지만 utils에 배치됨

**판정**: 기능적으로 문제 없지만, 모듈 경계 유지보수 관점에서는 parser 전용 유틸은 parser 하위로 이동 고려 가능

---

## 4. 헤더 구조 평가 (includes/)

### 4.1 장점
- 단일 include 경로로 사용이 편리
- 작은 규모에서는 단순 구조가 유지보수에 유리

### 4.2 단점
- 모듈 경계가 약해지고, 헤더 의존성이 느슨하게 확산됨
- `window_internal.h`처럼 내부용 헤더가 루트에 그대로 노출됨

**개선 방향**:
- 중장기적으로는 모듈별 하위 폴더 구조 도입 고려
  - 예: `includes/spatial/`, `includes/render/`, `includes/window/` 등
- 또는 internal 헤더는 `includes/internal/` 하위로 별도 분리

---

## 5. 역할 중복/경계 이슈

### 5.1 render vs window 경계
- render 루프의 정책(디바운스, 플래그)과 렌더 구현(픽셀 루프)이 분리되어 있긴 하지만,
  정책 제어가 window 레이어에 위치함
- `render_flags_set.c`가 window/ 하위인 점은 모듈 경계 관점에서 애매함

### 5.2 utils 범위
- 공용 코드의 범위를 넘는 일부 기능이 존재할 가능성
- parser 전용/renderer 전용 유틸이 있을 경우 해당 모듈로 이동 권장

---

## 6. 개선 제안 (우선순위 포함)

### High
1. `includes/` 내부 헤더 경계 정리
   - internal 헤더(`window_internal.h`)의 분리 혹은 이름 규칙 강화

### Medium
2. `render` 정책(디바운스/플래그)과 `window` 이벤트 경계 재정의
   - render 정책을 `render/`로 이동하고 window는 이벤트 전달 역할에 집중

### Low
3. `utils/` 재분류
   - parser 전용 유틸을 parser/로 이동
   - 공용 유틸만 utils/에 유지

---

## 7. 결론

현재 구조는 기능별 분리 관점에서 충분히 합리적이며, 특히 spatial/lighting/render 분리가 명확하다. 다만 헤더 레이어가 flat 구조로 유지되어 모듈 경계가 약해지고 있으며, window/render 경계는 장기 유지보수 관점에서 정리 여지가 있다. 단기적으로는 내부 헤더 정리부터 진행하는 것이 가장 효과적이다.

