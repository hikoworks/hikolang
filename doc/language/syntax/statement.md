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


## Semantics
Most statements are expressions, an expression will always result in a value.
Only a few control statements don't result in a value and therefor can not
be part of an expression.
