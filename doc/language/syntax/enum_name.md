# enum-name

## Syntax

[_identifier_](identifier.md) __(__ `=` [_expression_](expression.md) __)?__

## Semantics

Each name declared in a enum type must be unique.

From the point of view of the enum type a name does not have a value, it is just a name.
The value that is assigned to a name can be extracted at compile time using
`__get_enum_value__()` function.

