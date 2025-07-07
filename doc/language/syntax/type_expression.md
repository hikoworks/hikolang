# type-expression

## Syntax

[_expression_](expression.md)

A _type_expression_ is a [_expression_](expression.md) which is evaluted at
compile time to a:
 - type
 - type-template
 - interface
 - interface-template

## Semantics
Types in this language are first class values and are manipulated like any other
value. However in certain context a type has to be used as a type, these are
marked as a _type_expression_.

Types need to be evaluated at compile time for it to be used as a type.
