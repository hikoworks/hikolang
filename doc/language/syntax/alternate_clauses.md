# alternate-clauses

## Syntax

_alternate-clauses_ := _alternate-clauses-1_ __|__ _alternate-clauses-2_

_alternate-clauses-1_ :=\
    [_catch-clauses_]
    __(__ `elif` `(` [_condition-expression_] `)` `{` [_statement-list_ ]`}` [_catch-clauses_] __)*__\
    __(__ `else` `{` [_statement-list_] `}` __)?__\
    __(__ `empty` `{` [_statement-list_] `}` __)?__

_alternate-clauses-2_ :=\
    __(__ `else` `{` [_statement-list_] `}` __)?__\
    __(__ `empty` `{` [_statement-list_] `}` __)?__
    [_catch-clauses_]

[_condition-expression_]: condition_expression.md
[_catch-expression_]: catch_expression.md
[_statement-list]: statement_list.md

## Semantics

Bindings introduced inside [_condition-expression_]s, of `if`, `while`, `for` and `elif` clauses,
by successful unpacking are available to all clauses whose execution is downstream of that successful evaluation.


### catch-clauses

The [_catch-clauses_] are executed when the previous [_condition-expression_] in the
`if`, `elif`, `while` or `for` statements caused an error or fatal-error.

If there is no `elif` clause then the [_catch-clauses_] may apear after `else` and `empty` clauses.


### elif-clause

If the result of a `if`, `while`, `for` statement or previous `elif` clause result
is `false` the next `elif` clause is executed.


### else-clause

If the result of a `if`, `while`, `for` statement or previous `elif` clause result
is `false` the `else` clause is executed.


### empty-clause

The `empty` clause is executed when the [_condition-expression_] in the
`if`, `while` or `for` statement caused an empty value to:
 - initialize a new variable or immutable, that does not explicitly take an empty value
 - be assigned to a variable that can not take an empty value
 - used in a comparison that can not handle an empty value


