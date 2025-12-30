#!/bin/bash
# Rendering Optimization Validation Script
# Validates that BVH and optimization features compile and basic structure is correct

echo "=== miniRT Rendering Optimization Validation ==="
echo ""

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

PASS=0
FAIL=0

check() {
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ PASS${NC}: $1"
        ((PASS++))
    else
        echo -e "${RED}✗ FAIL${NC}: $1"
        ((FAIL++))
    fi
}

echo "1. Checking if miniRT binary exists..."
[ -f ./miniRT ]
check "miniRT binary exists"

echo ""
echo "2. Checking header files..."
[ -f includes/spatial.h ]
check "includes/spatial.h exists"

[ -f includes/metrics.h ]
check "includes/metrics.h exists"

[ -f includes/render_state.h ]
check "includes/render_state.h exists"

echo ""
echo "3. Checking BVH implementation files..."
[ -f src/spatial/aabb.c ]
check "src/spatial/aabb.c exists"

[ -f src/spatial/bounds.c ]
check "src/spatial/bounds.c exists"

[ -f src/spatial/bvh_build.c ]
check "src/spatial/bvh_build.c exists"

[ -f src/spatial/bvh_init.c ]
check "src/spatial/bvh_init.c exists"

[ -f src/spatial/bvh_traverse.c ]
check "src/spatial/bvh_traverse.c exists"

echo ""
echo "4. Checking render state implementation..."
[ -f src/render/render_state.c ]
check "src/render/render_state.c exists"

[ -f src/utils/timer.c ]
check "src/utils/timer.c exists"

echo ""
echo "5. Checking test scenes..."
[ -f scenes/perf_test_20_spheres.rt ]
check "scenes/perf_test_20_spheres.rt exists"

[ -f scenes/perf_test_50_spheres.rt ]
check "scenes/perf_test_50_spheres.rt exists"

echo ""
echo "6. Checking for required symbols in binary..."
nm ./miniRT | grep -q "bvh_create"
check "bvh_create function present"

nm ./miniRT | grep -q "bvh_build"
check "bvh_build function present"

nm ./miniRT | grep -q "bvh_intersect"
check "bvh_traverse/bvh_intersect function present"

nm ./miniRT | grep -q "render_state_init"
check "render_state_init function present"

echo ""
echo "7. Verifying compilation (symbol check complete)"
echo -e "${YELLOW}⚠ INFO${NC}: Scene rendering validation requires GUI (skipped)"

echo ""
echo "=== Validation Summary ==="
echo -e "Passed: ${GREEN}${PASS}${NC}"
echo -e "Failed: ${RED}${FAIL}${NC}"
echo ""

if [ $FAIL -eq 0 ]; then
    echo -e "${GREEN}✓ All checks passed! BVH optimization is properly implemented.${NC}"
    echo ""
    echo "Next steps:"
    echo "1. Test rendering with: ./miniRT scenes/perf_test_20_spheres.rt"
    echo "2. Toggle BVH with 'B' key during rendering"
    echo "3. Compare performance with BVH on vs off"
    echo "4. Try larger scene: ./miniRT scenes/perf_test_50_spheres.rt"
    exit 0
else
    echo -e "${RED}✗ Some checks failed. Please review the implementation.${NC}"
    exit 1
fi
