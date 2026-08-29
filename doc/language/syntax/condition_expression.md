# condition-expression

## Syntax

_condition-expression_ := [_expression-list_]

[_expression-list_]: expression_list.md

### condition-expression
A _condition-expression_ is an [_expression-list_](expression_list.md) which
results in a value that is convertible to a boolean value.

The last expression in the _condition-expression_ is not allowed to be an
assignment expression. This is to prevent a common bug; confusing `=` with `==`.

> [!TIP]
> The compiler will report an error if the last expression is an
> assignment. To bypass this restriction, use a
> [_sub-expression_](sub_expression.md).

If an expression results in a `__result__[T]` then:
 * If the result is *empty* then the `empty`-clause will be executed
 * If the result is *error* then the matching `catch`-clause will be executed.
 * If the result is *fatal-error* then the matching `catch fatal`-clause will be executed.
 * On *value* unwrap the `__result__[T]` to `T`.
