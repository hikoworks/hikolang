# switch-control-expression

[_do-clause_]: do_clause.md
[_expression_]: expression.md
[_catch-clauses_]: catch_clauses.md
[_code-block_]: code_block.md

## Syntax

_switch-control-expression_ :=\
    [_do-clause_]__?__\
    `switch` `(` [_expression_]`)` `{`\
        __(__ `case` `(` [_expression_] `)` [_code-block_] __)*__\
        __(__ `default` [_code-block_] __)?__\
    `}`\
    __(__ `empty` `{` [_statement-list_] `}` __)?__\
    [_catch-clauses_]


## Semantics

The expression in the `switch` clause is executed for the result value that
will be equality-tested with the expressions in each `case` clause. If one
of the `case` clauses matches the corresponding code-block is executed.
If none of the `case` clauses matches, the code-block of the `default` clause
is executed.

The expression in the `switch` clause may initialize a variable that can be
used in the subsequent `case` and `default` clause.

If the expression in the `switch` clause throws and error or results in _empty_
then these are caught in the `catch` and `empty`-clauses. The expression in
the `case` clauses are not allowed to throw errors or result in _empty_.


## Exhaustive check

If the members of an `enum` fully populate all the values of the underlying
type, then the enum is _dense_. Otherwise the enum is _sparse_.

A `default` clause is required when the enum in the `switch` clause is sparse.

If the enum is dense the `case` clauses check every enum-member then the
`default` clause is not required.
