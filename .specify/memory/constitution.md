<!--
SYNC IMPACT REPORT
==================
Version Change: 1.1.0 → 1.2.0
Last Updated: 2025-12-17

RATIONALE FOR VERSION 1.2.0:
- MINOR version bump: Added new principle (VIII)
- New principle: Project-Centric Documentation (NON-NEGOTIABLE)
- No backward-incompatible changes to existing principles

PREVIOUS VERSION (1.1.0):
- Added Principle VI: Git Branch Strategy Compliance (NON-NEGOTIABLE)
- Added Principle VII: Structured Log Management
- Updated Governance section to reflect workflow requirements

CURRENT VERSION (1.2.0):
- Added Principle VIII: Project-Centric Documentation (NON-NEGOTIABLE)
- Updated Documentation Standards to enforce project-centric language

PRINCIPLES DEFINED (8 total):
1. 42 Norminette Compliance (NON-NEGOTIABLE)
2. Readability-First Code
3. Mandatory Unit Testing (NON-NEGOTIABLE)
4. Bilingual Documentation
5. Build Verification Workflow (NON-NEGOTIABLE)
6. Git Branch Strategy Compliance (NON-NEGOTIABLE)
7. Structured Log Management
8. Project-Centric Documentation (NON-NEGOTIABLE) ⬅ NEW

MODIFIED SECTIONS:
- Core Principles: Added Principle VIII
- Documentation Standards: Enhanced to require project-centric language

TEMPLATES REQUIRING UPDATES:
✅ plan-template.md - Constitution Check section already references this constitution
✅ spec-template.md - Aligned with documentation requirements
✅ tasks-template.md - Should include git workflow and logging tasks
⚠ PENDING REVIEW - README.md and other docs need audit for "42" mentions that violate new principle

FOLLOW-UP ACTIONS:
- Audit README.md for unnecessary "42" references that don't add project value
- Update all documentation to focus on project content, not institutional affiliation
- Ensure badges and acknowledgments remain but narrative focuses on technical content
-->

# miniRT Constitution

## Core Principles

### I. 42 Norminette Compliance (NON-NEGOTIABLE)

All C source files (*.c) and header files (*.h) MUST comply with 42 school norminette
coding standards. This is a hard requirement with no exceptions.

- Code MUST pass `norminette` checks without warnings or errors
- All files MUST include the 42 Header format
- 42 Header USER and EMAIL fields MUST be populated via environment variables
- Line length, function complexity, file organization MUST follow norminette rules

**Rationale**: The 42 norminette ensures consistent, maintainable code style that is
required for project evaluation and establishes a shared baseline for all contributors.

### II. Readability-First Code

Code readability is the top priority. When coding style conflicts arise, choose the
approach that makes the code easier to understand and maintain.

- Variable and function names MUST be descriptive and meaningful
- Complex logic MUST be broken into well-named helper functions
- Magic numbers MUST be replaced with named constants
- Code organization MUST follow logical grouping principles

**Rationale**: Readable code reduces bugs, accelerates onboarding, and simplifies
debugging. In an educational context, clarity teaches better than cleverness.

### III. Mandatory Unit Testing (NON-NEGOTIABLE)

Every individual function MUST have unit tests written and passing after implementation.
No function is considered complete until its unit tests exist and pass.

- Unit tests MUST be created for each function after implementation
- Tests MUST verify expected behavior and edge cases
- Tests MUST be automated and repeatable
- Failed tests MUST block merging of code

**Rationale**: Unit testing catches regressions early, documents expected behavior,
and ensures each component works in isolation before integration.

### IV. Bilingual Documentation

All code MUST be documented in both English and Korean to support international
collaboration and local learning.

- Every function MUST have English Doxygen-style comments in the source code
- All functions and implementation details MUST be explained in Korean markdown files
- Korean documentation MUST reside in `docs/` directory at project root
- Documentation MUST be kept in sync with code changes

**Rationale**: English documentation enables broader code review and collaboration,
while Korean explanations ensure local learners can deeply understand the implementation
without language barriers.

### V. Build Verification Workflow (NON-NEGOTIABLE)

After any code change, the complete build-run-verify cycle MUST be executed to ensure
the project remains in a working state.

Required verification steps after every change:
1. Verify compilation succeeds without warnings
2. Verify the compiled program executes successfully
3. Verify no runtime errors occur during execution
4. Verify unit tests are created and passing

**Rationale**: Continuous verification prevents integration issues, maintains project
stability, and catches problems immediately when context is fresh.

### VI. Git Branch Strategy Compliance (NON-NEGOTIABLE)

All development work MUST follow the established git branch strategy. No direct commits
to main or develop branches are permitted.

- All work MUST be performed on feature branches
- Feature branches MUST follow naming convention: `###-feature-name` or `feature/descriptive-name`
- Feature branches MUST be created from develop branch
- Code MUST be merged via pull requests with review
- Commits MUST have descriptive messages following conventional commit format
- Main branch MUST only receive merges from develop or hotfix branches
- Develop branch MUST integrate completed features before main merge

**Rationale**: Structured git workflow ensures code quality through review, maintains
clear history of feature development, enables parallel work without conflicts, and
provides rollback capability for problematic changes.

### VII. Structured Log Management

All work activities, build outputs, test results, and operational logs MUST be stored
in the logs/ directory at project root for traceability and debugging.

- Logs MUST be organized by category: build logs, test logs, runtime logs, work logs
- Log files MUST include timestamp in filename (YYYY-MM-DD or YYYYMMDD_HHMMSS format)
- Build and test automation MUST redirect output to log files
- Logs MUST be retained for debugging and audit purposes
- logs/ directory MUST be excluded from git tracking (in .gitignore)
- Critical errors MUST be logged with sufficient context for reproduction

**Rationale**: Centralized logging enables efficient debugging, provides audit trail
for work history, facilitates collaboration by documenting decisions and issues, and
preserves evidence of test execution and verification results.

### VIII. Project-Centric Documentation (NON-NEGOTIABLE)

All project documentation MUST focus on the project's technical content and purpose
without unnecessary institutional references. Documentation MUST describe what the
project does, not where it originated.

- Documentation narrative MUST focus on project functionality, architecture, and usage
- Project descriptions MUST NOT include phrases like "This is a 42 project for..."
- CORRECT format: "This project is a ray tracing renderer..."
- INCORRECT format: "This is a 42 School ray tracing project..."
- Institutional badges, acknowledgments, and author credits MAY remain in appropriate sections
- Technical requirements derived from institutional standards (e.g., norminette) MUST be
  documented as technical constraints without redundant institutional context
- README.md SHOULD have clear separation: technical content first, acknowledgments at end

**Rationale**: Project-centric documentation improves clarity, makes the project more
accessible to wider audiences, and ensures documentation focuses on solving user problems
rather than explaining institutional context. Technical merit and educational value should
speak for themselves without repeated institutional framing.

## Documentation Standards

**README.md Requirements**:
- MUST be maintained at project root
- MUST include a history/changelog section documenting all significant updates
- MUST provide build instructions, usage examples, and project overview
- MUST follow project-centric documentation principle (Principle VIII)
- Project description MUST focus on technical content, not institutional affiliation
- Institutional acknowledgments SHOULD be placed in dedicated sections (e.g., "Acknowledgments")

**Korean Documentation Requirements** (`docs/` directory):
- MUST contain detailed explanations of all functions in Korean
- MUST explain design decisions, algorithms, and implementation rationale
- File organization MUST mirror source structure for easy navigation
- MUST be updated whenever corresponding source code changes

**Code Comments Requirements**:
- All C functions MUST have English Doxygen comments including:
  - Brief description
  - Parameter descriptions with types
  - Return value description
  - Example usage (for complex functions)

## Quality Assurance Workflow

Every code change MUST complete this workflow before being considered done:

1. **Implementation** - Write the code following norminette standards
2. **Unit Testing** - Create unit tests for new/modified functions
3. **Compilation Check** - Verify `make` or equivalent builds without errors/warnings
4. **Execution Test** - Run the program and verify expected behavior
5. **Runtime Verification** - Check for memory leaks, segfaults, and other errors
6. **Documentation Update** - Update English comments and Korean markdown files
7. **Norminette Check** - Run `norminette` and fix any violations

**Checkpoints**:
- Code review MUST verify all 7 steps completed
- Merge MUST be blocked if any step fails
- CI/CD (if configured) MUST enforce compilation and unit test success

## Governance

This constitution defines the non-negotiable principles and workflows for the miniRT
project. All development activity MUST comply with these principles.

**Version Control Requirements**:
- All work MUST follow the git branch strategy (Principle VI)
- Feature branches MUST be used for all development
- Code reviews MUST verify constitution compliance before merge
- Commit history MUST be preserved (no force-push to shared branches)

**Logging Requirements**:
- All work artifacts MUST be stored in logs/ directory (Principle VII)
- Build, test, and runtime logs MUST be preserved
- Work session logs SHOULD document decisions and rationale

**Amendment Process**:
- Constitution changes require documentation of rationale
- Version MUST be incremented using semantic versioning (MAJOR.MINOR.PATCH)
- MAJOR version for backward-incompatible principle changes
- MINOR version for new principles or substantial guidance additions
- PATCH version for clarifications, typo fixes, or minor wording improvements

**Compliance Enforcement**:
- All code reviews MUST verify constitution compliance
- Any deviation from NON-NEGOTIABLE principles MUST be rejected
- Complexity that conflicts with principles MUST be justified in writing and approved
- Template workflows in `.specify/templates/` MUST reference and enforce these principles

**Conflict Resolution**:
- When principles conflict, NON-NEGOTIABLE principles take precedence
- Between non-negotiable principles, apply judgment favoring project stability and correctness
- Document conflicts and resolutions in constitution amendments

**Version**: 1.2.0 | **Ratified**: 2025-12-15 | **Last Amended**: 2025-12-17
