# library-declaration

## Syntax

`library` [_string_literal_](string_literal.md) [_version_literal_](version_literal.md) [_build_guard_](build_guard.md)__?__ `;`


## Semantics

The `library` declaration tells the compiler to create an library file. The
name of the library is based on the [_string_literal_](string_literal.md)
and [_version_literal_](version_literal.md), the compiler will
automatically format the filename with the version number and the correct
extension needed for a library on the target platform.

The version literal allows multiple versions of the same library to coexist
on the system.

The compiler will automatically create:
 - A dynamic library,
 - A static library,
 - A header file for C/C++ that matches the ABI of the exposed functions and
   variables.

The optional [_build_guard_](build_guard.md) is evaluated during the
prologue-scan phase of compilation, this checks if the file should be compiled.
If no [_build_guard_](build_guard.md) is specified then the program
is compiled during the `build`-step.

Multiple files may have the same name, only if the conditional
compilation is mutually exclusive. If a file has a `fallback` condition, it will
be used if no other file with the same name is compiled.

