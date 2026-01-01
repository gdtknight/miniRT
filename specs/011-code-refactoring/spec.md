# Feature Specification: Code Refactoring for Calculation Optimization

**Feature Branch**: `011-code-refactoring`  
**Created**: 2026-01-01  
**Status**: Draft  
**Input**: User description: "Perform code refactoring to identify and eliminate: 1. Duplicate calculations that can be cached or computed once 2. Repetitive code patterns that can be abstracted 3. Code duplication that can be reduced through better design 4. Opportunities to improve code maintainability and performance"

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Eliminate Redundant Vector Calculations (Priority: P1)

Developers working on the ray tracing engine repeatedly calculate the same vector operations (dot products, magnitudes, normalizations) multiple times within the same function scope. By caching these calculations, rendering performance improves and code becomes more maintainable.

**Why this priority**: Directly impacts rendering performance as vector operations are called in hot paths (ray-object intersection, lighting calculations). This is the highest value optimization with immediate measurable impact.

**Independent Test**: Can be fully tested by running existing scene renders and comparing frame times before/after refactoring. Success means identical visual output with reduced computation time.

**Acceptance Scenarios**:

1. **Given** cylinder intersection code calculates `vec3_dot(ray->direction, cyl->axis)` multiple times, **When** the calculation is cached in a local variable, **Then** the dot product is computed only once and reused
2. **Given** lighting code calculates `vec3_dot(oc, cyl->axis)` repeatedly in one function, **When** these duplicate calculations are identified, **Then** they are replaced with a single cached value
3. **Given** screen-to-NDC conversion `(2.0 * x / (double)WINDOW_WIDTH) - 1.0` appears in multiple places, **When** this is extracted to a helper function or cached, **Then** code duplication is reduced

---

### User Story 2 - Abstract Repetitive Intersection Loop Pattern (Priority: P2)

The ray tracing code contains three nearly identical functions (`check_sphere_intersections`, `check_plane_intersections`, `check_cylinder_intersections`) that iterate through object arrays with the same loop structure. Abstracting this pattern reduces code duplication and makes maintenance easier.

**Why this priority**: Improves code maintainability and reduces the risk of introducing bugs when modifying intersection logic. While not directly impacting performance, it significantly reduces code volume and complexity.

**Independent Test**: Can be tested independently by verifying all intersection tests still work correctly after consolidating into a generic intersection checking function.

**Acceptance Scenarios**:

1. **Given** three functions with identical loop patterns checking different object types, **When** a generic intersection iterator function is created that accepts type-specific intersection functions via function pointers, **Then** all three functions use the same abstracted logic by passing their specific intersection handlers as parameters
2. **Given** each intersection function contains `temp_hit.distance = hit->distance` assignment, **When** this is moved into the generic function, **Then** code duplication is eliminated
3. **Given** future object types need intersection testing, **When** using the abstracted pattern, **Then** new object types can be added with minimal code duplication

---

### User Story 3 - Cache Geometry Calculations (Priority: P2)

Object properties like cylinder radius (`diameter / 2.0`) and squared radius are recalculated each time an intersection test occurs. Pre-computing these values during scene parsing and storing them in the object structures eliminates redundant calculations during rendering.

**Why this priority**: Reduces arithmetic operations in the render loop but requires structural changes to data models. Medium impact on performance with moderate implementation effort.

**Independent Test**: Can be tested by rendering scenes and validating that cached geometry values produce identical results to on-the-fly calculations.

**Acceptance Scenarios**:

1. **Given** cylinder objects calculate `diameter / 2.0` multiple times per intersection test, **When** radius is precomputed during scene parsing, **Then** the radius value is accessed directly without division
2. **Given** sphere intersection calculates `radius * radius` in every test, **When** squared radius is cached in the sphere structure, **Then** multiplication is eliminated from the hot path
3. **Given** plane and cylinder objects have fixed orientations, **When** normalized axes and plane coefficients are precomputed during parsing, **Then** runtime normalization and arithmetic operations are eliminated

---

### User Story 4 - Consolidate Similar Calculation Functions (Priority: P3)

Similar mathematical operations and setup code appear in multiple functions with slight variations. Identifying and consolidating these patterns improves code clarity and reduces maintenance burden.

**Why this priority**: Improves long-term maintainability but has minimal immediate performance impact. Can be done incrementally after higher-priority optimizations.

**Independent Test**: Can be tested by verifying that consolidated functions produce identical outputs to their original separate implementations.

**Acceptance Scenarios**:

1. **Given** multiple functions perform similar vector subtraction and normalization sequences, **When** common patterns are identified, **Then** shared utility functions are created as named helpers to eliminate duplication while maintaining readability
2. **Given** coordinate conversion logic appears in render functions, **When** this is extracted into dedicated named conversion utilities, **Then** the rendering code becomes more focused and readable (prefer named functions over inline for maintainability)
3. **Given** similar error checking or boundary validation appears across modules, **When** validation logic is consolidated, **Then** code consistency improves and bugs are easier to prevent

---

### Edge Cases

- How does the system handle zero-length vectors in normalization functions after caching magnitude calculations?
- What occurs when optimized code paths encounter numerical precision issues that weren't apparent in the original implementation?
- How are edge cases in cylinder cap calculations handled after radius caching (e.g., zero diameter, negative values)?

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST identify all locations where identical vector dot products are calculated multiple times within the same function scope
- **FR-002**: System MUST cache redundant calculations (dot products, magnitudes, radius values) in local variables or object structures
- **FR-003**: System MUST abstract the repetitive object intersection loop pattern into a generic function that accepts type-specific intersection functions via function pointers as parameters
- **FR-004**: System MUST precompute all geometric properties (radius, squared radius, height half, normalized axes, precomputed plane coefficients) during scene parsing and store them in object structures (static scene assumption: no runtime object modifications supported)
- **FR-004a**: System MUST cache all derived vector properties (normalized directions, cross products, basis vectors) in object structures (static scene: computed once at parse time)
- **FR-005**: System MUST consolidate duplicate screen-to-NDC coordinate conversion calculations into named helper functions for readability and maintainability (avoid inline expansion)
- **FR-006**: System MUST maintain identical rendering output after all refactoring changes
- **FR-007**: System MUST preserve all existing edge case handling and numerical stability safeguards
- **FR-008**: System MUST maintain or improve code readability while reducing duplication through named helper functions (prefer named functions over inline expansion for maintainability)
- **FR-009**: Refactored code MUST pass all existing unit tests and integration tests
- **FR-010**: System MUST document which calculations were cached and why in code comments
- **FR-011**: Memory footprint increase MUST stay within 20-50% of baseline to ensure aggressive caching of all derived properties for optimal speed
- **FR-012**: System MUST provide automated regression test suite with pixel-perfect visual validation (pixel comparison) to verify rendering correctness
- **FR-013**: System MUST include performance benchmark suite to measure and validate rendering speed improvements automatically

### Non-Functional Requirements

- **NFR-001**: Performance optimization strategy prioritizes speed over memory - aggressive caching of all derived properties is acceptable with 20-50% memory increase
- **NFR-002**: Cache all precomputable geometry values (radius, squared radius, normalized vectors, plane coefficients) to minimize runtime arithmetic operations
- **NFR-003**: Refactored code MUST maintain numerical precision equivalent to original implementation (no degradation in floating-point accuracy)

### Key Entities

- **Cached Calculation**: A computed value stored in a local variable or structure field to avoid redundant computation (e.g., dot products, magnitudes, geometric properties)
- **Generic Intersection Iterator**: An abstracted function that iterates through object arrays and performs intersection tests using function pointers to pass type-specific intersection functions as parameters, replacing type-specific loop functions
- **Precomputed Geometry Properties**: Derived values calculated once during parsing (radius, squared_radius, half_height, normalized axes, plane coefficients, basis vectors) and stored in object structures for fast access during rendering (static scene: values never invalidated after initial computation), prioritizing speed with acceptable 20-50% memory increase
- **Coordinate Conversion Utility**: A dedicated named function for converting screen coordinates to normalized device coordinates, eliminating duplicate conversion logic (implemented as named helper for readability and maintainability)

## Clarifications

### Session 2026-01-01

- Q: Performance vs memory tradeoff strategy for caching? → A: Optimize for speed (20-50% increase OK) - cache all derived properties aggressively
- Q: Testing validation strategy for refactored code? → A: B - Automated regression tests + visual validation (pixel comparison) + performance benchmarks
- Q: Scene modification model for cached geometry values? → A: Static scene assumption (no runtime modifications supported, caching always valid)
- Q: Implementation strategy for generic intersection iterator abstraction? → A: B - Function pointers approach (pass type-specific intersection function as parameter to generic iterator)
- Q: Code organization approach for helper extraction? → A: B - Named helper function for readability and maintainability

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Developers can measure at least 10% reduction in redundant calculations in hot path functions (intersection tests, lighting calculations)
- **SC-002**: Code review shows at least 30% reduction in duplicate code patterns across intersection and rendering modules
- **SC-003**: All existing scene files render with identical visual output before and after refactoring (pixel-perfect match)
- **SC-004**: Code maintainability improves with at least 200 fewer lines of code while maintaining the same functionality
- **SC-005**: Rendering performance improves by 15-25% as measured by frames per second on standard test scenes (targeting aggressive speed optimization through comprehensive caching)
- **SC-005a**: Memory footprint increase remains within 20-50% of baseline, confirming acceptable tradeoff for speed-optimized caching strategy
- **SC-006**: All existing test cases pass without modification after refactoring
- **SC-007**: Code complexity metrics (cyclomatic complexity, duplication percentage) show measurable improvement in refactored modules
- **SC-008**: Future object type additions require 40% less code due to abstracted patterns
- **SC-009**: Automated regression test suite validates all refactored code paths with pixel-perfect visual validation (pixel comparison) against baseline renders
- **SC-010**: Performance benchmark suite measures and validates 15-25% rendering speed improvement target with automated frame time tracking
