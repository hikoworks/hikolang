# ranged-for-control-expression

## Syntax

_ranged-for-loop-control-expression_ :=\
    [_do-clause_]__?__\
    `for` `(` [_left-expression_] `in` [_expression_] `)`\
        __(__ [_annotation_]__*__ [_code-block_] __)?__ \
    [_alternate-clauses_]


_range-expression_, _start-expression_, _condition-expression_, and
_increment-expression_ are all [_expression_]s.


## Semantics

### Alternate clauses

The full set of [_alternate-clauses_] are valid for `for` loops.
A `break` statement inside the `for`-loop's [_code-block_] skips over all the [_alternate-clauses_].


## Annotations

### [[fast]]

See: [_if-control-expression_]

A `[[fast]]` annotation on a else-clause would reorder instructions, this
improves the performance of retry loops.


### [[lean]]

See: [_if-control-expression_]

[_alternate-clauses_]: alternate_clauses.md
[_annotation_]: annotation.md
[_code-block_]: code-block.md
[_do-clause_]: do_clause.md
[_expression_]: expression.md
[_if-control-expression_]: if_control_expression.md
[_left-expression_]: left_expression.md
