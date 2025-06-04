# compile-condition

## Syntax

'`(` _compile_condition_ `)` __|__
[_identifier_](identifier.md) __|__
[_integer_literal_](integer_literal.md) __|__
[_version_literal_](version_literal.md) __|__
_compile_condition_ `<` _compile_condition_ __|__
_compile_condition_ `>` _compile_condition_ __|__
_compile_condition_ `<=` _compile_condition_ __|__
_compile_condition_ `>=` _compile_condition_ __|__
_compile_condition_ `==` _compile_condition_ __|__
_compile_condition_ `!=` _compile_condition_ __|__
_compile_condition_ `and` _compile_condition_ __|__
_compile_condition_ `or` _compile_condition_ __|__
`not` _compile_condition_

## Semantics

If a identifier is a `false`, then any comparison with a version-literal or integer-literal is `false` as well.

Build phases:
 - **download**: Download data before generating code.
 - **generate**: Generate code before building.
 - **build**: Build normal applications.

Build type:
 - **debug**: Application is build for debugging.
 - **release**: Application is build for release.

Operating systems:
 - **windows**: (version) Build for Microsoft Windows platform. Version of Windows
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
