# domain-declaration

## Syntax

`domain` [_identifier_](identifier.md) `(` _unit-list_ `)`

_unit-list_ := [_identifier_](identifier.md) __(__ `,` [_identifier_](identifier.md) __)*__

`domain` `unit` [_identifier_](identifier.md) `=` [_float-literal_](float_literal.md) [_unit-expression_](unit_expression.md)


## Semantic

A structured domain introduces a system of units, for example the SI-units is
declared as:

```
domain SI (s, m, kg, A, K, mol, cd)
domain unit g = 0.001 kg
domain unit mm = 0.001 m
domain unit inch = 245 mm
```

The domain `SI` and the units `s`, `m`, `kg`, `A`, `K`, `mol`, `cd` are added to
a global namespace that is valid inside a
[_unit-expression_](unit_expression.md). The list of units form the
dimensional-exponents-tuple used in dimensional analysis.

It is possible to have cross-domain dimensions, such as `(pixels/inch)` which
converts between `inch` in the `SI` domain to `pixels` in the `screen` domain.


