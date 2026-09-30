# effect-list

## Syntax

_effect-list_ := _effect_ __(__ `,` _effect_ __)*__

_effect := __(__ `+` __|__ `-`  __|__ `!` __|__ `=` __)__ [_identifier_]

[_identifier_]: identifier.md

## Semantics

These are the prefix operators for effects in the list:

 op     | description
 :----- |:----------------------------------------------------
 `+foo` | Add the effect `foo` to this block.
 `-foo` | Remove the effect `foo` from this block.
 `=foo` | Assert that this block has effect `foo`.
 `!foo` | Assert that this block DOES NOT have effect `foo`.

These are the default effects in the language. You can add more using the
[_syntax-effect_].

See [_enum-definition_](enum_definition.md) how new effects can be added
to the `std.effect` enum.



