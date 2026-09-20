# Body

## Syntax

_body_ := __(__\
      __(__ [_label_](label.md) `:` __)?__ [_statement_]\
    __|__ [_directive_]\
__)*__

[_directive_]: directive.md
[_label_]: label.md
[_statement_]: statement.md


# Semantics

The body is acts like a [_code-block_] at the file level.

The body is parsed from top to bottom, [_directive_]s may
modify the internal state of the parser:
 - Adding new error-codes
 - Adding new expression-operators
 - Adding new units and domains
 - Adding new effects
 - Adding new meta-types
 - Changing the current namespace