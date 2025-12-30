<!--
SYNC IMPACT REPORT
==================
Version Change: 2.1.0 → 2.2.0
Updated: 2025-12-19

RATIONALE FOR VERSION 2.2.0:
- MINOR version bump: Added new principle about 42 School constraints
- Added Principle VII: 42 School Function Constraints
- Defines allowed/prohibited functions for miniRT project
- Specifies permitted optimization techniques within 42 constraints
- No backward-incompatible changes to existing principles
- Existing workflows and templates remain valid

PRINCIPLES DEFINED (7 total):
1. Project Structure Standards (NON-NEGOTIABLE)
2. Code Quality Automation (NON-NEGOTIABLE)
3. Documentation and Wiki Synchronization (NON-NEGOTIABLE)
4. Workflow System (NON-NEGOTIABLE)
5. Tools and Environment Standards (NON-NEGOTIABLE)
6. Bilingual Specification Management (NON-NEGOTIABLE)
7. 42 School Function Constraints (NON-NEGOTIABLE) - NEW

ADDED PRINCIPLES:
- Principle VII: 42 School Function Constraints
  - Defines allowed functions for miniRT project (libc, math, MiniLibX, libft, GNL)
  - Explicitly prohibits pthread, fork, external libraries for parallelization
  - Lists prohibited optimization techniques (multithreading, multiprocessing, SIMD)
  - Specifies allowed optimization approaches (algorithmic, caching, math optimization)
  - Ensures all implementations comply with 42 School evaluation requirements

TEMPLATES REQUIRING UPDATES:
✅ plan-template.md - Will add 42 constraints check to constitution validation
✅ spec-template.md - No changes needed (functional requirements unchanged)
✅ tasks-template.md - No changes needed (task organization compatible)
✅ README.md - Already indicates 42 School project status

FOLLOW-UP ACTIONS:
- Add 42 function constraint checks to plan-template.md constitution section
- Verify no pthread or prohibited functions exist in current codebase
- Document optimization strategies within allowed constraints
- Update CI to validate function usage against allowed list
-->

# miniRT Project Constitution

## Core Principles

### I. Project Structure Standards (NON-NEGOTIABLE)

All project files MUST follow strict organization rules to maintain clarity between
CI automation, user-facing tools, and documentation.

**Script Organization**:
- CI-related scripts MUST be located in `.github/scripts/`
- User-facing utility scripts (create_wiki.sh, test_miniRT.sh) MUST remain at project root for accessibility
- No automation scripts may be scattered throughout the codebase

**Documentation Hierarchy**:
- Old/backup documentation (*.backup, *_OLD.md) MUST be moved to `docs/archive/`
- Wiki-related documentation MUST be organized in `docs/wiki/`
- Implementation planning documents MUST be located in `docs/project/`
- Active documentation MUST use clear, descriptive names without backup suffixes
- All Korean documentation MUST be centralized in `docs/` directory

**File Lifecycle Management**:
- Deprecated files MUST be archived, not left scattered at root level
- Active files MUST have clear purpose and location
- Logs and build artifacts MUST be in `.gitignore`

**Rationale**: Clear separation between CI infrastructure, user tools, and documentation
prevents confusion, improves maintainability, and ensures contributors can quickly locate
resources. Archiving old documents prevents clutter while preserving history.

### II. Code Quality Automation (NON-NEGOTIABLE)

All code changes MUST pass automated quality gates before being merged. Quality checks
are automated in CI with no manual exceptions.

**Mandatory Quality Gates**:
- norminette check MUST pass on all PRs with zero warnings or errors
- Memory leak check (valgrind) MUST be automated in CI with zero leaks tolerated
- Build MUST succeed without errors on all supported platforms
- Quality gate formula: `norminette + build + memory leak check = PASS/FAIL`

**CI Enforcement**:
- Failed norminette check MUST fail the CI pipeline
- Memory leaks detected by valgrind MUST fail the CI pipeline
- Build errors MUST fail the CI pipeline
- No PR may be merged with failing quality gates

**Reporting**:
- All quality check failures MUST provide clear, actionable error messages
- CI logs MUST be easily accessible and readable
- Status badges SHOULD reflect current quality gate status

**Rationale**: Automated quality gates ensure consistent code quality, prevent
regression, catch issues early, and maintain project stability without relying
on manual review. This enables confident iteration and rapid development.

### III. Documentation and Wiki Synchronization (NON-NEGOTIABLE)

GitHub Wiki MUST be automatically synchronized with source documentation to ensure
users always have access to current information that matches the released version.

**Automation Requirements**:
- Version tag creation MUST trigger automatic GitHub Wiki update
- Wiki generation MUST be based on source documentation in `docs/`
- Wiki MUST always reflect the latest release version
- Source docs in `docs/` are the single source of truth

**Release Synchronization**:
- Wiki updates MUST occur during the release process
- Wiki content MUST match the exact state of docs/ in the release tag
- Release version and documentation version MUST be consistent
- Wiki pages MUST display the release version they correspond to

**Synchronization Workflow**:
- On version tag creation: Extract docs → Generate Wiki markdown → Push to Wiki
- Documentation changes MUST trigger wiki update workflow when merged to main
- Failed wiki updates MUST be logged but MUST NOT block releases
- Release checklist MUST include wiki synchronization verification

**Content Requirements**:
- Wiki pages MUST be generated from Korean documentation in docs/
- Wiki structure MUST mirror docs/ organization for consistency
- Wiki MUST include version information on each page
- Each wiki page MUST clearly indicate which release version it documents

**Rationale**: Automatic wiki synchronization eliminates manual documentation drift,
ensures users see current information, reduces maintenance burden, and creates a
reliable documentation workflow tied to version releases. Release synchronization
ensures documentation always matches the codebase state for each version.

### IV. Workflow System (NON-NEGOTIABLE)

Development follows a structured workflow system with clear quality gates at each stage.

**Development Workflow**:
- Code changes MUST pass: norminette + build + test
- Commits MUST be atomic and well-described
- Local testing MUST be performed before pushing

**Pull Request Workflow**:
- PRs MUST pass: norminette + build + test + memory leak check
- Code review MUST verify adherence to all constitution principles
- PR title MUST clearly describe the change
- No PR may be merged with failing checks

**Release Workflow**:
- Releases MUST pass all PR checks
- Releases MUST trigger: Wiki auto-update + artifact generation
- Release notes MUST document changes clearly in Korean
- Version tags MUST follow semantic versioning (vX.Y.Z)

**Automation Principles**:
- All checks MUST be automated—no manual gates
- Failed checks MUST provide clear failure messages and remediation guidance
- Workflow status MUST be visible in PR interface

**Rationale**: Structured workflows with automated gates ensure consistency,
prevent human error, maintain project quality, and enable confident releases.
Clear stages help contributors understand expectations at each development phase.

### V. Tools and Environment Standards (NON-NEGOTIABLE)

Development environment and tooling MUST support multiple platforms and provide
automated dependency management.

**Platform Support**:
- Project MUST support Linux and macOS
- Build system MUST detect platform and configure appropriately
- Platform-specific issues MUST be documented in troubleshooting guides

**Dependency Management**:
- MinilibX build MUST be automated in Makefile
- External library dependencies MUST be clearly documented
- Missing dependencies MUST produce clear error messages with installation instructions

**Test Infrastructure**:
- Test scene files MUST be systematically organized in `scenes/`
- Test script (test_miniRT.sh) MUST be accessible at project root
- Test scenes MUST cover all supported features and edge cases

**Build Artifacts**:
- Logs MUST be stored in `logs/` directory
- Build artifacts MUST be stored in `build/` directory
- All logs and build artifacts MUST be in `.gitignore`

**Rationale**: Cross-platform support ensures broader usability, automated dependency
management reduces setup friction, systematic test organization improves quality
assurance, and proper artifact management keeps the repository clean.

### VI. Bilingual Specification Management (NON-NEGOTIABLE)

All feature specifications MUST be maintained in both English and Korean, with strict
synchronization requirements to ensure both language versions remain consistent.

**Directory Structure**:
- English specifications MUST be located in `specs/` directory
- Korean translations MUST be located in `docs/specs/` directory
- Directory structure MUST be mirrored between specs/ and docs/specs/
- File names MUST be identical in both directories

**Synchronization Requirements**:
- When English specification is created in specs/, Korean version MUST be created in docs/specs/
- When English specification is updated, Korean version MUST be updated simultaneously
- Both versions MUST be updated in the same commit/PR
- Version numbers MUST match between English and Korean documents
- Content MUST be semantically equivalent between language versions

**Release Consistency**:
- Release packages MUST include both English and Korean documentation
- Release notes MUST be available in both languages
- Wiki synchronization MUST process both language versions
- Documentation version tags MUST apply to both English and Korean versions

**Quality Assurance**:
- PRs touching specs/ MUST include corresponding docs/specs/ changes
- CI checks SHOULD verify both versions exist and are up-to-date
- Documentation reviews MUST verify semantic consistency between languages
- Out-of-sync documentation MUST NOT be merged

**Rationale**: Bilingual documentation ensures accessibility for both international
collaborators and Korean-speaking users. Synchronized maintenance prevents translation
drift and ensures all stakeholders have access to accurate, current information in
their preferred language. This supports both the 42 School community and broader
open-source contribution.

### VII. 42 School Function Constraints (NON-NEGOTIABLE)

All code implementations MUST comply with 42 School miniRT project constraints, which
strictly limit allowed functions and prohibit certain optimization techniques to ensure
projects are evaluated fairly according to curriculum requirements.

**Allowed Functions**:
- Standard C library: `open`, `close`, `read`, `write`, `printf`, `malloc`, `free`, 
  `perror`, `strerror`, `exit`
- Math library: All functions from `math.h` (linked with `-lm`)
- MiniLibX library: All `mlx_*` functions provided by the school's graphics library
- Custom implementations: `get_next_line` (if implemented by student)
- Custom library: All functions from `libft` (if written by student)

**Prohibited Functions and Techniques**:
- pthread library: `pthread_create`, `pthread_join`, `pthread_mutex_*`, and all 
  pthread-related functions MUST NOT be used
- Process management: `fork`, `pipe`, and multiprocessing functions MUST NOT be used
- External optimization libraries: SIMD intrinsics, OpenMP, Intel TBB, or any external
  parallelization libraries MUST NOT be used
- Multithreading/multiprocessing: Any form of parallel execution MUST NOT be implemented

**Allowed Optimization Strategies**:
- Algorithmic optimization: BVH (Bounding Volume Hierarchy), spatial partitioning, 
  octrees, kd-trees for reducing ray-object intersection tests
- Early termination: Ray culling, frustum culling, backface culling to skip 
  unnecessary calculations
- Caching: Precomputed values, lookup tables, memoization of expensive calculations
- Memory layout optimization: Structure packing, cache-friendly data organization, 
  memory access patterns
- Mathematical optimization: Reduced precision where appropriate, algebraic 
  simplification, optimized vector operations

**Validation Requirements**:
- Code review MUST verify no prohibited functions are used
- Function usage MUST be validated against allowed list before merge
- Any optimization technique MUST be implementable using only allowed functions
- CI checks SHOULD scan for prohibited function usage

**Evaluation Compliance**:
- All implementations MUST pass 42 School evaluation criteria
- Projects using prohibited functions WILL fail evaluation regardless of functionality
- Performance optimization MUST NOT compromise evaluation compliance
- Documentation MUST clearly indicate which optimization techniques are used

**Rationale**: 42 School projects are evaluated based on specific constraints to ensure
students learn core algorithms and data structures without relying on advanced libraries
or parallelization. These constraints ensure fair evaluation, promote understanding of
fundamental computer graphics concepts, and maintain consistency across all student
submissions. Compliance with these constraints is mandatory for project success.

## Documentation Standards

**README.md Requirements**:
- MUST be maintained at project root for GitHub display
- MUST include project overview, build instructions, and usage examples
- MUST include development history with version-tagged releases
- MUST link to GitHub Wiki for detailed documentation
- MUST be written in Korean as primary language

**Korean Documentation Requirements** (`docs/` directory):
- All documentation MUST be written in Korean (한글)
- MUST contain detailed explanations of features and implementation
- MUST explain design decisions, algorithms, and technical rationale
- MUST be updated whenever corresponding code changes
- MUST maintain clear directory structure (wiki/, archive/, project/)

**Code Comments Requirements**:
- All C functions MUST have Doxygen-style comments in English
- Comments MUST include: brief description, parameters, return values
- Complex algorithms SHOULD include example usage or algorithmic explanation
- 42 Header format MUST be included in all C files

## Quality Assurance Workflow

Every code change MUST complete this workflow before being considered ready for merge:

1. **Implementation** - Write code following norminette standards
2. **Local Build** - Verify `make` builds without errors or warnings
3. **Execution Test** - Run the program with test scenes and verify behavior
4. **Norminette Check** - Run `norminette` and fix any violations
5. **Memory Check** - Run with valgrind and verify zero leaks
6. **Documentation Update** - Update code comments and docs/ if needed
7. **Commit & Push** - Atomic commits with clear messages
8. **CI Validation** - Verify all automated checks pass in GitHub Actions

**Quality Gates**:
- Local testing SHOULD catch issues before push
- CI/CD MUST enforce all quality gates automatically
- Code review MUST verify constitution compliance
- Failed checks MUST block merging

**Test Scene Coverage**:
- All features MUST have corresponding test scenes in `scenes/`
- Regression test scenes MUST be preserved after bug fixes
- Test script (test_miniRT.sh) MUST validate all critical scenes

## Governance

This constitution defines the non-negotiable principles and automated workflows for the
miniRT project. All development activity MUST comply with these principles.

**Amendment Process**:
- Constitution changes require documented rationale in sync impact report
- Version MUST be incremented using semantic versioning (MAJOR.MINOR.PATCH)
- MAJOR version: Backward-incompatible principle changes or complete framework rewrites
- MINOR version: New principles added or substantial guidance expansions
- PATCH version: Clarifications, typo fixes, or minor wording improvements

**Compliance Enforcement**:
- All PRs MUST verify constitution compliance before merge
- CI/CD automation MUST enforce all NON-NEGOTIABLE principles
- Any deviation from NON-NEGOTIABLE principles MUST be rejected
- Template workflows in `.specify/templates/` MUST reference these principles

**Conflict Resolution**:
- NON-NEGOTIABLE principles take absolute precedence
- When principles conflict, prioritize: Code Quality → Documentation → Structure
- Document conflicts and resolutions in constitution amendments
- Escalate unresolvable conflicts through GitHub issues

**Review Cycle**:
- Constitution SHOULD be reviewed after major feature additions
- Outdated principles MUST be updated or removed
- New automation capabilities SHOULD be reflected in principles

**Version**: 2.2.0 | **Ratified**: 2025-12-19 | **Last Amended**: 2025-12-19
