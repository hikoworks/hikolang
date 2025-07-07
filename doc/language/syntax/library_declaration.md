# library-declaration

## Syntax

`library` [_string_literal_](string_literal.md) __(__ `if` [_compile_condition_](compile_condition.md) __)?__ `;`

## Semantics
This declares that a _ffi_ (foreign function interface) library must be linked
to the executable that imports this module or package.

The _string_literal_ is the name of the library to link to. The library must be
available in the system's library search path, or path specified by the
`--library-path`, `-L` option of the compiler.

The optional _compile_condition_ is evaluated during the prologue-scan phase of
compilation, this checks if the library should be linked. This can be used
to select the library based on the target platform or architecture.
