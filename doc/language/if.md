# If

## Syntax

`if` `(` _condition-expression_ `)` `{` [_statement-list_](statement_list.md) `}` [_catch-clauses_](catch_clauses.md)**?**\
__(__ `else` `if` `(` _condition-expression_ `)` `{` [_statement-list_](statement_list.md) `}` [_catch-clauses_](catch_clauses.md)**?** __)*__\
__(__ `else` `{` [_statement-list_](statement_list.md) `}` __)?__

Alternatively when there are no `elif` clauses, the `catch` clauses may be
placed after the `else` clause.

`if` `(` _condition-expression_ `)` `{` [_statement-list_](statement_list.md) `}`\
`else` `{` [_statement-list_](statement_list.md) `}` [_catch-clauses_](catch_clauses.md)**?**

### condition-expression
A _condition-expression_ is an [_expression-list_](expression_list.md) which
results in a value that is convertible to a boolean value.

The last expression in the _condition-expression_ is not allowed to be an
assignment expression. This is to prevent a common bug; confusing `=` with `==`.

> [!TIP]
> The compiler will report an error if the last expression is an
> assignment. To bypass this restriction, use a
> [_sub-expression_](sub_expression.md).

## Semantics
The `if` statement is used to conditionally execute a block of code. Each
_condition-expression_ is evaluated in order, until one of them evaluates to
`true`. The block of code associated with that _condition-expression_ is then
executed. If there are no `if` or `elif` clauses that evaluate to `true`, the
`else` block is executed.

Any errors that occur in the _condition-expression_ may be caught by the
[_catch-clauses_](catch_clauses.md) that directly follow the code-block that is
associated with that _condition-expression_.

If there are no `elif` clauses, the `catch` clauses may be placed after the
`else` block. This is more intuitive, as handling an error is secondary to both
the `if` and `else` branches.

> [!CAUTION]
> Only errors occuring in the _condition-expression_ are caught by
> the [_catch-clauses_](catch_clauses.md). Errors occuring in the code-blocks
> must be handled separately.
