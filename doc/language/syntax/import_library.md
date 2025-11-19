# import-lib

## Syntax

`import` `library` _lib-name_
    [_version_literal_](version_literal.md)__?__
    __(__ `if` [_guard_expression_](guard_expression.md) __)?__ `;`

lib-name := [_string_literal_](string_literal.md)


## Semantics

Link any applications that imports a module or its package recursively to
the library named _lib-name_.

The library is either an archive/static library or a dynamic/shared library 
and is found in the library path. 

The _lib-name_ is the base name of the library, the compiler will automatically
add the proper version-number and extension to the base name to link against
the actual library.


### Version number (optional)

An optional [_version_literal_](version_literal.md) is used to select a specific
version of a library.


### Build guard (optional)

The optional [_guard_expression_](guard_expression.md) is evaluated during the
prologue-scan phase of compilation, this checks if the `import` statment
should be processed or ignored.
