#!/bin/bash
# Test script to verify keyboard controls are properly implemented

echo "Testing miniRT Interactive Controls"
echo "===================================="
echo ""

# Check if miniRT exists
if [ ! -f "./miniRT" ]; then
    echo "Error: miniRT executable not found"
    echo "Please run 'make' first"
    exit 1
fi

# Check for test scene
if [ ! -f "scenes/test_all_objects.rt" ]; then
    echo "Error: Test scene not found"
    exit 1
fi

echo "✓ miniRT executable found"
echo "✓ Test scene available"
echo ""
echo "Manual Test Instructions:"
echo "=========================="
echo ""
echo "1. Camera Movement (WASD):"
echo "   - Launch: ./miniRT scenes/test_all_objects.rt"
echo "   - Press W: Camera should move forward"
echo "   - Press S: Camera should move backward"
echo "   - Press A: Camera should strafe left"
echo "   - Press D: Camera should strafe right"
echo ""
echo "2. Camera Rotation (RF):"
echo "   - Press R: Camera should pitch up"
echo "   - Press F: Camera should pitch down"
echo ""
echo "3. Object Selection ([/]):"
echo "   - Press ]: Select next object"
echo "   - Press [: Select previous object"
echo ""
echo "4. Object Movement (Numpad):"
echo "   - Select an object first with ]"
echo "   - Press Numpad 4/6: Move left/right"
echo "   - Press Numpad 2/8: Move down/up"
echo "   - Press Numpad 1/3: Move back/forward"
echo ""
echo "5. Light Control (Insert/Home/PgUp/Del/End/PgDn):"
echo "   - Press Insert: Move light right"
echo "   - Press Delete: Move light left"
echo "   - Press Home: Move light up"
echo "   - Press End: Move light down"
echo "   - Press Page Up: Move light forward"
echo "   - Press Page Down: Move light backward"
echo ""
echo "6. Exit:"
echo "   - Press ESC to exit"
echo ""
echo "Expected behavior:"
echo "- Scene should re-render after each movement"
echo "- Controls should respond immediately"
echo "- No crashes or memory leaks"
echo ""
echo "To run the interactive test, execute:"
echo "  ./miniRT scenes/test_all_objects.rt"
echo ""
