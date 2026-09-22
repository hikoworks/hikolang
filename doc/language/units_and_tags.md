# Units, Domains, and Domain Vectors

Units provide a way to associate meaning with numeric values. A value such as
`10.0` by itself says nothing about whether it represents ten meters, ten
seconds, ten kilograms, or something else. By attaching a unit to the value, the
type system can keep track of what the value represents and can check whether
operations between values are meaningful.

The unit system is based on three related concepts: **units**, **domains**, and
**domain vectors**.

## Units

A unit describes a quantity and its relationship to a base representation. For
example, the standard SI units can be declared as:

```text
.std.unit = enum {
    SI
    @domain(SI, 0) s
    @domain(SI, 1) m
    @domain(SI, 2) kg
}
```

Additional units can then be defined in terms of existing units:

```text
.std.unit = enum {
    @unit(1000.0 m) km
    @unit(3600.0 s) h
}
```

The declaration of `km` states that one kilometer is one thousand meters.
Similarly, one hour is three thousand six hundred seconds.

Units can be combined using multiplication, division, and integer powers.
Consequently, compound units such as meters per second or kilograms per meter
per second squared can be expressed directly:

```text
km/h
kg m s⁻²
```

The unit system does not need to retain these names after an expression has been
resolved. Instead, every unit expression is reduced to a scalar conversion
factor and a **domain vector**.

## Domains

A domain defines a collection of dimensions that are independent of the
dimensions in other domains.

The full SI domain, for example, contains the seven SI base dimensions:

```text
.std.unit = enum {
    SI
    @domain(SI, 0) s
    @domain(SI, 1) m
    @domain(SI, 2) kg
    @domain(SI, 3) A
    @domain(SI, 4) K
    @domain(SI, 5) mol
    @domain(SI, 6) cd
}
```

The positions in the domain identify dimensions. An exponent associated with a
position describes how many times that dimension occurs in a unit expression.

For example: `m` has the SI dimension vector: `SI(0,1,0,0,0,0,0)`. While: `m/s`
has: `SI(-1,1,0,0,0,0,0)`.

The first position represents seconds and the second represents meters, so `m/s`
contains one meter dimension and minus one second dimension.

Domains are independent. This is important because not every quantity
represented by a unit needs to belong to the same system of dimensions.

For example, a screen can have its own domain:

```text
.std.unit = enum {
    screen
    @domain(screen, 0) px
}
```

A screen density expressed as pixels per meter (`px/m`) then has two independent
components:

```text
screen(1) * SI(0,-1,0,0,0,0,0)
```

The `screen` dimension does not interact with the `SI` dimensions. This allows
the type system to distinguish pixels from physical length while still allowing
them to participate in ordinary unit algebra.


## Domain Vectors

A **domain vector** is the normalized representation of the dimensions belonging
to one domain.

For the SI domain: `SI(0,1,0,0,0,0,0)` represents length, while:
`SI(-1,1,0,0,0,0,0)` represents velocity.

A complete unit expression can contain vectors from multiple domains. Such an
expression is therefore represented as a collection of domain vectors.

For example: `px/inch` can be reduced to: `screen(1) * SI(0,-1,0,0,0,0,0)`.

The scalar conversion factor is kept separately. Thus the complete normalized
representation consists of: conversion factor + domain vectors.

For example, `55 km/h` is converted to: `15.2778 × SI(-1, 1,0,0,0,0,0)`.

The names `km` and `h` are no longer relevant after normalization. Only their
conversion factor and dimensions remain.


## Unit Algebra

The domain vectors make unit arithmetic straightforward.

Multiplication adds exponents: `m * s⁻¹`\
becomes: `SI(0,1,0,0,0,0,0) + SI(-1,0,0,0,0,0,0) -> SI(-1,1,0,0,0,0,0)`.

Division subtracts exponents: `m / s`\
becomes: `SI(0,1,0,0,0,0,0) - SI(1,0,0,0,0,0,0) -> SI(-1,1,0,0,0,0,0)`.

An exponent multiplies the vector: `m²`\
becomes: `SI(0, 2, 0, 0, 0, 0, 0)`

The same rules apply independently to every domain.

A dimension whose exponent becomes zero disappears from the normalized
representation. This is what makes unit cancellation possible: `m / m` becomes
the dimensionless vector: `SI(0,0,0,0,0,0,0)` and therefore produces an ordinary
scalar.

## Tags

Domains are not restricted to physical quantities. A domain can also be used to
give a value a nominal tag.

A tag is declared using `@tag`:

```text
.std.unit = enum {
    @tag my_tag
}
```

A tag is a unit in its own domain with a single dimension and a conversion
factor of one. Consequently: `#my_tag` has the domain vector: `my_tag(1)`.

Tags use exactly the same algebra as other units. Applying the inverse tag gives
an exponent of `-1`: `#-my_tag`. and the two cancel when combined: `#my_tag
#-my_tag` resulting in a dimensionless value.

This allows units to be used not only for physical measurements, but also as
lightweight nominal constraints on values. For example:

```text
foo = fn(flag : bool #my_tag) {
    if (flag) {   // tagged values, can still be used as scalars.
        return 42
    } else {
        return 0
    }
}

x = foo(true #my_tag)
```

requires an argument whose dimension contains `my_tag(1)`, without requiring a
separate runtime representation for the tag.

Becuase tags do not have a conversion-factor like units do, a tagged value can
still be used as a scalar. So a tagged value can be passed to function that does
not constrain on that tag.

```text
bar = fn(flag : bool) {
  ...
}

y = bar(true #my_tag)
```

Thus physical units and nominal tags share the same underlying mechanism: both
are dimensions represented by exponents within a domain.


## Units in Types

When a unit expression is used as part of a type, its concrete unit names and
conversion factor are not significant. The expression is normalized to its
domain vectors.

For example, these values all represent the same dimension:

```text
20.0 #(km/h)
5.0 #(m/s)
300.0 #(m/min)
```

Their scalar representations are different, but their domain vector is the same:

```text
SI(-1,1,0,0,0,0,0)
```

Therefore a function declared as:

```text
distance = fn(speed : f64 #(m/s), duration : f64 #s) {
    return speed * duration
}
```

can accept a speed expressed in any compatible unit, such as kilometers per hour
or meters per second.

The unit system therefore separates two concepts that are often conflated:

* **conversion**, which determines the numeric representation of a value;
* **dimension**, which determines what kind of quantity the value represents.

Unit names are used to perform conversion, while domain vectors are used for
dimensional type checking.
