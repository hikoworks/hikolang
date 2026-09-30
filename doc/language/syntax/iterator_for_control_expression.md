# iterator-for-control-expression

## Syntax

_iterator-for-loop-control-expression_ :=\
    [_do-clause_]__?__\
    `for` `(` [_variable_definition_]__?__ `;`  [_condition_expression_]__?__ `;` [_expression_]__?__ `)`\
        __(__ [_annotation_]__*__ [_code-block_] __)?__\
    [_alternate-clauses_]



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
[_if-control-expression_]: if_control_expression.md
[_code-block_]: code_block.md
[_condition_expression_]: condition_expression.md
[_do-clause_]: do_clause.md
[_expression_]: expression.md
[_variable_definition_]: variable_definition.md
