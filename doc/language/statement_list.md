# statement-list

## Syntax

_statement_ __(__ `;` _statement_ __)*__

## Automatic semicolon insertion
The tokenizer will automatically:
 - insert semicolons at the end of a line,
 - insert semicolons after control-expressions,
 - remove duplicate semicolons,
 - remove semicolons after the last statement.

See the documentation on the [tokenizer](../tokenizer.md) for more details.

