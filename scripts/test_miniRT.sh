#!/bin/bash

# miniRT 종합 테스트 스크립트

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}miniRT 종합 테스트${NC}"
echo -e "${BLUE}========================================${NC}\n"

# 1. 컴파일 확인
echo -e "${BLUE}[1] 컴파일 확인${NC}"
if [ -f "./miniRT" ]; then
    echo -e "${GREEN}✓${NC} miniRT 실행 파일 존재"
else
    echo -e "${RED}✗${NC} miniRT 실행 파일 없음"
    echo "make를 먼저 실행하세요."
    exit 1
fi

# 2. 사용법 테스트
echo -e "\n${BLUE}[2] 사용법 메시지${NC}"
OUTPUT=$(./miniRT 2>&1)
if echo "$OUTPUT" | grep -q "Usage"; then
    echo -e "${GREEN}✓${NC} 사용법 메시지 출력"
else
    echo -e "${RED}✗${NC} 사용법 메시지 없음"
fi

# 3. 오류 처리 테스트
echo -e "\n${BLUE}[3] 오류 처리${NC}"

# 파일 없음
OUTPUT=$(timeout 1 ./miniRT nonexistent.rt 2>&1 || true)
if echo "$OUTPUT" | grep -q "Error"; then
    echo -e "${GREEN}✓${NC} 파일 없음 오류 처리"
else
    echo -e "${RED}✗${NC} 파일 없음 오류 미처리"
fi

# 4. 파서 테스트
echo -e "\n${BLUE}[4] 장면 파일 파싱${NC}"

TEST_SCENES=(
    "scenes/test_simple.rt"
    "scenes/us01_sphere_only.rt"
    "scenes/us04_sphere_intersection.rt"
    "scenes/us04_plane_intersection.rt"
)

for scene in "${TEST_SCENES[@]}"; do
    if [ -f "$scene" ]; then
        # 파싱만 확인 (즉시 종료)
        timeout 0.5 ./miniRT "$scene" >/dev/null 2>&1 &
        PID=$!
        sleep 0.3
        if kill -0 $PID 2>/dev/null; then
            echo -e "${GREEN}✓${NC} $(basename $scene) 파싱 성공 (프로세스 실행 중)"
            kill $PID 2>/dev/null
            wait $PID 2>/dev/null
        else
            echo -e "${YELLOW}⚠${NC} $(basename $scene) 프로세스 종료됨"
        fi
    fi
done

# 5. 통계
echo -e "\n${BLUE}========================================${NC}"
echo -e "${BLUE}테스트 완료${NC}"
echo -e "${BLUE}========================================${NC}"

# 파일 정보
echo -e "\n${GREEN}실행 파일 정보:${NC}"
ls -lh miniRT
file miniRT

echo -e "\n${GREEN}장면 파일:${NC}"
ls -1 scenes/*.rt | wc -l
echo "개의 테스트 장면 준비됨"

echo -e "\n${YELLOW}참고:${NC}"
echo "- GUI 환경에서 실행하면 렌더링된 이미지를 볼 수 있습니다"
echo "- ESC 키 또는 X 버튼으로 종료할 수 있습니다"
echo "- 렌더링은 시간이 걸릴 수 있습니다 (픽셀별 계산)"

