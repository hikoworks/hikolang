# variant-definition

## Syntax

[_identifier_] __(__ `(` [_type-list_] `)` __)?__ __(__ `=` [_expression_] __)?__




[_identifier_]: identifier.md
[_type-list_]: type_list.md
[_expression_]: expression.md


## Semantics

Each variant-definition in a type must be unique.

From the point of view of the enum type a name does not have a value, it is just
a name. The value that is assigned to a name can be extracted at compile time
using `__get_enum_value__()` function.

