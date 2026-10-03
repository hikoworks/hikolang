# Binding Modes

Hikolang has three binding modes:
 * **value** - a value located in storage, or the temporary result of
   an expression.
 * **reference** - a reference to storage containing a value.
 * **movable** - a reference that permits the internals of the referenced
   object to be consumed.


## Type Qualifiers

```
a = 10.0    // `a` is an immutable binding whose value is accessed as `&const f64`.
b <- 20.0   // `b` is a variable binding whose value is accessed as `&&f64`.
```

Binding mutability determines whether a binding may be assigned/rebound.
Const qualification determines whether an access path may be used to modify the
referenced value.

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

Additionally there are explicit unsafe cast that are possible, which allows
modification of values through a previously const-qualified path, even if
it leads to a value that is managed by a immutable:
 - `&const T` -> `&T` - using `std.unsafe_mutable_cast()`
 - `&T` -> `&&T` - using `std.unsafe_movable_cast()`

## Const

A `const` (reference to a) value limits access to:
 * member functions that accept a const reference to `self`, or
 * read-only access to member variables.

A `const` qualifier does not mean that the value cannot be modified;
it is valid to use `std.unsafe_mutable_cast()` to modify the value anyway.


## Reference

A reference works like a non-nullable pointer, which will automatically
dereference when a value is expected.

Optional references `std.optional[&T]` exists and are internally optimized
as a pointer.

References can not be nested, which is why `&&T` can be a separate syntax
for a movable.

You are allowed to borrow multiple const and non-const references from
a single value. Borrowing from a reference produces a reference to the same
value.


## Movable

A moveable is a reference with added permission to allow the referenced value
to be consumed. Consume means to take ownership of the internal resources
of a value, and to leave that value in a indeterminate-but-valid state.

A value being in indeterminate-but-valid state means it is still fully
functional. For example you can reset or reassign a value to make it
determinate again.

The actual consuming is done by specialization of member-functions of
the object, such as:
 * move constructor
 * move assignement operator
 * specialization of the `swap()` function
 * other move specializations.

Some extra rules:
 * You are allowed to borrow multiple movables from a single value.
 * You can borrow a new movable from a movable.
 * You can borrow a reference from a movable.
 * Consuming a value does not invalidate references to the value.
   Existing references continue to refer to the same object, which may
   now be in an indeterminate-but-valid state.
 * It is valid to consume from a value multiple times, after the first
   time the result is indeterminate.
 * Consuming like most other operations on a object is not thread-safe.

## Expressions

With the following definitions:

```
a <- 2.0  // `T`        variable
b = 1.0   // `const T`  immutable
r = &a    // `&T`       reference
cr = &b   // `&const T` const-reference
m = &&a   // `&&T`      movable

my_type = struct { m <- 3.0 }
o = my_type()
co = &const o

get = fn() { return a }
get_ref = fn() -> & { return a }
get_const_ref = fn() -> &const { return a }
get_moveable = fn() -> && { return a }
```

The next table shows the outcome of several expressions:

  Expression       | Outcome    | Name                  
 ------------------|------------|-----------------------
  `a`, `o.m`       | `&&T`      | named
  `b`, `co.m`      | `&const T` | named const
  `r`              | `&T`       | named reference
  `cr`             | `&const T` | named const reference
  `m`              | `&&T`      | named movable
  `get()`          | `T`        | temporary
  `get_ref()`      | `&T`       | reference
  `get_const_ref()`| `&const T` | const reference
  `get_movable()`  | `&&T`      | movable

Most operator expressions, like `a + b`, are often lowered to a function call
`__add__(a, b)` which means that the resulting binding modes are limited by
what functions can return.

## Temporary

A temporary is a value-result of an expression. Temporaries are as-if they are
materialized as a variable before the function call. The variable is passed to
the function or initializer with the given bind selector, or implicitly the
forward `&&` selector.

For example:

```
a = foo(b + 10.0)

// lowered as:
materialization = b + 10
a = foo(&&materialization)
```

Here is another example where we explicitly select const reference passing:

```
a = foo(&const (b + 10.0))

// lowered as:
materialization = b + 10
a = foo(&const materialization)
```

## Bind selector

A bind selector is attached as metadata to an expression, it does not change the
result or type of the expression. The bind selector metadata is consumed by bind
specification of a variable/immutable initialization or function argument to
control how the type of the expression is interpreted.

 Selector   | Description
 -----------|------------
 `x`        | _no selector_
 `*x`       | Make a copy.
 `&x`       | Borrow a reference.
 `&const x` | Borrow a const reference.
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

A temporary expression implicitly has the `&&x` bind selector attached as
metadata, causing it to be passed as a movable by default. Although it is
possible to override the selector of a temporary expression to `*x`,
`&const x` and `&&x`, it is not possible to override it to `x` or `&x`.

See the next chapter on how each bind selector influences how an assignment or
argument pass is bound.


## Bind specification

 | Parameter        | Variable               | Return           | Description
 |------------------|------------------------|------------------|------------
 | `fn(x : *)`      | `x : *      <- <init>` | `fn() -> *`      | Make a semantic copy
 | `fn(x : &)`      | `x : &      <- <init>` | `fn() -> &`      | Borrow a reference
 | `fn(x : &const)` | `x : &const <- <init>` | `fn() -> &const` | Borrow a const reference
 | `fn(x : &&)`     | `x : &&     <- <init>` | `fn() -> &&`     | Require a movable
 | `fn(x :)`        | `x :        <- <init>` | `fn() ->`        | Infer the binding mode
 |                  | `x          <- <init>` | `fn()`           | Guided by selector
 | `fn(x)`          |                        |                  | Shorthand for `fn(x : *)`


### `fn(x : *)`, `x : * <- <init>` - Make a semantic copy

A value parameter may be implemented as a const reference when doing so cannot
make an additional alias observable by the program:
 * The function only reads from the argument, and
 * The function does not let a const reference to this argument escape to
   another thread, and
 * No non-const references to the original value escaped to another thread, and
 * The type-selector used is not `*x`.

In other cases the value is copied.


### `fn(x : &)`, `x : & <- <init>` - Borrow a reference

When using the selectors `x`, `&x` or `&&x`, take a reference of a value when:
 * A non-const value is passed in.
 * A non-const reference is passed in.
 * A movable is passed in.

All other combinations are invalid. See [Invalid Pass]


### `fn(x : &const)`, `x : &const <- <init>` - Borrow a const reference

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


### `x <- <init>`

This binding specification is more directly guided by the binding selector.

 * `x` - Copy the value
 * `*x` - Make a copy of the value
 * `&x` - Borrow a reference
 * `&const x` - Borrow a const reference
 * `&&x` - Forward the binding mode based on the type:
   - `T` - Borrow a reference
   - `const T` - Borrow a const reference
   - `&T` - Borrow a reference
   - `&const T` - Borrow a const reference
   - `&&` - Borrow a movable


[Invalid Pass]: #invalid-pass
## Invalid Pass

Invalid arguments causes the function overload to be dropped as a candidate
for overload resolution.

Invalid initialization of variables and immutable will cause a compile-time error.


## Overload resolution

When passing a value, reference or movable to an overloaded function there are
priorities.

  rhs type or selector       | exact match | priority                          
 :---------------------------|:------------|:----------------------------------
 `*x`                        | `: *`       | `:`, `: &const`         
 `T`, `&T`, `&x`, `&&T`      | `: &`       | `:`, `: &const`, `: *`         
 `&const T`, `&const x`      | `: &const`  | `:`, `: *`                      
 `&&x` (including temporary) | `: &&`      | `:`, `: &const`, `: &`, `: *` 

