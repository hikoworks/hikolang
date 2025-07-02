# positional-variable

## Syntax
`$` [_integer-literal_](integer_literal.md)

## Semantics
Inside a function's body, a positional variable is used to refer to the argument at the
specified position. The first positional variable is `$1`, the second is `$2`, and so on.

`$0` refers to the return value of the function, this can be used to query the type
of the return value, when the compiler is able to infer the type of the return value.
