# without-effect

## Syntax

_without-effect_ := `without_effect` `(` [_fqname_] __(__ `,` [_fqname_] __)*__ `)` `{` [_block_] `}`

[_block_]: block.md
[_fqname_]: _fqname.md

## Semantics

This statement checks if any statement in the block has one of the effects
listed, if it does the compiler will display an error.

The effects that may be listed have been added using the [_syntax_effect_]
statements.

[_syntax_effect_]: syntax_effect.md
