# if-control-expression

## Syntax

_if-control-expression_ :=\
    [_do-clause_]__?__\
    `if` `(` [_condition-expression_] `)` __(__ `{` [_code-block_] `}` __)?__\
    [_alternate-clauses_]


[_alternate-clauses_]: alternate_clauses.md
[_code-block_]: code_black.md
[_condition-expression_]: condition_expression.md
[_do-clause_]: do_clause.md

## Semantics
The `if` statement is used to conditionally execute a block of code. When
the [_condition-expression_] evaluates to `true` then the [_code-block_]
is executed, otherwise one of the [_alternative-clauses_] is executed.


> [!CAUTION] 
> Only errors occuring in the [_condition-expression_](condition_expression.md)
> are caught by the [_catch-clauses_](catch_clauses.md). Errors occuring in the
> code-blocks must be handled separately.

