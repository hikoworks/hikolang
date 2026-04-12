# Tutorial

## Variables

A variable binds a name with a memory location that stores an object of a
specific type. The memory location is managed by the variable, when the
variable goes out-of-scope the object at the memory location is destroyed
and the memory location may be reused.

A variable is introduced to the scope using `var <name> = <expression>`.
The object from the `<expression>` on the right hand side is
copied into the memory location managed by the variable.
The type of the copy adds the `mut` type-category to the type.

Example of a variable:

```
var sum = 42.0              // 42.0 : f64
for (var x in 0..=10) {     // x : int[0..=10]
    sum = sum + x
}
print(sum)
```

> [!NOTE]
> In the example above we convert the type of the literal `42` (`int[42..=42]`) to
> an integer with a larger range `i32`. If we did not change this type then we would not
> be able to change the value to anything but `42`.


## Immutable

The `let <name> = <expr>` creates an immutable variable, the variable is
first initialized, then the variable is sealed. Meaning the `mut` type-category
is removed from the type, and the managed object-value is frozen so that it can't
be changed at all.

Example of the immutables `x` and `y`:

```
var sum = 42 : i32          // 42 : int[42..=42], sum : i32
for (let x in 0..=10) {     // x : int[0..=10]
    let y = x + 10          // y : int[10..=20]
    sum = sum + x
}
print(sum)
```

`let` is implemented as syntactic sugar:

```
var <name> = <expr>
seal <name>
```

## Reference

A reference provides indirect access to an existing object.

Instead of owning its own storage, a reference points to another object and forwards all operations to it.
References do not copy or move values; they alias an existing object.

```
var a = 42 : i32      // a : i32
ref b = a             // b : &i32
b = 1
print(a)              // prints "1"
```

Here, `b` and `a` refer to the same underlying object. Assigning through b modifies a.


### Mutability and references

References preserve or restrict the capabilities of the object they refer to:
 - A `ref` requires the object to be mutable
 - Through a `ref`, mutation is allowed

```
var <name> = <expr> : &mut
```

This is syntactic sugar for:
 - converting <expr> into a reference (&)
 - requiring the referenced type to have the mutable capability

### Reseating references

A reference normally continues to point to the same object, but it can be reseated
using low-level operations:
 - `get_pointer()`
 - `set_pointer()`

These operate on the underlying pointer representation.

> [!NOTE]
> The pointer is an opaque LLVM pointer. It supports pointer arithmetic and can
> be used to build higher-level abstractions such as slices or spans.

### Sealing references

Sealing a reference:

```
var a = 42 : i32
ref b = a
seal b
```

 - prevents reseating `b` (the reference becomes fixed)
 - removes the ability to mutate through the reference `b`
 - freezes the referenced object managed by `a`; this means
   you can no longer modify this object

## View

A `view` is a restricted reference.

It refers to an object, but does not allow mutation through that reference.
Unlike a copy, a view is still live—it reflects changes made elsewhere.

```
var a = 42 : i32      // a : i32
view b = a            // b : &const i32
a = 1
print(b)              // prints "1"
```

Even though `b` cannot modify the object, it still observes changes made through other references.

### Capabilities
A view removes the mutable capability from the referenced type:

```
var <name> = <expr> : &const
```
This:
 - creates a reference (`&`)
 - removes the ability to mutate through it (`const`)

Important distinction, A view is not:
 - a copy
 - a snapshot

It is:
 - a reference with restricted capabilities

### Notes on mutability
Mutability controls what operations are allowed through a given reference—it does not guarantee that a value cannot change.
Even if you only hold a view, the underlying object may still be modified through other references.

### Common pitfall

```
let x = <expr> : &const
```

This creates:
 - a non-reseatable reference (due to let)
 - to a non-mutable object (&const)
 - and freezes the referenced object

This combination is often unintended and therefore considered an error.
If this behavior is required, it must be written explicitly:

```
var x = <expr> : &const
seal x
```

This version makes three things explicit that were previously implicit:
References are aliases, not just “pointers”
Mutability is a capability of access, not a property of the value itself
A view is live but restricted, not a snapshot
If you want, the next step would be aligning the let/seal section with this same “capability model” language so everything reads as one system instead of separate features.





## Function Definition

```
fn foo(a) {
    return a + 42
}

let b = foo(1)    // b : int[43..=43]
print(b)          // prints "43"
```

### Argument Types

`fn foo(a) {}` forwarding:
 - variable -> reference
 - move from variable -> move
 - immutable -> view
 - reference -> reference
 - view -> view

`fn foo(var a)` copy/move to an internal variable.

`fn foo(let a)` copy/move to an internal immutable.

`fn foo(ref a)` or `fn foo(var a : &mut)` a mutable reference to an object.
 - variable -> reference
 - immutable (error)
 - reference -> reference
 - view (error)

`fn foo(view a)` or `fn foo(var a : &const)` a non-mutable reference to an object.
 - variable -> view
 - immutable -> view
 - reference -> view
 - view -> view

```
fn bar(let a) {}
fn baz(ref a) {}
fn qux(view a) {}

fn baz(var a : &) {}
fn qux(var a : &const) {}
```
