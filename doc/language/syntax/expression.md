# Expression

## Syntax

[_sub-expression_](sub_expression.md) __|__
[_literal_](literal.md) __|__
[_name_](name.md) __|__
[_tag_](tag.md) __|__
[_context-argument_](context_argument.md) __|__
[_positional-argument_](positional_argument.md) __|__
[_variable-declaration_](variable_declaration.md) __|__
[_operator-expression_](operator_expression.md) __|__
[_if-control-expression_](if_control_expression.md) __|__
[_while-control-expression_](while_control_expression.md) __|__
[_for-control-expression_](for_control_expression.md) __|__
[_try-control-expression_](try_control_expression.md) __|__
[_switch-control-expression_](switch_control_expression.md) __|__
[_lambda_](lambda.md) __|__
[_struct-definition_](struct_definition.md) __|__

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

