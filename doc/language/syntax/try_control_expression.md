# try

## Syntax

`try` `{` [_statement-list_](statement_list.md) `}` [_catch_clauses_](catch_clauses.md)__?__

## Semantics
The `try` control-expression is used to introduce a code-block, with its own
scope, as an expression.

Additionally any errors that occur inside the `try` code-block may be caught by
the [_catch-clauses_](catch_clauses.md) that directly follow the code-block.

> [!CAUTION]
> Any errors that may occur inside the `try` block that are not
> caught by the [_catch-clauses_](catch_clauses.md) is a __static error__.

The `try` control-expression should be used in the following situations:
 1. When you want to introduce a new scope for variables, such as when you want
    to use the same variable name in different scopes.
 2. When you want to catch errors that occur inside the `try` code-block.
 3. When you want to use the `try` code-block as an expression, calculating a
    value from the code-block.
 4. Add additional flow control, where you can use `result`, `return`, `yield`
    `break` and `continue` statements inside the `try` code-block, to either
    return a value, or to exit a loop or a function early. 
