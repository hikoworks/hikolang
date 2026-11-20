# program-declaration

[_build-guard_]: build_guard.md
[_string-literal_]: string_literal.md
[_version-literal_]: version_literal.md

## Syntax

_program-declaration_:= `program` [_string-literal_]
    [_version-literal_]__?__ [_build-guard_]__?__ `;`


## Semantics

The `program` declaration tells the compiler to create an executable file. The
name of the executable is based on the _string_literal_, the compiler will
automatically append the correct extension needed for application for the
target platform.

Any variables and functions are declared in the root namespace. Typically the
function `main()` is the entry-point of a program.


### Version (optional)

If a version number is given, then the program has a version number, this
version number can be refelected within the application, and may be used for
a `--version` command line argument.

The version has no meaning within the compilation process.


### Build guard (optional)

The optional [_guard_expression_](guard_expression.md) is evaluated during the
prologue-scan phase of compilation, this checks if the file should be compiled.
If no [_guard_expression_](guard_expression.md) is specified then the file
is compiled when all other [_guard_expression_](guard_expression.md) fail.


