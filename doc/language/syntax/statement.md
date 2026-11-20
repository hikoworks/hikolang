# Statement

[_return-statement_]: return_statement.md
[_yield-statement_]: yield_statement.md
[_break-statement_]: break_statement.md
[_continue-statement_]: continue_statement.md
[_goto-statement_]: goto_statement.md
[_namespace-statement_]: namespace_statement.md
[_throw-statement_]: throw_statement.md


## Syntax

_statement_ :=\
      [_expression_](expression.md) `;`\
    __|__ [_return-statement_]\
    __|__ [_yield-statement_]\
    __|__ [_break-statement_] \
    __|__ [_continue-statement_]\
    __|__ [_goto-statement_]\
    __|__ [_namespace-statement_]\
    __|__ [_throw-statement_]


## Semantics

Most statements are expressions, an expression will always result in a value.
Only a few control statements don't result in a value and therefor can not
be part of an expression.
