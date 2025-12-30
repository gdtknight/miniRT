#!/bin/bash
set -e

SCRIPT_NAME="check-memory-leaks.sh"
VERSION="1.0.0"

TIMEOUT=600
TOOL=""
OUTPUT_FILE=""
SUMMARY_ONLY=false
SCENE_FILE=""

show_help() {
    cat << EOF
Memory Leak Detection Script
Version: ${VERSION}

Usage: ${SCRIPT_NAME} [OPTIONS] <scene_file>

Options:
  --tool <valgrind|leaks>   Force specific tool (auto-detect by default)
  --timeout <seconds>       Timeout per scene (default: 600)
  --output <file>           Output file for detailed log
  --summary                 Print summary only (no full output)
  --help                    Show help message

Examples:
  # Auto-detect platform and run check
  ./${SCRIPT_NAME} scenes/test/sphere.rt

  # Force valgrind with 5-minute timeout
  ./${SCRIPT_NAME} --tool valgrind --timeout 300 scenes/test/complex.rt

  # Output to specific file
  ./${SCRIPT_NAME} --output leak_report.txt scenes/test/scene.rt

Exit Codes:
  0   No leaks detected, success
  1   Memory leaks detected
  2   Tool not available or installation failed
  3   Scene file not found or invalid
  124 Timeout exceeded
EOF
}

parse_args() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            --tool)
                TOOL="$2"
                shift 2
                ;;
            --timeout)
                TIMEOUT="$2"
                shift 2
                ;;
            --output)
                OUTPUT_FILE="$2"
                shift 2
                ;;
            --summary)
                SUMMARY_ONLY=true
                shift
                ;;
            --help)
                show_help
                exit 0
                ;;
            *)
                if [ -z "$SCENE_FILE" ]; then
                    SCENE_FILE="$1"
                else
                    echo "ERROR: Unknown argument: $1"
                    show_help
                    exit 3
                fi
                shift
                ;;
        esac
    done

    if [ -z "$SCENE_FILE" ]; then
        echo "ERROR: Scene file required"
        show_help
        exit 3
    fi

    if [ ! -f "$SCENE_FILE" ]; then
        echo "ERROR: Scene file not found: $SCENE_FILE"
        exit 3
    fi
}

detect_platform() {
    if [ -n "$TOOL" ]; then
        return
    fi

    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        TOOL="valgrind"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        TOOL="leaks"
    else
        echo "ERROR: Unsupported platform: $OSTYPE"
        echo "Supported: linux-gnu (valgrind), darwin (leaks)"
        exit 2
    fi
}

install_valgrind() {
    if command -v valgrind &> /dev/null; then
        return 0
    fi

    echo "Installing valgrind..."
    if command -v apt-get &> /dev/null; then
        sudo apt-get update -qq
        sudo apt-get install -y valgrind
    else
        echo "ERROR: Cannot auto-install valgrind. Package manager not supported."
        echo "Please install manually: sudo apt-get install valgrind"
        exit 2
    fi

    if ! command -v valgrind &> /dev/null; then
        echo "ERROR: valgrind installation failed"
        exit 2
    fi
}

check_leaks_tool() {
    if ! command -v leaks &> /dev/null; then
        echo "ERROR: leaks command not found"
        echo "Install Xcode Command Line Tools: xcode-select --install"
        exit 2
    fi
}

run_valgrind() {
    local scene="$1"
    local log_file="${OUTPUT_FILE:-logs/memory-check-$(date +%Y%m%d-%H%M%S).log}"
    
    mkdir -p "$(dirname "$log_file")"
    
    local start_time=$(date +%s)
    
    if timeout "${TIMEOUT}" valgrind \
        --leak-check=full \
        --show-leak-kinds=all \
        --track-origins=yes \
        --error-exitcode=1 \
        --log-file="${log_file}" \
        ./miniRT "${scene}" > /dev/null 2>&1; then
        
        local end_time=$(date +%s)
        local duration=$((end_time - start_time))
        
        echo "✓ Memory Check: PASSED"
        echo "  Scene: ${scene}"
        echo "  Tool: valgrind"
        echo "  Duration: ${duration}s"
        echo "  Result: No leaks detected"
        
        if [ "$SUMMARY_ONLY" = false ]; then
            echo ""
            echo "Full log: ${log_file}"
        fi
        
        return 0
    else
        local exit_code=$?
        local end_time=$(date +%s)
        local duration=$((end_time - start_time))
        
        if [ $exit_code -eq 124 ]; then
            echo "⚠ Memory Check: TIMEOUT"
            echo "  Scene: ${scene}"
            echo "  Tool: valgrind"
            echo "  Duration: ${TIMEOUT}s (timeout)"
            echo ""
            echo "  The scene took too long to process. This may indicate:"
            echo "  - Infinite loop"
            echo "  - Extremely complex scene"
            echo "  - Performance issue"
            echo ""
            echo "  Increase timeout with: --timeout 900"
            return 124
        fi
        
        echo "✗ Memory Check: FAILED"
        echo "  Scene: ${scene}"
        echo "  Tool: valgrind"
        echo "  Duration: ${duration}s"
        echo ""
        echo "  LEAK SUMMARY:"
        
        if [ -f "${log_file}" ]; then
            grep -A 5 "LEAK SUMMARY" "${log_file}" | sed 's/^/  /'
            echo ""
            echo "  To reproduce:"
            echo "    valgrind --leak-check=full ./miniRT ${scene}"
            echo ""
            echo "  Full log: ${log_file}"
        fi
        
        return 1
    fi
}

run_leaks() {
    local scene="$1"
    local log_file="${OUTPUT_FILE:-logs/memory-check-$(date +%Y%m%d-%H%M%S).log}"
    
    mkdir -p "$(dirname "$log_file")"
    
    local start_time=$(date +%s)
    
    ./miniRT "${scene}" > /dev/null 2>&1 &
    local pid=$!
    
    sleep 2
    
    if ! ps -p $pid > /dev/null 2>&1; then
        echo "ERROR: miniRT process terminated unexpectedly"
        return 3
    fi
    
    leaks $pid > "${log_file}" 2>&1
    local leak_status=$?
    
    kill $pid 2>/dev/null || true
    wait $pid 2>/dev/null || true
    
    local end_time=$(date +%s)
    local duration=$((end_time - start_time))
    
    if grep -q "0 leaks for 0 total leaked bytes" "${log_file}"; then
        echo "✓ Memory Check: PASSED"
        echo "  Scene: ${scene}"
        echo "  Tool: leaks"
        echo "  Duration: ${duration}s"
        echo "  Result: No leaks detected"
        
        if [ "$SUMMARY_ONLY" = false ]; then
            echo ""
            echo "Full log: ${log_file}"
        fi
        
        return 0
    else
        echo "✗ Memory Check: FAILED"
        echo "  Scene: ${scene}"
        echo "  Tool: leaks"
        echo "  Duration: ${duration}s"
        echo ""
        echo "  LEAK SUMMARY:"
        
        if [ -f "${log_file}" ]; then
            grep -E "(leaks? for|Leak:)" "${log_file}" | head -20 | sed 's/^/  /'
            echo ""
            echo "  To reproduce:"
            echo "    ./miniRT ${scene} &"
            echo "    leaks \$!"
            echo ""
            echo "  Full log: ${log_file}"
        fi
        
        return 1
    fi
}

main() {
    parse_args "$@"
    detect_platform
    
    if [ "$TOOL" = "valgrind" ]; then
        install_valgrind
        run_valgrind "$SCENE_FILE"
    elif [ "$TOOL" = "leaks" ]; then
        check_leaks_tool
        run_leaks "$SCENE_FILE"
    else
        echo "ERROR: Invalid tool: $TOOL"
        exit 2
    fi
}

main "$@"
