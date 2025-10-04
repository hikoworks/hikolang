# syntax-unit

## Syntax

`syntax` `space` _space_name_ `(` _unit_name_ __(__ `,` _unit_name_ __)*__  `)` `;`

`syntax` `unit` _unit_name_ `=` 

space_name := [_identifier_](identifier.md)

unit_name := [_identifier_](identifier.md)

## Semantics

## Examples

The standard `space` for "SI International System of Units"

```
syntax space si (s, m, kg, A, K, mol, cd);
syntax unit km = 1000 * m
syntax unit m2 = m^2
syntax unit Hz = 1/s
```