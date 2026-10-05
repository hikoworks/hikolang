# Bindings

Goals these binding rules:
 * Const correctness.
 * Default simple / no bind specification means mostly copy semantics.
 * Ordinary use of a variable doesn't accidentally give a function permission
   to consume it. An explicit `&move` or `&&` is required to cross that boundary.
 * Temporaries are expected to be consumed, `&move` or `&&` is implied.
 * Temporaries can not be used as in/out-parameter.


This examples shows how different kinds of bindings are initialized.

```
a <- 1.0                   // Move-qualified variable binding.
b : const <- 2.0           // Const-qualified variable binding.
r <- &a                    // Unqualified reference binding to the storage of `a`.
cr <- &const r             // Const-qualified reference binding to the storage of `a`
q <- &std.remove_const(&b) // Unqualified reference binding to the storage of `b`
m <- &move a               // Move-qualified reference binding to the storage of `a`.
```

As shown in the example above, there are three mutually exclusive qualifiers. An
expression can be passed to a binding according to the following rules:

| Binding \\ Expression | `&T`        | `&const T`  | `&move T`  | temporary   |
| --------------------- | ----------  | ----------  | ---------- | ----------  |
| `fn(x : &)`           | `&T`        | [_invalid_] | `&T`       | [_invalid_] |
| `fn(x : &const)`      | `&const T`  | `&const T`  | `&const T` | `&const T`  |
| `fn(x : &move)`       | [_invalid_] | [_invalid_] | `&move T`  | `&move T`   |
| `fn(x : *)`           | `move T`    | `move T`    | `move T`   | `move T`    |
| `fn(x :)`             | `&T`        | `&const T`  | `&move T`  | `&move T`   |


In the table above we show how arguments behave for function arguments; this
works identical for variable initializer and function return specification.

These simple-syntax versions are asymetrical between bindings:
 * The function argument: `fn(x)`:
   - copy the value. identical to `fn(x : *)`.
 * The variable initializer: `x <- <init>`:
   - when the `<init>` expression has an explicit binding operator; preserve the
     reference and its qualifiers.
   - otherwise; copy the value.
 * The (empty) return specification `fn()`:
   - when the `return` expression has an explicit binding operator; preserve
     the reference and its qualifiers.
   - otherwise; create a temporary.

> [!note]
> `fn(x : *)` and `fn(x)` may use the as-if rule to take the argument as `&const T` instead.


## Expressions

Every expression produces one of the following:
 * unqualified reference - `&T`
 * const-qualified reference - `&const T`
 * move-qualified reference - `&move T`
 * temporary value

A variable binding used in an expression is represented as-if a move-qualified
reference `&move T` (the next move fragility rule applies to the expression as a
whole), while a const-qualified variable binding is represented
as-if a const-qualified reference `&const T`.

A move-qualified expression is fragile: passing it to a binding removes
the move qualification, resulting in `&`; unless either the `&&` or `&move`
binding operators are used.

When an expression produces a temporary value, the value is materialized and
implicitly borrowed with the `&move` binding operator when passed to a binding.

```
a <- 1.0      // Move-qualified variable binding.
foo(a)        // The expression `a`, because `&move T` is fragile will select `foo(x : &)`
foo(&move a)  // The expression `&move a` will select `foo(x : &move)`
```

These are the binding operators that can be used to modify an expression:
 * `&` - Borrow an unqualified reference.
 * `&const` - Borrow a const-qualified reference.
 * `&move` - Explicitly borrow a move-qualified reference; preserves the move
             qualification when the expression is passed to a binding.
 * `&&` - Explicitly borrow and forward the reference; preserving its qualifiers
          when the expression is passed to a binding.
 * `*` - Explicitly copy the (referenced) value as a temporary value.

These binding operators can only be used to explicitly select from the set of
implicit conversion:

 | operator | valid expression types
 |----------|----------------------------
 | `&`      | `&move`, `&`
 | `&const` | `&move`, `&`, `&const`, temporary
 | `&move`  | `&move`, temporary
 | `&&`     | `&move`, `&`, `&const`, temporary
 | `*`      | `&move`, `&`, `&const`, temporary


## Explicit cast

Special standard library functions can cast references beyond the
implicit conversion, including less safe cast:

 * `std.remove_const(x)` - Remove const-qualifier from a reference.
 * `std.remove_move(x)` - Remove move-qualifier from a reference.
 * `std.add_const(x)` - Add const-qualifier from a reference.
 * `std.add_move(x)` - Add move-qualifier from a reference.

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

Unlike C++ `const` does not imply mutability of the value.

The semantics of const-qualified expressions, references and bindings are
described by this document from how binding will select overloaded functions
that may actually modify the value.

Additionally a member variable accessed through a const-qualified expression
can only be read fron.


## Moving

In this language moving is implemented as a non-destructive move like in C++.

The semantics of move-qualified expressions, references and bindings are
described by this document from how binding will select overloaded functions
that may actually consume the resources of the referenced value.

The rules regarding the consumption itself is described in: [Moving](moving.md)


[_invalid_]: #invalid-binding
## Invalid Binding

Invalid arguments causes the function overload to be dropped as a candidate
for overload resolution.

Invalid initialization of variables will cause a compile-time error.


## Overload resolution

These are the overload priorities based on the result of the expression
other with binding operator that is passed to an function argument.

  Expression  | exact match      | priority                          
 :------------|:-----------------|:----------------------------------
 `&T`         | `fn(x : &)`      | `fn(x :)`, `fn(x : &const)`, `fn(x)`
 `&const T`   | `fn(x : &const)` | `fn(x :)`, `fn(x)`
 `&move`      | `fn(x : &move)`  | `fn(x :)`, `fn(x : &const)`, `fn(x : &)`, `fn(x)`
 temporary    | `fn(x : &move)`  | `fn(x :)`, `fn(x : &const)`, `fn(x)`

