# GitHub Wiki 설정 가이드

GitHub Wiki를 설정하려면 다음 단계를 따르세요:

## 방법 1: 웹 브라우저를 통한 설정 (권장)

### 1단계: Wiki 초기화
1. https://github.com/gdtknight/miniRT_final/wiki 접속
2. "Create the first page" 버튼 클릭
3. 아무 내용이나 입력 후 "Save Page" 클릭

### 2단계: Wiki 저장소 클론 및 페이지 업로드
```bash
# Wiki 저장소 클론
cd /tmp
git clone git@github.com:gdtknight/miniRT_final.wiki.git
cd miniRT_final.wiki

# 준비된 Wiki 파일 복사
cp /tmp/miniRT_wiki/*.md .

# Git 커밋 및 푸시
git add .
git commit -m "Add comprehensive Korean documentation"
git push origin master
```

### 3단계: 확인
https://github.com/gdtknight/miniRT_final/wiki 에서 확인

## 방법 2: 수동으로 각 페이지 생성

다음 파일들이 `/tmp/miniRT_wiki/` 에 준비되어 있습니다:

1. **Home.md** - Wiki 홈페이지
2. **프로그램-개요.md** - 프로그램 소개 및 구조
3. **프로그램-흐름.md** - 실행 흐름 상세 설명
4. **레이-트레이싱-알고리즘.md** - 레이 트레이싱 핵심 알고리즘
5. **조명-그림자-시스템.md** - Phong 조명 및 Soft Shadows
6. **씬-파일-파싱.md** - .rt 파일 형식 및 파싱
7. **벡터-수학.md** - 벡터 연산 상세 설명

각 파일을 GitHub Wiki 웹 인터페이스에서 새 페이지를 만들어 붙여넣기 하면 됩니다.

## Wiki 페이지 구조

```
Home (홈페이지)
├── 프로그램-개요
├── 프로그램-흐름
├── 레이-트레이싱-알고리즘
├── 조명-그림자-시스템
├── 씬-파일-파싱
└── 벡터-수학
```

## 빠른 명령어 (Wiki가 초기화된 후)

```bash
# 한 번에 모든 Wiki 페이지 업로드
cd /tmp
rm -rf miniRT_final.wiki
git clone git@github.com:gdtknight/miniRT_final.wiki.git
cd miniRT_final.wiki
cp /tmp/miniRT_wiki/*.md .
git add .
git commit -m "Add comprehensive Korean documentation from docs/00-전체설명"
git push origin master
```

## 참고사항

- Wiki 파일은 이미 `/tmp/miniRT_wiki/` 디렉토리에 생성되어 있습니다
- 모든 문서는 한글로 작성되어 있습니다
- 원본 문서는 `docs/00-전체설명/` 에 있습니다
