# syntax-effect

## Syntax

`syntax` `effect` [_fqname_]

[_fqname_]: fqname.md

## Semantic

This statement will add the given [_fqname_] to the possible identifiers
that can be used in the [_with_effect_] and [_without_effect_] statements.

If [_fqname_] is relative, it is relative to the current open namespace. Effects
are in their own domain and do not collide with names in other domains.

```
syntax effect io
syntax effect block
syntax effect unsafe
```

[_with_effect_]: with_effect.md
[_without_effect_]: without_effect.md

