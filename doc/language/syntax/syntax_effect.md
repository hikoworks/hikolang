# syntax-effect

## Syntax

_syntax-effect_ := `syntax` `effect` [_identifier_]

[_identifier_]: identifier.md

## Semantic

This statement will add the given [_identifier_] to the possible identifiers
that can be used in the [_with_effect_] and the `@effects()` [_attribute_].

Effects are in their own domain and do not collide with names in other domains.

```
syntax effect io
syntax effect block
syntax effect unsafe
```

[_attribute_]: attribute.md
[_with_effect_]: with_effect.md

