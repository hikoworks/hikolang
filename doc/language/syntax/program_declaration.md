# module-declaration

## Syntax

`program` [_string_literal_](string_literal.md) [_compile_condition_](compile_condition.md)__?__ `;`


## Semantics

The `program` declaration tells the compiler to create an executable file. The
name of the executable is based on the _string_literal_, the compiler will
automatically append the correct extension needed for application for the
target platform.

Any variables and functions are declared in the root namespace. Typically the
function `main()` is the entry-point of a program.

The optional [_compile_condition_](compile_condition.md) is evaluated during the
prologue-scan phase of compilation, this checks if the file should be compiled.
If no [_compile_condition_](compile_condition.md) is specified then the program
is compiled during the `build`-step.

Multiple files may have the same name, only if the conditional
compilation is mutually exclusive. If a file has a `fallback` condition, it will
be used if no other file with the same name is compiled.

