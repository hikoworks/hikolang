# catch-operator

## Syntax

[_expression_](expression.md) `catch` [_expression_](expression.md)

## Semantics
The `catch` operator is used to catch any error thrown by the left-hand side expression.

 - If the left-hand side expression does not throw an error, the value of the left-hand
   side expression is the result.
 - If the left-hand side expression throws an error, the right-hand side expression is
   evaluated and its value is the result.

