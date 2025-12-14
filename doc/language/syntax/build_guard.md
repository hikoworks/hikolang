# build-guard

## Syntax

`if` [_build-guard-expression_](build_guard_expression.md)

## Semantics

A guard is used to conditionally compile:
 - The current file if it suffixes a: `program`, `library` or `module`
   declaration.
 - Imports when it suffixes one of the `import` statements.
 - Functions when the guard is part of the function's attributes.

Since the [_build-guard-expression_](build_guard_expression.md) must be
evaluated before any file gets compiled, this expression can only query
information supplied by the compiler.

When the _guard_ is used on a `program`, `library` or `module` declaration,
there may be multiple files with the same identifier. In this case only one
_guard_ may evaluate to `true`. You may also have one file with the same
identifier without a _guard_ which will be used as a fallback when all other
guards evaluate to `false`.

When the _guard_ is used on a `fn` definition, there may be multiple functions
with the same identifier. In this case only one _guard_ may evaluate to `true`.
You may also have one function with the same identifier without a _guard_ which
will be used as a fallback when all other guards evaluate to `false`.

`import` statements do not need to be mutual exclusive, nor does it have
a fallback.
