# library-declaration

## Syntax

[_documentation_](documentation.md)__?__
`library` [_string_literal_](string_literal.md) [_version_literal_](version_literal.md)
    __(__ `if` [_guard_expression_](guard_expression.md) __)?__ `;`


## Semantics

The `library` declaration tells the compiler to create an library file. The
name of the library is based on the [_string_literal_](string_literal.md)
and [_version_literal_](version_literal.md), the compiler will
automatically format the filename with the version number and the correct
extension needed for a library on the target platform.

The version literal allows multiple versions of the same library to coexist
on the system.cd

The compiler will automatically create:
 - A dynamic library,
 - A static library,
 - A header file for C/C++ that matches the ABI of the exposed functions and
   variables.

The optional [_guard_expression_](guard_expression.md) is evaluated during the
prologue-scan phase of compilation, this checks if the file should be compiled.
If no [_guard_expression_](guard_expression.md) is specified then the file
is compiled when all other [_guard_expression_](guard_expression.md) fail.
