# variable-declaration

## Syntax

[_storage_class_](storage_class.md)__?__ [_binding_mode_](binding_mode.md) [_name_](name.md) _type_declaration_**?** `;`

## Semantics
Declares a variable with a [_name_](name.md) which can be used in the scope or
child scopes where the variable is declared.

Variable declarations may appear in the following contexts:
 - In global scope.
 - In a function or lambda.
 - In a code block of a control-expression
 - In a class declaration.

A _variable_declaration_ is a [_statement_](statement.md) as it was not assigned a value,
there can be no result.

The type and coercion-method of the variable can be set by using the
[_coerce_](coerce.md) operator on the _variable_declaration_ or on the
[_name_](name.md).

If the type is not explicitly set, the exact type of the first assigned value is
used as the type of the variable. The coercion-method is `:=`, which means that
any subsequent assignment to the variable must be of exactly the same type.
