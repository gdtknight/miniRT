# Vector Range Validation Implementation Plan

**Date**: 2026-02-09  
**Issue**: Parser does not validate that direction/normal vectors have components in range [-1,1]  
**Subject Requirement**: "3D normalized orientation/normal/axis vector, in the range [-1,1] for each x,y,z axis"

---

## Current State Analysis

### ✅ What EXISTS

**Current validation in `validate_direction_vector()`** (`parse_validation_strict.c:123-133`):
```c
t_parse_result	validate_direction_vector(t_vec3 *vec)
{
	double	len_sq;

	if (!vec)
		return (PARSE_ERR_FORMAT);
	len_sq = vec->x * vec->x + vec->y * vec->y + vec->z * vec->z;
	if (len_sq < EPSILON * EPSILON)
		return (PARSE_ERR_ZERO_VECTOR);
	return (PARSE_OK);
}
```

**Used in**:
- `parse_camera()` - Camera direction (line 70 of parse_elements.c)
- `parse_plane_data()` - Plane normal (line 113 of parse_objects.c)
- `parse_cyl_vectors()` - Cylinder axis (line 89 of parse_cylinder.c)

**Current behavior**:
- ✅ Rejects zero vectors
- ❌ Does NOT check component range [-1,1]
- After validation, vectors are normalized: `vec3_normalize(vec)`

---

### ❌ What is MISSING

**Range validation**: No check that each component (x, y, z) is in [-1,1]

**Test case that currently passes but should fail**:
```rt
# scenes/invalid/invalid_plane_normal_out_of_range.rt
pl 0,0,0  0,2.0,0  150,150,150
         ^^^^^^^^
         y=2.0 exceeds [-1,1] but parser accepts it
```

**Subject requirements that are not enforced**:
1. Camera direction: "in the range [-1,1] for each x,y,z axis"
2. Plane normal: "in the range [-1,1] for each x,y,z axis"
3. Cylinder axis: "in the range [-1,1] for each x,y,z axis"

---

## Implementation Plan

### Step 1: Create New Validation File (Norm Compliance)

**Problem**: `parse_validation_strict.c` already has 5 functions (Norm max = 5/file)
- Line 17: `in_range()` [public]
- Line 32: `parse_vector_strict()` [public]
- Line 66: `parse_color_values()` [static]
- Line 100: `parse_color_strict()` [public]
- Line 123: `validate_direction_vector()` [public]

**Solution**: Create new file `src/parser/parse_vector_validation.c`

**File**: `src/parser/parse_vector_validation.c`

```c
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_vector_validation.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/09 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/02/09 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "parser.h"

/**
 * @brief Validate vector components are in range [-1,1].
 *
 * Subject requires normalized vectors to have each component
 * in the range [-1,1] for direction/normal/axis vectors.
 *
 * @param vec Vector to validate.
 * @return t_parse_result PARSE_OK or PARSE_ERR_RANGE.
 */
t_parse_result	validate_vector_range(const t_vec3 *vec)
{
	if (!vec)
		return (PARSE_ERR_FORMAT);
	if (!in_range(vec->x, -1.0, 1.0))
		return (PARSE_ERR_RANGE);
	if (!in_range(vec->y, -1.0, 1.0))
		return (PARSE_ERR_RANGE);
	if (!in_range(vec->z, -1.0, 1.0))
		return (PARSE_ERR_RANGE);
	return (PARSE_OK);
}
```

**Note**: 
- `in_range()` is declared in `parser.h` (line 196) and defined in `parse_validation_strict.c` (line 17)
- **Checked in review**: Function is non-static (no `static` keyword, symbol type 'T' in object file)
- **Before implementation**: Reconfirm `in_range()` remains public (no code changes since review)
- New file ensures Norm compliance (1 function/file)

---

### Step 2: Update Makefile

**File**: `Makefile`

**Current structure**: Single `SRCS` variable (no separate PARSER_SRCS)

**Add new file** (line 77, after `parse_validation_strict.c`):
```makefile
SRCS = ... \
       $(SRC_DIR)/parser/parse_validation_strict.c \
       $(SRC_DIR)/parser/parse_vector_validation.c \
       $(SRC_DIR)/parser/parse_cylinder.c \
       ...
```

**Location**: Line 77 in Makefile (alphabetical order within parser section)

---

### Step 3: Update Header File

**File**: `includes/parser.h`

**Add declaration** (after line 117, before `validate_direction_vector`):
```c
t_parse_result	validate_vector_range(const t_vec3 *vec);
t_parse_result	validate_direction_vector(t_vec3 *vec);
```

**Location**: In the "Validation API (Enhanced)" section (line 110-118)
- Current line 110: `/*`
- Current line 111: ` * Validation API (Enhanced)`
- Current line 112: ` */`
- Current line 113-117: Existing function declarations
- **Insert new declaration here** (before line 118: `validate_direction_vector`)

---

### Step 4: Update Camera Parsing

**File**: `src/parser/parse_elements.c`

**Current code** (lines 67-74):
```c
result = parse_vector_strict(*token, &scene->camera.direction, token);
if (result != PARSE_OK)
	return (result);
result = validate_direction_vector(&scene->camera.direction);
if (result != PARSE_OK)
	return (result);
scene->camera.direction = vec3_normalize(scene->camera.direction);
```

**Updated code**:
```c
result = parse_vector_strict(*token, &scene->camera.direction, token);
if (result != PARSE_OK)
	return (result);
result = validate_vector_range(&scene->camera.direction);
if (result != PARSE_OK)
	return (result);
result = validate_direction_vector(&scene->camera.direction);
if (result != PARSE_OK)
	return (result);
scene->camera.direction = vec3_normalize(scene->camera.direction);
```

**Change**: Add `validate_vector_range()` call before `validate_direction_vector()`

**Lines affected**: Insert 3 lines after line 69

---

### Step 5: Update Plane Parsing

**File**: `src/parser/parse_objects.c`

**Current code** (lines 110-116):
```c
result = parse_vector_strict(*token, &obj->data.plane.normal, token);
if (result != PARSE_OK)
	return (result);
result = validate_direction_vector(&obj->data.plane.normal);
if (result != PARSE_OK)
	return (result);
obj->data.plane.normal = vec3_normalize(obj->data.plane.normal);
```

**Updated code**:
```c
result = parse_vector_strict(*token, &obj->data.plane.normal, token);
if (result != PARSE_OK)
	return (result);
result = validate_vector_range(&obj->data.plane.normal);
if (result != PARSE_OK)
	return (result);
result = validate_direction_vector(&obj->data.plane.normal);
if (result != PARSE_OK)
	return (result);
obj->data.plane.normal = vec3_normalize(obj->data.plane.normal);
```

**Change**: Add `validate_vector_range()` call before `validate_direction_vector()`

**Lines affected**: Insert 3 lines after line 112

---

### Step 6: Update Cylinder Parsing

**File**: `src/parser/parse_cylinder.c`

**Current code** (lines 86-92):
```c
result = parse_vector_strict(*token, &obj->data.cylinder.axis, token);
if (result != PARSE_OK)
	return (result);
result = validate_direction_vector(&obj->data.cylinder.axis);
if (result != PARSE_OK)
	return (result);
obj->data.cylinder.axis = vec3_normalize(obj->data.cylinder.axis);
```

**Updated code**:
```c
result = parse_vector_strict(*token, &obj->data.cylinder.axis, token);
if (result != PARSE_OK)
	return (result);
result = validate_vector_range(&obj->data.cylinder.axis);
if (result != PARSE_OK)
	return (result);
result = validate_direction_vector(&obj->data.cylinder.axis);
if (result != PARSE_OK)
	return (result);
obj->data.cylinder.axis = vec3_normalize(obj->data.cylinder.axis);
```

**Change**: Add `validate_vector_range()` call before `validate_direction_vector()`

**Lines affected**: Insert 3 lines after line 88

---

## Validation Order

**Important**: Range validation must happen BEFORE normalization:

1. ✅ **Parse** vector components (x, y, z)
2. ✅ **Validate range** [-1,1] for each component ← NEW
3. ✅ **Validate non-zero** (existing check)
4. ✅ **Normalize** vector

**Rationale**:
- Subject explicitly requires input values in [-1,1]
- Normalization would mask invalid input (e.g., (2,0,0) → (1,0,0))
- Error should be caught at parse time, not hidden

---

## Error Message

**Existing error message** (parse_error_msg.c:56):
```c
"Value out of range",  // PARSE_ERR_RANGE
```

**This is already appropriate** for vector component range errors.

**Example error output**:
```
Error
Line 8 (pl): Value out of range
```

---

## Test Cases

### Invalid scenes that should be rejected (after fix):

1. **`invalid_plane_normal_out_of_range.rt`** (already exists)
   - `pl 0,0,0  0,2.0,0  150,150,150`
   - y=2.0 exceeds range

2. **Cylinder axis out of range** (create new):
   - `cy 0,0,0  5,0,0  10  20  255,0,0`
   - x=5.0 exceeds range

3. **Camera direction negative boundary**:
   - `C 0,0,0  -1.5,0,0  70`
   - x=-1.5 below range

4. **Camera direction positive boundary**:
   - `C 0,0,0  0,1.1,0  70`
   - y=1.1 exceeds range

### Valid scenes that should still pass:

1. **Boundary values**:
   - `pl 0,0,0  1.0,0,0  255,0,0` (x=1.0, valid)
   - `pl 0,0,0  -1.0,0,0  255,0,0` (x=-1.0, valid)
   - `pl 0,0,0  0.99999,0,0  255,0,0` (x=0.99999, valid)

2. **Zero components**:
   - `pl 0,0,0  0,1,0  255,0,0` (valid)
   - `cy 0,0,0  0,0,1  10  20  255,0,0` (valid)

3. **Non-normalized but in range**:
   - `pl 0,0,0  0.5,0.5,0.5  255,0,0` (all in [-1,1], will be normalized)
   - Length: √(0.25+0.25+0.25) = 0.866, becomes (0.577, 0.577, 0.577)

---

## Impact Analysis

### Code Changes Summary

| File | Function | Lines Changed | Type |
|------|----------|---------------|------|
| `parse_vector_validation.c` | New file + `validate_vector_range()` | +38 | Add |
| `Makefile` | Add new source file | +1 | Modify |
| `parser.h` | Add declaration | +1 | Add |
| `parse_elements.c` | `parse_camera_data()` | +3 | Modify |
| `parse_objects.c` | `parse_plane_data()` | +3 | Modify |
| `parse_cylinder.c` | `parse_cyl_vectors()` | +3 | Modify |
| **TOTAL** | | **+49** | |

### 42 Norm Compliance

**Critical Issue Addressed**:
- ❌ `parse_validation_strict.c` already has **5 functions** (Norm max = 5/file)
- ✅ **Solution**: Create separate file `parse_vector_validation.c`

**Function count per file**:
- ✅ `parse_validation_strict.c`: Remains 5 functions (no change)
- ✅ `parse_vector_validation.c`: 1 function (within limit)

**Function length**:
- ✅ `validate_vector_range()`: **10 lines** (well within 25-line limit)

**Parameters**:
- ✅ `validate_vector_range(const t_vec3 *vec)`: **1 parameter** (within limit)

**Variables per function**:
- ✅ No local variables needed (within 5-variable limit)

**No norm violations introduced.**

---

## Testing Strategy

### Step 1: Build and Basic Test
```bash
make re
./miniRT scenes/valid/valid_us04_intersect_plane.rt
# Should still work (valid scene)
```

### Step 2: Test Invalid Scenes
```bash
./miniRT scenes/invalid/invalid_plane_normal_out_of_range.rt
# Expected: "Error\nLine 8 (pl): Value out of range"
```

### Step 3: Create Additional Test Cases
```bash
# Create test scenes for boundary cases
cat > scenes/invalid/invalid_cylinder_axis_out_of_range.rt << 'EOF'
A 0.2 255,255,255
C 0,0,-15 0,0,1 70
L 0,8,-8 0.7 255,255,255
cy 0,0,0 5,0,0 10 20 255,0,0
EOF

./miniRT scenes/invalid/invalid_cylinder_axis_out_of_range.rt
# Expected: Error on line 4
```

### Step 4: Regression Test
```bash
# Run all valid scenes to ensure nothing breaks
for f in scenes/valid/*.rt; do
    echo "Testing: $f"
    timeout 2 ./miniRT "$f" 2>&1 | grep -q "Error" && echo "FAIL: $f" || echo "PASS"
done
```

---

## Alternative Considerations

### Alternative 1: Validate after normalization
❌ **Not recommended**: Would hide invalid input and violate Subject requirement

### Alternative 2: Combine range check with zero check
❌ **Not recommended**: Violates single responsibility principle, harder to maintain

### Alternative 3: Use different error code
❌ **Not needed**: `PARSE_ERR_RANGE` is appropriate and already exists

### Alternative 4: Accept any non-zero vector and normalize silently
❌ **Violates Subject**: Explicit requirement states "in the range [-1,1]"

---

## Commit Plan

After implementation, create commit with:

**Type**: `fix(parser)`  
**Subject**: Vector range validation (Subject compliance)  
**Body**:
```
Changes:
- Create parse_vector_validation.c with validate_vector_range()
- Check vector components in [-1,1] range before normalization
- Apply to camera direction, plane normal, cylinder axis
- Enforce Subject requirement for normalized vectors

Location:
- src/parser/parse_vector_validation.c: New file + function
- src/parser/parse_elements.c: Camera direction validation
- src/parser/parse_objects.c: Plane normal validation
- src/parser/parse_cylinder.c: Cylinder axis validation
- includes/parser.h: Function declaration
- Makefile: Add new source file

Testing:
- scenes/invalid/invalid_plane_normal_out_of_range.rt now rejected
- All valid scenes still pass
- Boundary values (±1.0) correctly accepted
- Component 2.0 correctly rejected with PARSE_ERR_RANGE

Technical notes:
- Validation order: parse → range → zero → normalize
- Uses existing in_range() helper (parser.h:196) and PARSE_ERR_RANGE
- Norm compliant: New file with 1 function (10 lines, 1 parameter)
- Prevents accepting (5,0,0) which normalizes to (1,0,0)

Subject reference:
- "3D normalized orientation/normal/axis vector"
- "in the range [-1,1] for each x,y,z axis"
- miniRT.md lines 172, 205, 216
```

---

## Summary

**Problem**: Parser accepts vector components outside [-1,1] range, violating Subject  
**Solution**: Create `parse_vector_validation.c` with `validate_vector_range()` function  
**Files affected**: 6 files (+1 new file), +49 lines total  
**Norm impact**: None (new file with 1 function, within all limits)  
**Testing**: Invalid test case now rejected, all valid scenes pass  

**Critical fixes from code review**:
- ✅ Checked: `in_range()` is non-static in current codebase (parser.h:196, symbol 'T')
  - **Pre-build reconfirm**: Verify no code changes made `in_range()` static before implementation
- ✅ Fixed: Makefile uses single `SRCS` variable, not `PARSER_SRCS`
- ✅ Fixed: Commit plan reflects new file creation, not existing file modification
- ✅ Fixed: Code change summary shows +49 lines (including new file), 6 files affected
- ✅ Confirmed: parse_validation_strict.c has exactly 5 functions (Norm max reached)

**Next step**: Implement changes following this plan, then test thoroughly.
