# call-operator

## Syntax

_call-operator_ := [_expression_] `(` [_argument-list_]__?__ `)`


[_argument-list_]: argument_list.md
[_expression_]: expression.md


## Semantics

The `()` operator is used to call a function or method. The left-hand side
expression must be a function or method, and the right-hand side expression must
be a list of arguments.


## Annotations

### [[call]]

Explicitly emit a `call`-like instruction and do not inline.

### @fold

Force a function call to be evaluated (constant-folded) during the
translation phase of the compiler; even if the function is annotated
with `@with_effect(+phase_variant)`.

> [!note]
> Normally any function annotated with `@with_effect(+phase_variant)` is
> translated so that it is executed during runtime.

All arguments passed to a `@fold` call must already be available as
compile-time values. An error is raised if an argument requires
compile-time evaluation that has not otherwise been requested or
performed (by guaranteed constant-folding).

> [!note]
> The `@fold` annotation does not propagate to argument
> dependencies; it only forces evaluation of the annotated function call
> itself.
