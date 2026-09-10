# empty-operator

## Syntax

_empty-operator_ :=\
      [_expression_] `empty` [_expression_]

[_expression_]: expression.md

## Semantics
The `empty` operator is used to catch any empty value returned by the left-hand side expression.

 - If the left-hand side expression returns a value, the value of the
   left-hand side expression is the result.
 - If the left-hand side expression returnd emptt, the right-hand side
   expression is evaluated and its value is the result.
 - Any errors thrown from the left-hand expression are unhandled.
