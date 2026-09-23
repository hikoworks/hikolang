# enum-definition

[_annotation_]: annotation.md
[_expression_]: expression.md
[_fqname_]: fqname.md
[_identifier_]: identifier.md
[_underlying_]: underlying.md
[_unit-expression_]: unit_expression.md
[_string-literal_]: string_literal.md

## Syntax

_enum-definition_ := [_annotation_]__*__ `enum` [_underlying_]__?__ `{`\
    _enum-member_ `;`__*__\
`}`

_enum-member_ := [_annotation_]__*__ __(__ [_identifier_] | [_string-literal_] __)__ __(__ `=` [_expression_] __)?__\


## Semantics

An `enum` is a built-in meta-type; it defines enum-types. An instance of an
enum-type is a value represented by one of its members. A _enum-member_ is
a unique name with a unique integral value, together with optional
annotations.

If no [_underlying_] `int[min...max]` type is specified, the compiler
determines the underlying type when the enum freezes. Its range is the
minimum and maximum values of all members at that point.

> [!note]
> Freezing happens when a type becomes concrete when it is being used.
> The compiler tries to be as lazy as possible to delay making types
> concrete. When a type becomes frozen the data layout of instances
> can no longer be changed.

The name of a member may be any [_string-literal_]. A [_identifier_] may
be used instead of a [_string-literal_] as a short hand.

Every enum member has a distinct integral value. It is a compilation
error for two members with different names to have the same value.

A member declaration without an explicit value receives the smallest
integral value greater than the greatest value assigned to any member.
If the enum has no members, implicit values start at zero.


### Reopening members and merging

If a member declaration has the same name as an existing member,
it declares that member again. If it specifies a value, that value
must equal the existing member's value. Its annotations are merged with
those of the existing member.

```
foo = enum {   // int[0...6]
    a          // 0
    b = 5      // 5
    "c"        // 6
}
```

New members can be added at any time, before and after the enum is frozen,
as long as the value of members is inside the range of the underlying integer
type, and the value has not been used yet.

```
// The underlying was not specified so the range can grow.
foo = enum {   // int[0...7]
    d          // 7
}

freeze foo

foo = enum {
     e = 1      // 1 was not yet used, so can be added.
}
```

A member declaration whose name matches an existing member extends that
member. Its annotations are merged with the existing member's annotations.
If it specifies a value, that value must equal the existing member's value.

```
bar = enum {
    a = 1
    // b = 1   ERROR: values must be unique
}
```


### Exhaustive check

An enum is dense if every value in the range of its underlying integer type
is assigned to a member. Otherwise the enum is sparse.

> [!note]
> The simple case of enums are automatically dense.

A switch over an enum-type requires a default case if the enum is sparse at
the point the switch is compiled. If the enum is dense, a default case is
not required when all members are covered by cases.


## Built-in enum types

The following enum types are part of the language itself:


### void

```
.void = enum {
    empty
}
```


### bool

```
.bool = enum {
    @keyword false
    @keyword true
}
```


### std.error_code

Error codes that can be thrown and caught from functions.

```
// Simplified definition is the standard library
.std.error_code = enum u32 {
    empty = 0
    @auto_rethrow bad_alloc
}

// Adding a new error-code
.std.error_code = enum {
    io_error
}
```


#### @auto_rethrow

The compiler will synthesize a catch-rethrow for any
flow-control-expression that does not explicitly catch this error.


### std.effect

Effects that a function has on the system

```
.std.effect = enum u32 {
    /** This code behaves differently between elaboration and runtime. */
    phase_variant

    /** This code performs I/O operations. */
    @implies(phase_variant) io

    /** This code may cause the operating to block progress */
    blocking
}
```

#### .std.effect.phase_variant

This is a special built-in effect used by the compiler.

When `phase_variant` is not an effect of a function, then this function can
be called during elaboration as an optimization.

If `phase_variant` is an effect, then this function is by default delayed until
runtime with the following exceptions:
 - A call to this function is done during elaboration of types that are needed
   during compilation.
 - A call to this function has an explicit `@elaborate` annotation.


#### @implies(effect, ...)

When `@with_effect(...)` is used, it is as-if `@with_effect(...)` is also
used on the implied effects.


### std.unit

Units can be attached to types like a tag to do domain analysis on those types.

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
    @unit(245 mm) inch 
    @unit(s⁻¹) Hz
    @unit(kg m s⁻²) N 
}

.std.unit = enum {
    screen
    @domain(screen, 0) px
}

.std.unit = enum {
    @tag my_tag
}
```

#### @domain(domain, index)

 - domain: A `std.unit` to be used as a domain,
 - index: An exponation index inside this domain.


#### @unit(unit-expression)

The [_unit-expression_] for conversion to the new unit.


#### @tag

`@tag` declares a unit whose domain contains exactly one dimension and whose
conversion factor is 1. The resulting unit behaves algebraically like any other
unit, but is intended for nominal type tagging rather than physical measurement.

A `@tag` does not introduce a scaling factor like `@unit()` does. A tagged value
therefore retains its underlying scalar type and may be used where its
non-tagged scalar type is expected. In such cases, the tag is implicitly dropped
from the type.


### std.operator

```
.std.operator = enum u32 {
    @suffix(2000, __post_increment__)   "++"
    // duplicate definition merges the annotations.
    @prefix(3000, __increment__)        "++"
    @prefix(3000, __pos__)              "+"
    @left(6000, __add__)                "+"
    @right(16000, __inplace_add__)      "+="
    @right(15100)                       catch
}
```


#### @prefix(precedence [, function ])

Prefix operator
 - precedence: An integer priority, lower numbers bind closer.
 - function: The name of the function call that implements the operator.


#### @suffix(precedence [, function])

Suffix operator

 - precedence: An integer priority, lower numbers bind closer.
 - function: The name of the function call that implements the operator.


#### @right(precedence [, function])

Right associative

 - precedence: An integer priority, lower numbers bind closer.
 - function: The name of the function call that implements the operator.


#### @left(precedence [, function])

Left associative

 - precedence: An integer priority, lower numbers bind closer.
 - function: The name of the function call that implements the operator.
