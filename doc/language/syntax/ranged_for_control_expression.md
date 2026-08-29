# ranged-for-control-expression

## Syntax

_ranged-for-loop-control-expression_ :=\
    [_do-clause_]__?__\
    `for` `(` [_left-expression_] `in` [_expression_] `)` __(__ `{` [_code-block_] `}` __)?__ \
    [_alternate-clauses_]


_range-expression_, _start-expression_, _condition-expression_, and
_increment-expression_ are all [_expression_]s.

[_alternate-clauses_]: alternate_clauses.md
[_code-block_]: code-block.md
[_do-clause_]: do_clause.md
[_expression_]: expression.md
[_left-expression_]: left_expression.md

## Semantics

### Alternate clauses

The full set of [_alternate-clauses_] are valid for `for` loops.
A `break` statement inside the `for`-loop's [_code-block_] skips over all the [_alternate-clauses_].
