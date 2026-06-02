# syntax-unit

## Syntax

_syntax-unit_ := `syntax` `unit` [_identifier_] `=` [_float-literal_]__?__ [_unit-expression_]

[_float-literal_]: float_literal.md
[_identifier_]: identifier.md
[_unit-expression_]: unit_expression.md


## Semantic

Add a new [_identifier_] for a unit based on a [_unit_expression_].

```
syntax unit g = 0.001 kg
syntax unit mm = 0.001 m
syntax unit inch = 245 mm
syntax unit Hz = s⁻¹
syntax unit N = kg m s⁻²
```


