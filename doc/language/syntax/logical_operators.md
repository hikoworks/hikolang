# logical-operators

## Syntax

[_expression_](expression.md) `and` [_expression_](expression.md) __|__
[_expression_](expression.md) `or` [_expression_](expression.md) __|__
`not` [_expression_](expression.md)

## Semantics
The logical operators `and`, `or` and `not` are used to combine boolean expressions.

Features of the logical operators:
 - The `and` and `or` operators are short-circuit operators.
   Meaning that the right-hand side expression is not evaluated if the left-hand
   side expression is sufficient to determine the result.
 - The result of the `and` and `or` operators is the value-result of the last
   evaluated expression.
 - The result of the expressions are explicitly converted to a boolean value
   before the evaluation of the logical operator.

The operations of the logical operators are:
 - The `and` operator will return the first operand that is evaluated as `false`.
 - The `or` operator will return the first operand that is evaluated as `true`.
 - The `not` operator will return the negation of the boolean value of the
   expression.

## Examples

```
assert((0 or 5) == 5)
assert((1 or 5) == 1)

assert((0 and 5) == 0)
assert((1 and 5) == 5)

assert((not 0) == true)
assert((not 5) == false)
```
