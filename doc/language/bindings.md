# Bindings

This document defines how expressions initialize variable bindings, function
parameters, and return values, and how existing bindings are updated. It
specifies binding operators, binding qualifiers, implicit conversions, reference
reseating, copying, moving, const-restricted access, and explicit casts.

Binding resolution proceeds in three stages:

 1. Determine the effective expression type by applying the binding operator or,
    by default, treating `&latent T` as `&T`.
 2. Determine the resulting binding type from the binding context and qualifier.
 3. Apply the corresponding conversion to initialize or update the binding.

Binding operators affect binding resolution but do not themselves perform
conversions. The applicable binding and conversion rules determine whether an
operation forwards, materializes, borrows, copies, or moves a value.

Goals:

 * **Copy-like defaults.** Ordinary use of a named object must not implicitly
   consume its value. value-semantic bindings copy when the effective expression
   permits copying; temporaries can initialize objects directly.
 * **Explicit move capability.** Ordinary evaluation of a move-capable variable
   produces a latent-move-capable expression, preventing default use from
   granting permission to consume the value. The `&move` and `&&` operators can
   explicitly preserve or expose move capability where permitted. Explicit casts
   may also bypass the implicit qualification restrictions.
 * **Non-destructive moves.** Moving leaves the source object alive in a
   valid-but-unspecified state, unless the type's move operation guarantees a
   stronger postcondition. Operations defined for the type, including repeated
   moves, remain permitted.
 * **Path-based qualifications.** `const` restricts operations through a
   particular binding or reference; `move` grants permission to consume the
   value of a referenced object. Neither qualification is an intrinsic property
   of the object.
 * **Explicit escape hatches.** Explicit casts may bypass implicit qualification
   restrictions but do not themselves modify the object or consume its value,
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

 * **Object** - An instance of a type with storage and a lifetime. An object may
   be associated with a named variable or created by materializing a temporary.
 * **Value** - The data or state represented by an object, or produced by
   evaluating an  expression. A value has no independent storage, identity, or
   lifetime. An object may own resources associated with its value.
 * **Initial expression type** — The type category produced by evaluating an
   expression before applying a binding operator.
 * **Binding operator** — A syntactic modifier that determines the effective
   expression type and whether the binding operator was explicit. It does not
   itself perform a conversion.
 * **Effective expression type** - The expression type resulting from applying
   the binding operator or, by default, treating `&latent T` as `&T`. It
   determines the access capabilities considered during binding resolution,
   without implying a conversion.
 * **Binding qualifier** — A modifier on the destination that determines whether
   the binding uses value- or reference-semantics and what access capabilities
   it provides.
 * **Resulting binding type** — The type of the destination after applying the
   binding-context rules to the effective expression type and binding qualifier.
 * **Conversion** — The operation performed to initialize the destination, such
   as pass-through, materialize, borrow, copy, or move. It is determined by the
   applicable conversion rules after binding resolution.
 * **Reference qualification** — The access capabilities associated with a
   reference: unqualified (`&T`), const-restricted (`&const T`), move-capable
   (`&move T`), or latent-move-capable (`&latent T`). Qualifications govern
   access through a particular path, not intrinsic properties of an object's
   value.
 * **Explicit-binding-operator flag** — Records whether a binding operator was
   explicitly applied to the initial expression. Simplified binding forms use
   this flag to select the applicable binding-context rule.


## Types

 | Type        | Description                              | Object   | Expression | Reference | Binding |
 |-------------|------------------------------------------|:--------:|:----------:|:---------:|:-------:|
 | `move T`    | Move-capable variable binding            |    +     |            |           |    +    |
 | `const T`   | Const-restricted variable binding        |    +     |            |           |    +    |
 | `T`         | Temporary value                          |          |      +     |           |         |
 | `&T`        | Unqualified reference binding            |          |      +     |     +     |    +    |
 | `&move T`   | Move-capable reference binding           |          |      +     |     +     |    +    |
 | `&const T`  | Const-restricted reference binding       |          |      +     |     +     |    +    |
 | `&latent T` | Latent-move-capable reference expression |          |      +     |     +     |         |

 * &latent T — A transient expression category produced when evaluating a
   move-capable binding. It preserves the source's latent move capability but is
   not a storable or declarable binding type. Default binding-operator
   resolution treats it as &T.

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
an object, materialize a temporary, copy a value or move a value.

The effective expression type is an input to the binding-context rules, not a
description of an operation already performed. The binding context and binding
qualifier, together with that effective expression type, determine the resulting
binding type and the conversion to perform. The conversion tables define whether
that conversion forwards, materializes, borrows, copies, moves, or is invalid.

Consequently, the description of a binding operator in terms of value- or
reference-semantics describes its effect on the binding rules, not an operation
performed by the operator itself. No copy, move, or reference creation may be
inferred from the binding operator alone.

 * **Default** - When no binding operator is specified, the initial expression
   is interpreted treating `&latent T` as `&T`. This preserves ordinary
   copy-like semantics by preventing a move-capable reference from implicitly
   granting permission to consume the value of a referenced object.
 * `&x` — **Borrow reference**: Requests reference semantics for the initial
   expression's object, rather than independent value semantics. The effective
   expression type is an unqualified reference, except that an existing const
   restriction is preserved. The actual reference binding is determined by the
   subsequent binding rules.
 * `&const x` — **Borrow const-restricted reference**: Requests reference
   semantics with const-restricted access. The effective expression type is
   `&const T`, ensuring that subsequent binding resolution preserves the
   restriction on operations through that access path. The operator does not
   independently create a reference or make the object's value immutable.
 * `&move x` — **Borrow move-capable reference**: Requests reference semantics
   that explicitly preserve permission to consume the referenced object's value
   later. The effective expression type is `&move T`; this permission does not
   itself consume the value of an object. The subsequent binding conversion
   determines whether the value is actually moved, copied, or accessed through a
   reference.
 * `&&x` — **Forward expression**: Preserves the initial expression type and its
   qualification, except that a latent-move-capable reference becomes a
   move-capable reference.
 * `*x` — **Value semantics**: Requests value semantics rather than retaining
   reference semantics to an existing object. The effective expression type
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
flag when a binding operator was used on the initial expression; specifically
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
   value- or reference-semantics explicitly.
* `: const` - **Const-restricted binding**: Uses the binding context’s
   const-restricted form of its default binding behavior. It restricts
   operations through the resulting binding without necessarily creating a
   reference or an independent object, and without implying that the underlying
   object is intrinsically immutable.
 * `: &` - **Reference binding**: Binds through a reference. If the effective
   expression is a reference, borrows its referent; if it is a value, may
   materialize the value as an object and borrow a reference to it.
 * `: &const` - **Const-restricted reference binding**: Like `: &`, but restricts
   operations through the resulting reference without making the referent
   intrinsically immutable.
 * `: &move` - **Move-capable reference binding**: Like `: &`, but grants
   permission to move from the referent through the resulting reference without
   itself moving the object's value.
 * `: &&` - **Inferred binding**: Infers value- or reference-semantics from the
   effective expression type, preserving its qualification. The binding context
   determines the resulting binding type and conversion.
 * `: *` - **Value binding**: Binds using value semantics rather than retaining
   a reference to the existing object. The destination receives a value of its
   own, with copying or moving governed by the applicable rules and available
   operations.
 * `: *const` - **Const-restricted value binding**: Binds a value while
   restricting how that object may be accessed through the resulting binding. It
   combines value semantics with const-restricted access, without implying that
   the object's value is inherently immutable.

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

A variable binding introduces a name for either an object or a reference to
an existing object. Its general form is:

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

A const-restricted variable binding introduces a name for either an object or a
reference to an existing object. Its general form is:

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

It is a compilation error when the matching cell is a "redundant" or "error".
In this case it is caused by redundant syntax, or where the meaning of such a
syntax is not really clear.


### Function argument binding

A function argument binding introduces a name for either an object or a
reference to an existing object. Its general form is:

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

These are the overload priorities based on the result of the effective
expression.

Overload priority:
 1. exact match with effective expression (see below)
 2. `fn(a : &&)`
 3. `fn(a : &move)`
 4. `fn(a : &)`
 5. `fn(a : &const)`
 6. one of: `fn(a : *)`, `fn(a : *const)`, `fn(a)`, `fn(a :)`, `fn(a : const)`

Exact match of the binding qualifier with the type of the effective expression
after applying the binding operator has priority above this list:
 * `T` matches exactly with `fn(a : *)`, `fn(a : *const)`, `fn(a)`, `fn(a :)` or
   `fn(a : const)`
 * `&T` matches exactly with `fn(a : &)`
 * `&const T` matches exactly with `fn(a : &const)`
 * `&move T` matches exactly with `fn(a : &move)`

It is a compilation error when, after dropping non-matching overloads, multiple
overloads of the following are visible: `fn(a : *)`, `fn(a : *const)`, `fn(a)`,
`fn(a :)`, `fn(a : const)`


### Function return binding

A function return binding creates a temporary value or a reference to
an existing object. Its general form is:

> `foo` `=` `fn` `(` ... `)` __(__ `->` _binding-qualifier_**?** _type_**?** __)?__ `{`\
>      `return` _binding-operator_**?** _expr_\
> `}`

Unlike the previous binding contexts a function return value is an expression
used by the outer expression; each cell thus specifies the initial expression
type.

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

It is a compilation error when the matching cell is an "redundant". `-> *const`
is redundant because const restriction is not part of the type of a by-value
return expression. The caller determines the access qualification of any binding
it creates from the returned value.


## Conversion

Conversion occurs after the binding operator determines the effective expression
type and the binding context determines the resulting binding type. The
conversion depends on the initial expression type, the effective expression
type, and the resulting binding type.

The initial expression type identifies whether the source is a temporary value
or a reference to an existing object. The effective expression type determines
the expression's qualification for binding resolution. The conversion rules
additionally consider the initial expression type, so an explicit transformation
of the effective type does not necessarily discard every capability associated
with the original source.

Below is the conversion table, each conversion column represents a resulting
binding qualifier; each row specifies an initial and effective expression type.
"otherwise" means any other reachable effective expression type for that initial
expression type, excluding effective types listed in other rows for the same
initial expression type.

| Initial     | Effective    | `T`   | `const T` | `move T` | `&T`                | `&const T`          | `&move T`           |
|-------------|--------------|-------|-----------|----------|---------------------|---------------------|---------------------|
| `T`         | always       | Pass  | Pass      | Pass     | Materialize         | Materialize         | Materialize         |
| `&T`        | `T`          | Copy  | Copy      | Copy     | Copy -> Materialize | Copy -> Materialize | Copy -> Materialize |
| `&T`        | otherwise    | Copy  | Copy      | Copy     | Borrow              | Borrow              | -                   |
| `&const T`  | `T`          | Copy  | Copy      | Copy     | Copy -> Materialize | Copy -> Materialize | Copy -> Materialize |
| `&const T`  | otherwise    | Copy  | Copy      | Copy     | -                   | Borrow              | -                   |
| `&move T`   | `T`          | Move  | Move      | Move     | Move -> Materialize | Move -> Materialize | Move -> Materialize |
| `&move T`   | `&move T`    | Move  | Move      | Move     | Borrow              | Borrow              | Borrow              |
| `&move T`   | otherwise    | Copy  | Copy      | Copy     | Borrow              | Borrow              | -                   |
| `&latent T` | `T`          | Copy  | Copy      | Copy     | Copy -> Materialize | Copy -> Materialize | Copy -> Materialize |
| `&latent T` | `&move T`    | Move  | Move      | Move     | Borrow              | Borrow              | Borrow              |
| `&latent T` | otherwise    | Copy  | Copy      | Copy     | Borrow              | Borrow              | -                   |

Legend:
 * **Pass (pass-through)**: Use the temporary value directly, without copying or
   moving.
 * **Materialize**: Create an anonymous object initialized with the temporary
   value, then borrow a reference to that object.
 * **Borrow**: Create a reference to an existing object, respecting the
   destination's qualification.
 * **Copy -> Materialize**: Copy the original object's value into an anonymous
   object, then borrow a reference to that object.
 * **Move**: Use the move operation if supported; otherwise fallback to copy if
   supported; otherwise compilation fails.
 * **Move -> Materialize**: Initialize an anonymous object using the Move rule,
   including its copy fallback, then borrow a reference to that object.
 * **—**: The initial/effective type combination is unreachable or previously
   rejected under the binding-operator or binding-qualifier tables. The
   compiler should not read an unsupported conversion as a request to try
   a less restrictive conversion.

When a function's return binding requires a reference and the original return
expression has initial expression type `T`, the conversion materializes the
temporary in the caller's code-block. This applies even when an explicit binding
operator changes the effective expression type to a reference type. The binding
operator does not itself create a reference or materialize the temporary; these
operations are determined by the return-binding conversion.

This rule does not relocate objects that already exist independently in the
callee's code-block. Returning a reference to such an object is invalid if the
object does not outlive the returned reference.

```
// Materialization of make_value() is done in the caller's block:
foo = fn() -> & {
  return make_value()
}

// Materialization of make_value() is done in the caller's block:
bar = fn() -> & {
  return &make_value()
}

// Materialization of make_value() happens inside the function's body
// returning a reference to an object that is local to a function is invalid.
qux = fn() -> & {
  a <- make_value()
  return &a
}
```


## Constants

The `const` qualifier restricts the operations permitted through a particular
binding or reference. It provides read-only access through that path without
making the object's value intrinsically immutable.

```text
a <- "Hello World"
r : &const <- a    // Reading through r is permitted.
                   // Modifying or moving through r is not permitted.
```

An operation that receives const-restricted access may explicitly cast it to an
unqualified reference to modify the object's value. Such a cast bypasses the
restriction for the resulting access path.

The same object may also be accessible through other bindings or references with
different qualifications. Consequently, `const` restricts operations through a
particular access path, but does not guarantee that the object's value cannot
change.

Function overload resolution respects these restrictions. A const-restricted
reference binding can accept an unqualified, const-restricted, or move-capable
source reference. Conversely, a const-restricted source reference cannot
implicitly satisfy a destination requiring unqualified or move-capable access.
Explicit qualification casts may bypass these restrictions.


## Immutability

All objects associated with variables are mutable according to the language
semantics, including objects accessed through const-qualified bindings. The
const qualification restricts operations through a particular binding path; it
does not make the object's value intrinsically immutable. Explicit qualification
casts may bypass this restriction.

The implementation must preserve the observable behavior of modifications
permitted by the language. The compiler may place an object in a read-only
memory segment only when it can establish that doing so preserves observable
program behavior under the as-if rule.

Types, templates, and function overload-sets become frozen when they are first
used. Once frozen, their compile-time definitions cannot be changed. Evaluation
is lazy: these entities are evaluated only when needed, such as when generating
executable code for `main()` or another required symbol.

Variables that hold types or function overload-sets remain mutable at runtime,
but modifying their runtime values cannot alter the frozen compile-time
definitions or change the program's types and overload resolution.


## Temporaries

A temporary is a value produced by an expression rather than accessed through an
existing binding. Its initial expression type is `T`.

A temporary can initialize a variable binding directly and can be consumed
without an explicit move operator. When a reference binding requires
materialization, the temporary is materialized in the code-block scope where the
expression requiring the reference is evaluated.

The materialized temporary remains alive until that scope exits. A reference to
it is valid only while the temporary's object remains alive; binding a
reference does not extend the temporary's lifetime.

```text
r <- &make_value()    // Materializes the result in the current scope.
use(r)                // Valid while that scope remains active.
```

When the return-binding conversion requires materialization, the temporary is
materialized in the caller's code-block. This applies even when an explicit
binding operator changes the effective expression type to a reference type, as
in `return &make_value()`. The binding operator does not itself materialize the
temporary; the return-binding conversion determines whether materialization
occurs.

This rule does not apply to a reference to an object that already exists in the
callee's code-block, such as a local variable. Returning a reference to an
object whose lifetime ends when the callee's code-block exits is invalid.

Temporaries therefore support direct initialization and consumption while
allowing references to them only for the lifetime of their materialized
object.


## Copying

Copying initializes an independent object from an existing object's value
without changing the source. It requires the type to support copying.

When an object is initialized from a named variable or reference, the default
no-binding operator rule prevents implicit consumption. A move-capable
expression therefore loses its move capability unless an explicit binding
operator preserves it.

```text
a <- "Hello World"
b <- a                 // Copies the value from `a`; `a` remains unchanged.

r <- &a                // Borrows `a`.
c <- r                 // Copies the referenced value from `a`.

m <- &move a           // Creates a move-capable reference to `a`.
d : * <- m             // Copies; ordinary use loses move capability.
e : * <- &move m       // Moves from `a` through `m`.
```


## Moving

Moving initializes a destination object using the type's move operation when
supported. Otherwise, it falls back to copying if copying is supported. If
neither operation is supported, compilation fails.

When a move operation is performed, the source object remains alive in a
valid-but-unspecified state, unless the move operation guarantees a stronger
postcondition.

An object in this state remains valid, and all operations defined for its type
remain permitted. Their results may depend on the unspecified state. An object
may be reassigned or moved from again.

A move-capable reference grants permission to consume its referenced object's
value but does not itself perform a move. Moving occurs when a value-binding
conversion consumes the referenced object's value.

```text
a <- "Hello World"

b <- a                   // Copies; `a` remains unchanged.
r <- &move a             // Creates `a` reference; does not move.
c : * <- &move a         // Moves `a` into `c`.
d : * <- &move r         // Moves from a again through r.
```

After initializing `c`, `a` remains alive in a valid-but-unspecified state.
Initializing `d` from `r` is still permitted, and `a` remains valid afterward.

Ordinary use of a named variable does not implicitly grant move capability. The
explicit `&move` or `&&` operators preserve that capability, while temporaries
can be consumed without it.


# Lifetime

An object has a lifetime that begins when it is defined and ends when its
lifetime-ending conditions are met. The object's destructor is called directly
when its lifetime ends. Values do not have independent lifetimes; they are
represented by objects or produced by evaluating expressions.

The lifetime of a local object begins at the point of its variable definition
and ends at the end of its enclosing code-block. When a code-block exits, its
local objects are destroyed in reverse order of their definitions. The lifetime
of a global object begins when its definition is processed during compilation.
Its initialized state and lifetime are transferred to the generated executable,
where its lifetime ends at program termination. Global objects are destroyed in
reverse order of their definitions.

Updating an existing variable does not, by itself, end the lifetime of its
associated object. The `<-` operator calls `__assign__()`, while `=` calls
`__merge__()`. These operations determine how the object's value is changed;
their implementations are responsible for managing the lifetimes of any objects
they replace or destroy.

References do not extend the lifetimes of the objects they refer to. A reference
is valid only while its referent remains alive. Reseating a reference does not
change the lifetime of either the previously referenced object or the newly
referenced object.

A temporary is a value produced by an expression rather than accessed through an
existing binding. A temporary value can initialize a destination object directly
without creating a separate temporary object. When a reference binding requires
materialization, an anonymous object is created and initialized with the
temporary value, as if it were an anonymous variable defined in the code-block
where the expression requiring materialization is evaluated. The object remains
alive until that code-block exits, and its destructor is called when its
lifetime ends.

Temporaries materialized while evaluating function arguments belong to the
code-block where the function call occurs. The argument list does not introduce
a separate scope. When a function's return binding requires materializing a
temporary value, the object is materialized in the code-block containing the
function call. However, if the return expression produces a reference to a
temporary object created in the callee's code-block, returning that reference is
invalid because the object's lifetime ends when the callee's code-block exits.

When the compiler can determine that a reference is used after the lifetime of
its referent has ended, compilation fails.


# Multithreading / Hardware-I/O

The language adopts the C++ memory model for data races, atomic operations,
memory ordering, synchronization, and thread lifetimes, extended to include
hardware as an external execution agent accessing shared state.

A data race occurs when conflicting accesses to the same memory location execute
in different threads or hardware agents, at least one access is non-atomic, and
neither access happens-before the other. A data race results in undefined
behavior.

Concurrent access to shared mutable state must use atomic operations,
hardware-I/O operations where applicable, or appropriate synchronization.

The language provides the C++ memory-ordering semantics, including `relaxed`,
`acquire`, `release`, `acq_rel`, and `seq_cst` ordering for atomic and
hardware-I/O operations, subject to the constraints of each operation.
Hardware-I/O operations may establish synchronization with a device according to
their specified semantics.

Atomic operations are indivisible and participate in the modification order of
their atomic object. Hardware-I/O operations perform the prescribed device
interaction, which may require specific instructions or sequences of accesses to
multiple I/O ports or MMIO (Memory Mapped I/O). Their contracts define access
width, ordering, synchronization, and completion guarantees. Such operations
must not be eliminated, duplicated, combined, or replaced by cached values
unless explicitly permitted by their contracts.

Hardware may access ordinary memory through DMA. Such accesses participate in
the extended memory model, and synchronization must ensure the required
visibility and ordering between software and the device. Non-coherent DMA may
additionally require explicit cache maintenance.

In the absence of applicable ordering constraints, the compiler and execution
environment may optimize ordinary memory accesses as permitted by the memory
model. In particular, multiple reads from the same memory location may reuse a
previously loaded value, and multiple writes may be combined or eliminated.
Memory accesses may also be reordered.

Fences constrain the ordering of ordinary-memory, atomic, and hardware-I/O
operations according to their specified semantics. A fence does not
independently establish a happens-before relationship between agents;
synchronization must be established through the atomic operation, hardware-I/O
operation, or synchronization primitive defined by the memory model.

Ordering alone does not guarantee device-operation completion, device
acknowledgement, or DMA visibility. These guarantees are provided only by
operations or platform contracts that explicitly define them.


## Explicit Casts

The standard library provides functions for explicitly changing the
qualification of a reference when the corresponding transformation is not
permitted by implicit conversion.

| Function                      | Effect                                                             |
|-------------------------------|--------------------------------------------------------------------|
| `std.make_unqualified_ref(x)` | Returns an unqualified reference, `&T`, to the same referent.      |
| `std.make_move_ref(x)`        | Returns a move-capable reference, `&move T`, to the same referent. |

These functions change the qualification of the resulting reference, not the
object's value. They do not themselves copy, move, or modify the referent.

The resulting reference has the qualification specified by the function.
Qualifications are not combined: `std.make_unqualified_ref` does not preserve
move capability, and `std.make_move_ref` does not preserve a const restriction.
In particular, `std.make_move_ref` can explicitly remove a const restriction
while granting move capability.

These functions bypass the normal implicit qualification rules. The programmer
is responsible for ensuring that operations performed through the resulting
reference are valid.

For example, a method receiving a const-restricted reference can explicitly
remove the restriction from its access path to modify the underlying object:

```text
my_type = struct {
    counter <- 0.0

    foo = fn(@self self : &const my_type) {
        self_nc <- &&std.make_unqualified_ref(self)
        self_nc.counter += 1.0
    }
}

co = my_type()
co.foo()
```

Although `co` is const-restricted, `foo` explicitly obtains an unqualified
reference to the same object. This permits modification through `self_nc`
without changing the qualification of `co` or making the underlying object
intrinsically mutable.

Similarly, `std.make_move_ref` grants permission to consume the referenced
object's value, but does not itself perform a move. A subsequent
variable-binding conversion may move from the referent when the applicable
conversion rules permit it. The source binding remains alive in a
valid-but-unspecified state after an actual move, subject to any stronger
guarantee provided by the type's move operation.


## Updating Variable Bindings

When the name already resolves to an existing binding, the statement updates
that binding rather than defining a new one. Binding qualifiers and type
annotations are permitted only when defining a binding and must not be
specified when updating one. The existing binding's type and qualification
remain unchanged; the update operator determines the operation to perform.

The `<-` and `=` operators determine which operation is called:
* `<-` calls `__assign__()`.
* `=` calls `__merge__()`.

```text
a <- 1.0
a <- 2.0       // Calls a.__assign__(2.0).

b = 1.0        // Creates a const-restricted binding.
// b = 2.0     // Error: f64 does not support __merge__().

f = fn(a : *int) {}     // Creates an overload-set.
f = fn(a : *string) {}  // Merges the overload-sets, std.overload_set supports __merge__().
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

Very few types support `__merge__()`, examples that do support it:
 - Function overload-sets allow new functions to be added.
 - Types allow member functions and member variables, or enum names to be
   merged.
 - Templated types, functions and global variables also creates an overload-set
   of types, functions and variables.

The semantics of `__assign__()` and `__merge__()` are defined by the type. An
update is invalid when the required operation is unsupported or its binding
qualifications cannot be satisfied.


### Updating References

When updating an existing reference binding, the update must not specify a
binding qualifier or type annotation. An explicit binding operator that produces
a reference reseats the existing reference, preserving its qualification. This
rule takes precedence over the normal `<-` /` =` update dispatch. Otherwise:
 * `<-` calls `__assign__()` on the referenced object, and
 * `=` calls `__merge__()` on the referenced object.

```text
a <- 1.0
b <- 2.0

r <- &a
r <- 2.0              // Calls r.__assign__(2.0), modifying a.
r <- &b               // Binding operator resolves to a reference: reseats `r`.
r <- get_reference()  // Modifies `b` with the referenced object's value
r <- &get_reference() // Binding operator resolved to a reference: reseats `r`.
```

Reseating does not copy or move either referenced object's value, nor does it
change the reference binding's qualification. The new reference must be
implicitly convertible to the existing binding's reference qualification.

```text
cr = &a
cr <- &b             // Reseats cr; it remains const-restricted.

mr <- &move a
mr <- &move b        // Reseats mr; it remains move-capable.
// mr <- &b           // Error: cannot implicitly grant move capability.
```

A move-capable reference grants permission to consume its referenced object's
value but does not itself perform a move. Consumption occurs only when a
subsequent variable-binding conversion moves from the referenced object.

```text
a <- "Hello World"
r <- &move a         // Creates a reference; `a` remains unchanged.
c : * <- &move r     // Moves the value from `a` into `c`.
```

Reference bindings alias existing objects. Creating or reseating a reference
does not itself copy, move, or modify the referenced object's value.
