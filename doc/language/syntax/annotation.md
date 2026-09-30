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
[_function-definition_]: function_definition.md
[_variable-definition_]: variable_definition.md


## Semantics

Attributes apear in front of different syntactical constructs:

 * [_function-definition_]
 * [_expression_]
 * [_code-block_]
 * [_variable-definition_]



