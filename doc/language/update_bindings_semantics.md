# Updating Bindings Semantics

## Updating Variable Bindings

When the name already resolves to an existing binding, the statement updates
that binding rather than defining a new one. Binding qualifiers and type
annotations are permitted only when defining a binding and must not be
specified when updating one. The existing binding's type and qualification
remain unchanged; the update operator determines the operation to perform.

The `<-` and `=` operators determine which operation is called:
* `<-` calls `__assign__()`.
* `=` calls `__merge__()`.

```text
a <- 1.0
a <- 2.0       // Calls a.__assign__(2.0).

b = 1.0        // Creates a const-restricted binding.
// b = 2.0     // Error: f64 does not support __merge__().

f = fn(a : *int) {}     // Creates an overload-set.
f = fn(a : *string) {}  // Merges the overload-sets, std.overload_set supports __merge__().
```

Although both operators introduce or update bindings, their update semantics
differ. `<-` assigns a new value, while `=` merges a definition or value
according to the type's semantics.

The operations use binding qualifiers that reflect their permitted access:

```text
__assign__ = fn(@self self : &, other : &const)
__merge__  = fn(@self self : &const, other : &const)
```

Consequently, `<-` cannot update a const-restricted binding through
`__assign__`, because that operation requires unqualified access. The `=`
operator can invoke `__merge__` through const-restricted access, but only when
the type supports merging.

An implementation of `__merge__()` may explicitly remove the const restriction
from its receiver's access path to perform the merge. Any resulting mutation is
authorized by that explicit cast, not by the `=` operator itself. As with other
explicit qualification casts, the implementation is responsible for ensuring
that the resulting operations are valid.

Very few types support `__merge__()`, examples that do support it:
 - Function overload-sets allow new functions to be added.
 - Types allow member functions and member variables, or enum names to be
   merged.
 - Templated types, functions and global variables also creates an overload-set
   of types, functions and variables.

The semantics of `__assign__()` and `__merge__()` are defined by the type. An
update is invalid when the required operation is unsupported or its binding
qualifications cannot be satisfied.


### Updating References

When updating an existing reference binding, the update must not specify a
binding qualifier or type annotation. An explicit binding operator that produces
a reference reseats the existing reference, preserving its qualification. This
rule takes precedence over the normal `<-` /` =` update dispatch. Otherwise:
 * `<-` calls `__assign__()` on the referenced object, and
 * `=` calls `__merge__()` on the referenced object.

```text
a <- 1.0
b <- 2.0

r <- &a
r <- 2.0              // Calls r.__assign__(2.0), modifying a.
r <- &b               // Binding operator resolves to a reference: reseats `r`.
r <- get_reference()  // Modifies `b` with the referenced object's value
r <- &get_reference() // Binding operator resolved to a reference: reseats `r`.
```

Reseating does not copy or move either referenced object's value, nor does it
change the reference binding's qualification. The new reference must be
implicitly convertible to the existing binding's reference qualification.

```text
cr = &a
cr <- &b             // Reseats cr; it remains const-restricted.

mr <- &move a
mr <- &move b        // Reseats mr; it remains move-capable.
// mr <- &b           // Error: cannot implicitly grant move capability.
```

A move-capable reference grants permission to consume its referenced object's
value but does not itself perform a move. Consumption occurs only when a
subsequent variable-binding conversion moves from the referenced object.

```text
a <- "Hello World"
r <- &move a         // Creates a reference; `a` remains unchanged.
c : * <- &move r     // Moves the value from `a` into `c`.
```

Reference bindings alias existing objects. Creating or reseating a reference
does not itself copy, move, or modify the referenced object's value.
