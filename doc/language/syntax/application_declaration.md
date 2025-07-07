# module-declaration

## Syntax

`application` [_string_literal_](string_literal.md) __(__ `if` [_compile_condition_](compile_condition.md) __|__ `fallback` __)?__ `;`

## Semantics

A file with an `application` declaration is used to create an executable with
the name of the string literal. The `application` declaration is similar to a
`module` declaration, but it is used to create an executable application instead
of a library module.

The optional _compile_condition_ is evaluated during the prologue-scan phase of
compilation, this checks if the file should be compiled.

Multiple files may have the same application-name, only if the conditional
compilation is mutually exclusive. If a file has a `fallback` condition, it will
be used if no other file with the same application-name is compiled.
