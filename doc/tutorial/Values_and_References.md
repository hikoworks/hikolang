# Values and References

When working with data in [language name], it is important to distinguish
between **values** and **references**.

The key question is:

> **Who manages the storage containing this object?**

A **value** manages its own storage. A **reference** does not manage the storage
it refers to; it only provides access to storage managed somewhere else.

Hikolang has three value categories:
 * **value** - a literal value, an expression result, or a variable that manages
   the storage of a value.
 * **reference** - a reference to storage managed by something else.
 * **move-reference** - a reference that permits the internals of the referenced
   object to be consumed.

This distinction affects how variables are initialized, how function arguments
are passed, and whether modifying an object modifies the original object or a
copy.

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
reference is written as `&T`, where `T` is the referenced type.

For example:

```
// x manages the storage containing 42.0.
x := 42.0

// y refers to x's storage.
y := &x
```

The two variables therefore have different roles:

```
x ──manages──> [42.0]
                ▲
                │
y ──refers──────┘
```

Changing the value through `y` changes the value stored by `x`, because both access the same storage.


### References are not nested

References cannot be nested. Consequently, `&&T` does **not** mean "a reference
to a reference". Instead, `&&T` is the type of a **move-reference**. This
distinction is important when reading declarations:
 * `&T` - reference
 * `&&T` - move-reference

Taking the reference of a reference therefore still produces a reference to the underlying storage:

```
// x is a value.
x := 42.0

// y refers to x.
y := &x

// z also refers to x.
// It does not refer to y as a separate object.
z := &y
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

### Binding selector vs. Binding specification 

Up to this point we have seen how to use a binding selector. A selector is an
operator which adds a preferred binding-method to an expression. This preferred
binding-method is then consumed by the:
 - variable initializer,
 - immutable initializer, or the
 - function parameter of a function call.

In this example we see `&x` be passed as an initializer and as afunction
argument, setting the preferred binding method to a reference:

```
foo = fn(a) {
    return a
}

x = 42.0
y = &x
z = foo(&x)
```

A binding specification is made on the actual variable or function parameter,
it follows the type-inference-operator `<-`. In this example we use the
binding specification with the same result as the previous example:

```
foo = fn(a <- &) {
    return a
}

x = 42.0
y <- & = x
z = foo(x)
```

The binding specification is evaluated after the binding selector:

```
x = 42.0
y = &x

a = y           // y:   reference                  a: value (no selector: default)
b <- = y        // y:   reference                  b: reference (no selector: inferred)
c <- & = &x     // &x:  reference (selector)       c: reference (selector overridden)
d <- & = &&x    // &&x: move-reference (selector)  d: reference (selector overridden)

//e <- && = &x  // ERROR cannot make move-reference from reference
```

The binding-method attached to the explicit return type specification on a
function should be seen as a binding specification.

```
// return type specification acts as-if '->' is an inference operator.
foo = fn(a <- &) -> & {
    return a
}

x = 42.0

a = foo(x)       // a: copy of x
b <- = foo(x)    // b: reference to x
c <- & = foo(x)  // c: reference to x
d = &foo(x)      // d: reference to x
```

### Materialization



Consider:

```
foo = fn(a <- &) -> & {
    a := a + 1.0
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

The operator `<-` is a type-inference operator.

For example:

```
a <- := x
```

means that the binding of `a` is inferred from `x`.

The binding specification can explicitly select whether the result is bound as a value, reference, const value, or move-reference.

### The basic forms

The most common binding specifications are:

```
a := x
a <- := x
a <- * := x
a <- & := x
a <- &const := x
a <- && := x
```

The difference between them is easiest to understand as a set of binding operations:

 Binding        | Meaning
 :------------- |:-------------------------------------
 `:=`           | No binding specified. By default bind as value. Or use the binding selector expression.
 `<- :=`        | Infer the binding from the expression
 `<- * :=`      | Bind as a value
 `<- & :=`      | Bind as a reference
 `<- &const :=` | Bind as a const reference
 `<- && :=`     | Bind as a move-reference

The distinction between `:=` and `<- :=` is particularly important: `:=` by
default chooses a value binding, whereas `<- :=` uses the source expression's
binding information to participate in inference.


### Binding to variables and function parameters

The following table shows the resulting type of a variable initialized with each
binding specification. The type of `x` or function-argument is in the horizontal
header:

 Variable binding   | Parameter binding  | `T`        | `const T`   | `&T`       | `&const T` | `&&T`      |
 :----------------- |:------------------ | ---------- | ----------- | ---------- | ---------- | ---------- |
 `a := x`           | `foo(a)`           | `T`        | `T`         | `T`        | `T`        | `T`        |
 `a <- := x`        | `foo(a <-)`        | `T`        | `T`         | `&T`       | `&const T` | `&&T`      |
 `a <- * := x`      | `foo(a <- *)`      | `T`        | `T`         | `T`        | `T`        | `T`        |
 `a <- & := x`      | `foo(a <- &)`      | `&T`       | `&const T`  | `&T`       | `&const T` | `&T`       |
 `a <- &const := x` | `foo(a <- &const)` | `&const T` | `&const T`  | `&const T` | `&const T` | `&const T` |
 `a <- && := x`     | `foo(a <- &&)`     | `&&T`      | `&&const T` | -          | -          | `&&T`      |

There are two useful ways to remember this table:

 1. a plain `:=` means by default **make a value binding**; but this may be
    selector overridden using one off the binding expressions: `*x`, `&x`,
    `&const x` or `&&x`.
 2. an explicit binding specification such as `*`, `&`, `&const` or `&&`
    requests that kind of binding.

For example:

```
x: &T

a := x
```

makes `a` a value:

```
a: T
```

whereas:

```
a <- & := x
```

makes `a` a reference:

```
a: &T
```

#### Value binding

The `*` binding selector explicitly requests a value binding.

For example:

```
a <- * := x
```

If `x` is a reference, `a` receives the referenced value rather than retaining the reference.

Conceptually:

```
x ──refers──> [42]

a <- * := x

a ──manages──> [42]
```

The storage managed by `a` is distinct from the storage referenced by `x`.

The `*` selector therefore provides an explicit way to say:

> "Bind the value, not the reference."

#### Reference binding

The `&` selector explicitly requests a reference binding:

```
a <- & := x
```

For example:

```
x := 42.0
y <- & := x
```

Now:

```
x ──manages──> [42.0]
                ▲
                │
y ──refers──────┘
```

The two names refer to the same storage.

A const reference can be requested with `&const`:

```
a <- &const := x
```

This produces:

```
&const T
```

The reference can access the object, but cannot be used to modify it through that reference.

### Binding to immutables

The following table shows the resulting type of a immutable initialized with each
binding specification. The type of `x` is in the horizontal
header:

 Immutable binding | `T`         | `const T`   | `&T`       | `&const T` | `&&T`
 :---------------- |:----------- |:----------- |:---------- |:---------- |:-----------
 `a = x`           | `const T`   | `const T`   | `const T`  | `const T`  | `const T`
 `a <- = x`        | `const T`   | `const T`   | `&const T` | `&const T` | `&&const T`
 `a <- * = x`      | `const T`   | `const T`   | `const T`  | `const T`  | `const T`
 `a <- & = x`      | `&const T`  | `&const T`  | `&const T` | `&const T` | `&const T`
 `a <- &const = x` | `&const T`  | `&const T`  | `&const T` | `&const T` | `&const T`
 `a <- && = x`     | `&&const T` | `&&const T` | -          | -          | `&&const T`


An immutable binding is created with `=` rather than `:=`.

For example:

```
x = 42.0
```

The immutable binding cannot be modified or reseated.

Immutability applies to the binding itself. For a value, this means that the
value cannot be modified. For a reference, it also means that the reference
cannot be reseated.

This can be understood by remembering that a reference is itself a value
representing a location.

For example:

```
x := 42.0
r <- & := x
```

Here `r` is a reference whose value identifies the storage managed by `x`.

Making `r` immutable freezes that reference:

```
r <- & = x
```

The object referred to by `r` is not necessarily const. The reference itself is immutable.

This gives us an important distinction:

```
&T          mutable reference to mutable T
&const T    reference to const T
const T     immutable value
const &T    immutable reference to mutable T
```

In practice, the binding rules normalize these concepts so that immutable bindings add `const` to the appropriate type.

## Immutable binding inference



Notice the difference between:

```
a = x
```

and:

```
a <- = x
```

The first always creates an immutable value binding.

The second infers the binding from `x`, while also applying the immutability of the new binding.

For example, if `x` is `&T`:

```
a = x
```

produces a const value binding, while:

```
a <- = x
```

preserves the fact that `x` is a reference:

```
a: &T
```

but makes the reference binding immutable.

## Binding Selectors

A **binding selector** is attached to an expression to explicitly describe how
that expression should be bound.

The selectors are:

```
const x
*x
*const x
&x
&const x
&&x
```

These selectors are consumed by a variable initializer, immutable initializer, or function parameter.

Think of a binding selector as metadata attached to the expression:

```
expression
    │
    └── binding selector
             │
             ▼
       determines binding
```

## Value selectors

The `*` selector requests a value:

```
*x
```

The `*const` selector requests a const value:

```
*const x
```

For example:

```
a := *x
```

creates a value binding regardless of whether `x` itself is a reference.

Likewise:

```
a := *const x
```

requests a const value.

With the ordinary `:=` binding, however, the binding is still a value binding, so the const information does not necessarily appear in the resulting type.

## Reference selectors

The `&` selector requests a reference:

```
&x
```

For example:

```
a := &x
```

makes `a` a reference to the storage of `x`.

The `&const` selector requests a const reference:

```
a := &const x
```

This prevents modification of the referenced object through `a`.

## Move-reference selectors

The `&&` selector requests a move-reference:

```
a := &&x
```

A move-reference is different from an ordinary reference because it allows the internals of the referenced object to be **consumed**.

The object from which the contents are moved remains valid, but its state is **indeterminate**.

This is useful when an operation needs to take ownership of an object's internal resources without requiring the object itself to become invalid.

Conceptually:

```
x ──manages──> [object]
                 ▲
                 │
a ──move-ref─────┘
```

After consuming the internals through `a`, `x` still denotes a valid object, but its contents should no longer be assumed to have their previous state.

## Binding selectors in practice

The binding selector becomes especially useful when the expression and the desired binding do not naturally match.

For example:

```
x := 42.0

a := &x
b <- * := x
c <- &const := x
```

These produce three different relationships with `x`:

```
x ──manages──> [42.0]
                ▲
                │
a ──refers──────┘

b ──manages──> [42.0]    // separate value

c ──refers────> [42.0]    // const reference
```

The selector makes the programmer's intent explicit.

# Move-References

A move-reference has the type:

```
&&T
```

It should not be confused with a reference to a reference. References cannot be nested, so `&&T` has its own meaning.

A move-reference permits the referenced object's internals to be consumed.

The important consequence is that moving does **not** make the source object invalid.

Instead:

1. the internals of the source object may be consumed;
2. the source object remains valid;
3. its state becomes indeterminate.

This is particularly useful for types that manage resources internally.

For example, a container might own a dynamically allocated buffer. A move-reference could allow an operation to consume that buffer rather than copying every element.

Afterward, the original container still exists, but its contents should not be assumed to be the same as before the move.

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
:=             value
<- :=          infer binding
<- * :=        value
<- & :=        reference
<- &const :=   const reference
<- && :=       move-reference
```

Binding selectors perform the corresponding operation on expressions:

```
*x            value
*const x      const value
&x            reference
&const x      const reference
&&x           move-reference
```

Consider the following example:

```
x := 42.0

value := x
reference <- & := x
const_reference <- &const := x
```

The result is:

```
value            ──manages──> [42.0]

reference        ──refers────> [42.0]

const_reference  ──refers────> [42.0]
```

There is one object containing `42.0`, managed by `x`. The other bindings either create their own value or refer to that existing storage.

Once this distinction is understood, the binding tables become much easier to read: they simply describe how the language preserves, discards, or explicitly changes this binding information when an expression is used to initialize another binding or passed to a function.

## Quick Reference

| Syntax                | Meaning                           |
| --------------------- | --------------------------------- |
| `x`                   | use the expression's value        |
| `*x`                  | bind the value                    |
| `*const x`            | bind a const value                |
| `&x`                  | bind a reference                  |
| `&const x`            | bind a const reference            |
| `&&x`                 | bind a move-reference             |
| `x: T`                | explicitly typed value            |
| `x: &T`               | explicitly typed reference        |
| `x: &&T`              | explicitly typed move-reference   |
| `x := expr`           | create a value binding            |
| `x <- := expr`        | infer the binding from `expr`     |
| `x <- * := expr`      | explicitly bind a value           |
| `x <- & := expr`      | explicitly bind a reference       |
| `x <- &const := expr` | explicitly bind a const reference |
| `x <- && := expr`     | explicitly bind a move-reference  |

The central rule to remember is simple:

> **Values manage storage; references access storage managed elsewhere; move-references allow that storage's contents to be consumed.**

Everything else in the binding system exists to precisely control which of these relationships is established.
