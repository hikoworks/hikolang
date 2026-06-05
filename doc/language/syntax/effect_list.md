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

  Effect    | Description
 :--------  |:--------------
  io        | May performs I/O.
  block     | May cause progress to stop on this thread.
  allocate  | May allocate and free memory.
  unsafe    | May cause unexpected behavior.
  terminate | May cause the end of the thread or program.


[_syntax-effect_]: syntax_effect.md
