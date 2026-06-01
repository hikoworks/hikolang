# unit-expression

## Syntax

_simple-unit-expression_ :=\
     `(` _unit-expression_ `)`\
    __|__ [_identifier_](identifier.md)\
    __|__ _unit-expression_ `^` [_integer_literal_](integer_literal.md)\
    __|__ _unit-expression_ `**` [_integer_literal_](integer_literal.md)

_unit-expression_ :=\
      `(` _unit-expression_ `)`\
    __|__ [_identifier_](identifier.md)\
    __|__ _unit-expression_ `^` [_integer_literal_](integer_literal.md)\
    __|__ _unit-expression_ `**` [_integer_literal_](integer_literal.md)\
    __|__ _unit-expression_ `/` _unit-expression_\
    __|__ _unit-expression_ `*` _unit-expression_\
    __|__ _unit-expression_ _unit-expression_


## Semantic
A _simple-unit-expression_ follows:
 - a literal: Adds the _unit-expression_ to a type.
 - a type coercion: Checks using domain analysis if the source type has a
   compatible _unit-expression_.

In the example below you see how _unit-expression_ can be used together
with literals; and add `*` and remove `/` units from types. The actual values
are scaled to the base units of the domain they belong to.

```
let speed = 10.0 (km/h)                     // f64 m/s
let duration_in_hours = 2.0                 // f64
let duration = duration_in_hours * 1h       // f64 s
let distance = speed * duration             // f64 m
let distance_in_km = distance / 1.0 km      // f64
```

In the next example we show how _unit-expression_ can be used on a type to
match the unit of arguments passed to a function. The actual units used
on the type coercions are unimportant as the _unit-expression_ is reduced
to the domain's base units:

```
fn distance(speed : f64 (m/s), duration : f64 s) {
    return speed * duration
}

var distance_in_miles = distance(20.0 km/h, 5 h) / 1.0 mi
```

The [_identifier_](identifier.md)s in a _unit-expression_ are from the namespace
maintained by the [_domain-declaration_](domain_declaration.md)s. The expression
is reduced to a list of domain-exponent-tuples.

Here are some examples how a _unit-expression_ is converted into
domain-exponent-tuples and the internal scalar value.

```
domain SI (s, m, kg, A, K, mol, cd)
domain screen (px)
domain unit g = 0.001 kg
domain unit mm = 0.001 m
domain unit km = 1000.0 m
domain unit inch = 24.5 mm
domain unit h = 3600 s

var distance = 1.0 mm             // 0.001 SI(0,1,0,0,0,0,0)
var speed = 55 km/h               // 15.2778 SI(-1,1,0,0,0,0,0)
var screen_density = 72px/inch    // 2834.65 screen(1) * SI(0,-1,0,0,0,0,0)
```

