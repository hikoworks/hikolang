# effect-list

## Syntax

_effect-list_ := _effect_ __(__ `,` _effect_ __)*__

_effect := __(__ `+` __|__ `-`  __|__ `!` __|__ `=` __)__ [_identifier_]

[_identifier_]: identifier.md

## Semantics

These are the prefix operators for effects in the list:

   Op | Description
 :--- |:---------------
  `+` | Add an effect to a block.
  `-` | Remove an effect from the block.
  `=` | Require that all contained blocks and calls have this effect.
  `!` | Require that none of the contained blocks and calls have this effect.

These are the default effects in the language. You can add more using the
[_syntax-effect_].

See [_enum-definition_](enum_definition.md) how new effects can be added
to the `std.effect` enum.



