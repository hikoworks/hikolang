# binding-mode

## Syntax

_binding-mode_ := `var` __|__ `let` __|__ `ref` __|__ `view`


## Semantics

### binding modes

All binding modes are syntactic sugar around `var`.

#### let

The `let` binding mode makes a variable of which the type doesn't have the
`mut` capability, and where the value itself is frozen/unmodifiable.

`let a = expr` desugars into:

```
var a = expr
seal a
```

#### ref

The `ref` binding mode makes a reference to another value, the type retains
the `mut` capability. A reference is implemented as a non-nullable pointer to
an object, all access to a reference is forwarded to the referenced's object.

The actual pointer underlying the reference can be accessed and modified using
the `std.set_address()` and `std.get_address()` functions.

`ref a = expr` desugars into:

```
var a : & = expr
```

#### view

The `view` binding mode is like the `ref` binding mode, but the `mut` capability
of the type is removed.

`view a = expr` desugars into:

```
var a : &const = expr
```
