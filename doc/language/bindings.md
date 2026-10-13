# Bindings

This document defines how expressions initialize variable bindings, function
parameters, and return values, and how existing bindings are updated. It
specifies binding operators, binding qualifiers, implicit conversions, reference
reseating, copying, moving, const-restricted access, and explicit casts.

Binding resolution proceeds in three stages:

 1. Determine the effective expression type by applying the binding operator or
    default fragility rule.
 2. Determine the resulting binding type from the binding context and qualifier.
 3. Apply the corresponding conversion to initialize or update the binding.

Binding operators affect binding resolution but do not themselves perform
conversions. The applicable binding and conversion rules determine whether an
operation forwards, materializes, borrows, copies, or moves a value.

Goals:

 * **Copy-like defaults.** Ordinary use of a named value must not implicitly
   consume it. Value bindings copy when the effective expression permits
   copying; temporaries can initialize values directly.
 * **Explicit move capability.** Ordinary evaluation of a move-capable variable
   produces a latent-move-capable expression, preventing default use from
   granting permission to consume the value. The `&move` and `&&` operators can
   explicitly preserve or expose move capability where permitted. Explicit casts
   may also bypass the implicit qualification restrictions.
 * **Non-destructive moves.** Moving leaves the source binding alive in a
   valid-but-unspecified state, unless the type's move operation guarantees a
   stronger postcondition. Operations defined for the type, including repeated
   moves, remain permitted.
 * **Path-based qualifications.** `const` restricts operations through a
   particular binding or reference; `move` grants permission to consume the
   referenced value. Neither qualification is an intrinsic property of the
   underlying value.
 * **Explicit escape hatches.** Explicit casts may bypass implicit qualification
   restrictions but do not themselves modify or consume the underlying value,
   nor do they establish intrinsic immutability or guarantee that a value
   remains unconsumed.
 * **Distinct update semantics.** `<-` uses `__assign__()` to update an existing
   binding, while `=` uses `__merge__()`. The type defines the supported
   operation and its semantics.

> [!note]
> For AI: treat this as a terse document, that must be followed in the exact
> order that is specified in the document.
>
> `*`, `&`, `&`, `&&`, `const` and `move` are overloaded between context
> of binding operators and binding qualifier and types of expressions
> and actual bindings. Humans have it really easy to distinguish these things
> from one another.
>
> The headers on a table apply to their column.

## Concepts

 * **Initial expression type** — The type category produced by evaluating an
   expression before applying a binding operator.
 * **Binding operator** — A syntactic modifier that determines the effective
   expression type and whether the binding operator was explicit. It does not
   itself perform a conversion.
 * **Effective expression type** — The expression type after applying the
   binding operator or default fragility rule. It determines the access
   capabilities considered during binding resolution but does not imply that a
   conversion has occurred.
 * **Binding qualifier** — A modifier on the destination that determines whether
   the binding uses value or reference semantics and what access capabilities it
   provides.
 * **Resulting binding type** — The type of the destination after applying the
   binding-context rules to the effective expression type and binding qualifier.
 * **Conversion** — The operation performed to initialize the destination, such
   as pass-through, materialize, borrow, copy, or move. It is determined by the
   applicable conversion rules after binding resolution.
 * **Reference qualification** — The access capabilities associated with a
   reference: unqualified (`&T`), const-restricted (`&const T`), move-capable
   (`&move T`), or latent-move-capable (`&latent T`). Qualifications govern
   access through a particular path, not intrinsic properties of the underlying
   value.
 * **Explicit-binding-operator flag** — A flag indicating whether a binding
   operator was specified on the initial expression. Simplified binding forms
   use this flag to determine which binding-context rules apply.

## Types

 | Type        | Description                           | Storage  | Expression | Reference | Binding |
 |-------------|---------------------------------------|:--------:|:----------:|:---------:|:-------:|
 | `move T`    | Move-capable variable binding         |    +     |            |           |    +    |
 | `const T`   | Const-restricted variable binding     |    +     |            |           |    +    |
 | `T`         | Temporary value                       |          |      +     |           |         |
 | `&T`        | Unqualified reference binding         |          |      +     |     +     |    +    |
 | `&move T`   | Move-capable reference binding        |          |      +     |     +     |    +    |
 | `&const T`  | Const-restricted reference binding    |          |      +     |     +     |    +    |
 | `&latent T` | Latent-move-capable reference binding |          |      +     |     +     |         |


## Initial Expressions

Every initial expression produces one of the following types:
 * temporary value - `T`
 * unqualified reference - `&T`
 * const-restricted reference - `&const T`
 * move-capable reference - `&move T`
 * latent-move-capable reference - `&latent T`

The following initial expression kinds may result in the types:

| Expression kind                            | Possible resulting initial expression types
|--------------------------------------------|----------------
| evaluation of a function-call              | `T`, `&T`, `&const T` or `&move T`
| evaluation of a move-capable variable      | `&latent T`
| evaluation of a const-restricted variable  | `&const T`
| evaluation of an unqualified reference     | `&T`
| evaluation of a move-capable reference     | `&latent T`
| evaluation of a const-restricted reference | `&const T`

Non-binding operators, like mathematics operators `1 + 2` are lowered to
function-calls, therefore should be treated as function calls in respect of
the table above.

## Binding operators

Binding operators are type transformations, not binding conversions.

A binding operator determines the effective expression type used by the
subsequent binding rules. It does not, by itself, borrow a reference, create
storage, materialize a temporary, copy a value, move a value, or extend a
lifetime.

The effective expression type is an input to the binding-context rules, not a
description of an operation already performed. The binding context and binding
qualifier, together with that effective expression type, determine the resulting
binding type and the conversion to perform. The conversion tables define whether
that conversion forwards, materializes, borrows, copies, moves, or is invalid.

Consequently, the description of a binding operator in terms of reference or
value semantics describes its effect on the binding rules, not an operation
performed by the operator itself. No copy, move, or reference creation may be
inferred from the binding operator alone.

 * **Default**: When no binding operator is specified, the initial expression is
   interpreted using the default fragility rule. This preserves ordinary
   copy-like semantics by preventing a move-capable reference from implicitly
   granting permission to consume the referenced value.
 * `&x` — **Borrow reference**: Requests reference semantics for the initial
   expression's value, rather than independent value semantics. The effective
   expression type is an unqualified reference, except that an existing const
   restriction is preserved. The actual reference binding is determined by the
   subsequent binding rules.
 * `&const x` — **Borrow const-restricted reference**: Requests reference
   semantics with const-restricted access. The effective expression type is
   `&const T`, ensuring that subsequent binding resolution preserves the
   restriction on operations through that access path. The operator does not
   independently create a reference or make the underlying value immutable.
 * `&move x` — **Borrow move-capable reference**: Requests reference semantics
   that explicitly preserve permission to consume the referenced value later.
   The effective expression type is `&move T`; this permission does not itself
   consume the value. The subsequent binding conversion determines whether the
   value is actually moved, copied, or accessed through a reference.
 * `&&x` — **Forward expression**: Preserves the initial expression's effective
   type and qualification, except that a latent-move-capable reference becomes a
   move-capable reference. This explicitly exposes move capability that ordinary
   evaluation would otherwise suppress.
 * `*x` — **Value semantics**: Requests value semantics rather than retaining
   reference semantics to existing storage. The effective expression type
   becomes `T`, allowing the subsequent binding rules to interpret the
   expression accordingly. The operator itself does not copy, move, or create a
   value; the binding qualifier and applicable conversion rules determine what
   actually happens.


Here is the table showing how an initial expression of a certain type is
modified by each of the binding operators into an effective expression type:

 | operator    | `T`        | `&T`        | `&const T`  | `&move T`  | `&latent T`
 |-------------|------------|-------------|-------------|------------|-------------
 | default     | `T`        | `&T`        | `&const T`  | `&move T`  | `&T`
 | `&x`        | `&T`       | `&T`        | `&const T`  | `&T`       | `&T`
 | `&const x`  | `&const T` | `&const T`  | `&const T`  | `&const T` | `&const T`
 | `&move x`   | `&move T`  | error       | error       | `&move T`  | `&move T`
 | `&&x`       | `T`        | `&T`        | `&const T`  | `&move T`  | `&move T`
 | `*x`        | `T`        | `T`         | `T`         | `T`        | `T`


It is a compile error when trying to use the `&move x` operator on an initial
expression with the types `&T` or `&const T`.

The compiler will mark an effective expression with an explicit-binding-operator
flag when an binding operator was used on the initial expression; specifically
to handle simplified bindings.

## Bindings

Bindings appear in four contexts: variable binding, const-restricted variable
binding, function parameter declarations, and function return specifications.
All four use the same binding qualifiers and binding operators to control how
values and references are passed between expressions and bindings.

The bindings are similar in semantics, but they have asymmetric definitions to
make it that way.

 * `:` - **Default binding**: Uses the binding context’s default rules to
   determine how the effective expression is bound. It provides the ordinary,
   least-surprising behavior without requiring the programmer to specify
   reference or value semantics explicitly.
* `: const` - **Const-restricted binding**: Uses the binding context’s
   const-restricted form of its default binding behavior. It restricts
   operations through the resulting binding without necessarily creating a
   reference or an independent value, and without implying that the underlying
   object is intrinsically immutable.
 * `: &` - **Reference binding**: Binds through a reference, preserving access
   to the existing object rather than creating an independent value. It makes
   aliasing explicit while applying the context’s rules for reference
   qualification.
 * `: &const` - **Const-restricted reference binding**: Binds through a
   reference that restricts what can be done through that access path. It allows
   the object to be observed or passed along without granting the recipient
   unrestricted access, while not implying that the object itself is
   intrinsically immutable.
 * `: &move` - **Move-capable reference binding**: Binds through a reference
   that grants permission to move from the referenced object. It makes that
   capability explicit, distinguishing access that may consume a value from
   ordinary or const-restricted reference access.
 * `: &&` - **Inferred binding**: Infers value or reference semantics from the
   effective expression type, preserving its qualification. The binding context
   determines the resulting binding type and conversion.
 * `: *` - **Value binding**: Binds using value semantics rather than retaining
   a reference to the existing object. The destination receives a value of its
   own, with copying or moving governed by the applicable rules and available
   operations.
 * `: *const` - **Const-restricted value binding**: Binds a value while
   restricting how that value may be accessed through the resulting binding. It
   combines value semantics with const-restricted access, without implying that
   the underlying value is inherently immutable.

In the tables below:
 * The `T`, `&T`, `&const T`, and `&move T` column headers denote the effective
   expression type. Each cell specifies the resulting binding type.
 * When the "default" column has "-" for a row, then only the `T`, `&T`,
   `&const T`, and `&move T` columns should be used in the evaluation of the
   resulting type of the binding.
 * The simplified bindings listed in the bottom three rows behave differently
   when the effective expression's explicit-binding-operator flag is:
    - true: use only the `T`, `&T`, `&const T` and `&move T` columns from the
      effective expression type and lookup the binding type from the cells
      below.
    - false: lookup the binding type from the cells below the "default" column.

The operator table and binding-context tables must be applied as separate
stages. First determine the effective expression type using the binding operator
table. Then use that type and the binding qualifier to determine the resulting
binding type. Finally, apply the corresponding conversion rule to perform the
required operation. A conversion such as copy, move, or materialize is never
performed merely because the binding operator's row produces a particular
effective expression type.

### Variable binding

A variable binding introduces a name for either a value or a reference to
existing storage. Its general form is:

> _name_ __(__ `:` _binding-qualifier_**?** _type_**?** __)?__ `<-` _binding-operator_**?** _expr_


| Binding                | `T`        | `&T`        | `&const T`  | `&move T`  | default   |
|------------------------|------------|-------------|-------------|------------|-----------|
| `a : & <- <expr>`      | `&T`       | `&T`        | [_invalid_] | `&T`       | -         |
| `a : &const <- <expr>` | `&const T` | `&const T`  | `&const T`  | `&const T` | -         |
| `a : &move <- <expr>`  | `&move T`  | [_invalid_] | [_invalid_] | `&move T`  | -         |
| `a : * <- <expr>`      | `move T`   | `move T`    | `move T`    | `move T`   | -         |
| `a : *const <- <expr>` | `const T`  | `const T`   | `const T`   | `const T`  | -         |
| `a : && <- <expr>`     | `move T`   | `&T`        | `&const T`  | `&move T`  | -         |
| `a <- <expr>`          | `move T`   | `&T`        | `&const T`  | `&move T`  | `move T`  |
| `a : <- <expr>`        | `move T`   | `&T`        | `&const T`  | `&move T`  | `move T`  |
| `a : const <- <expr>`  | `const T`  | `&const T`  | `&const T`  | `&const T` | `const T` |



### Const-restricted variable binding

A const-restricted variable binding introduces a name for either a value or a
reference to existing storage. Its general form is:

> _name_ __(__ `:` _binding-qualifier_**?** _type_**?** __)?__ `=` _binding-operator_**?** _expr_


| Binding                | `T`        | `&T`        | `&const T`  | `&move T`  | default   |
|------------------------|------------|-------------|-------------|------------|---------  |
| `a : & = <expr>`       | `&const T` | `&const T`  | `&const T`  | `&const T` | -         |
| `a : &const = <expr>`  | redundant  | redundant   | redundant   | redundant  | -         |
| `a : &move = <expr>`   | error      | error       | error       | error      | -         |
| `a : * = <expr>`       | `const T`  | `const T`   | `const T`   | `const T`  | -         |
| `a : *const = <expr>`  | redundant  | redundant   | redundant   | redundant  | -         |
| `a : && = <expr>`      | error      | error       | error       | error      | -         |
| `a = <expr>`           | `const T`  | `&const T`  | `&const T`  | `&const T` | `const T` |
| `a : = <expr>`         | `const T`  | `&const T`  | `&const T`  | `&const T` | `const T` |
| `a : const = <expr>`   | redundant  | redundant   | redundant   | redundant  | redundant |


It is a compilation error when the matching cell is an "redundant" or "error".
In this case it is caused by redundant syntax, or where the meaning of such a
syntax is not really clear.


### Function argument binding

A function argument binding introduces a name for either a value or a reference
to existing storage. Its general form is:

> `foo` `=` `fn` `(` _name_ __(__ `:` _binding-qualifier_**?** _type_**?** __)?__ `)` `{` ... `}`\
> `foo` `(` _binding-operator_**?** _expr_ `)`


| Binding          | `T`        | `&T`        | `&const T`  | `&move T`  | default   |
|------------------|------------|-------------|-------------|------------|-----------|
| `fn(a : &)`      | `&T`       | `&T`        | [_invalid_] | `&T`       | -         |
| `fn(a : &const)` | `&const T` | `&const T`  | `&const T`  | `&const T` | -         |
| `fn(a : &move)`  | `&move T`  | [_invalid_] | [_invalid_] | `&move T`  | -         |
| `fn(a : *)`      | `move T`   | `move T`    | `move T`    | `move T`   | -         |
| `fn(a : *const)` | `const T`  | `const T`   | `const T`   | `const T`  | -         |
| `fn(a : &&)`     | `move T`   | `&T`        | `&const T`  | `&move T`  | -         |
| `fn(a)`          | `move T`   | `move T`    | `move T`    | `move T`   | `move T`  |
| `fn(a : )`       | `move T`   | `move T`    | `move T`    | `move T`   | `move T`  |
| `fn(a : const)`  | `const T`  | `const T`   | `const T`   | `const T`  | `const T` |



#### Overload resolution

These are the overload priorities based on the result of the effective expression
after the optional binding operator has been applied.

Overload priority:
 1. exact match with effective expression (see below)
 2. `fn(a : &&)`
 3. `fn(a : &move)`
 4. `fn(a : &)`
 5. `fn(a : &const)`
 6. `fn(a : *)`, `fn(a : *const)`, `fn(a)`, `fn(a :)`, `fn(a : const)`

Exact match of the binding qualifier with the type of the effective expression
after applying the binding operator has priority above this list:
 * `T` matches exactly with `fn(a : *)`, `fn(a : *const)`, `fn(a)`, `fn(a :)` or
   `fn(a : const)`
 * `&T` matches exactly with `fn(a : &)`
 * `&const T` matches exactly with `fn(a : &const T)`
 * `&move T` matches exactly with `fn(a : &move T)`

It is a compilation when, after dropping non-matching overloads, multiple
overloads of the following are visible: `fn(a : *)`, `fn(a : *const)`, `fn(a)`,
`fn(a :)`, `fn(a : const)`

### Function return binding

A function return binding creates a temporary value or a reference to
existing storage. Its general form is:

> `foo` `=` `fn` `(` ... `)` __(__ `->` _binding-qualifier_**?** _type_**?** __)?__ `{`\
>      `return` _binding-operator_**?** _expr_\
> `}`


Different from the previous binding a function return value is
an expression used by the outer expression; each cell thus specifies the initial
expression type.

| Binding          | `T`        | `&T`        | `&const T`  | `&move T`  | default   |
|------------------|------------|-------------|-------------|------------|-----------|
| `fn() -> &`      | `&T`       | `&T`        | [_invalid_] | `&T`       | -         |
| `fn() -> &const` | `&const T` | `&const T`  | `&const T`  | `&const T` | -         |
| `fn() -> &move`  | `&move T`  | [_invalid_] | [_invalid_] | `&move T`  | -         |
| `fn() -> *`      | `T`        | `T`         | `T`         | `T`        | -         |
| `fn() -> *const` | redundant  | redundant   | redundant   | redundant  | -         |
| `fn() -> &&`     | `T`        | `&T`        | `&const T`  | `&move T`  | -         |
| `fn()`           | `T`        | `&T`        | `&const T`  | `&move T`  | `T`       |
| `fn() ->`        | `T`        | `&T`        | `&const T`  | `&move T`  | `T`       |
| `fn() -> const`  | `T`        | `&const T`  | `&const T`  | `&const T` | `T`       |

It is a compilation error when the matching cell is an "redundant". In this case
it is caused by redundant syntax, because `const` is already implied.


## Conversion

Conversion occurs after the binding operator determines the effective expression
type and the binding context determines the resulting binding type. The
conversion depends on the initial expression type, the effective expression
type, and the resulting binding type.

The initial expression type identifies whether the source is a temporary value
or a reference to existing storage. The effective expression type determines the
expression's qualification for binding resolution. The conversion rules
additionally consider the initial expression type, so an explicit transformation
of the effective type does not necessarily discard every capability associated
with the original source.


### Conversion lookup

Select the conversion column using the following rules:

 * For an initial expression of type `T`, use the `T` column.
 * For an initial expression of type `&T` or `&const T`, use its initial
   expression type.
 * For an initial expression of type `&move T`:
    - If the effective expression type is `&T` or `&const T`, use that effective
      expression type's column.
    - Otherwise, use the `&move T` column. In particular, the `*` operator
      requests value semantics without discarding the source's existing move
      capability.
 * For an initial expression of type `&latent T`:
    - If the effective expression type is `&move T`: use the `&move T` column.
    - If the effective expression type is `T`: use the `T` column.
    - Otherwise use the `&T` column.

This lookup determines the conversion to perform; it does not change the
effective expression type or the resulting binding type.


### Conversion table

Select the row using the resulting binding type and the column using the
conversion lookup rules above.

| Resulting binding type | `T`          | `&T`        | `&const T`  | `&move T` |
| ---------------------- | -----------  | ----------- | ----------  | --------- |
| `T`                    | pass-through | copy        | copy        | move      |
| `const T`              | pass-through | copy        | copy        | move      |
| `move T`               | pass-through | copy        | copy        | move      |
| `&T`                   | materialize  | borrow      | [_invalid_] | borrow    |
| `&const T`             | materialize  | borrow      | borrow      | borrow    |
| `&move T`              | materialize  | [_invalid_] | [_invalid_] | borrow    |

An "invalid" conversion is a compilation error. Binding-context rules may also
reject an invalid qualification before conversion is considered.


### Conversion operations

 * **pass-through:** - Uses the expression directly in the destination binding
   without copying, moving, materializing a temporary, or creating a new borrow.
   A temporary directly initializes a value binding; a compatible reference is
   preserved as a reference.
 * **materialize:** - Materializes the temporary in storage, then borrows a
   reference to that storage. Its lifetime is governed by the language's
   lifetime rules.
 * **borrow:** - Establishes a reference to the existing storage, without
   copying or moving the referenced value. When the source is already a
   reference, the borrow targets the same underlying object and preserves the
   destination's permitted access. It does not borrow the reference binding
   itself.
 * **copy:** - Initializes a value by copying from the source. If the type does
   not support copying, compilation fails.
 * **move:** - Initializes a value using the type's move operation if supported.
   Otherwise, the value is copied if copying is supported; if neither operation
   is supported, compilation fails.
 * **invalid:** - The conversion is not permitted and causes a compilation
   error.

The `T`, `const T`, and `move T` value-binding rows use the same conversion
operations. Their differences concern the resulting binding's qualification, not
whether the source conversion copies or moves.

A move-capable source permits moving only when the conversion lookup retains its
move capability. An effective `&T` or `&const T` derived from an initial `&move
T` therefore permits copying, not moving. A latent-move-capable source permits
moving only when the effective expression type is `&move T`; ordinary use
otherwise retains copy-like semantics.


## Constants

The `const` qualifier restricts the operations permitted through a particular
binding or reference. It provides read-only access through that path without
making the underlying value intrinsically immutable.

```text
a <- "Hello World"
r : &const <- a    // Reading through r is permitted.
                   // Modifying or moving through r is not permitted.
```

An operation that receives const-restricted access may explicitly cast it to an
unqualified reference to modify the underlying value. Such a cast bypasses the
restriction for the resulting access path; it does not make the value
intrinsically mutable or immutable.

The same value may also be accessible through other bindings or references with
different qualifications. Consequently, `const` restricts operations through a
particular access path, but does not guarantee that the value cannot change.

Function overload resolution respects these restrictions. A const-restricted
reference binding can accept an unqualified, const-restricted, or move-capable
source reference. Conversely, a const-restricted source reference cannot
implicitly satisfy a destination requiring unqualified or move-capable access.
Explicit qualification casts may bypass these restrictions.


## Temporaries

A temporary is a value produced by an expression rather than accessed through an
existing binding. Its initial expression type is `T`.

A temporary can initialize a value binding directly and can be consumed without
an explicit move operator. When a reference binding requires materialization,
the temporary is materialized in the code-block scope where the expression
requiring the reference is evaluated.

The materialized temporary remains alive until that scope exits. A reference to
it is valid only while the temporary's storage remains alive; binding a
reference does not extend the temporary's [_lifetime_].

```text
r <- &make_value()    // Materializes the result in the current scope.
use(r)                // Valid while that scope remains active.
```

For a function return binding that converts a temporary into a reference,
materialization must occur at the caller's call site; the returned reference
must not refer to temporary storage that expires when the callee returns.

Temporaries therefore support direct initialization and consumption while
allowing references to them only for the [_lifetime_] of their materialized
storage.


## Copying

Copying initializes an independent value from an existing value without changing
the source. It requires the type to support copying.

When a value is initialized from a named variable or reference, the default
fragility rule prevents implicit consumption. A move-capable expression
therefore loses its move capability unless an explicit binding operator
preserves it.

```text
a <- "Hello World"
b <- a                 // Copies the value; a remains unchanged.

r <- &a                // Borrows a.
c <- r                 // Copies the referenced value.

m <- &move a           // Creates a move-capable reference to a.
d : * <- m             // Copies; ordinary use loses move capability.
e : * <- &move m       // Moves from a through m.
```

Copying a value is distinct from borrowing a reference to it. An explicit
reference binding aliases existing storage, while a value binding initializes an
independent value.

The default fragility rule therefore preserves copy-like semantics for ordinary
use of named values, while explicit `&move` permits a subsequent value-binding
conversion to move when supported.


## Moving

Moving initializes a destination from a source using the type's move operation.
The source binding remains alive in a **valid-but-unspecified state**, unless
the move operation guarantees a stronger postcondition.

A value in this state remains valid, and all operations defined for its type
remain permitted. Their results may depend on the unspecified state. A value may
be reassigned or moved from again.

A move-capable reference grants permission to consume its referenced value but
does not itself perform a move. Moving occurs when a value-binding conversion
consumes the referenced value.

```text
a <- "Hello World"

b <- a                   // Copies; a remains unchanged.
r <- &move a             // Creates a reference; does not move.
c : * <- &move a         // Moves a into c.
d : * <- &move r         // Moves from a again through r.
```

After initializing `c`, `a` remains alive in a valid-but-unspecified state.
Initializing `d` from `r` is still permitted, and `a` remains valid afterward.

Ordinary use of a named variable does not implicitly grant move capability. The
explicit `&move` or `&&` operators preserves that capability, while temporaries
can be consumed without it.




## Explicit casts

Special standard library functions allow explicit casts between reference
qualifications that are not permitted by implicit conversion:

| Function              | Effect                         |
| --------------------- | ------------------------------ |
| `std.remove_const(x)` | Removes the const restriction. |
| `std.remove_move(x)`  | Removes move capability.       |
| `std.add_const(x)`    | Adds a const restriction.      |
| `std.add_move(x)`     | Grants move capability.        |

These casts change the qualification of the resulting reference, not the
underlying value. In particular, `std.add_move` grants permission to move but
does not perform a move, while `std.remove_const` permits modification through
the resulting access path without making the value intrinsically mutable or
immutable.

Explicit casts bypass the normal qualification rules. The programmer is
responsible for ensuring that operations through the resulting reference are
valid.

For example, a method receiving a const-restricted reference can explicitly
remove the restriction to modify the underlying value:

```text
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

Although `co` is const-restricted, `foo` explicitly removes the restriction on
its access path, allowing it to modify `counter`.



## Updating Variable Bindings

When a binding already exists, `<-` and `=` update it instead of introducing a
new binding. The operator determines which operation is called:

* `<-` calls `__assign__()`.
* `=` calls `__merge__()`.

```text
a <- 1.0
a <- 2.0       // Calls a.__assign__(2.0).

b = 1.0        // Creates a const-restricted binding.
// b = 2.0     // Error: float does not support __merge__().
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

Function values use `__merge__` to combine definitions into an overload set:

```text
f = fn(a : *int) {}
f = fn(a : *string) {}
```

Types can use `__merge__` to combine members, and type templates can use it to
form overload sets.

The semantics of `__assign__()` and `__merge__()` are defined by the type. An
update is invalid when the required operation is unsupported or its binding
qualifications cannot be satisfied.



### Updating References

When updating an existing reference binding, an explicit binding operator that
produces a reference reseats the reference. Otherwise, `<-` calls `__assign__()`
and `=` calls `__merge__()` on the referenced value.

```text
a <- 1.0
b <- 2.0

r <- &a
r <- 2.0       // Calls r.__assign__(2.0), modifying a.
r <- &b        // Reseats r; a remains unchanged.
```

Reseating does not copy or move either referenced value, nor does it change the
reference binding's qualification. The new reference must be implicitly
convertible to the existing binding's reference qualification.

```text
cr = &a
cr <- &b             // Reseats cr; it remains const-restricted.

mr <- &move a
mr <- &move b        // Reseats mr; it remains move-capable.
// mr <- &b           // Error: cannot implicitly grant move capability.
```

A move-capable reference grants permission to consume its referenced value but
does not itself perform a move. Consumption occurs only when a subsequent
value-binding conversion moves from the referenced value.

```text
a <- "Hello World"
r <- &move a         // Creates a reference; a remains unchanged.
c : * <- &move r     // Moves the value from a into c.
```

Reference bindings alias existing storage. Creating or reseating a reference
does not itself copy, move, or modify the referenced value.
