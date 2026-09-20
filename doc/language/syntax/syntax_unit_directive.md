# syntax-unit-directive

## Syntax

_syntax-unit-directive_ :=
    `syntax` `unit` [_identifier_] `=` [_float-literal_]__?__ [_complex-unit-expression_] `;`

[_float-literal_]: float_literal.md
[_identifier_]: identifier.md
[_complex-unit-expression_]: unit_expression.md
[_syntax-domain-directive_]: syntax-domain_directive.md

## Semantic

Add a new [_identifier_] for a unit based on a [_complex-unit-expression_].

```
syntax unit g = 0.001 kg
syntax unit mm = 0.001 m
syntax unit inch = 245 mm
syntax unit Hz = s⁻¹
syntax unit N = kg m s⁻²
```

The base units are specified with the [_syntax-domain-directive_]

