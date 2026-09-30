# annotation

## Syntax

annotation :=\
      [_documentation_]\
    __|__ _attribute_\
    __|__ _modifier_


_attribute_ := `[[` [_fqname_] __(__ `(` [_argument-list_] `)` __)?__ `]]`

_modifier_ := `@` [_fqname_] __(__ `(` [_argument-list_] `)` __)?__

[_argument-list_]: argument_list.md
[_fqname_]: fqname.md
[_code-block_]: code_block.md
[_documentation_]: documentation.md
[_expression_]: expression.md


## Semantics

Attributes appear in front of different syntactical constructs:
 * [_code-block_]s
 * [_expression_]s


