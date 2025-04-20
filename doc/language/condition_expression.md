# Condition-expression

## Syntax

__(__ [_expression_](expression.md) __(__ `;` [_expression_](expression.md) __)*__ `;` __)?__ [_test-expression_](test_expression.md)

## Semantics
The expressions are evaluated in order from left to right. The last
test-expression is evaluated and the result is returned. The other expressions
are evaluated for their side-effects only.

The test-expression excludes the assignment expression, as it is not allowed to
be the last part of a condition-expression. This is to prevent confusion between
`=` and `==`. As a work around, the assignment expression may be contained
inside a sub-expression.

## Automatic semicolon insertion
The tokenizer will automatically:
 - insert a semicolon after a control-expressions,
 - remove duplicate semicolons,
 - remove the semicolon after the last statement in a block.