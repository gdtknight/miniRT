# Scene File Format Contract

**Version**: 1.0  
**Date**: 2025-12-15  
**Purpose**: Formal specification for .rt scene description files

## Overview

Scene files use a text-based format with one element per line. Elements can appear in any order. Whitespace (spaces, tabs) separates parameters within a line.

## Grammar

```ebnf
scene_file     = { element } ;
element        = ambient | camera | light | sphere | plane | cylinder ;
ambient        = "A" ratio color ;
camera         = "C" position orientation fov ;
light          = "L" position ratio color ;
sphere         = "sp" position diameter color ;
plane          = "pl" position normal color ;
cylinder       = "cy" position axis diameter height color ;

position       = vec3 ;
orientation    = normalized_vec3 ;
normal         = normalized_vec3 ;
axis           = normalized_vec3 ;
vec3           = float "," float "," float ;
normalized_vec3 = vec3 (* magnitude must equal 1.0 ±0.01 *) ;

color          = int "," int "," int (* each component [0-255] *) ;
ratio          = float (* range [0.0-1.0] *) ;
fov            = float (* range [0-180] *) ;
diameter       = positive_float ;
height         = positive_float ;

float          = [ "-" ] digit { digit } [ "." { digit } ] ;
positive_float = digit { digit } [ "." { digit } ] ;
int            = digit { digit } ;
digit          = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" ;
```

## Elements

### Ambient Lighting (A)

**Format**: `A <ratio> <R,G,B>`

**Parameters**:
1. `ratio` (double): Ambient light intensity [0.0-1.0]
2. `R,G,B` (int,int,int): RGB color [0-255] for each channel

**Constraints**:
- MUST appear exactly once per scene
- Ratio MUST be in range [0.0, 1.0]
- RGB values MUST be in range [0, 255]

**Example**:
```
A  0.2  255,255,255
```

**Error Cases**:
- Missing element: "Error\nMissing mandatory element: Ambient lighting"
- Duplicate: "Error\nAmbient lighting can only be declared once"
- Invalid ratio: "Error\nAmbient ratio must be between 0.0 and 1.0, got <value>"
- Invalid color: "Error\nRGB values must be between 0 and 255, got <value>"

---

### Camera (C)

**Format**: `C <x,y,z> <norm_x,norm_y,norm_z> <fov>`

**Parameters**:
1. `x,y,z` (double,double,double): Camera position in 3D space
2. `norm_x,norm_y,norm_z` (double,double,double): Normalized orientation vector
3. `fov` (double): Horizontal field of view in degrees [0-180]

**Constraints**:
- MUST appear exactly once per scene
- Orientation vector MUST be normalized (magnitude = 1.0 ±0.01)
- FOV MUST be in range [0, 180]

**Example**:
```
C  0,0,0  0,0,1  90
```

**Error Cases**:
- Missing element: "Error\nMissing mandatory element: Camera"
- Duplicate: "Error\nCamera can only be declared once"
- Non-normalized vector: "Error\nCamera orientation vector must be normalized, magnitude = <value>"
- Invalid FOV: "Error\nCamera FOV must be between 0 and 180, got <value>"

---

### Light (L)

**Format**: `L <x,y,z> <brightness> <R,G,B>`

**Parameters**:
1. `x,y,z` (double,double,double): Light position in 3D space
2. `brightness` (double): Light intensity ratio [0.0-1.0]
3. `R,G,B` (int,int,int): RGB color [0-255] for each channel

**Constraints**:
- MUST appear exactly once per scene (mandatory part)
- Brightness MUST be in range [0.0, 1.0]
- RGB values MUST be in range [0, 255]

**Example**:
```
L  -40,50,0  0.6  255,255,255
```

**Error Cases**:
- Missing element: "Error\nMissing mandatory element: Light"
- Duplicate: "Error\nLight can only be declared once (mandatory part)"
- Invalid brightness: "Error\nLight brightness must be between 0.0 and 1.0, got <value>"
- Invalid color: "Error\nRGB values must be between 0 and 255, got <value>"

---

### Sphere (sp)

**Format**: `sp <x,y,z> <diameter> <R,G,B>`

**Parameters**:
1. `x,y,z` (double,double,double): Sphere center position
2. `diameter` (double): Sphere diameter (must be positive)
3. `R,G,B` (int,int,int): RGB surface color [0-255]

**Constraints**:
- Multiple spheres allowed
- Diameter MUST be > 0
- RGB values MUST be in range [0, 255]

**Example**:
```
sp  0,0,20  20  255,0,0
```

**Error Cases**:
- Non-positive diameter: "Error\nSphere diameter must be positive, got <value>"
- Invalid color: "Error\nRGB values must be between 0 and 255, got <value>"

---

### Plane (pl)

**Format**: `pl <x,y,z> <norm_x,norm_y,norm_z> <R,G,B>`

**Parameters**:
1. `x,y,z` (double,double,double): Point on the plane
2. `norm_x,norm_y,norm_z` (double,double,double): Normalized normal vector
3. `R,G,B` (int,int,int): RGB surface color [0-255]

**Constraints**:
- Multiple planes allowed
- Normal vector MUST be normalized (magnitude = 1.0 ±0.01)
- RGB values MUST be in range [0, 255]

**Example**:
```
pl  0,-10,0  0,1,0  200,200,200
```

**Error Cases**:
- Non-normalized normal: "Error\nPlane normal vector must be normalized, magnitude = <value>"
- Invalid color: "Error\nRGB values must be between 0 and 255, got <value>"

---

### Cylinder (cy)

**Format**: `cy <x,y,z> <axis_x,axis_y,axis_z> <diameter> <height> <R,G,B>`

**Parameters**:
1. `x,y,z` (double,double,double): Cylinder center position
2. `axis_x,axis_y,axis_z` (double,double,double): Normalized axis vector
3. `diameter` (double): Cylinder diameter (must be positive)
4. `height` (double): Cylinder height (must be positive)
5. `R,G,B` (int,int,int): RGB surface color [0-255]

**Constraints**:
- Multiple cylinders allowed
- Axis vector MUST be normalized (magnitude = 1.0 ±0.01)
- Diameter MUST be > 0
- Height MUST be > 0
- RGB values MUST be in range [0, 255]

**Example**:
```
cy  0,0,0  0,1,0  10  20  0,255,0
```

**Error Cases**:
- Non-normalized axis: "Error\nCylinder axis vector must be normalized, magnitude = <value>"
- Non-positive diameter: "Error\nCylinder diameter must be positive, got <value>"
- Non-positive height: "Error\nCylinder height must be positive, got <value>"
- Invalid color: "Error\nRGB values must be between 0 and 255, got <value>"

---

## Parsing Rules

### General Rules

1. **Element Order**: Elements can appear in any order in the file
2. **Whitespace**: Parameters within a line are separated by one or more spaces or tabs
3. **Line Endings**: Unix (LF) or Windows (CRLF) line endings are both acceptable
4. **Empty Lines**: Empty lines are ignored (optional)
5. **Case Sensitivity**: Element identifiers (A, C, L, sp, pl, cy) are case-sensitive
6. **Comments**: Not supported in mandatory part (optional bonus)

### Validation Sequence

1. **File Access**: Check file exists and is readable
2. **Extension**: Verify file extension is `.rt`
3. **Line Parsing**: Parse each line, identify element type
4. **Parameter Parsing**: Extract and validate parameters for each element
5. **Range Validation**: Check all numeric values are within valid ranges
6. **Vector Validation**: Verify all normal/direction vectors are normalized
7. **Completeness Check**: Ensure A, C, L each appear exactly once
8. **Duplicate Check**: Ensure A, C, L do not appear multiple times

### Error Format

All errors MUST output to stderr in the format:
```
Error
<specific error message>
```

Program MUST exit with non-zero exit code after error.

---

## Complete Example Scene

```
A  0.2  255,255,255

C  0,0,0  0,0,1  90

L  -40,50,0  0.6  255,255,255

sp  0,0,20  20  255,0,0
sp  50,0,20  10  0,255,0

pl  0,-10,0  0,1,0  200,200,200

cy  25,0,10  0,1,0  10  30  0,0,255
```

This scene contains:
- Ambient light: 20% intensity, white
- Camera: at origin, looking in +Z direction, 90° FOV
- Light: positioned at (-40, 50, 0), 60% brightness, white
- Two spheres: red (diameter 20) and green (diameter 10)
- One plane: horizontal ground plane at y=-10, gray
- One cylinder: vertical, blue, diameter 10, height 30

---

## Edge Cases

### Numeric Precision
- Floating point comparisons use tolerance ±0.01 for normalized vectors
- Very small values (< 1e-6) may be treated as zero

### Boundary Values
- FOV = 0: Orthographic-like projection (extremely narrow view)
- FOV = 180: Hemispherical view (fisheye effect)
- Ratio = 0.0: No ambient light (pure black shadows)
- Ratio = 1.0: Full brightness (overexposed)
- Diameter/height → 0: Error (must be positive)

### Vector Normalization
- Tolerance: magnitude within [0.99, 1.01] acceptable
- Zero vector [0,0,0]: Error (cannot normalize)
- Non-unit vector: Error with actual magnitude reported

### File Issues
- File not found: "Error\nCannot open file: <filename>"
- Wrong extension: "Error\nInvalid file extension, expected .rt"
- Empty file: "Error\nScene file is empty or incomplete"
- Malformed line: "Error\nInvalid format at line <N>"

---

## Implementation Notes

**Parser Structure**:
```c
t_scene *parse_scene_file(const char *filename);
int parse_line(char *line, t_scene *scene, int line_num);
int parse_ambient(char **tokens, t_scene *scene);
int parse_camera(char **tokens, t_scene *scene);
int parse_light(char **tokens, t_scene *scene);
int parse_sphere(char **tokens, t_scene *scene);
int parse_plane(char **tokens, t_scene *scene);
int parse_cylinder(char **tokens, t_scene *scene);
```

**Validation Functions**:
```c
int validate_ratio(double value, const char *element);
int validate_rgb(int r, int g, int b, const char *element);
int validate_normalized_vector(t_vec3 vec, const char *element);
int validate_positive(double value, const char *element);
int validate_fov(double fov);
```

**Error Reporting**:
```c
void print_error(const char *message);
void print_error_value(const char *message, double value);
void print_error_line(const char *message, int line_num);
```

---

## Version History

- **1.0** (2025-12-15): Initial scene file format specification
