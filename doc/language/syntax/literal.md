# literal

## Syntax

[_integer-literal_](integer_literal.md) [_name_](name.md)__?__ [_tag_](tag.md)__?__ __|__\
[_float-literal_](float_literal.md) [_name_](name.md)__?__ [_tag_](tag.md)__?__ __|__\
[_string-literal_](string_literal.md) __|__\
[_boolean-literal_](boolean_literal.md) __|__\
[_null-literal_](null_literal.md) __|__\
[_character-literal_](character_literal.md) __|__\
[_version-literal_](version_literal.md) __|__

## Semantics
A literal is a constant value that is directly represented in the source code.
The literal can be an integer, float, string, boolean, null, or character.

version-literal is specifically added to be used with conditional-compilation,
to compare with operating system or library versions. This is done at a very
early stage of compilation, which is not able to use the standard library.
