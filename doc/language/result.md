# Result statement

## Syntax
<style type="text/css">
strong {
    color: green !important;
    font-weight: normal !important;
}
</style>

`result` _expression_

## Semantics
The `result` statement is used to return/yield a value from a code-block.

There are two different semantic meanings of the `result` statement:
 1. Inside branch control statements, such as `if`, `switch` and `try`
    statements, the `result` statement will return the value and exit the
    current block.
 2. Inside loop statements, such as `while` and `for` statements, the `result`
    statement will yield the value from the current block and continue the loop
    directly after the `result` statement.

