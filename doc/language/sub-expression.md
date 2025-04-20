# sub-expression

## Syntax

`(` [_expression_](expression.md) __(__ `;` [_expression_](expression.md) __)*__ `)`

## Semantics
A sub-expression is a group of expressions that are evaluated in order from
left to right. The last expression is evaluated and the result is returned. The
other expressions are evaluated for their side-effects only.

## Automatic semicolon insertion
The tokenizer will automatically:
 - after control-expressions,
 - remove duplicate semicolons,
 - and remove semicolons after the last statement in a block.

See the documentation on the [tokenizer](../tokenizer.md) for more details.

