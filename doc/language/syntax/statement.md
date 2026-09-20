# Statement

## Syntax

_statement_ :=\
      [_expression_](expression.md) `;`\
    __|__ [_return_statement_]\
    __|__ [_yield_statement_]\
    __|__ [_break_statement_] \
    __|__ [_continue_statement_]\
    __|__ [_goto_statement_]\
    __|__ [_throw_statement_]

[_return_statement_]: return_statement.md
[_yield_statement_]: yield_statement.md
[_break_statement_]: break_statement.md
[_continue_statement_]: continue_statement.md
[_goto_statement_]: goto_statement.md
[_throw_statement_]: throw_statement.md


## Semantics

Most statements are expressions, an expression will always result in a value.
Only a few control statements don't result in a value and therefor can not
be part of an expression.
