# import-lib

## Syntax

`import` `library` _lib-name_ [_semantic_version_]__?__ [_compile_condition_](compile_condition.md)__?__ `;`

lib-name := [_string_literal_](string_literal.md)

## Semantics

Link any applications that imports a module or its package recrusively to
the library named _lib-name_.

The library is either a archive/static library or a dynamic/shared library 
and is found in the library path. 

The _lib-name_ is the base name, it excludes both the prefix `lib` and the
extensions: `.so`, `.a`, `.lib`, `.dll`.

An optional _semantic-version_ is used to select a specific version of a
library.
