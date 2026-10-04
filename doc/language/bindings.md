# Bindings

This examples shows how different kinds of bindings are initialized.

```
a <- 1.0               // Move-qualified variable binding.
b : const <- 2.0       // Const-qualified variable binding.
r <- &a                // Unqualified reference binding to the storage of `a`.
cr <- &const r         // Const-qualified reference binding to the storage of `a`
q <- &remove_const(&b) // Unqualified reference binding to the storage of `b`
m <- &move a           // Move-qualified reference binding to the storage of `a`.
```

As shown in the example above, there are three mutually exclusive qualifiers. An
expression can be passed to a binding according to the following rules:

| Binding \\ Expression | `&T`        | `&const T`  | `&move T`  | temporary   |
| --------------------- | ----------  | ----------  | ---------- | ----------  |
| `fn(x : &)`           | `&T`        | [_invalid_] | `&T`       | [_invalid_] |
| `fn(x : &const)`      | `&const T`  | `&const T`  | `&const T` | `&const T`  |
| `fn(x : &move)`       | [_invalid_] | [_invalid_] | `&move T`  | `&move T`   |
| `fn(x :)`             | `&T`        | `&const T`  | `&move T`  | `&move T`   |
| `fn(x)`               | `move T`    | `move T`    | `move T`   | `move T`    |

In the table above we show how arguments behave for function arguments. This is
mostly true for variable initializers and the function return specification as
well, with the following exceptions:
 * The variable initializer: `x <- <init>`:
   - When the `<init>` expression has an explicit binding operator; preserve the
     reference and its qualifiers.
   - otherwise; copy the value.
 * The (empty) return specification `fn()`:
   - When the `return` expression has an explicit binding operator; preserve
     the reference and its qualifiers.
   - otherwise; create a temporary.

Additionally write access to member variables of a value can only be done
through a unqualified or move-qualified expression.

> [!note]
> `fn(x)` may use the as-if rule to take the argument as `&const T` instead.

## Expressions

Every expression produces one of the following:
 * unqualified reference - `&T`
 * const-qualified reference - `&const T`
 * move-qualified reference - `&move T`
 * temporary value

A variable binding used in an expression is represented as-if a move-qualified
reference `&move T`, while a const-qualified variable binding is represented
as-if a const-qualified reference `&const T`.

A move-qualified reference is fragile: passing it to a binding does not preserve
its move qualification unless either the `&&` or `&move` binding operators are
used.

When an expression produces a temporary value, the value is materialized and
implicitly borrowed with the `&move` binding operator when passed to a binding.

These are the binding operators that can be used to modify an expression:
 * `&` - Borrow an unqualified reference.
 * `&const` - Borrow a const-qualified reference.
 * `&move` - Explicitly borrow a move-qualified reference; preserves the move
             qualification when the expression is passed to a binding.
 * `&&` - Explicitly borrow and forward the reference; preserving its qualifiers
          when the expression is passed to a binding.
 * `*` - Explicitly copy the (referenced) value as a temporary value.

## Constants
Unlike C++ `const` does not imply mutability of the value.

The semantics of const-qualified expressions, references and bindings are
described by this document from how binding will select overloaded functions
that may actually modify the value.



## Moving
In this language moving is implemented as a non-destructive move like in C++.

The semantics of move-qualified expressions, references and bindings are
described by this document from how binding will select overloaded functions
that may actually consume the resources of the reverenced value.

The rules regarding the consumption itself is described in: [Moving](moving.md)

[_invalid_]: #invalid-binding
## Invalid Binding

Invalid arguments causes the function overload to be dropped as a candidate
for overload resolution.

Invalid initialization of variables will cause a compile-time error.


## Overload resolution

These are the overload prioriorties based on the result of the expression
other with binding operator that is passed to an function argument.

  Expression            | exact match      | priority                          
 :----------------------|:-----------------|:----------------------------------
 `&T`                   | `fn(x : &)`      | `fn(x :)`, `fn(x : &const)`, `fn(x)`         
 `&const T`             | `fn(x : &const)` | `fn(x :)`, `fn(x)`                      
 `&move`, temporary     | `fn(x : &move)`  | `fn(x :)`, `fn(x : &const)`, `fn(x : &)`, `fn(x)` 

