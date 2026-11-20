# argument-list

[_id_]: id.md
[_expression_]: expression.md

## Syntax

_argument-list := _argument_ __(__ `,` _argument_ __)*__

_argument_ :=\
      [_expression_]\
    __|__ [_id_] `=` [_expression_]

## Semantics


There are three forms of argument:
 - A positional argument.
 - A named argument.
 - A injected argument, which is an [_id_] that starts with `$` is used to
   explicitly inject a hidden context argument into the function recursively.


