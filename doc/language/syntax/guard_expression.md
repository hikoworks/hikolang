# guard-expression

## Syntax

`(` _guard_expression_ `)` __|__\
`not` _guard_expression_ __|__\
_guard_expression_ `and` _guard_expression_ __|__\
_guard_expression_ `or` _guard_expression_ __|__\
_guard_expression_ `in` _guard_expression_ __|__\
_guard_expression_ `==` _guard_expression_ __|__\
_guard_expression_ `!=` _guard_expression_ __|__\
_guard_expression_ `<` _guard_expression_ __|__\
_guard_expression_ `>` _guard_expression_ __|__\
_guard_expression_ `<=` _guard_expression_ __|__\
_guard_expression_ `>=` _guard_expression_ __|__\
[`_identifier_`](identifier.md) __|__
[`_string_literal_`](string_literal.md) __|__
[`_version_literal_`](version_literal.md) __|__
[`_integer_literal_`](integer_literal.md) __|__
[`_boolean_literal_`](boolean_literal.md)

### Operators
The following operators are available in the build guard expression:

 - `and`: Short-cutting logical AND
 - `or`: Short-cutting logical OR
 - `not`: Logical not
 - `in`: Check if a value in array
 - `==`: Compare equal
 - `!=`: Compare not equal
 - `<`: Compare less
 - `>`: Compare greater
 - `<=`: Compare less-or-equal
 - `>=`: Compare greater-or-equal
 - `(` `)`: brackets

### Types

  | type          | false  |
  |:------------- |:------ |
  | bool          | false  |
  | integer       | 0      |
  | string        | ""     |
  | array[string] | []     |

### Special constants

Beyond the constants listed below, you may add custom constants to the compiler
with the `--define` command line option, or the `HKDEFINE` environment variable.

Language:
 - **version**: version: Language version

Build type:
 - **build**: string: "debug", "release"
 - **debug**: bool
 - **release**: bool

Operating systems:
 - **os**: string: "windows", "macos", "linux"
 - **os_version**: version
 - **os_platform**: string: "desktop", "phone", "watch", "pad" 

The CPU:
 - **cpu_arch**: string: "x86", "arm"
 - **cpu_arch_level**: int: x86=0, x86_64v1=1, x86_64v2=2
 - **cpu_bits**: int: 32, 64
 - **cpu_features**: array[string]: true


