# coerce operators

## Syntax

[_expression_](expression.md) `:*` [_type-expression_](type_expression.md) __|__\
[_expression_](expression.md) `:=` [_type-expression_](type_expression.md) __|__\
[_expression_](expression.md) `:^` [_type-expression_](type_expression.md) __|__\
[_expression_](expression.md) `:!` [_type-expression_](type_expression.md)

## Semantics
The _coerce_operator_ is used to cast-to, convert or declare a type for a value
or variable.

If the left hand side is a variable declaration, then the type expression and
coercion-method is bound to the variable. If the bound type expression is a type
template, the first assignment will be used to fill in the parameters of this
type template to create a concrete type. The concrete type replaces the bound
type expression.

If the left hand side is an expression, the type expression is used to cast,
convert or check the value of the expression to the type of the
[_type-expression_](type_expression.md).

The type is generally used to select the correct overload of a function or
operator in the expression. After this the rules for each coerce-method are
applied.

### General coerce operator `:*`
The general coerce operator `:*` is used convert a value to a specific type.

On every assignment to the variable (including the first) the
[_type-expression_](type_expression.md) bound to the variable is used to convert
the value of the [_expression_](expression.md) or [_assignment_](assignment.md).

When converting a value to a type:
 - The value is up/down casted.
 - The value is converted by calling the constructor of the given type.
 - The value is converted by calling the `__convert__` function.
 - It is a **static error** if the value could not be converted.

### Exact match coerce operator `:=`
The exact match coerce operator `:=` is used to specifically check the type of a
a variable or expression.

It is a **static error** if the type of the value of the
[_expression_](expression.md) or [_assignment_](assignment.md) does not match
the [_type-expression_](type_expression.md) exactly.

### Upcast coerce operator `:^`
The upcast coerce operator `:^` is used to cast a value to a type that is a
super-class of the type of the value.

It is a **static error** if the type of the value in the
[_expression_](expression.md) or [_assignment_](assignment.md) is not the same
or a sub-class of _type-expression_.

### Downcast coerce operator `:!`
The downcast coerce operator `:!` is used to cast a value to a type that is a
sub-class of the type of the value.

It is a **static error** if the type of the value in the
[_expression_](expression.md) or [_assignment_](assignment.md) is not in the
same type-hierarchy as the [_type-expression_](type_expression.md).

This operator may also be used to upcast a value to a type that is a super-class
of the type of the value.

Since a downcast may fail at runtime the operator may throw the `bad_cast`
error.
