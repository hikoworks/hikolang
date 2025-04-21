# sub-expression

## Syntax

`(` [_expression-list_](expression_list.md) `)`

## Semantics
A sub-expression is a group of expressions that are evaluated in order from
left to right. The value of the sub-expression is the value of the last
evalutated expression.

## Automatic semicolon insertion
The tokenizer will automatically:
 - after control-expressions,
 - remove duplicate semicolons,
 - remove semicolons after the last expression in the sub-expression.

See the documentation on the [tokenizer](../tokenizer.md) for more details.

