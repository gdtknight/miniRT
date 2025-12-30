#!/bin/bash
set -e

SCRIPT_NAME="install-valgrind.sh"
VERSION="1.0.0"

show_help() {
    cat << EOF
Valgrind/Memory Leak Tool Installation Script
Version: ${VERSION}

Usage: ${SCRIPT_NAME} [OPTIONS]

Options:
  --force      Force reinstallation even if already installed
  --help       Show help message

Description:
  Automatically detects platform and installs appropriate memory leak
  detection tools:
  - Linux: valgrind (via apt-get)
  - macOS: Verifies leaks command (pre-installed with Xcode)

Exit Codes:
  0   Tool successfully installed or already available
  1   Installation failed
  2   Unsupported platform
EOF
}

install_valgrind_linux() {
    echo "Detected Linux platform"
    
    if command -v valgrind &> /dev/null && [ "$FORCE" != true ]; then
        echo "✓ valgrind already installed: $(valgrind --version | head -1)"
        return 0
    fi
    
    echo "Installing valgrind..."
    
    if ! command -v apt-get &> /dev/null; then
        echo "ERROR: apt-get not found. Manual installation required."
        echo "Please install valgrind using your package manager."
        return 1
    fi
    
    sudo apt-get update -qq || {
        echo "ERROR: Failed to update package lists"
        return 1
    }
    
    sudo apt-get install -y valgrind || {
        echo "ERROR: Failed to install valgrind"
        return 1
    }
    
    if ! command -v valgrind &> /dev/null; then
        echo "ERROR: valgrind installation completed but command not found"
        return 1
    fi
    
    echo "✓ valgrind successfully installed: $(valgrind --version | head -1)"
    return 0
}

install_valgrind_macos() {
    echo "Detected macOS platform"
    
    if command -v leaks &> /dev/null; then
        echo "✓ leaks command available (built-in macOS tool)"
        echo "  Version: $(leaks --version 2>&1 | head -1 || echo 'unknown')"
        return 0
    fi
    
    echo "✗ leaks command not found"
    echo ""
    echo "The 'leaks' command is part of Xcode Command Line Tools."
    echo "To install, run:"
    echo "  xcode-select --install"
    echo ""
    echo "After installation, verify with:"
    echo "  which leaks"
    echo ""
    return 1
}

install_valgrind_unsupported() {
    echo "ERROR: Unsupported platform: $OSTYPE"
    echo ""
    echo "Supported platforms:"
    echo "  - Linux (valgrind via apt-get)"
    echo "  - macOS (leaks command, requires Xcode Command Line Tools)"
    echo ""
    return 2
}

main() {
    FORCE=false
    
    while [[ $# -gt 0 ]]; do
        case $1 in
            --force)
                FORCE=true
                shift
                ;;
            --help)
                show_help
                exit 0
                ;;
            *)
                echo "ERROR: Unknown argument: $1"
                show_help
                exit 1
                ;;
        esac
    done
    
    echo "Memory Leak Tool Installation"
    echo "=============================="
    echo ""
    
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        install_valgrind_linux
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        install_valgrind_macos
    else
        install_valgrind_unsupported
    fi
    
    local exit_code=$?
    
    if [ $exit_code -eq 0 ]; then
        echo ""
        echo "✓ Memory leak detection tools ready!"
    fi
    
    return $exit_code
}

main "$@"
