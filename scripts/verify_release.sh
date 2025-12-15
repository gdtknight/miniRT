#!/bin/bash

# miniRT Release Gate Verification Script
# 릴리즈 게이트 체크리스트 자동 검증 스크립트

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

PASSED=0
FAILED=0
WARNINGS=0

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}miniRT Release Gate Verification${NC}"
echo -e "${BLUE}========================================${NC}\n"

print_status() {
    if [ $1 -eq 0 ]; then
        echo -e "${GREEN}✓ PASSED${NC}: $2"
        ((PASSED++))
    else
        echo -e "${RED}✗ FAILED${NC}: $2"
        ((FAILED++))
    fi
}

print_warning() {
    echo -e "${YELLOW}⚠ WARNING${NC}: $1"
    ((WARNINGS++))
}

# 1. Build System
echo -e "\n${BLUE}[1] Build System${NC}"
make re > /dev/null 2>&1 && print_status 0 "Build success" || print_status 1 "Build failed"
[ -f "miniRT" ] && print_status 0 "Executable exists" || print_status 1 "No executable"

# 2. Norminette
echo -e "\n${BLUE}[2] Norminette${NC}"
if command -v norminette &> /dev/null; then
    norminette src/ includes/ 2>&1 | grep -q "Error" && print_status 1 "Norminette errors" || print_status 0 "Norminette OK"
else
    print_warning "norminette not installed"
fi

# 3. 42 Header
echo -e "\n${BLUE}[3] 42 Headers${NC}"
HEADERS=$(find src includes -name "*.c" -o -name "*.h" | xargs grep -l "/\* \*\*\*" | wc -l)
TOTAL=$(find src includes -name "*.c" -o -name "*.h" | wc -l)
[ $HEADERS -eq $TOTAL ] && print_status 0 "All headers present ($HEADERS/$TOTAL)" || print_status 1 "Missing headers ($HEADERS/$TOTAL)"

# Summary
echo -e "\n${BLUE}========================================${NC}"
echo -e "${GREEN}Passed: $PASSED${NC} | ${RED}Failed: $FAILED${NC} | ${YELLOW}Warnings: $WARNINGS${NC}"
[ $FAILED -eq 0 ] && echo -e "${GREEN}✓ PASSED${NC}" || echo -e "${RED}✗ FAILED${NC}"
