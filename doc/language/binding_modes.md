# Binding Modes

Hikolang has three binding modes:
 * **value** - a value located in managed storage, or the temporary result of
   an expression.
 * **reference** - a reference to storage containing a value.
 * **movable** - a reference that permits the internals of the referenced
   object to be consumed.


## Type Qualifiers

Each of the binding modes have specific types:

 mode      | type       | source
 ----------|------------|---------------------------------------------
 value     | `T`        | variable, temporary result of an expression.
 value     | `const T`  | immutable.
 reference | `&T`       | copied reference, taken from movable, variable or temporary.
 reference | `&const T` | copied reference, taken from movable, variable, temporary or immutable.
 movable   | `&&T`      | copied movable, taken from a variable or temporary.

Here is a diagram how type-qualifiers can be safely converted:

```
    const T         T
       │            │
       │       ┌────┴────┐
       │       │         │
    borrow   borrow    move
       │       │         │
       │       ↓         ↓
       │      &T ←────  &&T
       │       │
       │    add const
       │       │
       │       ↓
       └──→ &const T
```

Additionally there are explicit unsafe cast that are possible:
 - `&const T` -> `&T` - using `std.unsafe_mutable_cast()`
 - `&T` -> `&&T` - using `std.unsafe_movable_cast()`

`const` limits access to member functions of the object that accept an const
reference as the `@self` argument, or read-only access to member variables.
`const` thus does not mean that the object itself is frozen, these member
function may still modify the value.

## References & Movables

A reference and movable are not nullable. Optional references `std.optional[&T]`
exists and are internally optimized as a pointer.

Both references and movable are reseatable by special compiler-blessed
functions from other reference or movable.

A movable can be moved from, after the function call the original value is in a
valid but indeterminate state. After the function call the value may be
optionally reset and reused. The function is not required to consume the moved
from value, in this case the original value remains in its original state.

An operation consumes a movable when it transfers ownership of some or all of
the object's resources out of the referenced object.

## Type selector

A type selector is attached as metadata to an expression, it does not change the
result or type of the expression. The type selector metadata is consumed by type
specification of a variable/immutable initialization or function argument to
control how the type of the expression is interpreted.

 Selector   | Description
 -----------|------------
 `x`        | Use the default binding behavior of the receiving type specification; never implicitly produce a movable.
 `*x`       | Explicitly make a copy of the value in (or referenced by) `x`.
 `&x`       | Take a reference to the value in `x`.
 `&const x` | Take a const reference to the value in `x`.
 `&&x`      | Preserve the binding mode of `x`, except that a value binding is promoted to a movable binding.

Compared to the other type-selector `&&x` is a bit special, it has the following
rules based on the original type of `x`:

 Type of `x` | The as-if type after `&&x`
 ------------|----------------------
 `T`         | `&&T`
 `const T`   | `&const T`
 `&T`        | `&T`
 `&const T`  | `&const T`
 `&&T`       | `&&T`

A temporary expression implicitly has the `&&x` type selector attached as
metadata, causing it to be passed as a movable by default. Although it is
possible to override the selector of a temporary expression to `*x`, `&x`,
`&const x` and `&&x`, it is not possible to override it to `x`.

See the next chapter on how each type selector influences how an assignment or
argument pass is bound.


## Type specification

 Parameter        | Variable               | Description
 -----------------|------------------------|------------
 `fn(x : *)`      | `x : *      <- <init>` | Take a copy
 `fn(x : &)`      | `x : &      <- <init>` | Take a reference
 `fn(x : &const)` | `x : &const <- <init>` | Take a const reference
 `fn(x : &&)`     | `x : &&     <- <init>` | Require a movable to be passed in
 `fn(x :)`        | `x :        <- <init>` | Infer the binding mode
 `fn(x)`          | `x          <- <init>` | Shorthand for `fn(x : *)` and `x : <- <init>`


### `fn(x : *)`, `x : * <- <init>` - Take a copy of the value

A value parameter may be implemented as a const reference when doing so cannot
make an additional alias observable by the program:
 * The function only reads from the argument, and
 * The function does not let a const reference to this argument escape to
   another thread, and
 * No non-const references to the original value escaped to another thread, and
 * The type-selector used is not `*x`.

In other cases the value is copied.

> [!note]
> `fn(x)` is short-hand for `fn(x : *)`, the second can have a type expression
> to constrain accepted type, while the first can not.


### `fn(x : &)`, `x : & <- <init>` - Take a reference

When using the selectors `x`, `&x` or `&&x`, take a reference of a value when:
 * A non-const value is passed in.
 * A non-const reference is passed in.
 * A movable is passed in.

All other combinations are invalid. See [Invalid Pass]


### `fn(x : &const)`, `x : &const <- <init>` - Take a const reference

Take a const reference, this is always valid. [Temporary Materialization].


### `fn(x : &&)`, `x : && <- <init>` - Require a movable to be passed in

Require a movable to be passed in, using the following:
 * A non-const value is passed in using the `&&x` selector. 
 * A movable is passed in using the `&&x` selector.

All other combinations are invalid. See [Invalid Pass]


### `fn(x :)`, `x : <- <init>` Infer the binding mode

The binding mode will be inferred from the expression and selector that is
passed in.

When the selector is:
 * `x` - `x` is passed as a reference:
   - If `x` is a value or non-const reference the result is `&T`.
   - If `x` is a const value or const reference the result is `&const T`.
   - If `x` is a movable the result is `&T`.
 * `*x` - The value in `x` is copied, the result is `T`.
 * `&x` - `x` is passed as a reference:
   - If `x` is a value or non-const reference the result is `&T`.
   - If `x` is a const value or const reference the result is `&const T`.
   - If `x` is a movable the result is `&T`.
   - If `x` is a temporary the result is `&T`.
 * `&const x` - The result is a const reference of type `&const T`.
 * `&&x` - `x` and its binding mode will be forwarded:
   - If `x` is a non-const value then take a movable, the result is `&&T`.
   - If `x` is a non-const reference the result is `&T`.
   - If `x` is a const value or const reference the result is `&const T`.
   - If `x` is a movable the result is `&&T`.
   - If `x` is a temporary the result is `&&T`.

> [!note]
> Temporaries implicitly have the `&&x` selector. This selector may be
> overridden but it can not be `x`.

If the passed in value is a temporary, then it will be materialized,
see: [Temporary Materialization]


### `fn(x)`

This uses the same rules as `fn(x :* )`.

### `x <- <init>`

This uses the same rules as `x : <- <init>`.



[Invalid Pass]: #invalid-pass
## Invalid Pass

Invalid arguments cause the function overload to be dropped as a candidate
for overload resolution.

Invalid initialization of variables and immutable will cause a compile-time error.

## Overload resolution

When passing a value, reference or movable to an overloaded function there are
priorities.

  rhs type or selector       | exact match | priority                          
 :---------------------------|:------------|:----------------------------------
 `T`, `*x`                   | `: *`       | `:`, `: &const`, `: &`         
 `&T`, `&x`, `&&T`           | `: &`       | `:`, `: &const`, `: *`         
 `&const T`, `&const x`      | `: &const`  | `:`, `: *`                      
 `&&x` (including temporary) | `: &&`      | `:`, `: &const`, `: &`, `: *` 



[Temporary Materialization]: #temporary-materialization
## Temporary Materialization

Temporaries are materialized before the function call or variable initialization
in the enclosing code-block. Therefore the lifetime of temporaries are extended
until the end of the code-block together with other automatic variables.


```
foo = fn(x : &const) {
  return &x
}

a = 10.0
b = &foo(a + 3.0)
c = b + 1.0
```

Is executed as-if:

```
...

a = 10.0
temporary = a + 3.0
b = &foo(&&temporary)   // The temporary is passed into the function.
c = b + 1.0             // b references the temporary.
```

If multiple arguments cause materializations, then those materializations happen
in the same order as the arguments.

If the temporary does not escape the operation requiring materialization, its
materialization may be delayed or eliminated. If a reference to the temporary
escapes, the temporary must be materialized with the lifetime required by that
reference.
