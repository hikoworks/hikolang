# Argument Declaration

## Syntax

[_value_type_](value_type.md)__?__ [_name_](name.md) _type_declaration_**?** _default_value_declaration_**?**

### default-value-declaration

`=` [_expression_](expression.md)


## Semantics
Declares an argument for a function or lambda with a [_name_](name.md)
which can be used as a variable inside the function's or lambda's code-block.

The following [_value_type_](value_type.md)s are allowed:
 - `let` : the value is immutable (default; if not specified).
 - `var` : the value was copied and is mutable.
 - `ref` : the value was passed by reference and is mutable.

The type declaration is optional. If not specified, the type is inferred from
the types passed in the [_function-call_](function_call.md). If the type is
specified, the type-expression must be elaborated to a concrete type at
compile-time.

The _default-value-declaration_ is optional. If specified the argument is
optional in the [_function-call_](function_call.md). The
[_expression_](expression.md) in the _default-value-declaration_ is evaluated in
the scope of the [_function-call_](function_call.md), including access to
variables at the call-site.
