# build-guard-expression

## Syntax

`(` _build_guard_expression_ `)` __|__\
`not` _build_guard_expression_ __|__\
_build_guard_expression_ `and` _build_guard_expression_ __|__\
_build_guard_expression_ `or` _build_guard_expression_ __|__\
_build_guard_constant_

_build_guard_constant_ := [`_identifier_`](identifier.md)

### Operators
The following operators are available in the build guard expression:

 - `and`: Logical and
 - `or`: Logical or
 - `not`: Logical not
 - `(` `)`: brackets

### Special constants
Beyond the constants listed below, you may add custom constants to the compiler
with the `--define` command line option, or the `HKDEFINE` environment variable.

Literals:
 - **true**: Always true
 - **false**: Always false

Build type:
 - **debug**: Application is build for debugging.
 - **release**: Application is build for release.

Operating systems:
 - **windows**: Build for Microsoft Windows platform.
 - **windows_10**: Microsoft Windows 10 platform.
 - **windows_11**: Microsoft Windows 11 platform.
 - **macos**: Build for Apple MacOS platform.
 - **macos_10_4**: Build for Apple OSX 10.4 platform.
 - **macos_15**: Build for Apple MacOS 15 platform.
 - **linux**: Build for Linux platform.
 - **ios**: `phone` or `pad` version of `macos`
 - **android**: `phone` or `pad` version of `linux`

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
 - **cpu_32**: CPU has 32 bit ABI
 - **cpu_64**: CPU has 64 bit ABI
