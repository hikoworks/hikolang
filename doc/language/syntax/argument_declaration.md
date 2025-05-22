# argument-declaration

## Syntax

[_binding_mode_](binding_mode.md)__?__ [_name_](name.md) [_type_declaration_](type_declaration.md)__?__ _default_value_declaration_**?** __|__\
[_binding_mode_](binding_mode.md)__?__ [_name_](name.md)`...` [_type_declaration_](type_declaration.md)__?__


### default-value-declaration

`=` [_expression_](expression.md)


## Semantics
Declares an argument for a function or lambda with a [_name_](name.md)
which can be used as a variable inside the function's or lambda's code-block.

The type declaration is optional. If not specified, the type is inferred from
the types passed in the [_function-call_](function_call.md). If the type is
specified, the type-expression must be elaborated to a concrete type at
compile-time.

The _default-value-declaration_ is optional. If specified the argument is
optional in the [_function-call_](function_call.md). In the call every
argument with a default value can be passed by name or by position.

When the function is called and the argument with a default value is not passed;
then the default value is evaluated on each call in the same scope as the function call.

