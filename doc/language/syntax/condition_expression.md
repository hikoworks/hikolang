# condition-expression

## Syntax

[_expression-list_](expression_list.md)

### condition-expression
A _condition-expression_ is an [_expression-list_](expression_list.md) which
results in a value that is convertible to a boolean value.

The last expression in the _condition-expression_ is not allowed to be an
assignment expression. This is to prevent a common bug; confusing `=` with `==`.

> [!TIP]
> The compiler will report an error if the last expression is an
> assignment. To bypass this restriction, use a
> [_sub-expression_](sub_expression.md).

