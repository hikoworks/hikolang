# syntax-operator

## Syntax

`syntax` _associativity_ 
    __(__ [_operator_](operator.md) __|__ [_identifier_](identifier.md) __)__
    _precedence_ _function_name_ `;`

associativity := __(__ `prefix` __|__ `suffix` __|__ `ltr` __|__ `rtl` __)__

precedence := [_integer_literal_](integer_literal.md)

function_name := [_identifier_](identifier.md)


## Semantics

This statement adds an expression for [_operator_expressions_](operator_expressions.md)
operators are globably scoped and are recursively imported using the [_import_module_](import_module.md)
statement.

The combination of (prefix, suffix, binary) and the (operator, identifier) must be unique.
Redefinition of an operator is allowed if they have identical _associativity_, _precedence_ and
_function_name_.

> [!IMPORTANT]
> We may need a global registry for operators.

Although you can add operators with [_identifier_](identifier.md), like `and`, `or`, `not` & `xor`,
it is recommended to use Unicode UAX #31 Pattern Syntax characters.

When parsing an expression, an operator with a higher _precedence_ (lower value) will be bound
tighter (as if by parentheses) to its arguments than any operator that has a lower _precedence_ (higher value).

Operators that have the same _precedence_ are bound to their arguments in the direction of their _associativity_:
`ltr` is left-to-right associated, `rtl` is right-to-left. Unary `prefix` operators are right-to-left associated,
`suffix` operators are left-to-right associated.

The _function_name_ is the function to call with the arguments of the operator. The search for the function
is done as-if a normal function of that name is called:
 1. In the local scope,
 2. In the scope of the current module,
 3. Argument depended lookup,
 4. Global scope.
