# if-control-expression

## Syntax

_if-control-expression_ :=\
    [_do-clause_]__?__\
    `if` `(` [_init_expression_]__?__ [_condition-expression_] `)` [_annotation_]__*__ [_code-block_]\
    [_alternate-clauses_]



## Semantics
The `if` statement is used to conditionally execute a block of code. When
the [_condition-expression_] evaluates to `true` then the [_code-block_]
is executed, otherwise one of the [_alternative-clauses_] is executed.


> [!CAUTION] 
> Only errors occuring in the [_condition-expression_](condition_expression.md)
> are caught by the [_catch-clauses_](catch_clauses.md). Errors occuring in the
> code-blocks must be handled separately.


## Annotations

### [[fast]]

Used on a [_code-block_] of a flow-control clause, indicating how this branch
is optimized. When using `[[fast]]` you are asking the compiler to make this
branch fast, even if other branches would be impacted negatively.

This will:
 * Order code to reduce latency for this fast-path.
 * Inline code as much as possible. (This is the default.)

> [!note]
> In most cases `[[fast]]` will have very little impact on performance.
> Instead look for `[[lean]]` for a much larger impact.


### [[lean]]

Used on a [_code-block_] of a flow-control clause, indicating how this branch
is optimized. When using `[[lean]]` you are asking the compiler to produce
code that impact the other branches positively.

This will:
 * Call functions rather than inline functions.
 * Reduce code-size.
 * Reduce register pressure.

[_alternate-clauses_]: alternate_clauses.md
[_annotation_]: annotation.md
[_code-block_]: code_black.md
[_condition-expression_]: condition_expression.md
[_do-clause_]: do_clause.md
[_init_expression_]: init_expression.md
