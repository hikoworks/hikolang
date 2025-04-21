# While

## Syntax

__(__ `do` `{` [_statement-list_](statement_list.md) `}` __)?__\
`while` `(` _condition-expression_ `)` __(__ `{` [_statement-list_](statement_list.md) `}` __)?__ [_catch-clauses_](catch_clauses.md)**?**\
__(__ `else` `{` [_statement-list_](statement_list.md) `}` __)?__

### condition-expression
See the [if; condition expression](if.md#condition-expression) section for details.

## Semantics
The `while` control-expression is a loop that executes the body of the
loop as long as the _condition-expression_ is true.

A `while` control-expression does the following steps:
 1. The `do` code-block is executed first.
 2. The `while` _condition-expression_ is evaluated.
 3. If the _condition-expression_ is `true`:
    4. The `while` code-block is executed.
    2. goto 1.
 4. If the _condition-expression_ is `false`:
    5. The `else` code-block is executed.
    6. The loop is exited.

When the `do` or `while` code-blocks is exited early by a `break` statement,
the `else` code-block is not executed.

Different from mainstream programming languages; both the `do` and `while`
code-blocks may exist at the same time, or not exist at all. The idea for an
`else` code-block on a loop comes from the Python programming language.

