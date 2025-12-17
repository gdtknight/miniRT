# CI/CD Pipeline Documentation

## Overview

This project uses GitHub Actions for continuous integration and deployment. The CI/CD pipeline ensures code quality, automated testing, and streamlined releases.

## Workflows

### 1. CI Workflow (`.github/workflows/ci.yml`)

**Triggers:**
- Push to `main`, `develop`, or feature branches
- Pull requests to `main` or `develop`

**Jobs:**

#### Norminette Check
- Installs and runs norminette on all C source files
- Validates 42 coding standard compliance
- Blocks merge if violations found

#### Build (Linux & macOS)
- Compiles miniRT on both platforms
- Validates cross-platform compatibility
- Uploads compiled binaries as artifacts

#### Test (Linux & macOS)
- Downloads compiled artifacts
- Tests scene file parsing
- Validates error handling
- Linux: Uses Xvfb for headless testing

**Status Badge:**
```markdown
![CI](https://github.com/YOUR_USERNAME/miniRT_final/workflows/CI/badge.svg)
```

### 2. PR Validation Workflow (`.github/workflows/pr-validation.yml`)

**Triggers:**
- Pull request opened, edited, synchronized, or reopened

**Jobs:**

#### Validate Commits
- Checks all commits follow Conventional Commits format
- Expected format: `type(scope): description`
- Valid types: `feat`, `fix`, `docs`, `style`, `refactor`, `test`, `chore`, `perf`, `ci`, `build`, `revert`

#### Validate Branch Name
- Enforces branch naming conventions:
  - `feature/description`
  - `fix/description`
  - `NNN-description` (3-digit number)

#### Validate PR Template
- Ensures PR description includes required sections
- Required: Description, Changes, Testing

#### Detect Sensitive Files
- Scans for potential secrets or sensitive data
- Checks file extensions: `.key`, `.pem`, `.env`
- Detects hardcoded credentials

### 3. Release Workflow (`.github/workflows/release.yml`)

**Triggers:**
- Push tags matching `v*.*.*` (e.g., `v1.0.0`)

**Jobs:**

#### Build Release Binaries
- Builds clean binaries for Linux and macOS
- Creates platform-specific artifacts
- Names: `miniRT-linux-x86_64`, `miniRT-macos-arm64`

#### Create GitHub Release
- Generates release notes from commits
- Groups changes by type (features, fixes, docs)
- Attaches compiled binaries
- Creates GitHub release automatically

## Using the CI/CD Pipeline

### For Developers

#### 1. Before Committing

**Run norminette locally:**
```bash
norminette src/ includes/
```

**Test commit message format:**
```bash
# Valid examples
git commit -m "feat: add soft shadow rendering"
git commit -m "fix(parser): handle empty files"
git commit -m "docs: update README"

# Invalid examples
git commit -m "added new feature"  # Missing type prefix
git commit -m "Fix bug"            # Type not lowercase
```

**Use commit validation script:**
```bash
# Set up git hook (optional)
cat > .git/hooks/commit-msg << 'EOF'
#!/bin/bash
bash scripts/check-commit-format.sh "$1"
EOF
chmod +x .git/hooks/commit-msg
```

#### 2. Creating a Pull Request

**Branch naming:**
```bash
# Good branch names
git checkout -b feature/soft-shadows
git checkout -b fix/memory-leak
git checkout -b 001-cicd-pipeline

# Bad branch names
git checkout -b new-feature
git checkout -b fix_bug
```

**PR checklist:**
- [ ] Branch name follows convention
- [ ] All commits use conventional format
- [ ] PR description uses template
- [ ] All CI checks passing
- [ ] Code reviewed
- [ ] Documentation updated

#### 3. Creating a Release

**Tag and push:**
```bash
# Create annotated tag
git tag -a v1.0.0 -m "Release version 1.0.0"

# Push tag to trigger release workflow
git push origin v1.0.0
```

**Release workflow automatically:**
1. Builds binaries for Linux and macOS
2. Generates release notes from commits
3. Creates GitHub release with binaries

### For Reviewers

#### Check CI Status

All workflows must pass before merging:
- ✅ Norminette check
- ✅ Build (Linux & macOS)
- ✅ Test (Linux & macOS)
- ✅ Commit validation
- ✅ Branch validation
- ✅ PR template validation

#### Review Checklist

- [ ] All CI checks green
- [ ] Code follows norminette
- [ ] Commits follow conventional format
- [ ] Tests added for new features
- [ ] Documentation updated
- [ ] No memory leaks
- [ ] Cross-platform compatibility verified

## Troubleshooting

### Norminette Failures

**Common issues:**
```bash
# Line too long
Error: Line exceeds 80 characters

# Fix: Break into multiple lines
long_function_call(param1, param2,
    param3, param4);
```

### Build Failures

**Linux build errors:**
```bash
# Missing dependencies
sudo apt-get install gcc make xorg libxext-dev libbsd-dev

# Rebuild
make fclean && make
```

**macOS build errors:**
```bash
# Ensure Xcode Command Line Tools installed
xcode-select --install

# Rebuild
make fclean && make
```

### Commit Message Validation Failures

**Fix commit messages:**
```bash
# Reword last commit
git commit --amend -m "feat: proper commit message"

# Interactive rebase for multiple commits
git rebase -i HEAD~3
# Change 'pick' to 'reword' for commits to fix
```

### PR Validation Failures

**Branch name issues:**
```bash
# Rename branch
git branch -m feature/proper-name
git push origin -u feature/proper-name
git push origin --delete old-branch-name
```

## CI/CD Maintenance

### Updating Workflows

**Workflow locations:**
- `.github/workflows/ci.yml` - Main CI pipeline
- `.github/workflows/pr-validation.yml` - PR checks
- `.github/workflows/release.yml` - Release automation

**After updating workflows:**
1. Test in a feature branch first
2. Verify all jobs pass
3. Document changes in PR
4. Update this documentation if needed

### Adding New Jobs

**Example: Add memory leak check**
```yaml
memory-check:
  name: Memory Leak Check
  runs-on: ubuntu-latest
  needs: build-linux
  steps:
    - name: Run valgrind
      run: |
        valgrind --leak-check=full ./miniRT test.rt
```

### Monitoring

**Check workflow status:**
- GitHub Actions tab in repository
- Status badges in README
- Email notifications (configure in GitHub settings)

## Best Practices

### Commit Messages

✅ **Good:**
```
feat: add ambient occlusion support
fix(lighting): correct shadow calculation
docs: update installation instructions
test: add unit tests for vector math
```

❌ **Bad:**
```
Added feature
fixed bug
Update README
test
```

### Branch Strategy

```
main (protected)
├── develop (integration)
│   ├── feature/soft-shadows
│   ├── feature/ambient-occlusion
│   └── fix/memory-leak
└── hotfix/critical-bug
```

### Release Strategy

**Semantic Versioning:**
- `MAJOR.MINOR.PATCH` (e.g., `1.2.3`)
- MAJOR: Breaking changes
- MINOR: New features (backward compatible)
- PATCH: Bug fixes

**Release cadence:**
- Patch: As needed for bug fixes
- Minor: When features complete
- Major: Significant architecture changes

## Environment Variables

### Required Secrets (GitHub Repository Settings)

- `GITHUB_TOKEN` - Automatically provided by GitHub Actions

### Optional Secrets

None required for current workflows.

## Cost and Limits

**GitHub Actions free tier:**
- 2,000 minutes/month for private repos
- Unlimited for public repos

**Current usage:**
- CI workflow: ~5-10 minutes per run
- Release workflow: ~15-20 minutes per release

## References

- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [Conventional Commits](https://www.conventionalcommits.org/)
- [Semantic Versioning](https://semver.org/)

---

**Last Updated:** 2025-12-17  
**Maintainer:** miniRT Team
