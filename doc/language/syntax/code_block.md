# code-block

[_annotation_]: annotation.md
[_effect-list_]: effect_list.md
[_statement-list_]: statement_list.md
[_enum-definition_]: enum_definition.md


## Syntax

_code-block_ := `{` [_statement-list_] `}`

## Semantics

## Annotations

### @effect([_effect-list_])

Add, remove or assert the existance or non-existance of effects from this code-block.

 op     | description
 :----- |:----------------------------------------------------
 `+foo` | Add the effect `foo` to this block.
 `-foo` | Remove the effect `foo` from this block.
 `=foo` | Assert that this block has effect `foo`.
 `!foo` | Assert that this block DOES NOT have effect `foo`.

Functions that call a function that has an effect, inherits that effect.
An effect like this can be removed using the `-` [_identifier_] syntax.

The default set of effects can be found here [_enum-definition_]. You
can also modify the `.std.effect` enum to add more effects.


### code-block: [[fast_path]]

Used on a block of a flow-control expression, indicating the path to optimize.
This will:
 * Order code to reduce latency for this fast-path.
 * Prefer calling, instead of inlining, on the slow-path.


### code-block: [[slow_path]]

Used on a block of a flow-control expression, indicating the path to not
optimize. This is the opposite of `@fast_path`.

