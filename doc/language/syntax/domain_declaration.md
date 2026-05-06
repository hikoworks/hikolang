# domain-declaration

## Syntax

`domain` [_identifier_](identifier.md) `(` _unit-list_ `)`

`domain` [_identifier_](identifier.md)


_unit-list_ := [_identifier_](identifier.md) __(__ `,` [_identifier_](identifier.md) __)*__

## Semantic

### Structured domain

A structured domain introduces a system of units, for example the SI-units is
declared as:

```
domain SI (s, m, kg, A, K, mol, cd)
```

The domain `SI` and the units `s`, `m`, `kg`, `A`, `K`, `mol`, `cd` are added to
a global namespace that is valid inside a
[_unit-expression_](unit_expression.md). The list of units form the
dimensional-exponents-tuple used in dimensional analysis.

It is possible to have cross-domain dimensions, such as `(pixels/inch)` which
converts between `inch` in the `SI` domain to `pixels` in the `screen` domain.


### Symbol domain

A symbol `domain` introduces a type in the global namespace. The type is a set
of possible names. When adding a name to a domain a 32-bit unique numeric value
is automatically assigned.

Some of the symbol domains are directly managed by the language itself:
 - Atoms for `error_code` are introduced with the `throw` statement.
 - Atoms for `type_tag` are introduced when using a tag in a type expression.

A `domain` is very similar to a `enum` where none of the elements have a
payload. The difference is that new names may be added to a `domain` after it
has been frozen. This is because a `domain` type is immediately completed,
objects of a `domain` are always 32 bit values.

This is very useful for `error_code` so that new names can be added anywhere in
the program, even if earlier on in the program a function throwing an
`error_code` has been called at compile time.

Names added to a `domain` never have the value zero assigned. A `domain`
has zero as the niche value.

The compiler has the choice on which numeric values are associated with a name.
Non-deterministic compilation order may change the order of names being added.
The compiler may use special optimization techniques when assigning values to
names.
