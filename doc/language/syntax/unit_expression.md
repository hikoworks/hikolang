# unit-expression

[_enum_]: enum.md
[_identifier_]: identifier.md
[Units and Tags]: ../unit_and_tags.md
## Syntax

_unit-expression_ :=\
      `(` _complex-unit-expression_ `)`\
    __|__ [_identifier_]\
    __|__ _unit-expression_ `^` [_integer_literal_]\
    __|__ _unit-expression_ `**` [_integer_literal_]

_complex-unit-expression_ :=\
      `(` _complex-unit-expression_ `)`\
    __|__ [_identifier_]\
    __|__ _complex-unit-expression_ `^` [_integer_literal_]\
    __|__ _complex-unit-expression_ `**` [_integer_literal_]\
    __|__ _complex-unit-expression_ `/` _complex-unit-expression_\
    __|__ _complex-unit-expression_ `*` _complex-unit-expression_\
    __|__ _complex-unit-expression_ _complex-unit-expression_

[_identifier_]: identifier.md
[_integer-literal_]: integer_literal.md

## Semantic

For an explenation of the concetps see [Units and Tags].

In the example below you see how a _unit-expression_ can be used together with
values. The [_identifier_]s in a _unit-expression_ are from the built-in
`std.unit` [_enum_].

The operators `#` and `#-` can be used on values, to add dimensions.
 - `a #U`   : multiply the value `a` with the unit `U`
 - `a #-U`  : multiply the value `a` with the unit `U⁻¹`

If the result of these operators becomes a dimensionless value, then the result is a
normal scalar (without units). For example a convert to a unit, then back into a
scaler:
 - `a = 1.0 #km` translates into: `1000.0 * SI(0,1,0,0,0,0,0)`.
 - `b = a #-foot` translates into `1000.0 / 0.3048 × SI(0,0,0,0,0,0,0)`, since
   the dimension vector is all-zero (aka dimensionless), it becomes the scalar
   `3280.84` 

An example of using `#` and `#-` operators on values:

```
speed = 10.0 #(km/h)                 // f64 m/s
duration_in_hours = 2.0              // f64
duration = duration_in_hours #h      // f64 s
distance = speed * duration          // f64 m
distance_in_km = distance #-km       // f64
```

When `T #U` is used on a type it can be used to check if the value corresponds
with that dimension. The unit expression on a type is normalized into just the
dimension tuple. Unit names do not survive normalization. So the check only
checks the dimension tuple.

```
distance = fn(speed : f64 #(m/s), duration : f64 #s) {
    return speed * duration
}

distance_in_miles = distance(20.0 #(km/h), 5 #h) #-mi
```


Here are some examples how a _unit-expression_ is converted into a list of
domain-exponent-tuples and the internal scalar value.

```
.std.unit = enum u32 {
    SI
    @domain(SI, 0) s
    @domain(SI, 1) m
    @domain(SI, 2) kg
    @domain(SI, 3) A
    @domain(SI, 4) K
    @domain(SI, 5) mol
    @domain(SI, 6) cd
    @unit(0.001 kg) g 
    @unit(0.001 m) mm
    @unit(1000.0 m) km
    @unit(24.5 mm) inch 
    @unit(3600 s) h
    @unit(s⁻¹) Hz
    @unit(kg m s⁻²) N 
}

.std.unit = enum {
    screen
    @domain(screen, 0) px
}

.std.unit = enum {
    // A tag is a unit, that is its own domain, with a single exponent.
    @tag my_tag
}

distance = 1.0 #mm             // 0.001 SI(0,1,0,0,0,0,0)
speed = 55 #(km/h)             // 15.2778 SI(-1,1,0,0,0,0,0)
screen_density = 72 #(px/inch) // 2834.65 [screen(1), SI(0,-1,0,0,0,0,0)]
flag = true #my_tag            // true my_tag(1)
```

