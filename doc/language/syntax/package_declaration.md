# library-declaration

## Syntax

`package` [_fully_qualified_name_](fully_qualified_name.md) [_version_literal_](version_literal.md) [_compile_condition_](compile_condition.md)__?__ `;`


## Semantics

The `package` declaration tells the compiler that it and its sub-modules can
be imported from a different repository.

If multiple repositories are imported that declare the same package, then
only the package with the highest [_version_literal_](version_literal.md) is
compiled. This makes it possible downstream repositories to import different
versions of a repository and resolve conflicts. 

The optional [_compile_condition_](compile_condition.md) is evaluated during the
prologue-scan phase of compilation, this checks if the file should be compiled.
If no [_compile_condition_](compile_condition.md) is specified then the program
is compiled during the `build`-step.

Multiple files may have the same name, only if the conditional
compilation is mutually exclusive. If a file has a `fallback` condition, it will
be used if no other file with the same name is compiled.

