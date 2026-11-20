# import

[_build-guard_]: build_guard.md
[_expression_]: expression.md

## Syntax

_import-eval_ := `import` `eval` [_expression_] `;`


## Semantics

Imports a module from a generated string, returned from the
[_expression_](expression.md). The generated string is compiled and its symbols
are imported into the current module.

The [_expression_](expression.md) is evaluated in the default namespace of
the module, after all `import module` statements in the prologue of the current
module have been processed.

The returned string must be a full module's text, including a prologue with a
basic `module` declaration and optional `import` statements.

A generated module cannot:
 - import repositories

> [!NOTE]
> It is the generator function's responsibility to cache results of an expensive
> generation.

