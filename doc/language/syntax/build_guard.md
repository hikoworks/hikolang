# build-guard

## Syntax

`if` [_build_guard_expression_](build_guard_expression.md) __|__ `fallback`

The build guard is appended at the end of prologue statements, such as
`module` and `import`-statements. Since
it is at the end of the statement, the expression is handled up to the
semicolon of the declaration.

## Semantics
The `if` build guard is used to conditionally compile a module or import.
while the `fallback` build guard is used to specify a module or import
that should be used if no other module or import is compiled.

A [_build_guard_expression_](build_guard_expression.md) is a simple
boolean expression, to keep it as simple as possible.
If the expression evaluates to `true`, the module or import is compiled.
If the expression evaluates to `false`, the module or import is not compiled.


