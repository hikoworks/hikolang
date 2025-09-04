# literal

## Syntax

_base_literal_ [_unit_expression_](unit_expression.md)__?__ [_tag_](tag.md)__*__

### base_literal
[_integer-literal_](integer_literal.md)  __|__\
[_float-literal_](float_literal.md) __|__\
[_string-literal_](string_literal.md) __|__\
[_boolean-literal_](boolean_literal.md) __|__\
[_null-literal_](null_literal.md) __|__\
[_character-literal_](character_literal.md) __|__\
[_version-literal_](version_literal.md) __|__\
[_tuple-literal_](tuple_literal.md) __|__\
[_array-literal_](array_literal.md)

## Semantics
A literal is a constant value that is directly represented in the source code.
The literal can be an integer, float, string, boolean, null, character or version.

The optional [_name_](name.md) is used for custom literals. The
[_name_](name.md) is used to identify a function that is called to convert the
literal (in string format) to the actual value.
