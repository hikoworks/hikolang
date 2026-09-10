# alternate-clauses

## Syntax

_alternate-clauses_ := _alternate-clauses-1_ __|__ _alternate-clauses-2_

_alternate-clauses-1_ :=\
    [_catch-clauses_]\
    __(__ `elif` `(` [_condition-expression_] `)` `{` [_statement-list_ ]`}` [_catch-clauses_] __)*__\
    __(__ `else` `{` [_statement-list_] `}` __)?__\
    __(__ `empty` `{` [_statement-list_] `}` __)?__

_alternate-clauses-2_ :=\
    __(__ `else` `{` [_statement-list_] `}` __)?__\
    __(__ `empty` `{` [_statement-list_] `}` __)?__\
    [_catch-clauses_]

[_condition-expression_]: condition_expression.md
[_catch-expression_]: catch_expression.md
[_statement-list]: statement_list.md


## Semantics

Bindings introduced by successful unpacking within a [_condition-expression_] of an
`if`, `while`, `for`, or `elif` clause are available to all clauses whose execution occurs
downstream of that successful evaluation.


### catch-clauses

[_catch-clauses_] handle errors and fatal errors produced while evaluating the
immediately preceding [_condition-expression_].

If `elif` clauses are present, [catch-clauses] must directly follow the [_condition-expression_]
of the preceding `if`, `for`, `while` or `elif` clause.

If no `elif` clauses are present, [_catch-clauses_] may appear after the `else`
and `empty` clauses.


### elif-clause

An `elif` clause is executed when the result of the `if`, `while`, or `for` condition,
or of the preceding `elif` condition, is false. If an `elif` clause is executed, its
condition is evaluated and its result determines which subsequent clause, if any, is executed.


### else-clause

The `else` clause is executed when the result of the `if`, `while`, or `for` condition,
or of every preceding `elif` condition, is false.


### empty-clause

The `empty` clause is executed when evaluation of the [_condition-expression_] of
an `if`, `while`, or `for` statement attempts to propagate an empty value in a
context that does not permit empty values, including when an empty value is:
 - used to initialize a variable or immutable that does not explicitly permit an empty value;
 - assigned to a variable that does not permit an empty value; or
 - used in a comparison that does not support empty values.

The `empty` clause is implemented as a `catch (empty)`, but it is assigned to
the first condition-expression of a control-flow expression and may be placed
in different locations than catch-clauses.
