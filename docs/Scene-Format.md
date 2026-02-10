# Scene Format

`.rt` 씬 파일의 상세 스펙을 설명합니다.

---

## 기본 규칙

- 파일 확장자: `.rt`
- 인코딩: ASCII
- 한 줄에 하나의 요소 정의
- `#`으로 시작하는 줄은 주석으로 무시
- 빈 줄은 무시
- 최대 줄 길이: 4096자

---

## 필수 요소 (각 1개)

### A — 주변광 (Ambient Light)

```
A  <ratio>  <R,G,B>
```

| 파라미터 | 타입 | 범위 | 설명 |
|---------|------|------|------|
| ratio | double | 0.0 ~ 1.0 | 주변광 강도 |
| R,G,B | int,int,int | 0 ~ 255 | 주변광 색상 |

예시: `A  0.2  255,255,255`

### C — 카메라 (Camera)

```
C  <x,y,z>  <nx,ny,nz>  <fov>
```

| 파라미터 | 타입 | 범위 | 설명 |
|---------|------|------|------|
| x,y,z | double,double,double | 제한 없음 | 카메라 위치 |
| nx,ny,nz | double,double,double | -1.0 ~ 1.0 각 성분 | 시선 방향 (정규화됨) |
| fov | double | 0 < fov < 180 | 수평 시야각 (도) |

예시: `C  0,0,-15  0,0,1  70`

- 방향 벡터는 자동 정규화
- 영벡터 (0,0,0)는 에러

### L — 조명 (Light)

```
L  <x,y,z>  <brightness>  <R,G,B>
```

| 파라미터 | 타입 | 범위 | 설명 |
|---------|------|------|------|
| x,y,z | double,double,double | 제한 없음 | 광원 위치 |
| brightness | double | 0.0 ~ 1.0 | 밝기 |
| R,G,B | int,int,int | 0 ~ 255 | 광원 색상 |

예시: `L  -10,10,10  0.7  255,255,255`

---

## 선택 요소 (0개 이상)

### sp — 구 (Sphere)

```
sp  <x,y,z>  <diameter>  <R,G,B>
```

| 파라미터 | 타입 | 범위 | 설명 |
|---------|------|------|------|
| x,y,z | double,double,double | 제한 없음 | 구 중심 |
| diameter | double | > 0 | 지름 |
| R,G,B | int,int,int | 0 ~ 255 | 색상 |

예시: `sp  0,0,0  12.6  255,0,0`

### pl — 평면 (Plane)

```
pl  <x,y,z>  <nx,ny,nz>  <R,G,B>
```

| 파라미터 | 타입 | 범위 | 설명 |
|---------|------|------|------|
| x,y,z | double,double,double | 제한 없음 | 평면 위 점 |
| nx,ny,nz | double,double,double | -1.0 ~ 1.0 각 성분 | 법선 방향 (정규화됨) |
| R,G,B | int,int,int | 0 ~ 255 | 색상 |

예시: `pl  0,-10,0  0,1,0  200,200,200`

### cy — 원기둥 (Cylinder)

```
cy  <x,y,z>  <nx,ny,nz>  <diameter>  <height>  <R,G,B>
```

| 파라미터 | 타입 | 범위 | 설명 |
|---------|------|------|------|
| x,y,z | double,double,double | 제한 없음 | 중심 |
| nx,ny,nz | double,double,double | -1.0 ~ 1.0 각 성분 | 축 방향 (정규화됨) |
| diameter | double | > 0 | 지름 |
| height | double | > 0 | 높이 |
| R,G,B | int,int,int | 0 ~ 255 | 색상 |

예시: `cy  0,0,0  0,1,0  5  10  0,255,0`

---

## 예제 씬 파일

### 최소 씬

```rt
A  0.2  255,255,255
C  0,0,-15  0,0,1  70
L  0,8,-8  0.7  255,255,255
sp  0,0,0  6  255,0,0
```

### 종합 씬

```rt
# Ambient
A  0.15  255,255,255

# Camera
C  0,5,-20  0,-0.1,1  70

# Light
L  -10,15,-5  0.8  255,255,255

# Ground plane
pl  0,-2,0  0,1,0  180,180,180

# Spheres
sp  -4,0,0  4  255,0,0
sp  0,0,0  4  0,255,0
sp  4,0,0  4  0,0,255

# Cylinder
cy  0,-2,5  0,1,0  2  6  255,255,0
```

---

## 에러 메시지

파싱 에러 시 `Error` + 줄 번호 + 에러 내용이 stderr로 출력됩니다.

| 상황 | 메시지 예시 |
|------|-----------|
| 파일 확장자 | `Error: Invalid file extension (expected .rt)` |
| 형식 오류 | `Error: line 3 (A): Invalid format` |
| 범위 초과 | `Error: line 5 (sp): Value out of range` |
| 필수 요소 누락 | `Error: Missing required element: Camera` |
| 중복 | `Error: line 7 (A): Duplicate element` |
| 알 수 없는 요소 | `Error: line 4: Unknown element type` |

---

## 제공되는 테스트 씬 (29개)

`scenes/valid/` 디렉토리에 다양한 테스트 씬이 포함되어 있습니다:

- `valid_smoke_simple.rt` — 최소 구성 (구 1개)
- `valid_scene_comprehensive.rt` — 종합 테스트 (다수 오브젝트)
- `valid_scene_complex.rt` — 복잡한 씬
- `valid_shadow_soft.rt` — 소프트 섀도우 테스트
- `valid_us04_intersect_*.rt` — 교차 판정 테스트 (구/평면/원기둥)
- `valid_camera_*.rt` — 카메라 설정 테스트 (FOV, 각도)
- `valid_light_*.rt` — 조명 테스트 (밝기, 위치)
- `valid_transform_*.rt` — 변환 테스트 (이동, 회전, 리사이즈)
