# variable-declaration

## Syntax

[_storage_class_](storage_class.md)__?__ [_binding_mode_](binding_mode.md) [_name_](name.md) _type_declaration_**?** ( `=` | `+=` ) [_expression_](expression.md)

## Semantics
Declares a variable with a [_name_](name.md) which can be used in the scope or
child scopes where the variable is declared; and initializes it.

Variable declarations may appear in the following contexts:
 - In global scope.
 - In a function or lambda.
 - In a code block of a control-expression
 - In a class declaration.
 - In an expression.

A _variable_initializer_ is a [_expression_](expression.md) which results in a
reference to declared variable.

The type and coercion-method of the variable can be set by using the
[_coerce_](coerce.md) operator on the _variable_declaration_ or on the
[_name_](name.md).

If the type is not explicitly set, the exact type of the first assigned value is
used as the type of the variable. The coercion-method is `:=`, which means that
any subsequent assignment to the variable must be of exactly the same type.

The assignment operator can be either:
 - `=` which assigns a value to the newly created variable, or
 - `+=` which assigns a value to a newly created variable or calls the `+=` operator
        an a existing variable of the same name and type.
