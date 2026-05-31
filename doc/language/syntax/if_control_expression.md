# if-control-expression

## Syntax

_if-control-expression_ := _if-control-expression-1_ __|__ _if-control-expression-2_

_if-control-expression-1_ :=
    `if` `(` [_condition-expression_] `)` `{` [_code-block_] `}` [_catch-clauses_]**?**\
    __(__ `else` `if` `(` [_condition-expression_] `)` `{` [_code-block_] `}` [_catch-clauses_]**?** __)*__\
    __(__ `else` `{` [_code-block_] `}` __)?__

Alternatively when there are no `elif` clauses, the `catch` clauses may be
placed after the `else` clause.

_if-control-expression-2_ :=
    `if` `(` [_condition-expression_] `)` `{` [_code-block_] `}`\
    `else` `{` [_code-block_] `}` [_catch-clauses_]**?**

[_catch-clauses_]: catch_clauses.md
[_code-block_]: code_black.md
[_condition-expression_]: condition_expression.md

## Semantics
The `if` statement is used to conditionally execute a block of code. Each
[_condition-expression_](condition_expression.md) is evaluated in order, until
one of them evaluates to `true`. The block of code associated with that
[_condition-expression_](condition_expression.md) is then executed. If there are
no `if` or `elif` clauses that evaluate to `true`, the `else` block is executed.

Any errors that occur in the _condition-expression_ may be caught by the
[_catch-clauses_](catch_clauses.md) that directly follow the code-block that is
associated with that [_condition-expression_](condition_expression.md).

If there are no `elif` clauses, the `catch` clauses may be placed after the
`else` block. This is more intuitive, as handling an error is secondary to both
the `if` and `else` branches.

> [!CAUTION] 
> Only errors occuring in the [_condition-expression_](condition_expression.md)
> are caught by the [_catch-clauses_](catch_clauses.md). Errors occuring in the
> code-blocks must be handled separately.

