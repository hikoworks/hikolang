# module-declaration

## Syntax

`program` [_string_literal_](string_literal.md)
    [_version_literal_](version_literal.md)__?__
    [_guard_expression_](guard_expression.md)__?__ `;`


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

Multiple files may have the same name, only if the conditional
compilation is mutually exclusive. If a file has a `fallback` condition, it will
be used if no other file with the same name is compiled.

