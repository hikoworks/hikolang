# catch-clauses

## Syntax

_catch-clauses_ :=\
    __(__ `catch` `(` [_error-list_] `)` `{` [_statement-list_ ]`}` __)*__\
    __(__ `catch` `{` [_statement-list_] `}` __)?__

[_error-list_]: error_list.md
[_statement-list]: statement_list.md

## Semantics

A catch-clause handles errors produced while evaluating the expression to
which the catch-clause is attached. Catch-clauses are evaluated in the order
in which they appear.

A catch-clause with a parenthesized catch-expression handles an error if
that catch-expression matches the error. A catch-clause without a
catch-expression handles any error that has not been handled by a
preceding catch-clause.

When an error is produced, the catch-expressions are considered in order.
The first matching catch-clause is selected and its statement-list is
executed. Once a catch-clause has been selected, no subsequent catch-clauses
are evaluated.

If no explicit catch-clause matches the error, then for an auto_rethrow
error the compiler automatically synthesises a catch-clause that rethrows
the error. Thus, every error must be caught by the caller, either explicitly
or by a compiler-synthesised catch-rethrow.

Errors produced while executing the statement-list of a catch-clause are
not handled by that same catch-clause. They are instead handled by
catch-clauses in the enclosing context.

A catch-clause may rethrow the error it is handling using a bare throw.
It may also throw a different error explicitly. For example:

```
if (a = foo()) {
    success()
} catch(out_of_bounds) {
    throw my_error
} catch {
    throw
}
```

If foo() throws out_of_bounds, the first catch-clause is selected. If it
throws any other error, the second catch-clause is selected and rethrows the
original error.

A catch-clause does not cause errors to be implicitly propagated or
transformed. An error is handled only when a matching catch-clause is
explicitly present.

