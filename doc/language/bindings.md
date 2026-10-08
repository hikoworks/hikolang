# Bindings

Goals these binding rules:
 * Const correctness.
 * Default simple / no bind specification means mostly copy-like semantics.
 * Ordinary use of a variable doesn't accidentally give a function permission
   to consume it. An explicit `&move` or `&&` is required to cross that boundary.
 * Temporaries are expected to be consumed.
 * Non-destructive move semantics.

## Types

 | Type       | Description                                                 | Expr result |
 |------------|-------------------------------------------------------------|:-----------:|
 | `T`        | The type of a value                                         |             |
 | `move T`   | The type of a move-qualified variable binding, or temporary |      +      |
 | `const T`  | The type of a const-qualified variable binding              |             |
 | `&T`       | The type of a unqualified reference binding                 |      +      |
 | `&move T`  | The type of a move-qualified reference binding              |      +      |
 | `&const T` | The type of a const-qualified reference binding             |      +      |



## Expressions

Every expression produces one of the following:
 * unqualified reference - `&T`
 * const-qualified reference - `&const T`
 * move-qualified reference - `&move T`
 * temporary value - `T` materialized as `move T` variable

Rules for how an expression is typed (all rules are applied in order):
 1. A move-qualified variable binding initially has the type `&move T`.
 2. A const-qualified variable binding initially has the type `&const T`.
 3. A reference binding initially retains its qualifier.
 4. A temporary is materialized as an anonymous move-qualified variable binding,
    then forwarded using the implied `&move` binding operator.
 5. Apply:
    - the (implied) binding operator, otherwise
    - fragility by converting `&move T` to `&T`.
 6. (optionally) Select function based on overload rules.
 7. Apply binding qualifier and bind.

```
a <- 1.0          // Move-qualified variable binding.
foo(a)            // `a` in an expression has type `&move T`, but the fragile expression
                  // loses its move qualification before binding, causing the type to be `&T`.
foo(&move a)      // The expression type `&move T` will select `foo(x : &move)`
foo(make_value()) // The expression type `move T` will select `foo(x : &move)`
```

Here is the table showing how an expression of a certain type is converted by
each binding operator:

 | op \ expr  | Description                                                 | `&T`        | `&const T`  | `&move T`
 |------------|-------------------------------------------------------------|-------------|-------------|------------
 |            |                                                             | `&T`        | `&const T`  | `&T` (fragile)
 | `&x`       | Strip move qualification                                    | `&T`        | `&const T`  | `&T`
 | `&const x` | Strip move qualification and require const access           | `&const T`  | `&const T`  | `&const T`
 | `&move x`  | Require move-capable source and preserve move qualification | [_invalid_] | [_invalid_] | `&move T`
 | `&&x`      | Preserve the expression's qualification exactly             | `&T`        | `&const T`  | `&move T`
 | `*x`       | Create a temporary from the expression                      | `move T`    | `move T`    | `move T`

An ordinary variable expression is intentionally fragile: its move qualification
is discarded unless an explicit binding operator preserves it. This way a
programmer can select exactly when a value may be consumed.


## Binding Qualifiers

As shown in the example above, there are three mutually exclusive qualifiers:
_unqualified_, _const-qualified_ and _move-qualified_. An expression-type after
applying the binding operator, can be passed to a binding, and result in a
binding-type according to the following table:

| Binding \\ Expression |                                              | `&T`        | `&const T`  | `&move T` |
|-----------------------|----------------------------------------------|-------------|-------------|-----------|
| `fn(a : &)`           | Bind as a unqualified reference              | `&T`        | [_invalid_] | `&T`      |
| `fn(a : &const)`      | Bind as a const-qualified reference          | `&const T`  | `&const T`  | `&const T`|
| `fn(a : &move)`       | Bind as a move-qualified reference           | [_invalid_] | [_invalid_] | `&move T` |
| `fn(a : *)`           | Bind as a move-qualified variable            | `move T`    | `move T`    | `move T`  |
| `fn(a :)`             | Bind as a reference, inferring qualification | `&T`        | `&const T`  | `&move T` |

> [!note]
> The `*` in a binding specification `fn(x : *)` and the prefix `*` expression
> operator `*x` are distinct language constructs with independent semantics.
> They are not interchangeable, but share a conceptual meaning: both explicitly
> express value semantics rather than reference semantics.

In the table above we show how arguments behave for function arguments; this
works identical for variable initializer and function return specification.

These simple-syntax versions are asymmetrical between bindings:
 * The function definition: `fn(a)` is identical to `fn(a : *)`.
 * The variable initializer: `x <- <init>`:
   - when the `<init>` expression has an explicit binding operator; preserve the
     reference and its qualifiers.
   - otherwise; bind as a move-qualified variable.
 * The (empty) return specification `fn()`:
   - when the `return` expression has an explicit binding operator; preserve
     the reference and its qualifiers.
   - otherwise; create a temporary.


## Overload resolution

These are the overload priorities based on the result of the expression
after the optional binding operator has been applied.

 | Expression type | preferred        | fallbacks in order
 |-----------------|------------------|---------------
 | `&T`            | `fn(a : &)`      | `fn(a :)`, `fn(a : &const)`, `fn(a : *)`
 | `&const T`      | `fn(a : &const)` | `fn(a :)`, `fn(a : *)`
 | `&move T`       | `fn(a : &move)`  | `fn(a :)`, `fn(a : &const)`, `fn(a : *)`, `fn(a : &)`


## Explicit cast

Special standard library functions can cast references beyond the
implicit conversion, including less safe cast:

 * `std.remove_const(x)` - Remove const-qualifier from a reference.
 * `std.remove_move(x)` - Remove move-qualifier from a reference.
 * `std.add_const(x)` - Add const-qualifier to a reference.
 * `std.add_move(x)` - Add move-qualifier to a reference.

Here is an example for a valid usage of removing the const qualifier:

```
my_type = struct {
    counter <- 0.0

    foo = fn(@self self : &const my_type) {
        self_nc <- &std.remove_const(self)
        self_nc.counter += 1.0
    }
}

co : const <- my_type()
co.foo()
```

## Constants

The semantics of const-qualified expressions, references and bindings are
described by this document from how binding will select overloaded functions
that may actually modify the value.

Additionally a member variable accessed through a const-qualified expression
can only be read from.


## Moving

Moving transfers the value from the source to the destination without ending the
source binding's lifetime. The source binding remains valid, but its value
enters a moved-from state.

Operations on a moved-from value are restricted to those explicitly permitted
for that state. Such operations may leave the value in a moved-from state or
establish a valid, fully specified state. A moved-from value may also be
reassigned.

The semantics of move-qualified expressions, references and bindings are
described by this document from how binding will select overloaded functions
that may actually consume the resources of the referenced value.

The rules regarding the consumption itself is described in: [Moving](moving.md)

Since we have not shown an example of moving an actual value yet, here is
an example:

```
a <- "Hello World"         // `a` is a move-qualified string value
b <- &move a               // Create a move-qualified reference, this does not move.
c : *std.string <- &move a // `c` is a move-qualified string value.
                           // The initializer selects a constructor taking
                           // `: &move`; that constructor consumes the value of `a`.

a <- "Hello Earth"
d : * <- &move a           // Equivalent to `c : *std.string <- &move a`
```


[_invalid_]: #invalid-binding
## Invalid Binding

Invalid arguments causes the function overload to be dropped as a candidate
for overload resolution.

Invalid initialization of variables will cause a compile-time error.


## Creating Bindings

### Variables

A variable binding associates a name with a value within a lexical scope.

A new variable binding is created when its name is not visible in the current
scope or any enclosing scope. Both `<-` and `=` can introduce a variable
binding:

```
a <- 1.0          // Move-qualified variable binding; with type `move T`
b : const <- 2.0  // Const-qualified variable binding; with type `const T`
c = 3.0           // Syntactic sugar for `c : const <- 3.0`.
```

A const qualifier applies to the access path to the value. It does not imply if
the binding itself is modifiable, neither does it imply that the value is
immutable.

The const qualifier is used to select functions based on their binding
qualifier, and restrict access to member variables. It is allowed to explicitly
cast a const-qualified reference to a unqualified reference to bypass these
restrictions.

A binding name cannot be introduced when a binding with the same name is already
visible in the current scope or any enclosing scope. Therefore, variables cannot
shadow variables in an outer scope.

```
a <- 1.0
{
    a <- 2.0  // Updates the existing `a`.
}
```

A variable binding definition can have annotations that affect the storage and
ownership of its value. The `@shared` annotation stores the value in
reference-counted heap storage. The binding automatically retains the value when
initialized and releases it when the binding's lifetime ends.
```
@shared d <- "Hello"     // Move-qualified shared variable binding.
                         // `d` owns a reference to heap storage.
```

Types and functions are treated as normal values, so the same variable
binding definition syntax is used for these.

```
f = fn(int : *)           // Const-qualified variable binding containing
                          // a function overload set with one function.

t = enum (int[0...1]) {   // Const-qualified variable binding containing an open enum.
    foo
}
```

### References

A reference binding associates a name with the storage of an existing value
within a lexical scope.

A new reference binding is created when its name is not visible in the current
scope or any enclosing scope. Both `<-` and `=` can introduce a reference
binding when the initializer produces a reference:

```text
a <- 1.0

r <- &a             // Unqualified reference binding.
cr <- &const a      // Const-qualified reference binding.
mr <- &move a       // Move-qualified reference binding.
```

A reference binding does not copy or move the referenced value. It simply refers
to the storage of that value.

As with variable bindings, `=` creates a const-qualified binding. It does not
make the referenced value const.

```text
a <- 1.0

r : const <- &a     // Const-qualified reference binding.
r2 = &a             // Syntactic sugar for `r2 : const <- &a`.
```

`r` is const-qualified, while the referenced storage, owned by `a`, remains
mutable.

## Updating Bindings

The meaning of `=` depends on whether the binding already exists: when creating
a binding it creates a const-qualified binding; when updating an existing
binding it calls `__merge__()`.

### Variables

When a binding already exists, `<-` and `=` update the existing binding rather
than creating a new one.

`<-` calls `__assign__()`:

```text
a <- 1.0
a <- 2.0       // Calls `a.__assign__(2.0)`.

b = 1.0        // Creates a const-qualified variable binding
//b = 2.0      // ERROR: `b.__merge__(2.0)` is not supported by float type.
```

`=` calls `__merge__()`:

```text
t = enum int[0...1] { foo }
t = enum { bar }            // Calls `t.__merge__(enum { bar })`
```

`__merge__()` receives the existing value through a const-qualified reference.
Const qualification does not imply immutability; it provides the const-qualified
view required by the binding and overload-resolution rules.

The use of `__merge__()` for `=` provides symmetry with `<-`:

 * `<-` introduces and updates values whose value changes through `__assign__()`.
 * `=` introduces values that are intended to be mostly constant and updates
       them through `__merge__()`.

For types and functions, `=` is primarily used to combine definitions at
compile time. Function values use `__merge__()` to combine functions into a
function overload set. Types can use `__merge__()` to combine their members, and
type templates use the same mechanism to form overload sets.

Thus, a const-qualified binding may still be updated. The const qualifier
affects how the binding is viewed and which operations are selected; it does
not by itself make the value immutable.

Function values use `__merge__()` to combine functions into a function overload
set:

```text
f = fn(a : *int) {}
f = fn(a : *string) {}
```

Types can use `__merge__()` to combine their members, and type templates use the
same mechanism to form overload sets.

The semantics of `__assign__()` and `__merge__()` are defined by the type. An
assignment or merge is invalid when the corresponding operation is not
supported by the value.


### Reference

Once a reference binding exists, `<-` and `=` operate on the referenced value
when the right-hand expression can be evaluated to a value. When the right-hand
expression has an explicit reference binding operator `&&x`, `&x`, `&const x`,
or `&move x`, the reference binding is reseated instead.

```text
a <- 1.0
b <- 2.0

r <- &a
r <- 2.0       // Calls `r.__assign__(2.0)`, modifying `a`.
r <- &b        // Reseat `r`; `a` is unchanged.
```

Using an explicit bind operators `&&`, `&x`, `&const x` or `&move x` will reseat
the reference binding instead of calling `__assign__()`. Reseating a reference
does not change its binding or reference qualification.
 

```text
r = &a
r <- &b        // Reseat `r`; `r` remains const-qualified.

cr <- &const a
cr <- &const b // Reseat `cr`; its reference remains const-qualified.
cr <- &a       // Reseat `cr`; `&a` is convertible to `&const T`.

mr <- &move a
mr <- &move b  // Reseat `mr`; its reference remains move-qualified.
// mr <- &a    // ERROR: unqualified reference can not be upgraded to a move-qualified reference.
```

The new reference must have compatible reference qualification as the existing
reference binding. Reseating does not copy or move either the old or new
referenced value.

A move-qualified reference does not itself move the referenced value:

```text
a <- "Hello World"
r <- &move a   // Creates a move-qualified reference. The value of `a` is unchanged.
```

The value can be moved only when the move-qualified reference is subsequently
passed to a binding that may consume it. For example by explicitly passing a
move-qualified reference to a variable binding:

```text
c : * <- &move a  // `c` is a value; `&move a` selects a move-consuming initializer.
```

Reference bindings therefore provide a raw reference to existing storage. They
do not own, copy, or move the referenced value merely by being initialized or
reseated.
