# conversion-expression

## Syntax

_conversion-expression_ :=\
      [_expression_] [_type-operator_] __(__ `&` __|__ `&&` __|__ `*` __)__\
    __|__ [_expression_] [_type-operator_] [_type-expression_]\

[_expression_]: expression.md
[_type-expression_]: type_expression.md
[_type-operator_]: type_operator.md

## Semantics

For the [_type-operator_] being `:~+`, `:~!` & `:~?` the result of
the [_expression_] is converted to the [_type-expression_].

For the [_type-operator_] being `:+`, `:!`, `:?` & `:` the result is checked
if it can be converted to the [_type-expression_], but it retains the type
of the [_expression_].

