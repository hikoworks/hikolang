# type-declaration

## Syntax

__(__ `:` __|__ `:=` __|__ `:<`  __)__ [_expression_](expression.md)

## Semantics
A type-declaration is used to declare the type of a variable or function
argument.

The expression must be evaluated at compile time to:
 - A concrete type,
 - A type template, or
 - An interface.

The type-declaration operators are:
 - `:` : The argument will accept any value that matches the interface, type,
         subtype, or can be converted to the type.
 - `:=` : The argument will only accept values that match the type exactly.
          The type cannot be a interface.
 - `:<` : The argument will only accept values that are subtypes of the type.
          The type cannot be a interface.
