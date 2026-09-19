# type-specifier

## Syntax

_type-specifier_ := [_type-operator_] [_type-expression_]

[_type-operator]: type_operator.md
[_type-expression_]: type_expression.md

## Semantics

For the [_type-operator_] being `:~+`, `:~!` & `:~?` the value is converted to
the [_type-expression_].

For the [_type-operator_] being `:+`, `:!`, `:?` & `:` the value is checked
if it can be converted to the [_type-expression_], but it retains the type
of the passed in value.

