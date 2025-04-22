# Expression

## Syntax

[_sub-expression_](sub_expression.md) __|__
[_literal_](literal.md) __|__
[_name_](name.md) __|__
[_variable-declaration_](variable_declaration.md) __|__
[_operator-expression_](operator_expression.md) __|__
[_if_](if.md) __|__
[_while_](while.md) __|__
[_for_](for.md) __|__
[_try_](try.md) __|__
[_switch_](switch.md) __|__
[_lambda_](lambda.md)

## Semantics
An _expression_ is a sequence of symbols that can be evaluated to produce a
value. When the result of the expression is not used, the restriction on the
type of the result is relaxed.

For parsing expressions an algorithm like [Shunting
Yard](https://en.wikipedia.org/wiki/Shunting-yard_algorithm) is used. All the
operators their precedence and associativity are documented in
[_operator-expression_](operator_expression.md). The other expressions listed in
this document are terminal expressions which are parsed outside of the shunting
yard.

