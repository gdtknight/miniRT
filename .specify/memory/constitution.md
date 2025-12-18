<!--
SYNC IMPACT REPORT
==================
Version Change: 1.2.1 → 2.0.0
Updated: 2025-12-19

RATIONALE FOR VERSION 2.0.0:
- MAJOR version bump: Complete rewrite of constitution principles
- Previous constitution was generic for 42 school projects
- New constitution is specific to miniRT project structure and workflows
- 5 new principles focused on automation, CI/CD, and project organization
- Removed generic principles that don't apply to miniRT's specific needs
- Backward incompatible: Complete governance framework change

PRINCIPLES DEFINED (5 total):
1. Project Structure Standards (NON-NEGOTIABLE) - NEW
2. Code Quality Automation (NON-NEGOTIABLE) - NEW
3. Documentation and Wiki Synchronization (NON-NEGOTIABLE) - NEW
4. Workflow System (NON-NEGOTIABLE) - NEW
5. Tools and Environment Standards (NON-NEGOTIABLE) - NEW

REMOVED PRINCIPLES:
- Readability-First Code (generic, not specific to miniRT)
- Mandatory Unit Testing (miniRT uses integration tests instead)
- Bilingual Documentation (miniRT uses Korean-only docs)
- Build Verification Workflow (replaced by Workflow System)
- GitHub Issue Tracking (not enforced in miniRT)
- File Organization Standards (replaced by Project Structure Standards)
- 42 Norminette Compliance (absorbed into Code Quality Automation)

ADDED PRINCIPLES:
- Principle I: Project Structure Standards - Script organization, documentation hierarchy, CI/CD separation
- Principle II: Code Quality Automation - norminette, valgrind, build checks in CI
- Principle III: Documentation and Wiki Synchronization - Auto-update wiki on version tags
- Principle IV: Workflow System - Development, PR, and Release workflows
- Principle V: Tools and Environment Standards - Linux/macOS support, MinilibX automation

TEMPLATES REQUIRING UPDATES:
✅ plan-template.md - Constitution check section remains generic
✅ spec-template.md - No changes needed (functional requirements unchanged)
✅ tasks-template.md - No changes needed (task organization compatible)
✅ README.md - Already describes project structure per new Principle I

FOLLOW-UP ACTIONS:
- Implement GitHub Actions workflows per Principle II and IV
- Create wiki sync automation per Principle III
- Verify all scripts follow Principle I organization
- Document MinilibX build automation per Principle V
- Update CI/CD pipelines to enforce all quality gates
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
users always have access to current information.

**Automation Requirements**:
- Version tag creation MUST trigger automatic GitHub Wiki update
- Wiki generation MUST be based on source documentation in `docs/`
- Wiki MUST always reflect the latest release version
- Source docs in `docs/` are the single source of truth

**Synchronization Workflow**:
- On version tag creation: Extract docs → Generate Wiki markdown → Push to Wiki
- Documentation changes MUST trigger wiki update workflow when merged to main
- Failed wiki updates MUST be logged but MUST NOT block releases

**Content Requirements**:
- Wiki pages MUST be generated from Korean documentation in docs/
- Wiki structure MUST mirror docs/ organization for consistency
- Wiki MUST include version information on each page

**Rationale**: Automatic wiki synchronization eliminates manual documentation drift,
ensures users see current information, reduces maintenance burden, and creates a
reliable documentation workflow tied to version releases.

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

**Version**: 2.0.0 | **Ratified**: 2025-12-19 | **Last Amended**: 2025-12-19
