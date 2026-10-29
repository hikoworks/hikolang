# name-member

[_type-expression_]: type_expression.md
[_integer-literal_]: integer_literal.md
[_identifier_]: identifier.md

## Syntax

_name-member_ := [_identifier_] _index_**?** _associated-type_**?**

_index_ := `@` [_integer-literal_]

_associated-type_ := `=` [_type-expression_]


## Semantic

The _name-member_ syntax is used to introduce an enum-like name in a type
definition. The syntax overlaps somewhat with a const-constrained variable
definition. With the differences that it may have an _index_, or may be just
an [_identifier_].

