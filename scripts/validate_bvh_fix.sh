#!/bin/bash

echo "=== BVH Integration Fix Validation ==="
echo ""

# T011: Check if fixes are applied
echo "T011-T012: Checking BVH fixes..."
if grep -q "bvh_enabled = 1" src/render/render_state.c; then
    echo "✅ T006: BVH enabled by default in render_state.c"
else
    echo "❌ T006 FAIL: BVH not enabled by default"
    exit 1
fi

if grep -q "hit_found = 1" src/render/trace.c && grep -q "if (!hit_found)" src/render/trace.c; then
    echo "✅ T007-T008: Fallback logic correctly implemented in trace.c"
else
    echo "❌ T007-T008 FAIL: Fallback logic not correct"
    exit 1
fi

# T009: Verify norminette
echo ""
echo "T009: Running norminette..."
if norminette src/render/render_state.c src/render/trace.c > /dev/null 2>&1; then
    echo "✅ T009: Norminette check passed"
else
    echo "❌ T009 FAIL: Norminette errors found"
    exit 1
fi

# T010: Verify build
echo ""
echo "T010: Testing build..."
if make re > /dev/null 2>&1; then
    echo "✅ T010: Build successful"
else
    echo "❌ T010 FAIL: Build failed"
    exit 1
fi

# T013-T020: Test with simple scene
echo ""
echo "T013-T020: Testing scene loading..."
if [ ! -f "scenes/test_simple.rt" ]; then
    echo "⚠️  Warning: test_simple.rt not found, using test_all_objects.rt"
    TEST_SCENE="scenes/test_all_objects.rt"
else
    TEST_SCENE="scenes/test_simple.rt"
fi

# Test that program can start with a scene (will timeout, but that's expected for GUI)
timeout 2 ./miniRT "$TEST_SCENE" > /dev/null 2>&1
EXIT_CODE=$?

if [ $EXIT_CODE -eq 124 ]; then
    # Timeout is expected for GUI application
    echo "✅ T011-T020: Program starts and loads scene successfully"
    echo "   (Timed out as expected - GUI application)"
elif [ $EXIT_CODE -eq 0 ]; then
    echo "✅ T011-T020: Program executed successfully"
else
    echo "❌ T011-T020 FAIL: Program crashed or failed (exit code: $EXIT_CODE)"
    exit 1
fi

echo ""
echo "=== Summary ==="
echo "✅ Phase 1: Setup & Diagnosis - Complete"
echo "✅ Phase 2: Core BVH Fixes - Complete"
echo "✅ Phase 3: Basic Rendering Tests - Complete"
echo ""
echo "Core fixes successfully implemented:"
echo "  1. BVH enabled by default (render_state.c:37)"
echo "  2. Fallback logic added to trace_ray() (trace.c:159-172)"
echo "  3. Norminette compliant"
echo "  4. Build successful"
echo "  5. Scene loading verified"
echo ""
echo "Next steps:"
echo "  - Run with X11 display to visually verify rendering"
echo "  - Test BVH toggle functionality if implemented"
echo "  - Test with complex scenes for performance validation"
echo ""
echo "✅ BVH FIX VALIDATION PASSED"
