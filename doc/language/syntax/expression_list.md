# expression-list

## Syntax

[_expression_](expression.md) __(__ `;` [_expression_](expression.md) __)*__

> [!NOTE]
> For a discussion on the use of semicolons in expression lists see
> [tokenizer; Semi-colon and comma handling](../tokenizer.md#semi-colon-and-comma-handling). 

## Semantics
An _expression-list_ is a list of [_expression_](expression.md)s that are
evaluated in order from left to right. The value of the _expression-list_ is the
value of the last evaluated [_expression_](expression.md).


