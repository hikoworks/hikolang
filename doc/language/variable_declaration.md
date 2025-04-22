# variable-declaration

## Syntax

[_storage_modifier_](storage_modifier.md) [_name_](name.md) _type_declaration_**?** __(__ `=` [_expression_](expression.md) __)?__

## Semantics
Declares a variable with a [_name_](name.md) which can be used in the scope or
child scopes where the variable is declared.

Variable declarations may appear in the following contexts:
 - In global scope.
 - In a function or lambda.
 - In a code block of a control-expression
 - In a class declaration.

A _variable_declaration_ is a [_expression_](expression.md) which results in a reference
to declared variable.
