# Lambda

## Syntax

`{` __(__ [_argument-list_](argument_list.md) `|` __)?__ [_statement-list_](statement_list.md) `}`


## Arguments

## Capture
Any variable that is used inside the lambda is automatically captured by copy.

## Result
The result of the lambda is the value returned by the `return` statement. If
there is no `return` statement, the result of the lambda is the value of the
last expression in the [_statement-list_](statement_list.md). Or `void` if
the last statement is not an expression.



