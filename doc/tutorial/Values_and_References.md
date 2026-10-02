# Value, References and Movables

Hikolang has three expression categories:
 * **value** - a value located in managed storage, or the temporary result of an
   expression.
 * **reference** - a reference to storage managed by something else.
 * **movable** - a reference that permits the internals of the referenced
   object to be consumed.

This distinction affects how variables are initialized, how function arguments
are passed, and whether modifying an object modifies the original object or a
copy.

Here are the possible bindings, including constness, and which conversions
can take place:

```
    const T ←────── T
       │            │
       │       ┌────┴────┐
       │       ↓         ↓
       │      &T ←────  &&T
       │       │
       │       ↓
       └──→ &const T
```

## Values

A value is the ordinary form of data in Hikolang. A value can be:

* a literal,
* the result of an expression, or
* a variable or immutable that binds and manages the storage of a value.

For example:

```
// Argument 'a' receives a copy of the value passed into it.
foo = fn(a) {
    return a
}

x = 42.0
y = foo(x)
```

Here, `x` manages the storage containing `42.0`. When `x` is passed to `foo`,
the value is copied into the function's parameter `a`. The function then returns
that value, and `y` is initialized with the returned value.

Conceptually:

```
x ──manages──> [42]

foo(x)
a ──manages──> [42]    // copy

y ──manages──> [42]    // returned value also copied
```

The important point is that `x`, `a` and `y` manage separate storage locations.


## References

A reference provides access to storage managed by something else. The type of a
reference is written as `&T`, where `T` is the referenced type. For example:

```
// x manages the storage containing 42.0.
x <- 42.0

// y refers to x's storage.
y <- &x
```

The two variables therefore have different roles:

```
x ──manages──> [42.0]
                ▲
                │
y ──refers──────┘
```

Changing the value through `y` changes the value stored by `x`, because both
access the same storage.


### References are not nested

References cannot be nested. Consequently, `&&T` does **not** mean "a reference
to a reference". Instead, `&&T` is the type of a **movable**. This
distinction is important when reading declarations:
 * `&T` - reference
 * `&&T` - movable

Taking the reference of a reference therefore still produces a reference to the
underlying storage:

```
// x is a value.
x <- 42.0

// y refers to x.
y <- &x

// z also refers to x.
// It does not refer to y as a separate object.
z <- &y
```

Conceptually:

```
x ──manages──> [42.0]
                ▲ ▲
                │ │
y ──refers──────┘ │
                  │
z ──refers────────┘
```

## movables

A movable is a reference with additional permission to move from the referenced
object. The type of a movable is `&&T`; since references can't be nested
this is not ambiguous.

A movable permits the referenced object's internals to be consumed. However
the referenced object's needs to maintain its invariant. So the following
must apply to an object after it was moved from:

 * the internals of the source object may be consumed;
 * the source object remains valid;
 * its state becomes indeterminate.

This is particularly useful for types that manage resources internally.

For example, a container might own a dynamically allocated buffer. A movable
could allow an operation to consume that buffer rather than copying every
element.

Afterward, the original container still exists, but its contents should not be
assumed to be the same as before the move.

## Binding selector vs. Binding specification 

Up to this point we have seen how to use a binding selector. A selector is an
operator which adds a preferred binding-method to an expression. This preferred
binding-method is then consumed by the:
 - variable initializer,
 - immutable initializer, or the
 - function parameter of a function call.

In this example we see `&x` (binding selector) be passed as an initializer and
as a function argument, setting the preferred binding method to a reference:

```
foo = fn(a) {
    return a
}

x = 42.0
y = &x
z = foo(&x)
```

A binding specification is made on the actual variable or function parameter,
it follows the type-inference-operator `:`. In this example we use the
binding specification with the same result as the previous example:

```
foo = fn(a : &) {
    return a
}

x = 42.0
y : & = x
z = foo(x)
```

The binding specification is evaluated after the binding selector:

```
x = 42.0
y = &x

a = y          // y:   reference            a: value     (default)
b : = y        // y:   reference            b: reference (inferred)
c : = &y       // y:   reference (make ref) b: reference (inferred)
d : = &&x      // x:   movable   (forward)  b: movable   (inferred)
d : = &&y      // y:   reference (forward)  b: reference (inferred)
e : & = &x     // &x:  reference (make ref) c: reference (overridden)
f : & = &&x    // &&x: movable   (forward)  d: reference (overridden)

//g : && = &x  // ERROR cannot make movable from reference
```

The binding-method attached to the explicit return type specification on a
function should be seen as a binding specification.

```
// return type specification acts as-if '->' is an inference operator.
foo = fn(a : &) -> & {
    return a
}

x = 42.0

a = foo(x)       // a: copy of x
b : = foo(x)     // b: reference to x
c : & = foo(x)   // c: reference to x
d = &foo(x)      // d: reference to x
```

### Materialization

When a reference or movable is taken from the temporary result of an
expression, the result is being materialized. The materialization is as-if a
anonymous variable is created in the same block as the expression. As such these
anonymous variables are treated in the same way as normal variables in regard
to life-time rules.

Consider:

```
foo = fn(a : &) -> & {
    a <- a + 1.0
    return a
}

x = foo(1.0)
```

This return specification `-> &` has an **explicit binding selector**.

The `1.0` argument is materialized in the caller's block. The function receives
a reference to that materialized value and increments it:

```
<materialized storage> ──manages──> [1.0]
                                      ▲
                                      │
a ──refers────────────────────────────┘
```

After the function executes:

```
<materialized storage> ──manages──> [2.0]
                                      ▲
                                      │
x ──refers────────────────────────────┘
```

Because the function's result is explicitly bound as a reference, `x` becomes a
reference to the materialized value rather than a new value containing a copy.

This is one of the reasons binding information is part of the type-inference
process.

## Binding Specifications

A **binding specification** tells Hikolang how a variable, immutable, or
function parameter should infer its type.

The operator `:` is a type-inference operator.

For example:

```
a : <- x
```

means that the binding of `a` is inferred from `x`.

The binding specification can explicitly select whether the result is bound as a
value, reference, const value, or movable. The most common binding
specifications are:

```
a <- x             // By default: bind as value,
                   // otherwise preserve the explicit binding selector of 'x'.
a : <- x           // Bind by preserving the value-category of the expression.
a : * <- x         // Bind as a value
a : & <- x         // Bind as a reference
a : &const <- x    // Bind as a const reference
a : && <- foo()    // Bind as a movable (only explicit moves, or temporaries)
```

`a <- x` does not have a binding specification, and will therefore by default
make a copy of the result of the expression and bind that as a value. In
previous chapters we have used `a <- &x` to add an explicit binding selector to
`x` in that case `a` will conform to that explicit binding.

The following table shows the resulting type of a variable initialized with each
binding specification. The type of `x` or function-argument is in the horizontal
header:

 Selector   | Description
 :----------|:-------------
 `x`        | No selector, see type-specification
 `*x`       | Request to copy the value, this may materialize a temporary.
 `&x`       | Request a reference to the value
 `&const x` | Request a const reference to the value
 `&&x`      | Forward the value, reference or movable

 Type specfication | Description
 :-----------------|:-----------------------------
 `fn(a)`           | Efficient argument passing
 `fn(a : *)`       | Copy the value
 `fn(a : &)`       | Require a reference
 `fn(a : &const)`  | Require a const reference
 `fn(a : &&)`      | Require a movable
 `fn(a :)`         | Infer passing mode


 * `v`: variable; `T`
 * `cv`: const variable; `const T`
 * `r`: reference; `&T`
 * `cr`: reference to const; `&const T`
 * `m`: movable; `&&T`
 * `t`: temporary from expression; `&&T`

 Sel \\ Spec  | `fn(a)`    | `fn(a:*)` | `fn(a:&)` | `fn(a:&const)` | `fn(a:&&)` | `fn(a:)`
 :------------|:---------- |:----------|:----------|:---------------|:-----------|:----------
 `v`          | `&const T` | `T`       | `&T`      | `&const T`     | -          | `&T`
 `*v`         | `T`        | `T`       | -         | `&const T`     | -          | `T`
 `&v`         | `&const T` | `T`       | `&T`      | `&const T`     | -          | `&T`
 `&const v`   | `&const T` | `T`       | -         | `&const T`     | -          | `&const T`
 `&&v`        | `&const T` | `T`       | `&T`      | `&const T`     | `&&T`      | `&&T`
 `cv`         | `&const T` | `T`       | -         | `&const T`     | -          | `&const T`
 `*cv`        | `T`        | `T`       | -         | `&const T`     | -          | `T`
 `&cv`        | `&const T` | `T`       | -         | `&const T`     | -          | `&const T`
 `&const cv`  | `&const T` | `T`       | -         | `&const T`     | -          | `&const T`
 `&&cv`       | `&const T` | `T`       | -         | `&const T`     | -          | `&const T`
 `r`          | `&const T` | `T`       | `&T`      | `&const T`     | -          | `&T`
 `*r`         | `T`        | `T`       | -         | `&const T`     | -          | `T`
 `&r`         | `&const T` | `T`       | `&T`      | `&const T`     | -          | `&T`
 `&const r`   | `&const T` | `T`       | -         | `&const T`     | -          | `&const T`
 `&&r`        | `&const T` | `T`       | `&T`      | `&const T`     | -          | `&T`
 `cr`         | `&const T` | `T`       | -         | `&const T`     | -          | `&const T`
 `*cr`        | `T`        | `T`       | -         | `&const T`     | -          | `T`
 `&cr`        | `&const T` | `T`       | -         | `&const T`     | -          | `&const T`
 `&const cr`  | `&const T` | `T`       | -         | `&const T`     | -          | `&const T`
 `&&cr`       | `&const T` | `T`       | -         | `&const T`     | -          | `&const T`
 `m`          | `&const T` | `T`       | `&T`      | `&const T`     | -          | `&T`
 `*m`         | `T`        | `T`       | -         | `&const T`     | -          | `T`
 `&m`         | `&const T` | `T`       | `&T`      | `&const T`     | -          | `&T`
 `&const m`   | `&const T` | `T`       | -         | `&const T`     | -          | `&const T`
 `&&m`        | `&const T` | `T`       | `&T`      | `&const T`     | `&&T`      | `&&T`
 `t`          | `&const T` | `T`       | `&T`      | `&const T`     | `&&T`      | `&&T`
 `*t`         | `T`        | `T`       | -         | `&const T`     | -          | `T`
 `&t`         | `&const T` | `T`       | `&T`      | `&const T`     | -          | `&T`
 `&const t`   | `&const T` | `T`       | -         | `&const T`     | -          | `&const T`
 `&&t`        | `&const T` | `T`       | `&T`      | `&const T`     | `&&T`      | `&&T`
 

### Overload resolution

When passing a value, reference or movable to an overloaded function there are
priorities.

  rhs expression                          | exact      | priority                          
 :--------------------------------------- |:---------- |:----------------------------------
 `x : *T`, `*x`                           | `: *`      | `:`, `: &const`, `: &`         
 `x : &T`, `&x`, `x : &&T`, `&&(x : &T)`  | `: &`      | `:`, `: &const`, `: *`         
 `x : &const T`, `&const x`               | `: &const` | `:`, `: *`                      
 `&&x`, temporary                         | `: &&`     | `:`, `: &const`, `: &`, `: *` 

> [!note]
> `x : &&T` is passed as-if `x : &T`; because movable objects are not implicitly
> passed as movable.
>
> `&&(x : &T)` Forwards a reference, since a reference to `x` is not a movable.

Temporary results of expressions (not a name by itself) may be implicitly passed
as a movable. You may also explicitly pass a movable using the `&&x` syntax.

This shows passing of movables to functions:

```
foo = fn(a : &) { }
foo = fn(a : &&) { }

x = 42.0
y : = &&x          // y has the possession of move capability
foo(y)             // fn(a : &): don't exercise that capability
foo(&&y)           // fn(a : &&): explicitly exercise/select moveable overload
foo(make_value())  // fn(a : &&): temporary → implicitly movable
```


### Binding to immutables

The following table shows the resulting type of an immutable initialized with
each binding specification. The type of `x` is in the horizontal header:

 Immutable binding | `T`         | `const T`   | `&T`       | `&const T` | `&&T`
 :---------------- |:----------- |:----------- |:---------- |:---------- |:-----------
 `a = x`           | `const T`   | `const T`   | `const T`  | `const T`  | `const T`
 `a : = x`         | `const T`   | `const T`   | `&const T` | `&const T` | -
 `a : * = x`       | `const T`   | `const T`   | `const T`  | `const T`  | `const T`
 `a : & = x`       | `&const T`  | `&const T`  | `&const T` | `&const T` | `&const T`
 `a : &const = x`  | `&const T`  | `&const T`  | `&const T` | `&const T` | `&const T`
 `a : && = x`      | -           | -           | -          | -          | -


An immutable binding is created with `=` rather than `<-`.

For example:

```
x = 42.0
```

The immutable binding cannot normally be modified or reseated.

Immutability applies to the binding itself. For a value, this means that the
value cannot be modified. For a reference, it also means that the reference
cannot be reseated.

This can be understood by remembering that a reference is itself a value
representing a location.

For example:

```
x <- 42.0
r : & <- x
s : & = x
```

Here `r` and `s` are references whose value identifies the storage managed by
`x`. The object referred to by `s` is not necessarily const. The reference
itself is immutable.

This gives us an important distinction:

 Type     | Description
 :--------|:------------------
 &T       | mutable reference to mutable T
 &const T | reference to const T
 const T  | immutable value
 const &T | immutable reference to mutable T

In practice, the binding rules normalize these concepts so that immutable
bindings add `const` to the appropriate type.

## Immutable binding inference


```
x <- 42.0   // x : f64
y <- &x     // y : &f64

a = y       // a : const f64
b <- y      // b : f64

c : = y     // c : const &f64
d : <- y    // d : &f64
```


## Binding Selectors

A **binding selector** is attached to an expression to explicitly describe how
that expression should be bound.

The selectors are:

 Selector        | Description
 :-------------- |:-----------------------
 `a <- x`        | Pass `x` by value or reference
 `a <- const x`  | Pass `x` by value or const reference
 `a <- *x`       | Copy the value of `x`
 `a <- *const x` | Copy the value of `x`
 `a <- &x`       | Pass `x` by reference
 `a <- &const x` | Pass `x` by const reference
 `a <- &&x`      | Forward `x` by value, reference, const reference or movable

These selectors are consumed by:
 - a variable initializer,
 - an immutable initializer,
 - a function parameter.

Think of a binding selector as metadata attached to the expression, used by
the selector's consumer to change its behavior


## Forwarding selector

The `&&` selector gives explicit permission to be consumed:

```
a <- &&x
```




# Putting It All Together

The value/reference system can be summarized by separating two concepts:

### What does the name refer to?

A binding can manage storage itself or refer to storage managed elsewhere.

```
T       manages storage
&T      refers to storage
&&T     refers to storage and permits consumption
```

### How is the binding inferred?

The binding specification determines how the variable, immutable, or parameter is bound.

```
<-             value
: <-          infer binding
: * <-        value
: & <-        reference
: &const <-   const reference
: && <-       movable
```

Binding selectors perform the corresponding operation on expressions:

```
*x            value
*const x      const value
&x            reference
&const x      const reference
&&x           movable
```

Consider the following example:

```
x <- 42.0

value <- x
reference <- & <- x
const_reference <- &const <- x
```

The result is:

```
value            ──manages──> [42.0]

reference        ──refers────> [42.0]

const_reference  ──refers────> [42.0]
```

There is one object containing `42.0`, managed by `x`. The other bindings either create their own value or refer to that existing storage.

Once this distinction is understood, the binding tables become much easier to read: they simply describe how the language preserves, discards, or explicitly changes this binding information when an expression is used to initialize another binding or passed to a function.


