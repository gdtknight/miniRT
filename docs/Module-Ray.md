# Module: Ray

광선-오브젝트 교차 판정 알고리즘을 담당하는 모듈입니다.

---

## 소스 파일

| 파일 | 역할 |
|------|------|
| `intersect_object.c` | `intersect_object_new` — 통합 교차 디스패치 + 구/평면 교차 |
| `intersect_cyl_new.c` | `intersect_cylinder_new` — 원기둥 교차 (측면 + 캡) |

---

## 구 교차 (Sphere)

이차방정식으로 레이-구 교차를 판정합니다.

```
Ray: P(t) = O + t × D
Sphere: |P - C|² = r²

대입:
|O + tD - C|² = r²
(D·D)t² + 2(D·(O-C))t + ((O-C)·(O-C) - r²) = 0

a = D·D = 1 (방향 정규화)
oc = O - C
b = 2 × (D · oc)
c = oc·oc - r²

판별식 D = b² - 4ac
D < 0 → 교차 없음
D >= 0 → t = (-b ± √D) / 2a, 가장 작은 양의 t 선택
```

- `radius_sq` (반지름 제곱)을 사전 캐싱하여 제곱 연산 절감
- 법선: `normalize(hit_point - center)`

---

## 평면 교차 (Plane)

내적으로 레이-평면 교차를 판정합니다.

```
Plane: (P - P₀) · N = 0  (P₀: 평면 위 점, N: 법선)
Ray: P(t) = O + tD

대입:
(O + tD - P₀) · N = 0
t = ((P₀ - O) · N) / (D · N)

D · N ≈ 0 → 평행 (교차 없음)
t > EPSILON → 유효한 교차
```

- 법선 방향 보정: `dot(ray.direction, normal) > 0`이면 법선 반전 (양면 렌더링)

---

## 원기둥 교차 (Cylinder)

원기둥은 측면(무한 실린더)과 양쪽 캡(원형 디스크)을 개별 판정한 뒤 가장 가까운 교차를 선택합니다.

### 측면 교차

축 방향을 제거하여 2D 원 교차 문제로 환원합니다.

```
축: A (정규화)
oc = O - C (중심까지 벡터)

D_perp = D - (D·A)A    // 방향의 축 직교 성분
oc_perp = oc - (oc·A)A  // oc의 축 직교 성분

a = D_perp · D_perp
b = 2 × (D_perp · oc_perp)
c = oc_perp · oc_perp - r²

판별식으로 t 계산 후, 높이 검증:
m = (D·A)×t + (oc·A)
|m| <= half_height → 유효
```

### 캡 교차

양쪽 캡은 축 방향으로 `±half_height` 위치의 평면-원 교차로 판정합니다.

```
cap_center = center ± half_height × axis
t = ((cap_center - O) · axis) / (D · axis)
hit_point = O + t × D
|hit_point - cap_center|² <= r² → 유효
```

### 법선 계산

- **측면**: `normalize(hit_point - (center + m × axis))`
- **캡**: `±axis` (상단/하단 구분)

---

## 교차 디스패치

`intersect_object_new(ray, obj, hit)`:

```c
switch (obj->type)
{
    case OBJ_SPHERE:   → 구 교차
    case OBJ_PLANE:    → 평면 교차
    case OBJ_CYLINDER: → 원기둥 교차
}
```

모든 교차 함수는 가장 가까운 양의 `t` 값(EPSILON 이상)을 찾아 `t_hit` 구조체에 기록합니다.
