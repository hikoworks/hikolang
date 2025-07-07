# argument-declaration-list

## Syntax

[_argument-declaration_](argument_declaration.md) __(__ `,` [_argument-declaration_](argument_declaration.md) __)*__

## Semantics
A list of argument declarations, used in function and lambda declarations.

Inside a function or lambda, the argument declarations are available as
variables by their [_name_](name.md), and positionally as `$0`, `$1`, etc.

