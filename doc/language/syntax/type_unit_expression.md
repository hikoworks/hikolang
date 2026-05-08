# type-unit-expression

## Syntax

_type-unit-expression_ :=\
     `(` _unit-expression_ `)`\
    __|__ [_identifier_](identifier.md)\
    __|__ [_identifier_](identifier.md) `**` [_integer_literal_](integer_literal.md)
    
_unit-expression_ :=\
      `(` _unit-expression_ `)`\
    __|__ [_identifier_](identifier.md)\
    __|__ _unit-expression_ `**` [_integer_literal_](integer_literal.md)\
    __|__ _unit-expression_ `/` _unit-expression_\
    __|__ _unit-expression_ `*` _unit-expression_ 

## Semantic
This expression follows a literal or type-coercion
