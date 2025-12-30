#!/bin/bash

# miniRT Enhanced Test Suite with Error Case Coverage

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Test counters
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Run a single test with expected result
run_test() {
    local test_name="$1"
    local test_file="$2"
    local expected="$3"  # "pass" or "fail"
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    # Run with timeout
    if timeout 5 ./miniRT "$test_file" > /dev/null 2>&1; then
        actual="pass"
    else
        actual="fail"
    fi
    
    # Check if result matches expectation
    if [ "$actual" == "$expected" ]; then
        echo -e "${GREEN}✓ PASS${NC}: $test_name"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo -e "${RED}✗ FAIL${NC}: $test_name (expected $expected, got $actual)"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
}

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}miniRT Enhanced Test Suite${NC}"
echo -e "${BLUE}========================================${NC}\n"

# 1. Compilation check
echo -e "${BLUE}[1] Compilation Check${NC}"
if [ -f "./miniRT" ]; then
    echo -e "${GREEN}✓${NC} miniRT executable exists"
else
    echo -e "${RED}✗${NC} miniRT executable not found"
    echo "Run 'make' first."
    exit 1
fi

# 2. Valid scene tests
echo -e "\n${BLUE}[2] Valid Scene Tests${NC}"

if [ -f "scenes/test_simple.rt" ]; then
    run_test "Simple scene" "scenes/test_simple.rt" "pass"
fi

if [ -f "scenes/test_all_objects.rt" ]; then
    run_test "All objects scene" "scenes/test_all_objects.rt" "pass"
fi

if [ -f "scenes/us01_sphere_only.rt" ]; then
    run_test "Sphere only" "scenes/us01_sphere_only.rt" "pass"
fi

if [ -f "scenes/us04_sphere_intersection.rt" ]; then
    run_test "Sphere intersection" "scenes/us04_sphere_intersection.rt" "pass"
fi

# 3. Error handling tests
echo -e "\n${BLUE}[3] Error Handling Tests${NC}"

# Missing file
run_test "Missing file" "nonexistent.rt" "fail"

# Test existing error scenes
ERROR_SCENES=(
    "scenes/error_ambient_out_of_range.rt:Ambient out of range"
    "scenes/error_duplicate_ambient.rt:Duplicate ambient"
    "scenes/error_fov_out_of_range.rt:FOV out of range"
    "scenes/error_invalid_format.rt:Invalid format"
    "scenes/error_missing_sphere.rt:Missing sphere"
)

for entry in "${ERROR_SCENES[@]}"; do
    IFS=':' read -r scene_file test_name <<< "$entry"
    if [ -f "$scene_file" ]; then
        run_test "$test_name" "$scene_file" "fail"
    fi
done

# 4. Usage message test
echo -e "\n${BLUE}[4] Usage Message Test${NC}"
TOTAL_TESTS=$((TOTAL_TESTS + 1))
OUTPUT=$(./miniRT 2>&1)
if echo "$OUTPUT" | grep -q "Usage"; then
    echo -e "${GREEN}✓ PASS${NC}: Usage message displayed"
    PASSED_TESTS=$((PASSED_TESTS + 1))
else
    echo -e "${RED}✗ FAIL${NC}: Usage message not displayed"
    FAILED_TESTS=$((FAILED_TESTS + 1))
fi

# 5. Test Summary
echo -e "\n${BLUE}========================================${NC}"
echo -e "${BLUE}Test Summary${NC}"
echo -e "${BLUE}========================================${NC}"

echo -e "\nTotal Tests: $TOTAL_TESTS"
echo -e "${GREEN}Passed: $PASSED_TESTS${NC}"
echo -e "${RED}Failed: $FAILED_TESTS${NC}"

if [ $FAILED_TESTS -eq 0 ]; then
    echo -e "\n${GREEN}✓ ALL TESTS PASSED${NC}"
    EXIT_CODE=0
else
    echo -e "\n${RED}✗ SOME TESTS FAILED${NC}"
    EXIT_CODE=1
fi

# File information
echo -e "\n${BLUE}Build Information:${NC}"
ls -lh miniRT

echo -e "\n${YELLOW}Note:${NC}"
echo "- GUI environment required for rendering visualization"
echo "- Press ESC or X to close the window"
echo "- Rendering may take time depending on scene complexity"

exit $EXIT_CODE
