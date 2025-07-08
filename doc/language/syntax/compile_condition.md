# compile-condition

## Syntax

`if` [_expression_](expression.md) __|__ `fallback`

The compile condition is appended at the end of some declarations, such as
[`module`](module_declaration.md) and [`import`](import_declaration.md). Since
it is at the end of the declaration, the expression is handled up to the
semicolon of the declaration.

## Semantics
The `if` compile condition is used to conditionally compile a module or import.
while the `fallback` compile condition is used to specify a module or import
that should be used if no other module or import is compiled.

The expression is evaluated during the prologue-scan phase of compilation.
If the expression evaluates to `true`, the module or import is compiled.
If the expression evaluates to `false`, the module or import is not compiled.

During the prologue-scan phase, the compiler is very limited to what kind of
expressions can be used. No function, a subset of operators, and only a set
of special constants are available.

### Operators
The following operators are available in the compile condition expression:

 - `==`: Equal to.
 - `!=`: Not equal to.
 - `<`: Less than.
 - `<=`: Less than or equal to.
 - `>`: Greater than.
 - `>=`: Greater than or equal to.
 - `and`: Logical and
 - `or`: Logical or
 - `not`: Logical not

### Literals
The following literals are available in the compile condition expression:

 - [_integer-literal_](literal.md#integer-literal)
 - [_boolean-literal_](literal.md#boolean-literal)
 - [_string-literal_](literal.md#string-literal)
 - [_version-literal_](literal.md#version-literal)

### Special constants
Beyond the constants listed below, you may add custom constants to the compiler
with the `--define` command line option, or the `HKDEFINE` environment variable.

Build phases:
 - **download**: Download data before generating code.
 - **generate**: Generate code before building.
 - **build**: Build normal applications.

Build type:
 - **debug**: Application is build for debugging.
 - **release**: Application is build for release.

Operating systems:
 - **windows**: (version) Build for Microsoft Windows platform. Version of
   Windows
 - **macos**: (version) Build for Apple MacOS platform. Version of MacOS.
 - **linux**: (version) Build for Linux platform. Version of Linux kernel.
 - **ios**: (version) Mobile version of `macos`, which is also set.
 - **android**: (version) Mobile version of `linux`, which is also set.

Platform:
 - **desktop**: 
 - **phone**: 
 - **watch**: 
 - **pad**: 

The CPU:
 - **x86**: Intel x86 CPU. Integer is register size.
 - **x86_32**
 - **x86_64**
 - **x86_64v1**
 - **x86_64v2**
 - **x86_64v3**
 - **x86_64v4**
 - **x86_AVX**: Intel x86 AVX feature.
 - **arm**: Arm CPU. Integer is register size.
 - **arm_32**
 - **arm_64**
