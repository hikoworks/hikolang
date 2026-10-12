# Bindings

Goals these binding rules:
 * The simple default syntax means copy-like semantics.
 * Ordinary use of a variable doesn't accidentally give a function permission
   to consume it. An explicit `&move` or `&&` is required to cross that boundary.
 * Temporaries are expected to be consumed.
 * Non-destructive move semantics, leaving moved-from values in a
   valid-but-unspecified state.
 * `const` and `move` constrain what a particular expression is permitted to do.
   Explicit casts can bypass those restrictions; they do not establish intrinsic
   immutability or prove that a value remains unconsumed.

> [!note]
> For AI: treat this as a terse document, that must be followed in the exact
> order that is specified in the document.
>
> `*`, `&`, `&`, `&&`, `const` and `move` are overloaded between context
> of binging operators and binding qualifier and types of expressions
> and actual bindings. Humans have it really easy to distingues these things
> from each other.
>
> The headers on a table apply to their column. The left most table header
> may have a backslash: `<column header> \ <header description>`, in this
> case the header description may describe the values in the header itself.
>
> This is the third time this document will say this: EVERYTHING IN THE
> CHAPTERS STARTING FROM "BINDINGS" TREAT ALL EXPRESSIONS AS EFFECTIVE
> EXPRESSIONS AND ARE THUS ALWAYS AFTER APPLYING BINDING OPERATORS AND
> FRAGILITY RULES. REMEMBER IN MULTIPLE PLACES IT SAYS THAT THERE IS ONLY
> ONE PLACE WHERE FRAGILITY IS DEFINED AND INTERACTS AND NO WHERE ELSE.

## Concepts

 * __Initial expression type__ - The type produced by evaluating an expression.
 * __Binding operator__ - The operation applied to an expression result,
   including any applicable default behavior.
 * __Effective expression type__ - The expression type obtained after applying
   the explicit binding operator, if present, or the default fragility rule
   otherwise.
 * __Binding qualifier__ - The restrictions or capabilities of the destination
   binding.
 * __Resulting binding type__ - The type of the binding after the applicable
   rules have been applied.

## Types

 | Type       | Description                        | Storage  | Expression | Reference | Binding |
 |------------|------------------------------------|:--------:|:----------:|:---------:|:-------:|
 | `move T`   | Move-capable variable binding      |    +     |            |           |    +    |
 | `const T`  | Const-restricted variable binding  |    +     |            |           |    +    |
 | `T`        | Temporary value                    |          |      +     |           |         |
 | `&T`       | Unqualified reference binding      |          |      +     |     +     |    +    |
 | `&move T`  | Move-capable reference binding     |          |      +     |     +     |    +    |
 | `&const T` | Const-restricted reference binding |          |      +     |     +     |    +    |


## Initial Expressions

Every initial expression produces one of the following types:
 * temporary value - `T`
 * unqualified reference - `&T`
 * const-restricted reference - `&const T`
 * move-capable reference - `&move T`

The following initial expression kinds may result in the types:

| Expression kind                            | Possible resulting initial expression types
|--------------------------------------------|----------------
| evaluation of a function-call              | `T`, `&T`, `&const T` or `&move T`
| evaluation of a move-capable variable      | `&move T`
| evaluation of a const-restricted variable  | `&const T`
| evaluation of an unqualified reference     | `&T`
| evaluation of a move-capable reference     | `&move T`
| evaluation of a const-restricted reference | `&const T`

Non-binding operators, like mathematics operators `1 + 2` are lowered to
function-calls, therefor should be treated as function calls in respect of
the table above.

## Binding operators

Binding operators are used to control how a binding qualifier may interpret the
result of an expression:

 * **Default binding**: When no binding operator is specified, the initial
   expression is interpreted using the default fragility rule. This preserves
   ordinary copy-like semantics by preventing a move-capable reference from
   implicitly granting permission to consume the referenced value.
 * `&x` — **Borrow reference**: Borrows a reference to the initial expression's
   value without consuming it. The resulting reference is unqualified, unless
   the initial expression is const-restricted, in which case the const
   restriction is preserved.
 * `&const x` — **Borrow const-restricted reference**: Borrows a reference
   while restricting access through that reference to const-qualified
   operations. This allows a value to be accessed without granting unqualified
   or move-capable permissions.
 * `&move x` — **Borrow move-capable reference**: Borrows a move-capable
   reference, explicitly preserving permission to consume the referenced value
   later. This does not itself move or consume the value. The operator is
   invalid when the initial expression does not provide move-capable access.
 * `&&x` — **Forward expression**: Forwards the initial expression's type and
   qualification exactly, bypassing the default fragility rule. This allows
   the caller to explicitly preserve the distinction between temporary values,
   ordinary references, const-restricted references, and move-capable
   references.
 * `*x` — **Value semantics**: Produces a value rather than a reference, copying
   from existing storage when necessary. This explicitly requests value
   semantics, allowing the destination to receive an independent value rather
   than a reference to existing storage. The operation is invalid if the
   required copy or move is unsupported.

Here is the table showing how an initial expression of a certain type is
modified by each of the binding operators into an effective expression type:

 | op \ expr   | `T`        | `&T`        | `&const T`  | `&move T`
 |-------------|------------|-------------|-------------|------------
 | default     | `T`        | `&T`        | `&const T`  | `&T` (fragile)
 | `&x`        | `&T`       | `&T`        | `&const T`  | `&T`
 | `&const x`  | `&const T` | `&const T`  | `&const T`  | `&const T`
 | `&move x`   | `&move T`  | error       | error       | `&move T`
 | `&&x`       | `T`        | `&T`        | `&const T`  | `&move T`
 | `*x`        | `T`        | `T`         | `T`         | `T`

This section is the sole definition of this fragility rule. The "Binding"
section after this will treat the type of the effective expression as having the
binding operator or fragility rule already applied.

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
   const-restricted form of its default binding behavior. It restricts operations
   through the resulting binding without necessarily creating a reference or an
   independent value, and without implying that the underlying object is
   intrinsically immutable.
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
 * `: &&` - **Exact forwarding**: Preserves the effective expression’s reference
   and qualification characteristics as precisely as the binding context allows.
   It is intended for forwarding an effective expression without silently
   weakening or changing its capabilities through the usual default rules.
 * `: *` - **Value binding**: Binds using value semantics rather than retaining a
   reference to the existing object. The destination receives a value of its
   own, with copying or moving governed by the applicable rules and available
   operations.
 * `: *const` - **Const-restricted value binding**: Binds a value while
   restricting how that value may be accessed through the resulting binding. It
   combines value semantics with const-restricted access, without implying that
   the underlying value is inherently immutable.


If the result of a binding is a reference `&`, `&const` or `&move` and the
effective expression is a temporary `T`, then the references is borrowed from
the materialization of a temporary. Materialization of temporaries is specified
in: [Lifetime](lifetime.md).

If the result of a binding is a variable `move T` or `const T` and the effective
expressions are:
 * `&T` or `&const T`: Copy initialize the value. It is a compile error if
   the type does not support copying.
 * `&move T` or `T`: Move initialize the value, or copy initialize the value.
   It is a compile error if the type supports neither moving or copying.


### Variable binding

A variable binding introduces a name for either a value or a reference to
existing storage. Its general form is:

> _name_ __(__ `:` _binding-qualifier_**?** _type_**?** __)?__ `<-` _binding-operator_**?** _expr_

The `T`, `&T`, `&const T`, and `&move T` column headers denote the effective
expression type. Each cell specifies the resulting binding type.

When the "default" column has "-" for a row, then only the `T`, `&T`,
`&const T`, and `&move T` columns should be used in the evaluation of the
resulting type of the binding.

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

The simplified bindings listed in the bottom three rows behave differently when
the effective expression's explicit-binding-operator flag is:
 * true: use only the `T`, `&T`, `&const T` and `&move T` columns from the
   effective expression type and lookup the binding type from the cells below.
 * false: lookup the binding type from the cells below the "default" column.


### Const-restricted variable binding

A const-restricted variable binding introduces a name for either a value or a
reference to existing storage. Its general form is:

> _name_ __(__ `:` _binding-qualifier_**?** _type_**?** __)?__ `=` _binding-operator_**?** _expr_

The `T`, `&T`, `&const T`, and `&move T` column headers denote the effective
expression type. Each cell specifies the resulting binding type.

When the "default" column has "-" for a row, then only the `T`, `&T`,
`&const T`, and `&move T` columns should be used in the evaluation of the
resulting type of the binding.

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

The simplified bindings listed in the bottom three rows behave differently when
the effective expression's explicit-binding-operator flag is:
 * true: use only the `T`, `&T`, `&const T` and `&move T` columns from the
   effective expression type and lookup the binding type from the cells below.
 * false: lookup the binding type from the cells below the "default" column.

It is a compilation error when the matching cell is an "redundant" or "error".
In this case it is caused by redundant syntax, or where the meaning of such a
syntax is not really clear.


### Function argument binding

A function argument binding introduces a name for either a value or a reference
to existing storage. Its general form is:

> `foo` `=` `fn` `(` _name_ __(__ `:` _binding-qualifier_**?** _type_**?** __)?__ `)` `{` ... `}`\
> `foo` `(` _binding-operator_**?** _expr_ `)`

The `T`, `&T`, `&const T`, and `&move T` column headers denote the effective
expression type. Each cell specifies the resulting binding type.

When the "default" column has "-" for a row, then only the `T`, `&T`,
`&const T`, and `&move T` columns should be used in the evaluation of the
resulting type of the binding.

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

The simplified bindings listed in the bottom three rows behave differently when
the effective expression's explicit-binding-operator flag is:
 * true: use only the `T`, `&T`, `&const T` and `&move T` columns from the
   effective expression type and lookup the binding type from the cells below.
 * false: lookup the binding type from the cells below the "default" column.


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

The `T`, `&T`, `&const T`, and `&move T` column headers denote the effective
expression type. Different from the previous binding a function return value is
an expression used by the outer expression; each cell thus specifies the initial
expression type.

When the "default" column has "-" for a row, then only the `T`, `&T`,
`&const T`, and `&move T` columns should be used in the evaluation of the
resulting type of the binding.

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

The simplified bindings listed in the bottom three rows behave differently when
the effective expression's explicit-binding-operator flag is:
 * true: use only the `T`, `&T`, `&const T` and `&move T` columns from the
   effective expression type and lookup the binding type from the cells below.
 * false: lookup the binding type from the cells below the "default" column.

When the function return type is `T` and the input's effective expression
type is:
 * `&T` or `&const T` the input's value is copied; it is a compilation error if
   the value can not be copied. 
 * `&move T` the input's value is moved or copied; it is a compilation error if
   the value can neither be moved or copied.
 * `T` the input's value is forwarded.

It is a compilation error when the matching cell is an "redundant". In this case
it is caused by redundant syntax, because `const` is already implied.

## Combined type conversion

The binding operator and binding qualifier together define how an initial
expression is converted. In the table below you select the columns `T`, `&T`,
`&const T` or `&move T` from the initial expression type; the row is the result
from the binding qualifier tables; the cell gives the type of conversion that
happens.

 | Binding    | `T`         | `&T`    | `&const T`   | `&move T`
 |------------|-------------|---------|--------------|------------
 | `T`        | forward     | copy    | copy         | move
 | `const T`  | forward     | copy    | copy         | move
 | `move T`   | forward     | copy    | copy         | move
 | `&T`       | materialize | forward | invalid      | borrow
 | `&const T` | materialize | borrow  | forward      | borrow
 | `&move T`  | materialize | invalid | invalid      | forward
 
Function return values can be `T`, while other value semantic bindings are
`const T` or `move T`.

The conversions are:
 * forward: The value/reference of the initial expression is forwarded into the
   binding
 * materialize: The temporary is materialized, then a reference is borrowed from
   the materialization.
 * copy: The value is copied. It is a compilation error if the value does not
   support copying.
 * move: The value is moved or copied. It is a compilation error if the value
   support neither moving or copying.
 * invalid: This conversion is a compilation error.


## Constants

The `const` qualifier restricts how a value may be accessed through a particular
binding or reference. It provides a const-restricted view of a value, allowing
the language to distinguish operations that accept const-restricted access from
those that require unqualified or move-capable access.

A const-restricted binding is written using `const`:

```text
a : const <- 1.0
b = 2.0
```

Both `a` and `b` are const-restricted bindings.

Const-restricted does not, by itself, make a value immutable. It restricts
the operations available through the restricted access path. The same underlying
value may remain accessible through another binding or reference with different
qualifications. Consequently, a const-restricted binding does not guarantee that
the value cannot change; it guarantees only the restrictions associated with
const-restricted access.

When a function is called with a const-restricted expression, overload resolution
takes its qualification into account. A function accepting a const-restricted
reference can receive values through const-restricted, unqualified, or
move-capable references. A function requiring an unqualified reference cannot
accept a const-restricted reference, because doing so would discard the access
restriction. A function requiring a move-capable reference likewise cannot
accept a const-restricted reference, because const-restricted does not grant
permission to consume the value.

Member access follows the same principle. A member variable accessed through a
const-restricted expression can only be read from through that access path.

Const-restricted therefore describes the permitted access to a value, rather
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
a <- make_value()       // Initialize a move-capable variable from a temporary.
```

Here, `make_value()` produces a temporary of type `T`. The initializer uses
value-binding semantics, so `a` becomes a move-capable value variable
initialized from the result. No intermediate reference binding is required.

Temporaries can also be bound to reference parameters or reference variables. In
these cases, the binding establishes a reference to the temporary's storage
rather than initializing a new value. The temporary must remain alive for as
long as the reference is permitted to be used.

When overload resolution considers a temporary expression, its type `T` is used
to determine which bindings are eligible and their relative priority. This
ordering allows value initialization to be preferred while still permitting
functions to accept references to temporaries when appropriate.

A temporary's eligibility for move-capable access does not mean that its
expression type is `&move T`. The expression retains its type `T` until the
binding context interprets it. Binding a temporary to a reference and
initializing a value from a temporary are distinct operations, even when both
can be performed without copying the underlying value.

The [lifetime] of a temporary is governed by the context in which it is created
and bound. A reference to a temporary must not outlive the temporary's storage.
The language's [lifetime] rules determine when that storage can be released,
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
ordinary expression of a named move-capable variable loses its move-capable
through fragility. Consequently, a variable binding does not
implicitly consume a named source.

```text
a <- "Hello World"
b <- a
```

The expression `a` initially has type `&move T`, because `a` is a move-capable
variable binding. Without an explicit binding operator, fragility removes the
move-capable, producing `&T`. The value binding for `b` therefore
initializes a new value by copying from `a`. The value of `a` remains unchanged.

The same principle applies when the source is a reference binding:

```text
a <- "Hello World"
r <- &a
b <- r
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

r <- &a   // Reference to a.
r2 <- &r  // Another reference to the same storage.
b <- r    // Copy the value referenced by r.
```

Both `r` and `r2` refer to the same underlying value. The explicit `&` operator
in `r2 <- &r` requests reference binding rather than value initialization. By
contrast, `b` owns a separate value initialized by copying the value referenced
by `r`. Modifying the value through an appropriate reference to `a` does not
modify `b`, and modifying `b` does not modify the value owned by `a`.

A move-capable reference does not automatically cause a move when used in an
ordinary expression:

```text
a <- "Hello World"
r <- &move a
b : * <- r           // Copies the referenced value.
c : * <- &move r     // Consumes the value referenced by r.
```

The expression `r` initially has type `&move T`, but its move-capable is
discarded through fragility when used without an explicit binding operator.
Consequently, `b` is initialized by copying the referenced value, provided
copying is supported. In contrast, `&move r` explicitly preserves the move-capable,
allowing `c` to consume the value from `a`.

Copying therefore provides a non-destructive way to initialize values from named
expressions. Fragility prevents an ordinary expression from implicitly granting
permission to consume a named value, keeping copying and moving distinct
operations.


## Moving

Moving transfers a value from a source to a destination without ending the
source binding's lifetime. The destination receives the value, while the source
remains a valid binding in a moved-from state.

A moved-from value remains valid, but its previous value is no longer guaranteed
to be preserved and may be unspecified. All operations on a moved-from value
remain valid, but their result may be depended on unspecified state. Some
operations may leave the value in an unspecified or establish a fully
specified state. A moved-from value may also be reassigned to establish a fully
specified state.

The compiler and runtime will allow all operations on moved-from values and
allow a moved-from value to be moved from again.

Moving is distinct from creating a reference to a value. A move-capable
reference grants permission for a subsequent operation to consume the referenced
value, but creating or passing that reference does not itself perform a move.
The move occurs when the reference is bound to a destination that requests value
semantics.

Consider the following example:

```text
a <- "Hello World"        // a is a move-capable string value.

b <- &move a              // b is a move-capable reference to a.
                          // The value of a is unchanged.

c : * <- &move a          // Initialize c by moving the value from a.
```

The declaration `a` creates a move-capable value. The declaration of `b`
creates a reference to the existing value and preserves its move-capable.
The third declaration explicitly requests value semantics through the `*`
binding qualifier. The initializer supplies a move-capable reference, so the
destination can consume the referenced value. The string value is transferred to
`c`, and `a` remains alive in a moved-from state.

An ordinary expression does not implicitly grant permission to consume a named
value. For example:

```text
a <- "Hello World"

b <- a                     // Does not request a move from a.
c <- &move a               // Creates a move-capable reference.
d : * <- &move a           // Explicitly requests a move from a.
```

The distinction is intentional. The expression `a` does not preserve its move-capable
when used without an explicit binding operator. By contrast,
`&move a` explicitly preserves that qualification, allowing a destination that
accepts a move-capable source to consume the value.

Move-capable therefore expresses permission, not an operation. A
move-capable reference identifies a value that may be consumed; a
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










> [!note]
> When temporary value of type `T` is bound as a reference, the temporary value
> may need to be materialized as a move-capable anonymous variable `move T`
> and its > reference is borrowed from that anonymous variable.

> [!note]
> The binding specification `fn(x : *)` and the prefix binding operator `*x`
> are distinct language constructs with independent semantics. They are not
> interchangeable, but share a conceptual meaning: both explicitly
> express value semantics rather than reference semantics.





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


[_invalid_]: #invalid-binding
## Invalid Binding

Invalid arguments causes the function overload to be dropped as a candidate
for overload resolution.

Invalid initialization of variables will cause a compile-time error.



## Updating Bindings

### Variables

When a binding already exists, `<-` and `=` update the existing binding rather
than creating a new one.

`<-` calls `__assign__()`:

```text
a <- 1.0
a <- 2.0       // Calls `a.__assign__(2.0)`.

b = 1.0        // Creates a const-restricted variable binding
//b = 2.0      // ERROR: `b.__merge__(2.0)` is not supported by float type.
```

`=` calls `__merge__()`:

```text
t = enum int[0...1] { foo }
t = enum { bar }            // Calls `t.__merge__(enum { bar })`
```

The `<-` and `=` as binding and assign operators have symmetric semantics:
 * `<-` introduces and updates values whose value changes through `__assign__()`.
 * `=` introduces values that are intended to be mostly constant and updates
       them through `__merge__()`.

For types and functions, `=` is primarily used to combine definitions at
compile time. Function values use `__merge__()` to combine functions into a
function overload set. Types can use `__merge__()` to combine their members, and
type templates use the same mechanism to form overload sets.

The `__assign__()` and `__merge()__` functions have definitions that reflect
how they can be selected based on their bindings qualifier:
 * `__assign__ = fn(@self self : &, other : &const)`
 * `__merge__ = fn(@self self : &const, other : &const)`

Which means for variables introduced with `=` the `__assign__()` function would
not be valid when reassigned using the `<-` operator. While most variables can't
be reassigned with `=` either since most types don't implement `__merge__()`.

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
r <- &b        // Reseat `r`; `r` remains const-restricted.

cr <- &const a
cr <- &const b // Reseat `cr`; its reference remains const-restricted.
cr <- &a       // Reseat `cr`; `&a` is convertible to `&const T`.

mr <- &move a
mr <- &move b  // Reseat `mr`; its reference remains move-capable.
// mr <- &a    // ERROR: unqualified reference `&a` can not be upgraded to a move-capable reference.
```

The new reference must have compatible reference qualification as the existing
reference binding. Reseating does not copy or move either the old or new
referenced value.

A move-capable reference does not itself move the referenced value:

```text
a <- "Hello World"
r <- &move a   // Creates a move-capable reference. The value of `a` is unchanged.
```

The value can be moved only when the move-capable reference is subsequently
passed to a binding that may consume it. For example by explicitly passing a
move-capable reference to a variable binding:

```text
c : * <- &move a  // `c` is a value; `&move a` selects a move-consuming initializer.
```

Reference bindings therefore provide a raw reference to existing storage. They
do not own, copy, or move the referenced value merely by being initialized or
reseated.
