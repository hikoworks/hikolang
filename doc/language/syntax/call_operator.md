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

### @elaborate

Call this function during elaboration. This means any function that is annotated
with `@with_effect(phase_variant)` will be executed in the compiler's
environment.

> [!note]
> Normally any function annotated with `@with_effect(phase_variant)` is delayed
> until runtime.

Any arguments passed into the call will be elaborated as well. It is an error
if one or more arguments are not available during elaboration.

The function being called can still be optimized to be partially executed
during elaboration, for example function calls to functions that are not
annotated with `@with_effect(phase_variant)`.

Function inlining, constant folding and other optimizations are still valid.
