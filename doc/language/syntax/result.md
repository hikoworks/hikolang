# Result statement

## Syntax

`result` [_expression_](expression.md) `;`

## Semantics
The `result` statement is used to return or yield a value from the first
enclosing control-expression that is used as an expression.

This means that if a control-expression is used as a statement, the `result`
statement will return or yield the value from the next enclosing
control-expression. It is a __static error__ when there is no enclosing
control-expression that is used as an expression.

There are two different semantic meanings of the `result` statement:
 1. Inside branch control-expression, such as `if`, `switch` and `try`
    statements, the `result` statement will return a value.
 2. Inside loop control-expression, such as `while` and `for` statements, the
    `result` statement will yield the value and continue
    the loop directly after the `result` statement.

It is a warning when there are statements following `result` inside
branch control-expressions like `if`, `switch` and `try`.
