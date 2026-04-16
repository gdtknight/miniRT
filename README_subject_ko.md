*This project has been created as part of the 42 curriculum by yoshin, jyoo.*

# miniRT

C 언어와 MiniLibX 라이브러리로 작성한 실시간 인터랙티브 레이트레이서. `.rt` 파일로 기술된 씬을 구·평면·원기둥·원뿔로 렌더링하며, Phong 셰이딩 모델(ambient + diffuse + specular + shadow)로 조명을 계산합니다.

## Description (프로젝트 개요)

miniRT는 42 커리큘럼의 첫 번째 컴퓨터 그래픽스 프로젝트입니다. 씬 기술 파일을 파싱하고, 가상 카메라에서 primary ray를 쏘고, 단순 기하 primitive와의 교차를 계산하고, 교차점을 셰이딩하는 일련의 레이트레이서를 처음부터 직접 구현하는 것이 목표입니다.

본 구현은 mandatory 베이스라인을 넘어 다음 기능들을 포함합니다.

- **Primitive**: 구, 평면, 원기둥(캡 포함), 원뿔 (보너스)
- **셰이딩**: 완전한 Phong 모델 (ambient + diffuse + specular)
- **그림자**: 하드 섀도우 및 stratified 샘플링 기반 소프트 섀도우 (보너스)
- **가속 구조**: midpoint 분할과 any-hit 섀도우 순회를 사용하는 BVH(Bounding Volume Hierarchy). 평면은 BVH에서 분리해 별도로 테스트
- **인터랙티브 조작**: 카메라/오브젝트/광원 실시간 조작. 입력 중에는 저품질 프리뷰로 떨어졌다가 입력이 멈추면 풀 품질로 다시 렌더링하는 디바운스 FSM 적용
- **텍스처**: 체커보드 패턴, XPM 범프맵
- **HUD**: 화면 위 씬 정보, 성능 메트릭, 페이지네이션된 키가이드 오버레이

렌더러는 macOS(OpenGL + AppKit MiniLibX)와 Linux(X11 + Xext MiniLibX)에서 동작하며, Makefile이 플랫폼을 자동 감지합니다.

## Features (기능 목록)

### Mandatory (subject 필수)

- 엄격한 검증과 `Error\n` 보고를 갖춘 `.rt` 씬 파싱
- Primitive: 구(`sp`), 평면(`pl`), 원기둥(`cy`, 캡 포함)
- 카메라(`C`), ambient 광(`A`), 점광원(`L`)
- ambient + diffuse Lambertian 셰이딩
- 하드 섀도우 (광원당 occlusion ray 1개)
- 적용 가능한 오브젝트의 이동·회전·크기 조정
- MiniLibX 기반 윈도우/이벤트 처리 (`ESC` 및 윈도우 닫기 버튼에서 정상 종료)
- 메모리 누수 없는 실행 (Linux에서 valgrind로 검증)

### Bonus (본 구현의 확장)

- 원뿔 primitive(`co`, 캡 포함)
- Specular highlight (완전한 Phong 반사 모델)
- 사전 계산된 offset LUT를 활용한 stratified disk 샘플링 기반 소프트 섀도우
- 다중 점광원 및 광원별 선택(`=`)
- 체커보드 패턴(`checker:R,G,B`)
- XPM 범프 매핑(`bump:file.xpm`)
- 평면 분리 및 any-hit 섀도우 순회를 적용한 BVH 가속
- BVH 콘솔 시각화(`--bvh-vis`)
- 4-state 디바운스 FSM 기반의 카메라/오브젝트/광원 인터랙티브 조작 (입력 중 저품질 프리뷰 → 입력 종료 후 풀 품질 렌더링)
- 페이지네이션된 씬 정보·성능 메트릭·키가이드 HUD 오버레이

## Instructions (사용 방법)

### 사전 요구사항

- `cc`, `make`
- **Linux**: `sudo apt-get install libx11-dev libxext-dev libbsd-dev`
- **macOS**: Xcode Command Line Tools

### 빌드

```bash
git clone --recursive <repo-url> miniRT
cd miniRT
make            # libft, MiniLibX, miniRT 모두 빌드
make bonus      # `make`와 동일 — 보너스는 항상 함께 컴파일됨
make clean      # 오브젝트 파일 삭제
make fclean     # 오브젝트 + 바이너리 + 라이브러리 정리
make re         # 전체 재빌드
make norm       # src/, includes/에 norminette 검사 실행
```

`--recursive` 없이 클론한 경우, `git submodule update --init`로 `lib/libft`를 받아야 합니다.

### 실행

```bash
./miniRT scenes/valid/valid_smoke_simple.rt        # 스모크 테스트
./miniRT <scene.rt>                                # 임의의 씬 렌더링
./miniRT <scene.rt> --bvh-vis                      # BVH 트리도 함께 덤프
```

### 메모리 검사 (Linux)

```bash
valgrind --suppressions=miniRT.supp --leak-check=full \
  ./miniRT scenes/valid/valid_smoke_simple.rt
```

`miniRT.supp`는 사용자 코드가 해제할 수 없는 X11/XCB/MiniLibX의 "still reachable" 할당을 필터링합니다.

### 씬 파일 포맷 (`.rt`)

```
A  <ratio>           <R,G,B>                       # ambient (1개, 필수)
C  <x,y,z>           <nx,ny,nz>  <fov>             # camera  (1개, 필수)
L  <x,y,z>           <bright>    <R,G,B>           # light   (1개 이상, 필수)
sp <x,y,z>           <diameter>  <R,G,B>
pl <x,y,z>           <nx,ny,nz>  <R,G,B>
cy <x,y,z>           <nx,ny,nz>  <d> <h>  <R,G,B>
co <x,y,z>           <nx,ny,nz>  <d> <h>  <R,G,B>  # 보너스
```

오브젝트 라인 뒤에 콜론(`:`)으로 구분된 보너스 옵션을 붙일 수 있습니다.

- `checker:R,G,B` — 지정된 보조 색상의 체커보드 패턴 (RGB 필수, 범위 `[0,255]`)
- `bump:file.xpm` — XPM 범프맵 (작업 디렉터리 기준 경로)

두 옵션은 같은 오브젝트에 함께 지정할 수 있습니다.

### 조작법

| 카테고리 | 키 | 동작 |
|---|---|---|
| 카메라 이동 | W/X, A/D, Q/Z | 전진/후진, 좌/우, 상승/하강 |
| 카메라 회전 (pitch) | E/C | 위/아래 |
| 카메라 회전 (yaw) | 1/3 | 우/좌 |
| 카메라 리셋 | S | 초기 위치 복원 |
| 오브젝트 선택 | TAB | 다음 오브젝트로 순환 |
| 오브젝트 이동 | R/T, F/G, V/B | X, Y, Z축 |
| 오브젝트 리사이즈 | Y/U, N/M | 반지름, 높이 (원기둥/원뿔) |
| 오브젝트 회전 | I/J, O/K, P/L | X, Y, Z축 |
| 광원 선택 | = | 다음 광원으로 순환 |
| 광원 이동 | [ / ], ; / ', , / . | X, Y, Z축 |
| UI | H / Up / Down / ESC | HUD 토글 / 페이지 이동 / 종료 |

## Performance (성능)

측정 환경: Apple M1, 1440×900 윈도우, `-O0` (컴파일 최적화 없음), 섀도우 16샘플, BVH 활성화. 원본 측정값은 `docs/archive/benchmark-baseline.md`와 `docs/archive/benchmark-results.md`에 보관되어 있습니다.

| 씬 | Baseline | 최종 (R4) | 개선율 |
|---|---:|---:|---:|
| `perf_spheres_20.rt` (구 20개) | 1,835.7 ms | 1,218.5 ms | **33.6 %** |
| `perf_spheres_50.rt` (구 50개) | 3,366.0 ms | 1,432.2 ms | **57.4 %** |
| `perf_all_objects.rt` (구 7 + 원기둥 7 + 평면 3) | 27,173.7 ms | 6,066.8 ms | **77.7 %** |

4라운드 최적화 내역: (R1) BVH-fallback 브루트포스 패스 제거, (R2) 셰이딩 핫 패스의 `fast_pow32`와 sqrt 캐싱, (R3) 사전 계산된 섀도우 offset LUT, (R4) BVH에서 평면 분리 + any-hit 섀도우 순회. 가장 무거운 혼합 씬에서는 섀도우 교차 테스트가 369 M → 83.7 M으로 줄었고, BVH 스킵률은 32.9 % → 82.0 %로 상승했습니다.

## Resources (참고 자료)

### 레이트레이싱 기본

- *Ray Tracing in One Weekend* — Peter Shirley. 기본 파이프라인, 벡터 수학, Lambertian/Phong 셰이딩.
- *Physically Based Rendering: From Theory to Implementation* (PBRT) — Pharr, Jakob, Humphreys. BVH 구축·순회 참고.
- *Real-Time Rendering* (4판) — Akenine-Möller 외. Phong 반사 모델, AABB 교차.
- Scratchapixel 2.0 — `https://www.scratchapixel.com/` — ray-primitive 교차 유도 (원기둥, 원뿔 캡 등).
- Inigo Quilez — `https://iquilezles.org/articles/` — 수치 안정성 관련 노트.

### MiniLibX

- 42 docs MiniLibX 매뉴얼 — 이미지 버퍼, 이벤트 루프, 플랫폼별 키 코드.

### AI 사용 내역

AI 어시스턴트(Claude Code, ChatGPT)는 다음과 같이 범위가 명확한 작업에서 리서치·리뷰 보조 도구로 사용했습니다. 모든 결과 코드는 한 줄씩 직접 읽고, 42 Norm에 맞게 수정하고, 벤치마크로 검증했으며, 작성자가 완전히 이해하고 있습니다.

- **성능 최적화를 위한 공간 가속 (BVH)**: midpoint vs. SAH 분할의 트레이드오프, 깊이/리프 크기 휴리스틱, 무한 primitive(평면)를 BVH에서 분리한 근거, 섀도우 ray의 any-hit 단축 경로를 토론. AI는 순회 스택 설계와 AABB slab 교차 수식을 점검하는 용도로 활용했고, `src/spatial/bvh/`와 `src/spatial/aabb/`의 구현은 `scenes/perf/` 벤치마크에 맞춰 직접 작성·튜닝했습니다.
- **부드러운 그림자를 위한 셰이딩**: 광원 주변의 stratified disk 샘플링 패턴 유도, 매 샘플마다의 삼각함수 호출을 없애기 위한 섀도우 offset LUT(`cos/sin/sqrt` 테이블) 사전 계산, 하드 섀도우 fast path를 깨지 않으면서 다중 샘플 가림 결과를 Phong 셰이더에 통합. AI는 샘플 수의 bias/variance 트레이드오프를 검토하는 데 활용했고, `src/lighting/shading/`와 `src/lighting/shadow/` 셰이더 코드는 직접 작성했습니다.
- **공간 가속 디버깅을 위한 BVHD (BVH 시각화)**: 깊이별 들여쓰기, 노드별 AABB 범위, 리프별 primitive 개수를 출력하는 콘솔 트리 덤프 포맷을 AI와 함께 스케치하여 비정상적 분할 진단과 평면 분리 검증이 쉬워졌습니다. `--bvh-vis` 플래그와 `src/spatial/debug/` 구현은 직접 작성했습니다.
- **성능 측정 Metric 서브시스템**: 레이트레이서 비용을 의미 있게 반영하는 카운터 — 프레임 시간, primary/shadow ray 수, BVH 노드 방문 수, BVH 스킵률, 교차 테스트 수 — 의 선정과, 측정 자체가 결과에 영향을 주지 않도록 HUD에 노출하는 방법(지연 포맷팅, 단일 스레드용 atomic-free 카운터)을 함께 정리. `src/metrics/` 모듈과 HUD 연동은 직접 구현했고, 위 4라운드 최적화의 근거 데이터로 사용했습니다 (`README.md` 참고).
- **42 Norm 제약하의 리팩터링**: 25줄을 넘는 함수 분할, 추출된 헬퍼 네이밍, 파일당 5개 함수 한도에 가까운 파일의 재구성.
- **문서화**: 모듈별 위키 페이지와 본 README의 초안 작성. 작성자가 직접 편집·사실 확인.
