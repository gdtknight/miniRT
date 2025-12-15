<!--
SYNC IMPACT REPORT
==================
Version Change: 0.0.0 → 1.0.0
Created: 2025-12-15

RATIONALE FOR VERSION 1.0.0:
- Initial constitution ratification for 42 miniRT project
- Establishes foundational governance and development principles

PRINCIPLES DEFINED:
1. 42 Norminette Compliance (NON-NEGOTIABLE)
2. Readability-First Code
3. Mandatory Unit Testing (NON-NEGOTIABLE)
4. Bilingual Documentation
5. Build Verification Workflow (NON-NEGOTIABLE)

ADDED SECTIONS:
- Core Principles (5 principles)
- Documentation Standards
- Quality Assurance Workflow
- Governance

TEMPLATES REQUIRING UPDATES:
✅ plan-template.md - Constitution Check section references this constitution
✅ spec-template.md - Aligned with documentation requirements
✅ tasks-template.md - Aligned with testing and verification requirements

FOLLOW-UP ACTIONS:
- None - All placeholders filled
- All template dependencies verified
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

## Documentation Standards

**README.md Requirements**:
- MUST be maintained at project root
- MUST include a history/changelog section documenting all significant updates
- MUST provide build instructions, usage examples, and project overview

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

**Version**: 1.0.0 | **Ratified**: 2025-12-15 | **Last Amended**: 2025-12-15
