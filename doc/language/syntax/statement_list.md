# statement-list

## Syntax

__(__ [_label_](label.md)__?__ [_statement_](statement.md) __)*__

> [!NOTE]
> For a discussion on the use of semicolons in statement lists see
> [tokenizer; Semicolon and comma handling](../tokenizer.md#semicolon-and-comma-handling). 

## Semantics

Each statement may be preceded by a [_label_](label.md) to use as a target for a
[_goto_](goto.md) statement, or if the [_label_](label.md) is used on a loop
statement it can be used as a target for a [_break_](break.md) or
[_continue_](continue.md) statement.  A label may apear after the last statement.
