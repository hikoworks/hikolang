# statement-list

[_attribute_]: attribute.md
[_code-block_]: code_block.md
[_documentation_]: documentation.md
[_directive_]: directive.md
[_label_]: label.md
[_statement_]: statement.md


## Syntax

_statement-list_ := __(__\
      [_documentation_]\
    __|__ [_attribute_]\
    __|__ [_label_] `:`\
    __|__ [_statement_]\
__)*__



> [!NOTE]
> For a discussion on the use of semicolons in statement lists see
> [tokenizer; Semicolon and comma handling](../tokenizer.md#semicolon-and-comma-handling). 


## Semantics

Each statement may be preceded by a [_label_](label.md) to use as a target for a
[_goto_](goto.md) statement, or if the [_label_](label.md) is used on a loop
statement it can be used as a target for a [_break_](break.md) or
[_continue_](continue.md) statement. A label may apear after the last statement.

[_documentation_] can appear once in front of statement to document it, it is
a reportable error if the documentation does not document a statement.

[_attribute_]s modify a statement, they are gathered together and attached to
the statement following them.
