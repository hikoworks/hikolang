# Expression

## Syntax

[_test-expression_](test_expression.md) __|__
[_assignment-expression_](assignment_expression.md)

## Semantics
Every expression results in a value including control-expressions like `if`,
`switch`, `while`, `for` and `try`.

The [_assignment-expression_](assignment_expression.md) is a special case as it
should not be the last part of a
[_condition-expression_](condition_expression.md), to prevent confusion between
`=` and `==`.

