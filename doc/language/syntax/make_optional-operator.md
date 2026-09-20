# make-optional-operator

## Syntax

_make-optional-operator_ := `?` [_expression_]

[_expression_]: expression.md


## Semantics

Wrap the result of the [_expression_] in a type of `std.optional[T]`, where
`T` is the type of the [_expression_]'s result.


If the [_expression_] is a function, then wrap the _value_ or _empty_ return
in a type of `std.optional[T]`, where `T` is the return type of the function.