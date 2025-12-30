# 한글 명세서 (Korean Specifications)

이 디렉토리는 `specs/` 디렉토리의 영문 명세서에 대응하는 한글 버전을 포함합니다.

## 원칙 (Constitution Principle VI)

프로젝트 헌법 Principle VI에 따라:
- **영문 명세서**: `specs/` 디렉토리에 위치
- **한글 명세서**: `docs/specs/` 디렉토리에 위치 (이 디렉토리)
- **동기화 필수**: 양쪽 버전은 항상 동일한 내용을 포함해야 함
- **동시 업데이트**: 명세서 변경 시 영문과 한글을 같은 커밋에서 함께 업데이트

## 디렉토리 구조

`specs/`와 동일한 구조를 유지합니다:

```
docs/specs/
├── README.md                           # 이 파일
├── 001-raytracer/                      # 레이트레이서 기본 구현
│   ├── spec.md                        # 기능 명세서
│   ├── plan.md                        # 구현 계획
│   ├── tasks.md                       # 작업 목록
│   └── ...
├── 002-realistic-shadows/              # 현실적인 그림자 기능
│   ├── spec.md
│   ├── plan.md
│   └── tasks.md
└── ...
```

## 현재 상태

### ✅ 완료된 명세서
- 현재 없음 (TODO)

### ⚠️ 번역 필요
다음 영문 명세서들의 한글 버전 작성이 필요합니다:
- `specs/001-raytracer/` - 레이트레이서 기본 구현
- `specs/002-realistic-shadows/` - 현실적인 그림자
- `specs/001-workflow-improvements/` - 워크플로우 개선
- `specs/001-scene-info-hud/` - 장면 정보 HUD

## 작성 가이드

### 1. 새 명세서 작성 시
```bash
# 1. 영문 명세서 작성
specs/XXX-feature-name/spec.md

# 2. 한글 명세서 작성 (동일한 커밋에서)
docs/specs/XXX-feature-name/spec.md

# 3. 같은 PR에 포함하여 제출
```

### 2. 기존 명세서 업데이트 시
```bash
# 1. 영문 명세서 수정
edit specs/XXX-feature-name/spec.md

# 2. 한글 명세서도 수정 (동일한 커밋에서)
edit docs/specs/XXX-feature-name/spec.md

# 3. 버전 번호가 일치하는지 확인
# 4. 내용이 의미적으로 동일한지 확인
```

### 3. 번역 원칙
- **의미 동등성**: 영문과 한글이 동일한 의미를 전달해야 함
- **기술 용어**: 일관된 번역 사용 (아래 용어집 참조)
- **코드 예제**: 영문과 동일하게 유지
- **버전 번호**: 영문과 한글이 동일해야 함

## 기술 용어 번역 가이드

| English | 한글 | 비고 |
|---------|------|------|
| Ray Tracing | 레이 트레이싱 | |
| Specification | 명세서 | |
| Feature | 기능 | |
| User Story | 사용자 스토리 | |
| Requirements | 요구사항 | |
| Implementation | 구현 | |
| Workflow | 워크플로우 | |
| Quality Gate | 품질 게이트 | |
| CI/CD | CI/CD | 그대로 사용 |
| Pull Request | Pull Request, PR | 그대로 사용 |

## 릴리즈 시 체크리스트

릴리즈 전 다음을 확인하세요:

- [ ] 모든 `specs/` 명세서에 대응하는 `docs/specs/` 한글 버전 존재
- [ ] 영문과 한글 버전의 버전 번호 일치
- [ ] 영문과 한글 버전의 내용이 의미적으로 동일
- [ ] 최근 업데이트가 양쪽에 모두 반영됨
- [ ] Wiki 동기화에 양쪽 버전 모두 포함됨

## 관련 문서

- **프로젝트 헌법**: `.specify/memory/constitution.md` - Principle VI
- **영문 명세서**: `specs/` 디렉토리
- **한글 일반 문서**: `docs/00-전체설명/`, `docs/01-시작하기/` 등
