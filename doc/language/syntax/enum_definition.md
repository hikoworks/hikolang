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

An `enum` is a built-in meta-type.

A _enum-member_ consists of a name and an optional value. The name may be any
valid and secure Unicode string, this string will be normalized to NFC.
If the string follows Unicode annex #31 you may use a [_identifier_] as short
hand.

Like other meta-types after the type is frozen the data-layout of instances
can no longer be changed. Enum-members, enum-metadata may still be
added after being frozen.

Duplicate members are merged if they have the same value. It is a compilation
error if the duplicate has a different value. If a duplicate member has a
different annotation, then those annotations are merged. 

If a member has not value, then the member is automatically assigned the next
number following the member with the highest number. If a member without a value
is merged with a duplicate with a value, then it is merged with the previous
definition.

If an `enum` definition does not have a [_underlying_] then by default it will
become `u32`. If a `enum` definition without a [_underlying_] is merged, then it
merges without problem. If a `enum` definition with a [_underlying_] is merged
then the [_underlying_] must match the previous definition.

## Built-in enum types

The following enum types are part of the language itself:

### void

```
.void = enum u0 {
    @keyword void
}
```

### bool

```
.bool = enum u1 {
    @keyword false,
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

The following annotations may be attached to members:
 - `@auto_rethrow`: The compiler will synthesize a catch-rethrow for any
   flow-control-expression that does not explicitly catch this error.

### std.effect

Effects that a function has on the system

```
.std.effect = enum u32 {
    /** This code performs I/O operations. */
    io

    /** This code may cause the operating to block progress */
    blocking
}
```

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
```

The following annotations may be attached to members:
 - `@domain(domain, index)`:
   - A `std.unit` to be used as a domain,
   - An exponation index inside this domain.
 - `@unit(unit-expression)`: The [_unit-expression_] for conversion to the new
   unit.

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

The following annotations may be attached to members:
 - `@prefix(precedence [, function ])`: Prefix operator
   - precedence: An integer priority, lower numbers bind closer.
   - function: The name of the function call that implements the operator.
 - `@suffix(precedence [, function])`: Suffix operator
   - precedence: An integer priority, lower numbers bind closer.
   - function: The name of the function call that implements the operator.
 - `@right(precedence [, function])`: Right associative
   - precedence: An integer priority, lower numbers bind closer.
   - function: The name of the function call that implements the operator.
 - `@left(precedence [, function])`: Left associative
   - precedence: An integer priority, lower numbers bind closer.
   - function: The name of the function call that implements the operator.
