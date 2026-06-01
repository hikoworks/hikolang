# with-effect

## Syntax

_with_effect_ := `with_effect` `(` _with_effect_item_ __(__ `,` _with_effect_item_ __)*__ `)` `{` [_block_] `}`

_with_effect_item_ := `-`__?__ [_fqname_]

[_block_]: block.md
[_fqname_]: fqname.md

## Semantics

The block is treated to have the given effects. These effects travel up the
call tree and can be checked using the [_without_effect_] statement.

If an effect is prefixed with `-` then the effect is explicitly removed from
the block, for example if the programmer has proven that the effect does not
actually happen.

The [_fqname_] names that may be used are added using the [_syntax_effect_]
statement.

[_without_effect_]: without_effect.md
[_syntax_effect_]: syntax_effect.md
