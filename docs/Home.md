# miniRT Wiki

C로 작성한 실시간 인터랙티브 레이트레이서. BVH 가속 구조, Phong 조명, 소프트 섀도우를 지원합니다.

---

## 목차

| 페이지 | 설명 |
|--------|------|
| [Architecture](Architecture) | 전체 아키텍처 및 렌더링 파이프라인 |
| [Data-Structures](Data-Structures) | 핵심 자료구조 (t_scene, t_object, t_ray 등) |
| [Module-Parser](Module-Parser) | .rt 파일 파서 모듈 |
| [Module-Render](Module-Render) | 렌더링 루프 및 메트릭 수집 |
| [Module-Spatial](Module-Spatial) | BVH 공간 가속 구조 |
| [Module-Lighting](Module-Lighting) | Phong 조명 및 그림자 시스템 |
| [Module-Ray](Module-Ray) | 광선-오브젝트 교차 판정 |
| [Module-Window](Module-Window) | MiniLibX 윈도우 및 이벤트 |
| [Module-HUD](Module-HUD) | HUD 오버레이 및 키가이드 |
| [Scene-Format](Scene-Format) | .rt 씬 파일 상세 스펙 |
| [Build-Guide](Build-Guide) | 빌드 및 설치 가이드 |
| [Performance](Performance) | 벤치마크 결과 및 최적화 히스토리 |
| [CHANGELOG](CHANGELOG) | 버전별 변경 이력 |

---

## 빠른 시작

```bash
git clone --recursive https://github.com/gdtknight/miniRT.git
cd miniRT
make
./miniRT scenes/valid/valid_smoke_simple.rt
```

조작법, 씬 파일 형식 등 자세한 내용은 각 Wiki 페이지를 참조하세요.
