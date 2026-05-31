# syntax-domain

## Syntax

`syntax` `domain` [_identifier_](identifier.md) `(` _unit-list_ `)`

_unit-list_ := [_identifier_](identifier.md) __(__ `,` [_identifier_](identifier.md) __)*__


## Semantic

A structured domain introduces a system of units, for example the SI-units is
declared as:

```
domain SI (s, m, kg, A, K, mol, cd)
```

The domain `SI` and the units `s`, `m`, `kg`, `A`, `K`, `mol`, `cd` are added to
a global namespace that is valid inside a
[_unit-expression_](unit_expression.md). The list of units form the
dimensional-exponents-tuple used in dimensional analysis.


