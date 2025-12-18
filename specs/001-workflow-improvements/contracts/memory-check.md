# Contract: Memory Leak Detection

**Version**: 1.0.0  
**Date**: 2025-12-18  
**Status**: Specification

## Overview

This contract defines the behavior, interface, and guarantees for automated memory leak detection in the miniRT project using valgrind (Linux) and leaks (macOS).

---

## Script Interface

### Script Location
`.github/scripts/check-memory-leaks.sh`

### Usage
```bash
check-memory-leaks.sh [OPTIONS] <scene_file>

Options:
  --tool <valgrind|leaks>   Force specific tool (auto-detect by default)
  --timeout <seconds>       Timeout per scene (default: 600)
  --output <file>           Output file for detailed log
  --summary                 Print summary only (no full output)
  --help                    Show help message

Examples:
  # Auto-detect platform and run check
  ./check-memory-leaks.sh scenes/test/sphere.rt

  # Force valgrind with 5-minute timeout
  ./check-memory-leaks.sh --tool valgrind --timeout 300 scenes/test/complex.rt

  # Output to specific file
  ./check-memory-leaks.sh --output leak_report.txt scenes/test/scene.rt
```

### Exit Codes
- `0`: No leaks detected, success
- `1`: Memory leaks detected
- `2`: Tool not available or installation failed
- `3`: Scene file not found or invalid
- `124`: Timeout exceeded

---

## Platform-Specific Behavior

### Linux (valgrind)

#### Prerequisites
- valgrind 3.15 or newer
- Auto-install if missing: `apt-get install -y valgrind`

#### Execution Command
```bash
timeout ${TIMEOUT} valgrind \
  --leak-check=full \
  --show-leak-kinds=all \
  --track-origins=yes \
  --error-exitcode=1 \
  --log-file=${LOG_FILE} \
  ./miniRT ${SCENE_FILE} > /dev/null 2>&1
```

#### Configuration Options

**--leak-check=full**
- Provides detailed information about each leak
- Shows allocation point and leak location

**--show-leak-kinds=all**
- Reports: definitely lost, indirectly lost, possibly lost, still reachable
- All categories treated as failures (zero tolerance policy)

**--track-origins=yes**
- Tracks origin of uninitialized values
- Helps identify root cause of leaks

**--error-exitcode=1**
- Exit with code 1 if any leaks detected
- Enables automated pass/fail determination

#### Output Format
```
==12345== Memcheck, a memory error detector
==12345== Command: ./miniRT scenes/test/sphere.rt
==12345== 
==12345== HEAP SUMMARY:
==12345==     in use at exit: 1,024 bytes in 2 blocks
==12345==   total heap usage: 150 allocs, 148 frees, 45,678 bytes allocated
==12345== 
==12345== LEAK SUMMARY:
==12345==    definitely lost: 512 bytes in 1 blocks
==12345==    indirectly lost: 512 bytes in 1 blocks
==12345==      possibly lost: 0 bytes in 0 blocks
==12345==    still reachable: 0 bytes in 0 blocks
==12345==         suppressed: 0 bytes in 0 blocks
```

#### Success Criteria
Output must contain:
```
==XXXXX== All heap blocks were freed -- no leaks are possible
```

Or:
```
==XXXXX== LEAK SUMMARY:
==XXXXX==    definitely lost: 0 bytes in 0 blocks
==XXXXX==    indirectly lost: 0 bytes in 0 blocks
==XXXXX==      possibly lost: 0 bytes in 0 blocks
```

#### Suppression (if needed)
Create valgrind.supp for known library issues:
```
{
   MinilibX_known_leak
   Memcheck:Leak
   match-leak-kinds: definite
   fun:malloc
   obj:*/libmlx.a
}
```

Use with: `--suppressions=valgrind.supp`

---

### macOS (leaks)

#### Prerequisites
- Xcode Command Line Tools
- leaks command (pre-installed)
- No auto-install needed (verify availability only)

#### Execution Strategy
```bash
# Start program in background
./miniRT ${SCENE_FILE} > /dev/null 2>&1 &
PID=$!

# Wait for program to initialize
sleep 2

# Run leaks check
leaks ${PID} > ${LOG_FILE} 2>&1
LEAK_STATUS=$?

# Clean up
kill ${PID} 2>/dev/null || true
wait ${PID} 2>/dev/null || true

# Check results
if grep -q "0 leaks for 0 total leaked bytes" ${LOG_FILE}; then
    exit 0
else
    exit 1
fi
```

#### Alternative: MallocStackLogging
For detailed stack traces:
```bash
export MallocStackLogging=1
./miniRT ${SCENE_FILE} &
PID=$!
sleep 2
leaks ${PID}
```

#### Output Format
```
Process 12345: 1024 nodes malloced for 45678 KB
Process 12345: 2 leaks for 1024 total leaked bytes.

Leak: 0x7f8a1b000000  size=512  zone: DefaultMallocZone
    Call stack: [bt] | 0x100001234 (miniRT + 1234) | main.c:42
    
Leak: 0x7f8a1b000200  size=512  zone: DefaultMallocZone
    Call stack: [bt] | 0x100001567 (miniRT + 5678) | parser.c:89
```

#### Success Criteria
Output must contain:
```
Process XXXXX: 0 leaks for 0 total leaked bytes.
```

---

## Output Specification

### Console Output (Summary)

**On Success:**
```
✓ Memory Check: PASSED
  Scene: scenes/test/sphere.rt
  Tool: valgrind
  Duration: 12.3s
  Result: No leaks detected
```

**On Failure:**
```
✗ Memory Check: FAILED
  Scene: scenes/test/sphere.rt
  Tool: valgrind
  Duration: 15.7s
  
  LEAK SUMMARY:
  - Definitely lost: 512 bytes in 1 block
    Location: src/parser/parse_scene.c:142
    Function: ft_strdup
  
  - Indirectly lost: 512 bytes in 1 block
    Location: src/parser/parse_object.c:89
    Function: allocate_object
  
  Total leaked: 1,024 bytes
  
  To reproduce:
    valgrind --leak-check=full ./miniRT scenes/test/sphere.rt
```

**On Timeout:**
```
⚠ Memory Check: TIMEOUT
  Scene: scenes/test/complex.rt
  Tool: valgrind
  Duration: 600s (timeout)
  
  The scene took too long to process. This may indicate:
  - Infinite loop
  - Extremely complex scene
  - Performance issue
  
  Increase timeout with: --timeout 900
```

### Detailed Log File

Full output saved to file specified by `--output` or default location:
- CI: `${GITHUB_WORKSPACE}/logs/memory-check-${TIMESTAMP}.log`
- Local: `./logs/memory-check-${TIMESTAMP}.log`

Format: Raw valgrind/leaks output (no filtering)

Size limit: 10MB (truncate if exceeded, keep first 1MB and last 1MB)

---

## Integration with CI

### GitHub Actions Workflow Integration

```yaml
- name: Check Memory Leaks
  run: |
    mkdir -p logs
    for scene in scenes/test/*.rt; do
      echo "Testing: $scene"
      .github/scripts/check-memory-leaks.sh \
        --output "logs/$(basename $scene .rt)-leak.log" \
        "$scene" || {
          echo "::error::Memory leak in $scene"
          exit 1
        }
    done

- name: Upload Leak Logs
  if: failure()
  uses: actions/upload-artifact@v3
  with:
    name: memory-leak-logs
    path: logs/*.log
    retention-days: 30
```

### Parallel Execution

For faster CI runs:
```yaml
strategy:
  matrix:
    scene:
      - scenes/test/sphere.rt
      - scenes/test/plane.rt
      - scenes/test/cylinder.rt
  fail-fast: false

steps:
  - name: Check ${{ matrix.scene }}
    run: .github/scripts/check-memory-leaks.sh "${{ matrix.scene }}"
```

---

## Performance Characteristics

### Execution Time

| Scene Complexity | Normal Runtime | valgrind Runtime | Slowdown |
|------------------|----------------|------------------|----------|
| Simple (1 object) | 0.5s | 5-10s | 10-20x |
| Medium (10 objects) | 2s | 20-40s | 10-20x |
| Complex (100 objects) | 10s | 100-200s | 10-20x |

### Timeout Recommendations
- Simple scenes: 60s
- Medium scenes: 300s (5 minutes)
- Complex scenes: 600s (10 minutes)
- Very complex: 900s (15 minutes)

### Memory Overhead
- valgrind: 2-3x normal memory usage
- leaks: Negligible overhead

---

## Error Handling

### Scenario: Tool Not Installed

**Linux (valgrind missing):**
```bash
if ! command -v valgrind &> /dev/null; then
    echo "valgrind not found. Installing..."
    sudo apt-get update -qq
    sudo apt-get install -y valgrind
    
    if ! command -v valgrind &> /dev/null; then
        echo "::error::Failed to install valgrind"
        exit 2
    fi
fi
```

**macOS (leaks missing):**
```bash
if ! command -v leaks &> /dev/null; then
    echo "::error::leaks command not found"
    echo "Install Xcode Command Line Tools:"
    echo "  xcode-select --install"
    exit 2
fi
```

### Scenario: Scene File Not Found
```bash
if [ ! -f "$SCENE_FILE" ]; then
    echo "::error::Scene file not found: $SCENE_FILE"
    exit 3
fi
```

### Scenario: miniRT Binary Missing
```bash
if [ ! -x "./miniRT" ]; then
    echo "::error::miniRT binary not found or not executable"
    echo "Run 'make' first"
    exit 2
fi
```

### Scenario: Timeout Exceeded
```bash
timeout ${TIMEOUT} valgrind ... || {
    exit_code=$?
    if [ $exit_code -eq 124 ]; then
        echo "::warning::Timeout exceeded (${TIMEOUT}s)"
        exit 124
    fi
    exit $exit_code
}
```

---

## Testing Strategy

### Test Cases

**1. No Leaks (Expected: Pass)**
```bash
# Simple valid scene
./check-memory-leaks.sh scenes/test/simple_sphere.rt
# Expected: exit 0, "No leaks detected"
```

**2. Intentional Leak (Expected: Fail)**
```c
// test_leak.c - for testing
void intentional_leak() {
    char *leak = malloc(42);
    // Intentionally don't free
}
```
```bash
# Compile test program with leak
# Run check
./check-memory-leaks.sh test_program.rt
# Expected: exit 1, "definitely lost: 42 bytes"
```

**3. Timeout (Expected: Timeout Exit Code)**
```bash
# Scene with infinite loop (for testing)
timeout 5 ./check-memory-leaks.sh --timeout 5 scenes/test/infinite.rt
# Expected: exit 124, timeout message
```

**4. Invalid Scene (Expected: Error)**
```bash
./check-memory-leaks.sh nonexistent.rt
# Expected: exit 3, "Scene file not found"
```

---

## Suppression Policy

### When to Suppress Leaks

**NEVER suppress:**
- Leaks in miniRT code (src/ and includes/)
- Leaks in our allocations

**MAY suppress (with justification):**
- Known library leaks (MinilibX, system libraries)
- Leaks in third-party code we don't control
- Leaks on program exit (if library doesn't clean up)

### Suppression File Format
```
{
   Descriptive_Name
   Memcheck:Leak
   match-leak-kinds: definite
   fun:malloc
   ...
   obj:*/problematic_library.so
}
```

### Review Process
- All suppressions require code review justification
- Document why suppression is necessary
- Link to library issue tracker if applicable
- Revisit suppressions each release

---

## Maintenance

### Updating Thresholds
Currently: Zero tolerance (0 leaks allowed)

If policy changes:
```bash
# Allow up to N bytes leaked (NOT RECOMMENDED)
LEAKED_BYTES=$(grep "definitely lost:" ${LOG_FILE} | awk '{print $4}')
if [ "$LEAKED_BYTES" -gt 1024 ]; then
    exit 1
fi
```

### Adding New Platforms
1. Add platform detection in script
2. Document tool requirements
3. Update this contract
4. Test on new platform

### Tool Version Updates
- Test with new valgrind/leaks versions
- Update minimum version requirements
- Document any new options or behavior changes

---

## FAQ

**Q: Why zero tolerance for leaks?**  
A: Memory leaks are bugs. Even small leaks accumulate over time and cause crashes.

**Q: Why is valgrind so slow?**  
A: It instruments every memory operation. This is necessary for accurate leak detection.

**Q: Can I disable leak checking locally?**  
A: Yes, but CI will still enforce it. Use `make` without leak checks for fast iteration.

**Q: What if I have a legitimate long-running scene?**  
A: Increase timeout with `--timeout` option. Document why in scene file comments.

**Q: How do I debug a specific leak?**  
A: Download full log from CI artifacts, search for your function name, trace back allocation.

---

## Version History

- **1.0.0** (2025-12-18): Initial contract specification
