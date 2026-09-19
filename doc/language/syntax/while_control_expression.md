# while-control-expression

## Syntax

_while-control-expression_ :=\
    [_do-clause_]__?__\
    `while` `(` [_init_expression_]__?__ [_condition-expression_] `)` __(__ `{` [_code-block_] `}` __)?__\
    [_alternate-clauses_]


[_alternate-clauses_]: alternate_clauses.md
[_code-block_]: code_block.md
[_condition-expression_]: condition_expression.md
[_do-clause_]: do_clause.md
[_init_expression_]: init_expression.md

## Semantics
The `while` control-expression is a loop that executes the body of the
loop as long as the [_condition-expression_] is true.

A `while` control-expression does the following steps:
 1. The `do` code-block is executed first.
 2. The `while` [_condition-expression_] is evaluated.
 3. If the [_condition-expression_] is `true`:
    4. The `while` code-block is executed.
    2. goto 1.
 4. If the [_condition-expression_] is `false`:
    5. The `else` code-block is executed.
    6. The loop is exited.

When the `do` or `while` code-blocks is exited early by a `break` statement,
the `else` code-block is not executed.

Different from mainstream programming languages; both the `do` and `while`
code-blocks may exist at the same time, or not exist at all. The idea for an
`else` code-block on a loop comes from the Python programming language.

If the [_init_expression_] or [_condition-expression_] throws an error, result
in _empty_ or evaluate as _false_, then the matching [_alternate-clauses_]
are executed.

```
b = do {
   ...
} while (a = foo(); a < 10) {
   ...
} catch (io_error) {
   ...
   break with "error"
} elif (a > 14) {
   ...
   break with "too big"
} else {
   ...
   break with "looped"
} empty {
   ...
   break with "no result"
}
```

