<!--
SYNC IMPACT REPORT
==================
Version Change: 1.2.0 → 1.2.1
Updated: 2025-12-17

RATIONALE FOR VERSION 1.2.1:
- PATCH version bump: Enhancement to existing Principle VII
- No new principles added, no principles removed
- Clarification of documentation structure requirements within Principle VII
- Adds detailed subdirectory organization standards for docs/
- Maintains backward compatibility with existing principle framework

PRINCIPLES DEFINED (7 total):
1. 42 Norminette Compliance (NON-NEGOTIABLE)
2. Readability-First Code
3. Mandatory Unit Testing (NON-NEGOTIABLE)
4. Bilingual Documentation
5. Build Verification Workflow (NON-NEGOTIABLE)
6. GitHub Issue Tracking (NON-NEGOTIABLE)
7. File Organization Standards (NON-NEGOTIABLE) ← ENHANCED

MODIFIED PRINCIPLES:
- Principle VII (File Organization Standards) - Enhanced with detailed documentation structure requirements:
  * Added numbered category structure (01-시작하기/, 02-개발/, 03-기능/, 04-릴리스/, 05-프로젝트_히스토리/, 06-참고자료/)
  * Added file naming conventions (Korean with underscores, issue-numbered summaries)
  * Added navigation requirements (docs/README.md as master guide, per-category READMEs)
  * Added content organization rules (feature docs, historical reports, active dev docs, user guides)
  * Added maintenance requirements (document lifecycle management)

TEMPLATES REQUIRING UPDATES:
✅ plan-template.md - No changes needed (constitution check already references organizational standards)
✅ spec-template.md - No changes needed (structure requirements are operational, not specification-level)
✅ tasks-template.md - No changes needed (task organization already compatible)
✅ README.md - Already compliant (exception granted for root-level placement)

FOLLOW-UP ACTIONS:
- Reorganize docs/ directory into numbered category structure
- Create docs/README.md as master navigation guide in Korean
- Add category-level README.md files to each subdirectory
- Migrate existing documentation files to appropriate categories
- Verify all filenames use Korean with underscores
- Update cross-references between documentation files
-->

# 42 miniRT Constitution

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
- All documentation in `docs/` directory MUST be written in Korean (한글)
- Documentation MUST be kept in sync with code changes

**Rationale**: English documentation enables broader code review and collaboration,
while Korean explanations ensure local learners can deeply understand the implementation
without language barriers. Consistent Korean documentation in the docs/ directory
provides centralized learning resources for local developers.

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

### VI. GitHub Issue Tracking (NON-NEGOTIABLE)

All work requests MUST be tracked as GitHub issues, and all commits related to work
requests MUST mark the associated issue number in the commit title.

- Every work request, feature, bug fix, or enhancement MUST have a GitHub issue created
- Issue numbers MUST be referenced in commit titles using format: `[#123] commit message`
- Commits MUST link to their corresponding issue for complete traceability
- Issues MUST remain open until all related work is committed and verified

**Rationale**: Issue tracking ensures proper project management, provides clear work
history, enables traceability between requirements and implementation, and facilitates
code review by connecting changes to their motivating requirements.

### VII. File Organization Standards (NON-NEGOTIABLE)

All project files MUST follow strict organization rules to maintain a clean, navigable
project structure.

**Log Files**:
- All log files (*.log, *.logs) MUST be stored in the `logs/` directory at project root
- No log files may be scattered throughout the codebase or left at project root

**Documentation Files**:
- All markdown documentation files (*.md) MUST be stored in the `docs/` directory at project root
- All documentation in `docs/` directory MUST be written in Korean (한글)
- Exception: Root-level `README.md` may remain at project root for GitHub display purposes
- No documentation files should be scattered throughout the codebase

**Documentation Structure** (docs/ directory):
- MUST use numbered category structure with Korean names:
  * `01-시작하기/` (Getting Started) - Requirements, build guides, scene file guides
  * `02-개발/` (Development) - File organization, norminette, testing, CI/CD
  * `03-기능/` (Features) - Feature-specific docs by subsystem (조명/, 렌더링/, 파싱/, etc.)
  * `04-릴리스/` (Releases) - Release notes, checklists, guides, verification reports
  * `05-프로젝트_히스토리/` (Project History) - Implementation summaries (by issue), status reports
  * `06-참고자료/` (References) - Git summaries, legacy documents, archived material
- File naming MUST use Korean with underscores: `파일_이름.md`
- Implementation summaries MUST be named by issue number: `NNN_기능_설명.md`
- Date-based files SHOULD use YYYY-MM-DD prefix when applicable

**Navigation Requirements**:
- `docs/README.md` MUST exist as master navigation guide in Korean
- Each category subdirectory SHOULD have its own README.md explaining contents
- Cross-references between docs MUST use relative paths

**Content Organization Rules**:
- Feature documentation goes in `03-기능/[subsystem]/`
- Historical/legacy reports go in `05-프로젝트_히스토리/`
- Active development docs go in `02-개발/`
- User-facing guides go in `01-시작하기/`

**Maintenance Requirements**:
- When creating new features, documentation goes in `03-기능/`
- When completing implementations, summaries go in `05-프로젝트_히스토리/구현_요약/`
- Outdated docs SHOULD be moved to `06-참고자료/` with date prefix

**Rationale**: Structured documentation enables easy navigation, prevents document sprawl,
maintains Korean consistency, supports project growth, and ensures developers can quickly
find relevant information. Numbered categories provide intuitive ordering and scalability.
The systematic organization reduces cognitive load when navigating the codebase and makes
it easier for new contributors to understand the project layout. Centralizing Korean
documentation in docs/ provides a single source of truth for learning resources.

## Documentation Standards

**README.md Requirements**:
- MUST be maintained at project root
- MUST include a history/changelog section documenting all significant updates
- MUST provide build instructions, usage examples, and project overview

**Korean Documentation Requirements** (`docs/` directory):
- MUST contain detailed explanations of all functions in Korean
- MUST explain design decisions, algorithms, and implementation rationale
- MUST follow numbered category structure per Principle VII
- MUST be updated whenever corresponding source code changes
- All documentation files MUST be written in Korean (한글) per Principle VII
- MUST maintain docs/README.md as master navigation guide

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
8. **File Organization Check** - Verify log files in `logs/` and documentation in `docs/`

**Checkpoints**:
- Code review MUST verify all 8 steps completed
- Merge MUST be blocked if any step fails
- CI/CD (if configured) MUST enforce compilation and unit test success

## Governance

This constitution defines the non-negotiable principles and workflows for the 42 miniRT
project. All development activity MUST comply with these principles.

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

**Version**: 1.2.1 | **Ratified**: 2025-12-15 | **Last Amended**: 2025-12-17
