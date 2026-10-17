# Type Definitions

## Overview

A type definition associates a name with a type produced by a metatype.
Metatypes define how member declarations and other definition contents are
interpreted to construct a type.

The general form of a type definition is:

```text
name = metatype : underlying-types {
    definition
}
```

The metatype processes the definition and produces a type, which is then
associated with `name`.

A type definition may also be parameterized, allowing template arguments to
modify its definition before the metatype processes it.

The language provides built-in metatypes, including `struct`, and supports
user-defined metatypes through the `@metatype` annotation.


## Metatypes

A metatype is a function that constructs a type from a definition. Metatypes
determine how the contents of a definition are interpreted, including which
members are created and what properties those members have.

A function is declared as a metatype using the `@metatype` annotation:

```text
@metatype my_metatype = fn(definition, underlying_types) {
    ...
    return type_from_definition
}
```

The function receives the definition to process and returns the resulting type.
A metatype may inspect, transform, or otherwise interpret the definition before
constructing the type.

A metatype must produce a valid type when used in a type definition.


### Built-in metatypes

Built-in type constructors are metatypes. In particular, `struct` is a metatype
rather than a separate mechanism for defining types.

For example:

```text
my_type = struct {
    float_member <- 0.5
    int_member : int[0...10] <- 5

    __init__(x : int[0...10]) {
        int_member <- x
    }
}
```

Here, `struct` interprets the definition and produces a structure type. The
resulting type is associated with `my_type`.

The structure-specific behavior is therefore determined by the `struct`
metatype. Other metatypes may interpret the same kinds of definition contents
differently or support additional member forms.


## Members

A definition may contain variable members, function members, and name members.
These member categories describe different kinds of declarations available to a
metatype when constructing a type.

Members may also have annotations that modify their properties or how they are
interpreted.


### Variable members

A variable member declares a variable belonging to the type or its instances,
depending on the member's annotations.

A variable member may specify an initial value and optionally an explicit type.

The following form declares a variable member with an explicit type and initial
value:

```text
member_name : Type <- initial_value
```

The initial value must be compatible with the declared type.

For example:

```text
float_member <- 0.5
int_member : int[0...10] <- 5
```

Here, `float_member` has an inferred type, while `int_member` is explicitly
declared to have the type `int[0...10]`.

The expression `int[0...10]` denotes an integer type constrained to the
inclusive range from `0` to `10`.

### Function members

A function member declares a function associated with a type or its instances.
It consists of a function signature and a body.

For example:

```text
__init__(x : int[0...10]) {
    int_member <- x
}
```

The function accepts a parameter named `x` of type `int[0...10]` and assigns its
value to `int_member`.

The name `__init__` identifies the initializer used when constructing an
instance of the type. When an instance is constructed, the initializer is
invoked according to the language's instance-construction rules, and the
supplied arguments must satisfy its parameter types.

For example:

```text
my_obj <- my_type(5)
```


### Name members

A name member declares a named alternative within a type. It is similar to an
enumeration case and may optionally specify a type-tuple describing the values
associated with that alternative.

A name member without a type-tuple consists only of its name. A name member with
a type-tuple can carry associated values, similar to associated-value cases in
Swift enumerations.

A type-tuple has the following forms:

```text
// Just an identifier is a member name with an automatic index.
bar

// An identifier with an explicit index.
baz = 1

// A name with a struct type definition in an enum definition
// is used for enum members that carry associated values.
foo = struct{a : int[0...10]; b : f64; c : string}

// A name with a string that holds the associated value.
// This `,` syntax is special for variable definitions and does not indicate
// a tuple.
qux = string, 4
```

This declared the name member `foo` with three associated values: an unlabeled
value of type `int[0...10]`, an unlabeled value of type `f64`, and a value
labeled `x` of type `string`.

The associated values supplied for a name member must conform to the types and
labels declared by its type-tuple.


## Member annotations

Annotations may be applied to members to modify their properties or
classification. The following annotations have defined meanings.


### Public members

The `@public` annotation marks a member as public, making it accessible
according to the language's public-accessibility rules.


### Type members

The `@type` annotation marks a member as a type-member. A type-member belongs to
the type itself rather than to individual instances and is therefore a
non-instance member.

The distinction between instance members and type-members determines whether a
member is associated with an individual instance or with the type as a whole.

Member annotations are interpreted as part of the type definition and are
processed according to the rules of the selected metatype.


## Parameterized type definitions

A type definition may declare template parameters. These parameters allow a
definition to be modified with supplied arguments before the metatype constructs
the resulting type.

Template parameters are declared in square brackets after the metatype name.
Each parameter specifies a name and a type constraint.

For example:

```text
my_template = my_metatype[T : type, N : int] {
    float_member <- 0.5
    int_member : T <- N

    __init__(x : T) {
        int_member <- x
    }
}
```

This definition declares two template parameters:

- `T : type` accepts a type argument.
- `N : int` accepts an integer argument.

The parameters may be referenced throughout the definition. For example, `T`
determines the declared type of `int_member` and the type of the initializer
parameter `x`, while `N` specifies the initial value of `int_member`.


### Template instantiation

A parameterized definition is instantiated by supplying arguments in square
brackets, in the order in which its parameters were declared.

For example:

```text
my_type = my_template[int[0...5], 3]
```

This binds `T` to `int[0...5]` and `N` to `3`.

Template arguments are applied to the definition before it is passed to the
metatype function that constructs the type. Consequently, the metatype receives
the definition with its template parameters substituted by the supplied
arguments.

In this example, the resulting definition contains the following effective
declarations:

```text
int_member : int[0...5] <- 3

__init__(x : int[0...5]) {
    int_member <- x
}
```

The metatype processes this modified definition and produces the resulting type,
which is associated with `my_type`.

A template argument must satisfy the corresponding parameter's type constraint.
Instantiation must also produce a valid definition and a valid resulting type.

The resulting type can be used like any other type:

```text
my_obj <- my_type(1)
```

The argument `1` satisfies the initializer's parameter type, `int[0...5]`.


## Definition Merging

A type definition is not necessarily complete when it is first declared.
Multiple definitions may contribute to the type associated with the same
variable name. These definitions are merged into a combined definition before
the type is evaluated.

Definition merging allows a type to be assembled incrementally. The members of
the resulting definition are determined by combining the contributions made by
its constituent definitions, subject to the merging rules of the language.


### The merge operation

The special function `__merge()__` defines how two definitions bound to the same
variable name are merged.

When multiple definitions are bound to the same variable name, `__merge()__` is
invoked to combine them. The resulting definition is used as the input to the
metatype responsible for constructing the type.

The merge operation must respect the type's current lifecycle state. While a
type is open, its definition may be extended through merging. Once the type is
frozen, only the additions and modifications permitted for frozen types may be
merged.

The specific rules governing conflicts between members, duplicate declarations,
incompatible annotations, and other overlapping definitions must be respected
during merging.


## Type Lifecycle

Types have two lifecycle states: **open** and **frozen**.


### Open types

A type remains open until it is used for the first time. While open, its
definition may be extended by merging additional definitions associated with the
same variable name.

The metatype is responsible for interpreting the merged definition and producing
the type. This evaluation is deferred until the type is used for the first time.

Consequently, declarations contributing to an open type can be collected and
merged before the metatype constructs the final type. The type produced by the
metatype reflects the combined definition rather than any individual
contribution.


### Freezing a type

When a type is used for the first time, its merged definition is evaluated by
its metatype. The resulting type is then frozen.

Freezing establishes the type's finalized definition and prevents subsequent
merges from arbitrarily changing its members or structure.

The precise meaning of first use is determined by the language's evaluation and
type-checking rules. Regardless of the triggering operation, a type must be
frozen before it is used in a manner that requires its finalized definition.

Once frozen, the type retains its identity and finalized structure. Subsequent
definitions cannot freely alter that structure.


### Merging into frozen types

Freezing does not necessarily prohibit every subsequent extension. Certain
member categories may support additional merges after a type has been frozen.

Such merges must comply with the restrictions of the relevant member category
and the underlying type representation.

For example, an enumeration-like type may permit additional name members to be
introduced after freezing, provided their associated indices are still available
and have not already been used in the underlying integer representation.

An extension that would introduce a conflicting index is invalid.

These permitted extensions do not reopen the type for unrestricted merging. Only
the specific additions allowed for the frozen type may be accepted.

The merge rules for each member category and metatype determine which extensions
are valid after freezing.


## Type Identity and Interning

Types are interned according to a canonical name and their template arguments.
The canonical name is established by the name of the first variable to which the
type is assigned.

The canonical name and template arguments together determine the type's
interning identity. Definitions contributing to the same named type are merged
under that identity, subject to the type's lifecycle and merging rules.

Distinct canonical names produce distinct type identities, even when the
resulting types have otherwise identical members and structure.


### Canonical names and aliases

The canonical name belongs to the type identity rather than to each variable
referring to the type.

When a type is assigned to another variable, the new variable becomes an alias
for the existing type. This assignment does not change the canonical name or
establish a new type identity.

For example:

```text
my_type = my_template[int[0...5], 3]
my_alias = my_type
```

Here, `my_type` establishes the canonical name of the resulting type. The
assignment to `my_alias` preserves that identity and does not cause the type's
definition to be merged under a new name.

A subsequent definition bound to the original variable name contributes to the
same named type and is subject to the applicable merging rules. Assigning an
alias alone does not constitute such a contribution.


### Template arguments and type identity

Template arguments form part of the type's interning identity. Consequently,
different template argument lists produce different interning identities, even
when they originate from the same parameterized definition.

For example:

```text
small_int = my_template[int[0...5], 3]
large_int = my_template[int[0...10], 3]
```

The resulting types have distinct identities because their canonical names and
template arguments differ.

Repeated requests for a type with the same canonical name and template arguments
refer to the same interned type rather than creating independent types with the
same identity.


## Interaction Between Merging, Templates, and Metatypes

Template instantiation and definition merging precede the construction of an
open type's finalized representation.

Template arguments modify the definition before it is processed by the metatype.
Definitions bound to the same variable name are merged according to the
applicable rules, and the resulting definition is passed to the metatype when
the type is evaluated for the first time.

The resulting type is then frozen. Any subsequent extensions are governed by the
rules for merging into frozen types.

Thus, type construction consists of three conceptually distinct operations:

1. **Definition preparation:** Template arguments are applied, and definitions
   contributing to the same named type are merged.
2. **Type evaluation:** The metatype processes the merged definition and
   constructs the type when it is first used.
3. **Freezing and extension:** The resulting type is frozen. Subsequent merges
   are restricted to the extensions permitted for frozen types.

These operations establish the relationship between type identity, definition
composition, metatype evaluation, and the lifecycle of the resulting type.


## Definition validity

A type definition must produce a valid type under the rules of its selected
metatype.

A definition or instantiation is invalid if, for example, a supplied template
argument violates its parameter constraint, a declared initial value is
incompatible with its member's type, an associated value violates a name
member's type-tuple, or the metatype fails to produce a valid type.

The language's type-checking and evaluation rules determine when such errors are
detected.


## Type-Template Matching and Function Specialization

A type-template name may be used in a type annotation without specifying its
template arguments. In this form, the name matches any instantiation of that
type-template, regardless of the arguments supplied during instantiation.

For example, `int` is a type-template whose argument specifies an integer
interval. The types `int[0...10]` and `int[0...100]` are distinct instantiations
of the same type-template.

```text
foo = fn(a : int) {
    ...
}
```

The parameter annotation `a : int` matches values whose types are instantiations
of the `int` type-template, independently of their interval arguments.


### Function specialization

When a function is called with an argument whose concrete type matches a
type-template in the function signature, a function specialization is created
for the concrete argument type.

The specialization retains the original, fully instantiated argument type rather
than replacing it with the unparameterized type-template specified in the
function signature.

For example:

```text
foo = fn(a : int) {
    ...
}

foo(5)       // Specialization for the concrete type of 5
foo(50)      // Specialization for the concrete type of 50
```

Assuming the literals have their corresponding inferred interval types, the
calls may produce specializations accepting `int[5...5]` and `int[50...50]`,
respectively.

Within each specialization, the parameter `a` retains its concrete type. The
function body can therefore be type-checked and evaluated with knowledge of the
specific type supplied by the caller.

Type-template matching and type identity remain distinct concepts. Matching
determines whether a concrete type is eligible for a function signature;
specialization determines the concrete function implementation associated with
that type. Neither operation changes the identity of the original type.

The rules for reusing existing specializations, selecting specializations for
calls with multiple arguments, and handling recursive calls are defined
separately.


## Lazy Evaluation and Definition Dependencies

Type definitions are evaluated lazily. Declaring a type does not necessarily
cause its metatype to execute immediately. Instead, evaluation is deferred until
the type is used for the first time.

As a result, the effective definition of a type may depend on changes that occur
after its declaration but before its first evaluation. These dependencies
include other types and global variables referenced by the definition.


### Dependencies on other types

A type definition may refer to another type that has not yet been frozen.

For example, consider a type `B` whose definition uses type `A`. Because type
`A` remains open until its first use, additional definitions may be merged into
`A` after the declaration of `B`.

When `B` is first used in an evaluation, the evaluation of its definition
resolves its dependency on `A`. The current merged definition of `A` is
evaluated, and `A` becomes frozen as part of resolving this dependency.

Consequently, the resulting type `B` may reflect members that were added to `A`
after `B` was declared.

If `A` has already been frozen before `B` is evaluated, the dependency instead
resolves against the already-evaluated type, including any extensions permitted
after freezing.


### Dependencies on global variables

A type definition may also reference a global variable whose value is used
during type construction.

Because evaluation is deferred, the value used by the metatype may differ from
the value that the variable held when the type was declared.

For example, suppose `B` uses the global variable `qux` in its definition. If
`qux` changes before `B` is first evaluated, the metatype may observe the
updated value when constructing `B`.

Once `B` has been evaluated and frozen, subsequent changes to `qux` do not
automatically cause `B` to be re-evaluated. The constructed type retains the
result of its completed evaluation, subject to the rules for extensions of
frozen types.


### Const restrictions on definition dependencies

To limit the effects of changes to global variables, a global variable must be
const-qualified before it can be referenced in a type definition that depends on
its value.

The const restriction constrains the operations through which the variable may
be modified, reducing the possibility that a deferred type evaluation observes
an unexpected value.

Const qualification does not, by itself, imply that the type definition captures
an immutable snapshot of the variable's value. Any modifications that remain
permitted under the language's const rules may still affect the value observed
when the definition is evaluated.

The rules governing const qualification, aliases, and permitted modifications
determine which changes are possible after a type definition has been declared.


### Evaluation consistency

The evaluation of a type must resolve its dependencies according to the
language's rules for type evaluation and freezing. Once the type has been
frozen, its constructed representation must remain stable except where the rules
explicitly permit extensions.

Implementations must ensure that dependency resolution and type interning
respect the established identity of each type and do not create conflicting
representations of the same interned type.

The precise evaluation order, handling of cyclic dependencies, and timing of
const-restriction checks are governed by the language's evaluation and
type-checking rules.

