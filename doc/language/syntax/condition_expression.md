# condition-expression

## Syntax

_condition-expression_ := [_expression_]

[_expression_]: expression.md

### condition-expression
A _condition-expression_ is an [_expression_] which
results in a value that is convertible to a boolean value.

The [_expression_] is not allowed to be an assignment expression.
This is to prevent a common bug; confusing `=` with `==`.

> [!TIP]
> The compiler will report an error if the expression is an
> assignment. To bypass this restriction, use a
> subexpression: `(` [_expression_] `)`
