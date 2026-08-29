# catch-clauses

## Syntax

_catch-clauses_ :=\
    __(__ `catch` `(` [_catch-expression_] `)` `{` [_statement-list_ ]`}` __)*__\
    __(__ `catch` `{` [_statement-list_] `}` __)?__\
    __(__ `catch` `fatal` `(` [_catch-expression_]`)` `{` [_statement-list_] `}` __)*__\
    __(__ `catch` `fatal` `{` [_statement-list_] `}` __)?__

[_catch-expression_]: catch_expression.md
[_statement-list]: statement_list.md

## Semantics
