# miniRT Wiki

이 디렉토리는 GitHub Wiki의 소스 파일을 포함합니다.

**최신 버전**: v2.4.0 (2026-03)

**주요 변경사항**:
- **v2.4.0**: 모듈 리팩토링 (input/metrics 분리), 파일 리네이밍, dead code 제거, 버그 수정, 문서 전면 갱신
- **v2.3.0**: Object Resize/Rotation, Debounce FSM 재설계 (PREVIEW 제거), Keymap 변경, Input 모듈 분리
- **v2.2.0**: Plane BVH 분리, Shadow BVH Any-Hit, fast_pow32, inv_dir 사전 계산, S4 성능 77.7% 개선
- **v2.1.0**: BVH 가속 구조로 2-10배 렌더링 속도 향상, 디바운싱 렌더링, 100% Norminette 준수

상세 내용은 [버전 히스토리](버전-히스토리.md)를 참조하세요.

---

## 📁 디렉토리 구조

```
wiki/
├── Home.md                          # Wiki 홈페이지
├── 빠른-시작-가이드.md              # 설치 및 실행
├── 프로그램-개요.md                 # 프로젝트 소개
├── 레이-트레이싱-알고리즘.md        # 광선 추적 원리
├── 조명-시스템.md                   # Phong 조명 및 그림자
├── BVH-가속-구조.md                 # 공간 최적화
├── 씬-파일-형식.md                  # .rt 파일 명세
├── 인터랙티브-컨트롤.md             # 키보드 조작
├── HUD-시스템.md                    # 정보 표시
├── 개발-가이드.md                   # 코드 구조 및 확장
├── API-레퍼런스.md                  # 함수 및 구조체
├── FAQ.md                           # 자주 묻는 질문
├── 문제-해결.md                     # 일반적인 문제
└── README.md                        # 이 파일
```

---

## 🔄 GitHub Wiki 동기화

이 디렉토리의 마크다운 파일을 GitHub Wiki에 수동으로 업로드해야 합니다.

### 방법 1: 웹 인터페이스

1. GitHub 저장소의 Wiki 탭으로 이동
2. "New Page" 클릭
3. 파일 이름과 내용 복사/붙여넣기
4. "Save Page" 클릭

### 방법 2: Git Clone (권장)

```bash
# Wiki 저장소 클론
git clone https://github.com/YOUR_USERNAME/miniRT.wiki.git

# Wiki 파일 복사
cp wiki/*.md miniRT.wiki/

# 커밋 및 푸시
cd miniRT.wiki
git add .
git commit -m "Update wiki from local source"
git push
```

---

## ✏️ 문서 작성 규칙

### 파일 명명

- **한글 제목**: `프로그램-개요.md`
- **영문 제목**: `Quick-Start-Guide.md`
- **공백 대신 하이픈** 사용

### 마크다운 스타일

- **헤더**: `#`, `##`, `###` (계층 구조)
- **코드 블록**: ` ```language ` (언어 지정)
- **링크**: `[텍스트](파일명.md)` (상대 경로)
- **강조**: `**굵게**`, `*기울임*`, `` `코드` ``

### 구조

모든 문서는 다음 구조를 따릅니다:

```markdown
# 문서 제목

간단한 설명

---

## 섹션 1

내용...

---

## 섹션 2

내용...

---

## 🔗 관련 문서

- [다른 문서](다른-문서.md)

---

**Last Updated**: YYYY-MM-DD  
**Version**: X.X.X
```

---

## 📖 문서 목록

### 시작 가이드
- ✅ Home.md
- ✅ 빠른-시작-가이드.md
- ✅ 프로그램-개요.md

### 기술 문서
- ✅ 레이-트레이싱-알고리즘.md
- ✅ 조명-시스템.md
- ✅ BVH-가속-구조.md
- ✅ 씬-파일-형식.md

### 사용 가이드
- ✅ 인터랙티브-컨트롤.md
- ✅ HUD-시스템.md

### 개발 문서
- ✅ 개발-가이드.md
- ✅ API-레퍼런스.md

### 참고 자료
- ✅ FAQ.md
- ✅ 문제-해결.md

---

## 🔍 검색 태그

문서 검색을 위한 주요 키워드:

- **설치**: 빠른-시작-가이드
- **렌더링**: 레이-트레이싱-알고리즘, 조명-시스템
- **최적화**: BVH-가속-구조
- **장면**: 씬-파일-형식
- **조작**: 인터랙티브-컨트롤
- **문제**: FAQ, 문제-해결
- **개발**: 개발-가이드, API-레퍼런스

---

## 📝 TODO

향후 추가할 문서:

- [ ] BVH-가속-구조.md (상세 구현)
- [ ] HUD-시스템.md (HUD 구조)
- [ ] 개발-가이드.md (코드 확장)
- [ ] API-레퍼런스.md (함수 레퍼런스)
- [ ] 문제-해결.md (트러블슈팅)

---

**Maintained by**: miniRT Team  
**Last Updated**: 2026-03
**Version**: 2.4.0
