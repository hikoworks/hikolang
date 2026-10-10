# Bindings

Goals these binding rules:
 * Const correctness.
 * The simple default syntax means copy-like semantics.
 * Ordinary use of a variable doesn't accidentally give a function permission
   to consume it. An explicit `&move` or `&&` is required to cross that boundary.
 * Temporaries are expected to be consumed.
 * Non-destructive move semantics.
 * `const` and `move` constrain what a particular expression is permitted to do.
   Explicit casts can bypass those restrictions; they do not establish intrinsic
   immutability or prove that a value remains unconsumed.

## Binding Forms

Bindings appear in three contexts: variable initialization, function parameter
declarations, and function return specifications. All three use the same binding
qualifiers and binding operators to control how values and references are passed
between expressions and bindings.

A binding consists of a name or declaration context, an optional binding
qualifier, an optional type, and an expression or operation that provides the
initial value. The binding qualifier determines how the resulting value or
reference is exposed to the binding. The binding operator determines how the
expression is interpreted when establishing that binding.

The language provides short forms for common cases. These omit information that
can be inferred from the context, while retaining the same underlying binding
semantics. The defaults are intentionally asymmetric: function parameters
default to value semantics, variable initializers default to value semantics
unless the expression explicitly specifies a binding operator, and function
returns follow the same rule as variable initializers.


### Named variable / reference binding

A named binding introduces a name for either a value or a reference to existing
storage. Its general form is:

```
<name> `:` <binding-qualifier>? <type>? <- <binding-operator>? <expr>
```

The short form omits the binding qualifier and type:

```
<name> <- <binding-operator>? <expr>
```

The binding qualifier specifies how the new binding receives the expression
result. The optional type constrains or specifies the type of the binding. The
optional binding operator controls how the expression is interpreted before the
result is bound.

The choice of binding operator distinguishes value initialization from reference
initialization. For example, an expression without an explicit binding operator
is interpreted using the default value-binding semantics, whereas `&x`,
`&const x`, and `&move x` explicitly request reference semantics with the
corresponding qualification.

The declaration syntax is shared by value and reference bindings. The
distinction follows from the type of the expression result and the binding
specification, rather than from a separate declaration form for references.


### Function / Type parameter binding

Function parameters use the same binding qualifiers and types as named variable
bindings:

```
foo = fn(<name> `:` <binding-qualifier>? <type>?) { ... }
foo(<binding-operator>? <expr>)
```

The short parameter declaration omits the qualifier and type:

```
foo = fn(<name>) { ... }
foo(<binding-operator>? <expr>)
```

A parameter declaration specifies how an argument is bound when a function is
called. The binding qualifier determines which expression-result qualifications
the parameter accepts and how the argument is exposed inside the function. The
optional type further constrains which arguments are eligible.

When a function has multiple overloads, the argument expression's type after
applying any explicit binding operator determines which overloads are eligible
and their relative priority.

The short form `fn(x)` is equivalent to `fn(x :*)`: it declares a value-binding
parameter, with the argument bound to a move-qualified variable inside the
function.


### Function return value binding

Function return specifications use the same binding model:

```
foo = fn() -> <binding-qualifier>? <type>? {
    return <binding-operator>? <expr>
}
```

The short form omits the return qualifier and type:

```
foo = fn() {
    return <binding-operator>? <expr>
}
```

The return specification determines how the return expression is bound at the
function boundary. As with variable initialization and function parameters, the
binding qualifier and binding operator are distinct: the former specifies the
resulting binding, while the latter controls how the expression is interpreted.

An omitted return specification does not always imply an unqualified reference
or a value with a fixed type. Instead, the binding operator used by the return
expression determines whether the short form behaves like an explicitly
specified return binding or defaults to value semantics. An explicit
reference-binding operator preserves the requested reference semantics; without
one, the return expression uses value semantics.

Consequently, a function returning a reference must express the intended
reference binding explicitly, while returning a value can use the default
syntax. In every case, the declared or inferred return binding must be
compatible with the expression result after the applicable binding rules have
been applied.


## Types

 | Type       | Description                                                 | Expr result | Variable | Reference |
 |------------|-------------------------------------------------------------|:-----------:|:--------:|:---------:|
 | `T`        | The type of a value, including temporaries                  |      +      |          |           |
 | `move T`   | The type of a move-qualified variable binding               |             |    +     |           |
 | `const T`  | The type of a const-qualified variable binding              |             |    +     |           |
 | `&T`       | The type of a unqualified reference binding                 |      +      |          |     +     |
 | `&move T`  | The type of a move-qualified reference binding              |      +      |          |     +     |
 | `&const T` | The type of a const-qualified reference binding             |      +      |          |     +     |


## Expressions

Every expression produces one of the following:
 * temporary value - `T`
 * unqualified reference - `&T`
 * const-qualified reference - `&const T`
 * move-qualified reference - `&move T`

Rules for how an expression is typed (all rules are applied in order):
 1. A move-qualified variable binding initially has the type `&move T`.
 2. A const-qualified variable binding initially has the type `&const T`.
 3. A reference binding initially retains its qualifier.
 4. A temporary may be materialized when a reference is borrowed.
 5. If the expression has an explicit binding operator:
    - apply the binding operator to the expression, otherwise
    - apply fragility by converting a `&move T` expression to `&T`.
 6. (optionally) Select function based on overload rules.
 7. Apply binding qualifier of the selected function and bind to the named binding.

```
a <- 1.0          // Move-qualified variable binding.
foo(a)            // `a` in an expression has type `&move T`, but the fragile expression
                  // loses its move qualification before binding, causing the type to be `&T`.
foo(&move a)      // The expression type `&move T` will select `foo(x : &move)`
foo(make_value()) // make_value() has as result `T`, selecting `foo(x : *)`
```

Here is the table showing how an intermediate expression result of a certain type
is converted by each binding operator:

 | op \ expr  | Description                                                 | `T`        | `&T`        | `&const T`  | `&move T`
 |------------|-------------------------------------------------------------|------------|-------------|-------------|------------
 |            | Without an binding op, move-qualifier is fragile            | `T`        | `&T`        | `&const T`  | `&T` (fragile)
 | `&x`       | Strip move qualification                                    | `&T`       | `&T`        | `&const T`  | `&T`
 | `&const x` | Strip move qualification and require const access           | `&const T` | `&const T`  | `&const T`  | `&const T`
 | `&move x`  | Require move-capable source and preserve move qualification | `&move T`  | [_invalid_] | [_invalid_] | `&move T`
 | `&&x`      | Preserve the expression's qualification exactly             | `T`        | `&T`        | `&const T`  | `&move T`
 | `*x`       | Creates a value copy of the expression.                     | `T`        | `T`         | `T`         | `T`

An ordinary variable expression is intentionally fragile: its move qualification
is discarded unless an explicit binding operator preserves it. This way a
programmer can select exactly when a value may be consumed.


## Binding Qualifiers

The table below shows how the binding qualifier and the expression type
determine the resulting binding type. The rows represent the binding
declarations and their requested qualifiers; the columns represent the
expression types after applying any binding operator. Each cell specifies the
type of binding produced when the corresponding expression is bound to the
corresponding declaration. A cell marked invalid indicates that the expression
cannot be bound in that way.

| Binding \\ Expr  | Description                           | `T`        | `&T`        | `&const T`  | `&move T`  |
|------------------|---------------------------------------|------------|-------------|-------------|------------|
| `fn(a : &)`      | Bind as a unqualified reference       | `&T`       | `&T`        | [_invalid_] | `&T`       |
| `fn(a : &const)` | Bind as a const-qualified reference   | `&const T` | `&const T`  | `&const T`  | `&const T` |
| `fn(a : &move)`  | Bind as a move-qualified reference    | `&move T`  | [_invalid_] | [_invalid_] | `&move T`  |
| `fn(a : *)`      | Bind as a move-qualified variable     | `move T`   | `move T`    | `move T`    | `move T`   |
| `fn(a : *const)` | Bind as a const-qualified variable    | `const T`  | `const T`   | `const T`   | `const T`  |
| `fn(a :)`        | Infer binding based on the expression | `move T`   | `&T`        | `&const T`  | `&move T`  |
| `fn(a : const)`  | Infer binding based on the expression | `const T`  | `&const T`  | `&const T`  | `&const T` |

> [!note]
> When temporary value of type `T` is bound as a reference, the temporary value
> may need to be materialized as a move-qualified variable `move T` and its
> reference is borrowed from the temporary.

> [!note]
> The binding specification `fn(x : *)` and the prefix binding operator `*x`
> are distinct language constructs with independent semantics. They are not
> interchangeable, but share a conceptual meaning: both explicitly
> express value semantics rather than reference semantics.

In the table above we show how arguments behave for function arguments; this
works identical for variable initializer and function return specification.
The following tables shows equivalence of the binding types:

| Function Parameter | Function Return  | Variable Init          |
|--------------------|------------------|------------------------|
| `fn(a : &)`        | `fn() -> &`      | `a : & <- <expr>`      |
| `fn(a : &const)`   | `fn() -> &const` | `a : &const <- <expr>` |
| `fn(a : &move)`    | `fn() -> &move`  | `a : &move <- <expr>`  |
| `fn(a : *)`        | `fn() -> *`      | `a : * <- <expr>`      |
| `fn(a : *const)`   | `fn() -> *const` | `a : *const <- <expr>` |
| `fn(a : const)`    | `fn() -> const`  | `a : const <- <expr>`  |

The short form binding versions are asymmetrical between functions argument
definition, variable initializer and function return specification:

 * The function argument definition: `fn(a)` is identical to `fn(a : *)`.
 * The variable initializer: `x <- <init>`:
   - when `<init>` has an explicit binding operator; identical to `x : <- <init>`
   - otherwise; identical to `x : * <- <init>`.
 * The (empty) function return specification `fn()`:
   - when `return` has an explicit binding operator; identical to `fn() ->`
   - otherwise; identical to `fn() -> *`


## Overload resolution

These are the overload priorities based on the result of the expression
after the optional binding operator has been applied.

 | Expression type | preferred in order
 |-----------------|-----------------
 | `T`             | `fn(a : *)`, `fn(a : *const)`, `fn(a :)`, `fn(a : const)`, `fn(a : &move)`, `fn(a : &const)`, `fn(a : &)`
 | `&T`            | `fn(a : &)`, `fn(a :)`, `fn(a : const)`, `fn(a : &const)`, `fn(a : *)`, `fn(a : *const)`
 | `&const T`      | `fn(a : &const)`, `fn(a :)`, `fn(a : const)`, `fn(a : *)`, `fn(a : *const)`
 | `&move T`       | `fn(a : &move)`, `fn(a :)`, `fn(a : const)`, `fn(a : &const)`, `fn(a : *)`, `fn(a : *const)`, `fn(a : &)`

The following bindings are ambiguous and cause a compilation error if both
overloads are visible after dropping invalid overloads.
 * `(a :)` and `fn(a : const)`
 * `(a : *)` and `fn(a : *const)`

## Explicit cast

Special standard library functions can cast references beyond the
implicit conversion:

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

The `const` qualifier restricts how a value may be accessed through a particular
binding or reference. It provides a const-qualified view of a value, allowing
the language to distinguish operations that accept const-qualified access from
those that require unqualified or move-qualified access.

A const-qualified binding is written using `const`:

```text
a : const <- 1.0
b = 2.0
```

Both `a` and `b` are const-qualified bindings. The second declaration is
shorthand for the first form of binding, with the qualifier changed accordingly.

Const qualification does not, by itself, make a value immutable. It restricts
the operations available through the qualified access path. The same underlying
value may remain accessible through another binding or reference with different
qualifications. Consequently, a const-qualified binding does not guarantee that
the value cannot change; it guarantees only the restrictions associated with
const-qualified access.

When a function is called with a const-qualified expression, overload resolution
takes its qualification into account. A function accepting a const-qualified
reference can receive values through const-qualified, unqualified, or
move-qualified references. A function requiring an unqualified reference cannot
accept a const-qualified reference, because doing so would discard the access
restriction. A function requiring a move-qualified reference likewise cannot
accept a const-qualified reference, because const qualification does not grant
permission to consume the value.

Member access follows the same principle. A member variable accessed through a
const-qualified expression can only be read from through that access path. This
restriction applies regardless of whether the underlying value can be modified
through some other path.

Const qualification therefore describes the permitted access to a value, rather
than an intrinsic property of the value itself. It allows the language to
enforce read-only access where required while retaining the flexibility to
support values whose state may change through other, appropriately qualified
bindings.


## Temporaries

A temporary is a value produced by an expression rather than accessed through an
existing variable binding. A temporary expression has type `T`, distinguishing
it from a reference to existing storage. Its interpretation depends on the
binding context in which it is used.

When a temporary is bound to a value binding, the destination is initialized
directly from the temporary. The temporary's value can be acquired without
requiring an explicit move operator. The destination's binding qualifier
determines the qualification of the resulting variable; the temporary expression
itself does not determine whether the destination is a value or a reference.

```text
a <- make_value()       // Initialize a move-qualified value from a temporary.
```

Here, `make_value()` produces a temporary of type `T`. The initializer uses
value-binding semantics, so `a` becomes a move-qualified value variable
initialized from the result. No intermediate reference binding is required.

Temporaries can also be bound to reference parameters or reference variables. In
these cases, the binding establishes a reference to the temporary's storage
rather than initializing a new value. The temporary must remain alive for as
long as the reference is permitted to be used.

When overload resolution considers a temporary expression, its type `T` is used
to determine which bindings are eligible and their relative priority. The
preferred binding for a temporary is a value binding, followed by an inferred
binding, a move-qualified reference binding, a const-qualified reference
binding, and an unqualified reference binding. This ordering allows value
initialization to be preferred while still permitting functions to accept
references to temporaries when appropriate.

A temporary's eligibility for move-capable access does not mean that its
expression type is `&move T`. The expression retains its type `T` until the
binding context interprets it. Binding a temporary to a reference and
initializing a value from a temporary are distinct operations, even when both
can be performed without copying the underlying value.

The lifetime of a temporary is governed by the context in which it is created
and bound. A reference to a temporary must not outlive the temporary's storage.
The language's lifetime rules determine when that storage can be released,
independently of whether the temporary is copied, moved, or bound by reference.

Temporaries therefore provide a natural source of values for initialization and
function calls. They can be consumed without an explicit move operator, while
named variables retain the protection of fragile expression semantics. This
distinction allows temporary values to be used efficiently without implicitly
granting ordinary expressions permission to consume existing named values.


## Copying

Copying initializes a destination value from an existing value without changing
the source value. The destination receives an independent value whose initial
state is determined by the copy operation supported by the type.

Copying is distinct from binding a reference. A reference provides access to
existing storage, whereas copying creates a value in the destination. The copy
operation is determined by the type and may be unavailable for types that do not
support copying.

When a value is initialized from an expression that refers to an existing value,
the binding rules determine whether the source is copied or consumed. An
ordinary expression of a named move-qualified variable loses its move
qualification through fragility. Consequently, a value binding does not
implicitly consume a named source.

```text
a <- "Hello World"
b : * <- a
```

The expression `a` initially has type `&move T`, because `a` is a move-qualified
variable binding. Without an explicit binding operator, fragility removes the
move qualification, producing `&T`. The value binding for `b` therefore
initializes a new value by copying from `a`. The value of `a` remains unchanged.

The same principle applies when the source is a reference binding:

```text
a <- "Hello World"
r <- &a
b : * <- r
```

Here, `r` is an unqualified reference to the storage owned by `a`. The
expression `r` has type `&T`. Initializing `b` with value-binding semantics
copies the referenced value into a new value, provided copying is supported. It
does not copy the reference binding itself, nor does it move the value from `a`.

Copying a reference is distinct from copying the value it refers to. An explicit
reference binding operator establishes a reference to existing storage, subject
to the reference qualification rules. Without such an operator, a reference
binding's expression is interpreted according to its qualification and the
fragility rules. In particular, assigning a reference expression to a value
binding copies the referenced value when the resulting expression type permits
copying.

For example:

```text
a <- "Hello World"

r <- &a              // Reference to a.
r2 <- &r             // Another reference to the same storage.
b : * <- r            // Copy the value referenced by r.
```

Both `r` and `r2` refer to the same underlying value. The explicit `&` operator
in `r2 <- &r` requests reference binding rather than value initialization. By
contrast, `b` owns a separate value initialized by copying the value referenced
by `r`. Modifying the value through an appropriate reference to `a` does not
modify `b`, and modifying `b` does not modify the value owned by `a`.

Copying from a const-qualified reference follows the same principle:

```text
a <- "Hello World"
r <- &const a
b : * <- r
```

The expression `r` has type `&const T`. The value binding can initialize `b` by
copying from the const-qualified source, provided the type supports copying from
const-qualified access. The const qualification restricts operations performed
through the source reference; it does not prevent the value from being copied.

A move-qualified reference does not automatically cause a move when used in an
ordinary expression:

```text
a <- "Hello World"
r <- &move a
b : * <- r           // Copies the referenced value.
c : * <- &move r     // Consumes the value referenced by r.
```

The expression `r` initially has type `&move T`, but its move qualification is
discarded through fragility when used without an explicit binding operator.
Consequently, `b` is initialized by copying the referenced value, provided
copying is supported. In contrast, `&move r` explicitly preserves the move
qualification, allowing `c` to consume the value from `a`.

Creating or reseating a reference does not itself copy or move the referenced
value. Copying and moving occur when a value binding initializes its destination
from the source expression, according to the expression's resulting
qualification and the operations supported by the type.

Copying therefore provides a non-destructive way to initialize values from named
expressions. Reference bindings preserve access to existing storage, while value
bindings create new values. Fragility prevents an ordinary expression from
implicitly granting permission to consume a named value, keeping copying and
moving distinct operations.


## Moving

Moving transfers a value from a source to a destination without ending the
source binding's lifetime. The destination receives the value, while the source
remains a valid binding in a moved-from state.

A moved-from value remains valid, but its previous value is no longer guaranteed
to be preserved and may be unspecified. All operations on a moved-from value
remain valid, but their result may be depended on unspecified state. Some
operations may leave the value (in a new) unspecified or establish a fully
specified state. A moved-from value may also be reassigned to establish a fully
specified state.

The compiler and runtime will allow all operations on moved-from values and
allow a moved-from value to be moved from again.

Moving is distinct from creating a reference to a value. A move-qualified
reference grants permission for a subsequent operation to consume the referenced
value, but creating or passing that reference does not itself perform a move.
The move occurs when the reference is bound to a destination that requests value
semantics.

Consider the following example:

```text
a <- "Hello World"        // a is a move-qualified string value.

b <- &move a              // b is a move-qualified reference to a.
                          // The value of a is unchanged.

c : * <- &move a          // Initialize c by moving the value from a.
```

The first declaration creates a move-qualified value. The second declaration
creates a reference to the existing value and preserves its move qualification.
Neither declaration consumes the value stored in `a`.

The third declaration explicitly requests value semantics through the `*`
binding qualifier. The initializer supplies a move-qualified reference, so the
destination can consume the referenced value. The string value is transferred to
`c`, and `a` remains alive in a moved-from state.

An ordinary expression does not implicitly grant permission to consume a named
value. For example:

```text
a <- "Hello World"

b <- a                     // Does not request a move from a.
c <- &move a               // Creates a move-qualified reference.
d : * <- &move a           // Explicitly requests a move from a.
```

The distinction is intentional. The expression `a` does not preserve its move
qualification when used without an explicit binding operator. By contrast,
`&move a` explicitly preserves that qualification, allowing a destination that
accepts a move-qualified source to consume the value.

Move qualification therefore expresses permission, not an operation. A
move-qualified reference identifies a value that may be consumed; a
value-binding operation performs the move. This separation makes moving explicit
for named values while allowing temporary values to be consumed naturally.

From this follows:

```
a <- "Hello World"
b : * <- &move a
c : * <- &move a    // It is valid to move from a moved-from value
                    // `c` will now take on a moved-from state
                    // `a` will be set/remain to a moved-from state.
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
f = fn(a : *int)          // Const-qualified variable binding containing
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

For an existing reference binding, a right-hand expression that explicitly uses
a binding operator that results in a reference reseats the reference. Otherwise,
`<-` uses the assignment operation and `=` uses the merge operation on the
referenced value.

```text
a <- 1.0
b <- 2.0

r <- &a
r <- 2.0       // Calls `r.__assign__(2.0)`, modifying `a`.
r <- &b        // Reseat `r`; `a` is unchanged.
```

Using an explicit bind operators `&&`, `&x`, `&const x` or `&move x` that
results in a reference will reseat the reference binding instead of calling
`__assign__()`. Reseating a reference does not change its binding or reference
qualification.
 

```text
r = &a
r <- &b        // Reseat `r`; `r` remains const-qualified.

cr <- &const a
cr <- &const b // Reseat `cr`; its reference remains const-qualified.
cr <- &a       // Reseat `cr`; `&a` is convertible to `&const T`.

mr <- &move a
mr <- &move b  // Reseat `mr`; its reference remains move-qualified.
// mr <- &a    // ERROR: unqualified reference `&a` can not be upgraded to a move-qualified reference.
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
