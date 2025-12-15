# Release Gate Checklist: miniRT Ray Tracer

**Purpose**: Pre-submission/release gate requirements quality validation ensuring specification completeness, clarity, and measurability for 42 school project submission

**Created**: 2025-12-15

**Feature**: [../spec.md](../spec.md)

**Context**: This checklist validates requirements quality before submission, ensuring all specifications are complete, unambiguous, and testable. Focus areas: memory safety requirements, scene rendering specifications, and mandatory submission criteria.

---

## Requirement Completeness - Core Functionality

- [ ] CHK001 - Are ray-object intersection requirements defined for all geometric primitives (sphere, plane, cylinder including caps)? [Completeness, Spec §US4]
- [ ] CHK002 - Are camera projection requirements specified with FOV calculation and ray generation formulas? [Gap]
- [ ] CHK003 - Are rendering pipeline requirements documented from scene parsing through pixel output? [Completeness]
- [ ] CHK004 - Are lighting calculation requirements defined for both ambient and diffuse components? [Completeness, Spec §US3]
- [ ] CHK005 - Are shadow ray requirements specified including occlusion testing methodology? [Completeness, Spec §US3]
- [ ] CHK006 - Are object visibility requirements defined for handling overlapping geometry? [Completeness, Spec §US1]
- [ ] CHK007 - Are viewport-to-world coordinate transformation requirements documented? [Gap]
- [ ] CHK008 - Are pixel color calculation requirements specified including color clamping and blending? [Gap]

## Requirement Completeness - Scene File Parsing

- [ ] CHK009 - Are parsing requirements defined for all mandatory elements (A, C, L)? [Completeness, Spec §US2]
- [ ] CHK010 - Are parsing requirements defined for all object types (sp, pl, cy)? [Completeness, Spec §US2]
- [ ] CHK011 - Are element order independence requirements explicitly specified? [Completeness, Spec §US2]
- [ ] CHK012 - Are whitespace and newline handling requirements documented? [Gap]
- [ ] CHK013 - Are singleton element enforcement requirements (A, C, L appear exactly once) clearly defined? [Completeness, Spec §US2]
- [ ] CHK014 - Are file extension validation requirements specified (.rt enforcement)? [Completeness, Edge Cases]
- [ ] CHK015 - Are empty file handling requirements documented? [Completeness, Edge Cases]
- [ ] CHK016 - Are missing mandatory element detection requirements specified? [Completeness, Edge Cases]

## Requirement Clarity - Numeric Ranges & Validation

- [ ] CHK017 - Is the ambient lighting ratio range [0.0, 1.0] validation requirement explicitly specified? [Clarity, miniRT_requirements.md §A]
- [ ] CHK018 - Is the RGB color range [0-255] validation requirement explicitly specified? [Clarity, miniRT_requirements.md §A]
- [ ] CHK019 - Is the FOV range [0, 180] validation requirement explicitly specified? [Clarity, miniRT_requirements.md §C]
- [ ] CHK020 - Is the light brightness ratio range [0.0, 1.0] validation requirement explicitly specified? [Clarity, miniRT_requirements.md §L]
- [ ] CHK021 - Is the normalized vector validation requirement (magnitude = 1.0) explicitly specified? [Clarity, miniRT_requirements.md §C, §pl, §cy]
- [ ] CHK022 - Are floating-point precision requirements specified for all numeric validations? [Gap]
- [ ] CHK023 - Are sphere diameter validation requirements (must be positive) specified? [Gap]
- [ ] CHK024 - Are cylinder dimension validation requirements (diameter > 0, height > 0) specified? [Gap]

## Requirement Clarity - Object Transformation Specifications

- [ ] CHK025 - Are translation requirements quantified with coordinate system definitions? [Clarity, Spec §US6]
- [ ] CHK026 - Are rotation restrictions for spheres explicitly documented (rotation has no visual effect)? [Clarity, Spec §US6]
- [ ] CHK027 - Are rotation restrictions for lights explicitly documented (point lights are omnidirectional)? [Clarity, Spec §US6]
- [ ] CHK028 - Are plane orientation requirements defined using normal vector interpretation? [Clarity, Spec §US6]
- [ ] CHK029 - Are cylinder orientation requirements defined using axis vector interpretation? [Clarity, Spec §US6]
- [ ] CHK030 - Are camera orientation requirements defined with up-vector assumptions or specifications? [Gap]

## Requirement Consistency - Lighting Model

- [ ] CHK031 - Are ambient lighting requirements consistent across all object types? [Consistency, Spec §US3]
- [ ] CHK032 - Are diffuse lighting calculation requirements consistent with ray-surface angle definitions? [Consistency, Spec §US3]
- [ ] CHK033 - Are light intensity scaling requirements consistent between brightness ratio and distance? [Consistency]
- [ ] CHK034 - Are shadow requirements consistent with light occlusion testing? [Consistency, Spec §US3]
- [ ] CHK035 - Do color mixing requirements align between ambient, diffuse, and object color? [Consistency]

## Acceptance Criteria Quality - Scene Rendering Outputs

- [ ] CHK036 - Are expected rendering outputs defined with specific scene file references? [Measurability, miniRT_requirements.md §Reference Images]
- [ ] CHK037 - Are visual acceptance criteria defined with pixel-perfect reference images or tolerance thresholds? [Gap]
- [ ] CHK038 - Are sphere rendering requirements measurable (circular projection, no distortion)? [Measurability, Spec §US4]
- [ ] CHK039 - Are cylinder cap rendering requirements explicitly defined and measurable? [Gap]
- [ ] CHK040 - Are plane infinite extent rendering requirements specified? [Gap]
- [ ] CHK041 - Are lighting gradient requirements quantifiable (smooth transitions, no banding)? [Measurability]
- [ ] CHK042 - Are hard shadow edge requirements measurable (crisp boundaries)? [Measurability, Spec §US3]
- [ ] CHK043 - Are camera perspective requirements quantifiable (correct FOV distortion)? [Measurability]

## Scenario Coverage - Primary Flows

- [ ] CHK044 - Are requirements defined for the primary flow: valid scene → successful parse → render → display? [Coverage, Spec §US1]
- [ ] CHK045 - Are requirements defined for window interaction: ESC key exit? [Coverage, Spec §US5]
- [ ] CHK046 - Are requirements defined for window interaction: X button close? [Coverage, Spec §US5]
- [ ] CHK047 - Are requirements defined for command-line invocation with scene file argument? [Coverage, Spec §US1]

## Scenario Coverage - Alternate Flows

- [ ] CHK048 - Are requirements defined for multiple object rendering (not just single objects)? [Coverage, Spec §US1]
- [ ] CHK049 - Are requirements defined for scenes with different camera angles? [Coverage, miniRT_requirements.md §Reference Images]
- [ ] CHK050 - Are requirements defined for varying light positions and intensities? [Coverage]
- [ ] CHK051 - Are requirements defined for objects at different scales (diameter/height variations)? [Coverage, Spec §US7]

## Scenario Coverage - Exception/Error Flows

- [ ] CHK052 - Are error handling requirements defined for file not found? [Coverage, Edge Cases]
- [ ] CHK053 - Are error handling requirements defined for invalid file extension? [Coverage, Edge Cases]
- [ ] CHK054 - Are error handling requirements defined for malformed numeric values? [Coverage, Edge Cases]
- [ ] CHK055 - Are error handling requirements defined for out-of-range values (RGB > 255, FOV > 180)? [Coverage, Spec §US2]
- [ ] CHK056 - Are error handling requirements defined for invalid normalized vectors (magnitude ≠ 1)? [Coverage, Spec §US2]
- [ ] CHK057 - Are error handling requirements defined for duplicate singleton elements (multiple A/C/L)? [Coverage, Spec §US2]
- [ ] CHK058 - Are error output format requirements specified ("Error\n" + explicit message)? [Completeness, Spec §US2]
- [ ] CHK059 - Are error exit code requirements specified (clean exit with proper return code)? [Gap]

## Scenario Coverage - Edge Cases

- [ ] CHK060 - Are requirements defined for ray tangent to sphere (discriminant = 0)? [Coverage, Edge Cases]
- [ ] CHK061 - Are requirements defined for ray parallel to cylinder axis? [Coverage, Edge Cases]
- [ ] CHK062 - Are requirements defined for ray originating inside objects? [Coverage, Edge Cases]
- [ ] CHK063 - Are requirements defined for camera FOV boundary values (0, 180)? [Coverage, Edge Cases]
- [ ] CHK064 - Are requirements defined for ambient lighting boundary values (0.0, 1.0)? [Coverage, Edge Cases]
- [ ] CHK065 - Are requirements defined for very small objects (diameter < 0.1)? [Coverage, Edge Cases]
- [ ] CHK066 - Are requirements defined for scenes with many objects (>20)? [Coverage, Edge Cases]
- [ ] CHK067 - Are requirements defined for objects at identical distances (z-fighting)? [Coverage, Edge Cases]
- [ ] CHK068 - Are requirements defined for ray missing all objects (background color)? [Coverage, Spec §US4]

## Non-Functional Requirements - Memory Safety (CRITICAL GATE)

- [ ] CHK069 - Are zero memory leak requirements explicitly stated as mandatory? [Completeness, Critical]
- [ ] CHK070 - Are Valgrind validation requirements specified as pre-submission gate? [Gap, Critical]
- [ ] CHK071 - Are memory allocation failure handling requirements defined for all malloc calls? [Gap, Critical]
- [ ] CHK072 - Are memory cleanup requirements defined for normal exit paths (ESC, X button)? [Completeness, Spec §US5]
- [ ] CHK073 - Are memory cleanup requirements defined for error exit paths? [Gap, Critical]
- [ ] CHK074 - Are resource cleanup requirements defined for MinilibX resources (windows, images)? [Gap]
- [ ] CHK075 - Are file descriptor cleanup requirements specified for scene file parsing? [Gap]
- [ ] CHK076 - Are memory leak testing acceptance criteria quantified (0 bytes leaked, 0 blocks)? [Measurability, Critical]

## Non-Functional Requirements - Performance

- [ ] CHK077 - Are rendering time requirements specified for simple scenes (1-5 objects)? [Completeness, Plan §Technical Context]
- [ ] CHK078 - Are rendering time requirements specified for complex scenes (up to 20 objects)? [Completeness, Plan §Technical Context]
- [ ] CHK079 - Are acceptable response time requirements defined for window operations? [Gap]
- [ ] CHK080 - Are computational complexity requirements documented for intersection testing? [Gap]

## Non-Functional Requirements - Code Quality (42 School Requirements)

- [ ] CHK081 - Are norminette compliance requirements explicitly stated as NON-NEGOTIABLE? [Completeness, Plan §Constitution]
- [ ] CHK082 - Are unit testing requirements specified as mandatory for all functions? [Completeness, Plan §Constitution]
- [ ] CHK083 - Are testing framework requirements documented (Criterion framework specified)? [Completeness, Plan §Constitution]
- [ ] CHK084 - Are code readability requirements defined per constitution? [Completeness, Plan §Constitution]
- [ ] CHK085 - Are build verification workflow requirements specified? [Completeness, Plan §Constitution]

## Non-Functional Requirements - Portability

- [ ] CHK086 - Are platform requirements specified (Linux/macOS with X11/XQuartz)? [Completeness, Plan §Technical Context]
- [ ] CHK087 - Are dependency requirements explicitly documented (MinilibX, libm, optional libft)? [Completeness, Plan §Technical Context]
- [ ] CHK088 - Are compilation flag requirements specified (-lmlx -lXext -lX11 -lm)? [Completeness, Plan §Technical Context]
- [ ] CHK089 - Are C standard version requirements documented (C99 or compatible)? [Completeness, Plan §Technical Context]

## Non-Functional Requirements - Window Management

- [ ] CHK090 - Are window minimize/restore requirements specified? [Completeness, Spec §US5]
- [ ] CHK091 - Are window focus switching requirements defined (no corruption on restore)? [Completeness, Spec §US5]
- [ ] CHK092 - Are window redraw requirements specified? [Gap]
- [ ] CHK093 - Are MinilibX image usage requirements documented (strongly recommended)? [Completeness, miniRT_requirements.md §Window Management]

## Dependencies & Assumptions

- [ ] CHK094 - Are MinilibX API assumptions documented (available functions, behavior)? [Gap, Assumption]
- [ ] CHK095 - Are coordinate system assumptions documented (right-handed, origin location, axis directions)? [Gap, Assumption]
- [ ] CHK096 - Are color space assumptions documented (RGB linear vs sRGB)? [Gap, Assumption]
- [ ] CHK097 - Are floating-point precision assumptions documented (double vs float)? [Gap, Assumption]
- [ ] CHK098 - Are testing framework dependencies verified and documented? [Dependency, Plan §Constitution]

## Ambiguities & Conflicts

- [ ] CHK099 - Is "properly shaded" quantified with specific lighting formula references? [Ambiguity, Spec §US1]
- [ ] CHK100 - Is "correctly positioned" defined with measurable coordinate accuracy? [Ambiguity, Spec §US1]
- [ ] CHK101 - Is "reasonable time" quantified with specific second thresholds for rendering? [Ambiguity, Spec §Edge Cases]
- [ ] CHK102 - Is "distortion artifacts" defined with specific visual defect criteria? [Ambiguity, Spec §Edge Cases]
- [ ] CHK103 - Is "comprehensive validation" specified with complete list of validation rules? [Ambiguity]
- [ ] CHK104 - Are conflicting requirements identified between spec and miniRT_requirements.md? [Conflict Check]
- [ ] CHK105 - Is light color usage clarified (miniRT_requirements.md states "unused in mandatory part")? [Ambiguity, miniRT_requirements.md §L]

## Traceability & Documentation Quality

- [ ] CHK106 - Is a requirement ID scheme established for cross-referencing? [Gap, Traceability]
- [ ] CHK107 - Are all user story acceptance scenarios traceable to implementation requirements? [Traceability]
- [ ] CHK108 - Are all miniRT_requirements.md elements mapped to spec.md user stories? [Traceability]
- [ ] CHK109 - Are all edge cases documented in spec.md traceable to test cases? [Traceability]
- [ ] CHK110 - Are Makefile rule requirements documented and traceable (all, clean, fclean, re, bonus, dev)? [Completeness, miniRT_requirements.md §Makefile]

## Mathematical Correctness

- [ ] CHK111 - Are ray-sphere intersection requirements specified with quadratic formula reference? [Completeness, Spec §US4]
- [ ] CHK112 - Are ray-plane intersection requirements specified with mathematical formula? [Completeness, Spec §US4]
- [ ] CHK113 - Are ray-cylinder intersection requirements specified for all surfaces (side, caps)? [Completeness, Spec §US4]
- [ ] CHK114 - Are vector normalization requirements defined with magnitude calculation? [Completeness]
- [ ] CHK115 - Are diffuse lighting requirements specified with Lambert's cosine law or equivalent? [Gap]
- [ ] CHK116 - Are view ray generation requirements specified with camera matrix mathematics? [Gap]
- [ ] CHK117 - Are coordinate transformation requirements documented with matrix operations? [Gap]

## Bonus Requirements Quality (Optional)

- [ ] CHK118 - Are Phong reflection model requirements specified with formula and parameters? [Completeness, miniRT_requirements.md §Bonus]
- [ ] CHK119 - Are checkerboard pattern requirements defined with size and color specifications? [Completeness, miniRT_requirements.md §Bonus]
- [ ] CHK120 - Are multi-light accumulation requirements specified? [Completeness, miniRT_requirements.md §Bonus]
- [ ] CHK121 - Are colored light mixing requirements defined? [Completeness, miniRT_requirements.md §Bonus]
- [ ] CHK122 - Are bonus object requirements specified (cone/hyperboloid/paraboloid intersection math)? [Gap, miniRT_requirements.md §Bonus]
- [ ] CHK123 - Are bump mapping requirements defined with normal perturbation specifications? [Gap, miniRT_requirements.md §Bonus]
- [ ] CHK124 - Are bonus evaluation justification requirements documented? [Completeness, miniRT_requirements.md §Bonus]

## Build System Requirements

- [ ] CHK125 - Are compilation requirements specified (no warnings, all flags documented)? [Gap]
- [ ] CHK126 - Are relink prevention requirements specified (Makefile efficiency)? [Gap]
- [ ] CHK127 - Are dependency tracking requirements specified for incremental builds? [Gap]
- [ ] CHK128 - Are clean/fclean behavior requirements explicitly defined? [Completeness, miniRT_requirements.md §Makefile]
- [ ] CHK129 - Are bonus compilation separation requirements specified? [Completeness, miniRT_requirements.md §Makefile]

## Test Scene Requirements

- [ ] CHK130 - Are reference scene file requirements specified for submission? [Gap, miniRT_requirements.md §Evaluation]
- [ ] CHK131 - Are test scene diversity requirements documented (normal cases, edge cases, error cases)? [Completeness, miniRT_requirements.md §Evaluation]
- [ ] CHK132 - Are minimal test scene requirements defined for each geometric primitive? [Gap, miniRT_requirements.md §Evaluation]
- [ ] CHK133 - Are evaluation demonstration scenario requirements documented? [Gap, miniRT_requirements.md §Evaluation]

---

## Summary

**Total Items**: 133  
**Critical Gate Items**: CHK069-CHK076 (Memory Safety - MUST be 100% leak-free)  
**High Priority**: CHK001-CHK043 (Core functionality and measurability)  
**Traceability**: 100+ items include spec references or gap markers (>75% coverage)

**Focus Areas**:
- **Memory Safety**: Zero-tolerance for leaks (Valgrind gate requirement)
- **Scene Rendering**: Pixel-perfect specifications with measurable criteria
- **Requirements Completeness**: All mandatory 42 school submission criteria

**Next Steps**:
1. Address [Gap] items by adding missing requirements to spec.md
2. Resolve [Ambiguity] items by quantifying vague terms
3. Create reference scene files with expected output images
4. Document mathematical formulas for all intersection tests
5. Establish requirement ID scheme for traceability
6. Define Valgrind test procedure as formal acceptance gate
