# variable-definition

[_attribute_]: attribute.md
[_expression_]: expression.md
[_body_]: body.md
[_file_]: file.md
[_fqname_]: fqname.md
[_identifier_]: identifier.md
[_init-expression_]: init_expression.md
[_code-block_]: code-block.md
[_condition-expression_]: condition_expression.md
[_type-specifier_]: type_specifier.md
[_type-expression_]: type_expression.md
[_type-definition_]: type_definition.md

## Syntax

_variable-definition_ :=\
      [_fqname_] [_type-specifier_]__?__ `=` _initializer_\
    __|__ [_fqname_] [_type-specifier_]__?__ `<-` _initializer_\
    __|__ [_fqname_] [_type-specifier_]__?__ `=` `global` `(` [_argument_declaration_list_](argument_declaration_list.md) `)` _initializer_\
    __|__ [_fqname_] [_type-specifier_]__?__ `<-` `global` `(` [_argument_declaration_list_](argument_declaration_list.md) `)` _initializer_\
    __|__ [_fqname_] [_type-specifier_]__?__ `=` [_type-expression_] __(__ `,` _index-expression_ __)?__\
    __|__ [_fqname_] [_type-specifier_]__?__ `<-` [_type-expression_] __(__ `,` _index-expression_ __)?__


_initializer_ :=\
      [_expression_]\
    __|__ `unintialized`\
    __|__ `extern`\
    __|__ `default`

_index-expression_ := [_expression_]

## Semantics

Declares a variable with a [_fqname_] which can be used in the scope or child
scopes where the variable is declared.

Variable declarations may appear in the following contexts:
 - In the [_body_] of a [_file_]; which makes it a global variable.
 - In a [_code-block_].
 - In a [_init-expression_] of a control-expression; the variable is then
   declared in the scope of all following [_condition-expression_]s and
   [_code-block_]s of that control-expression.
 - In a [_type-definition_]'s [_code-block_] where it becomes part of the
   definition, resulting in: a member-variable in a `struct`-like type or
   member-name in an `enum`-like type, or something else.

If the type is not explicitly set, the exact type of the first assigned value is
used as the type of the variable.


Examples:

```
a : T <- default            // Variable default value of specific type
a <- b                      // Variable type is inferred
a :~+ T <- b                // Value is converted to type T.
a :~+ T <- b :~+ U          // ((b :~+ U) :~+ T)
thread_local a : T <- b     // A thread_local variable
shared a : T <- b           // A variable on the heap managed by ref-counting
static(.rwdata) a : T <- b  // static, in an explicit data segment.
```


### initializer

 Initializer          | Description
 :------------------- | :------------
 `=` [_expression_]   | Create an immutable variable.
 `<-` [_expression_]  | Create a variable.
 `<-` `extern`        | Link to an external variable of the same name.
 `<-` `default`       | Initialize the variable to the default value.
 `<-` `uninitialized` | The variable is not initialized, you must initialize it later before reading from it.


### assignment operator

If a second variable declaration of the same name exists, then this becomes a
assignment operator. The assignment operator depending on the initializer symbol
will call different methods on the value.

 Initializer          | Method         | Description
 :------------------- | :------------- | :----------
 `<-` [_expression_]  | `__assign__()` | Assigns to a variable
 `=` [_expression_]   | `__merge__()`  | Merge function/type definitions

Special types, like functions, have overload sets that may be merged.

> [!caution]
> Technically any type can implement the `__merge__()` method,
> but it should be extremely rare. As we want `=` to mean immutable.


### enum member

A variable definition is also used for declaring enum members by the `enum`
metatype. The `,` _index-expression_ syntax allows both a type for the
associated data and an index to the enum name to be supplied: They looks like:

```
my_enum = enum {
    foo                     // a single literal (not a variable definition)
    bar <- 3                // An `int` is used for the enum index
    baz <- (:f64, :string)  // A tuple for associated values
    qux <- string, 6        // A string as associated value and index.
}
```

## annotations

A variable definition consumes the pending-annotations.


### @alignas(integer-expression)

Make the allocation for this objects aligned to the given expression.


### [[deprecated(message)]]

This variable/function is deprecated. The compiler will emit a warning message
where the variable is used, including the `message` passed in the attribute.


### @export(abi)

The function will be available in the executable using the `abi`
specified.

  abi      | Description
 :-------- |:---------------
  `"c"`    | Use the C ABI.
  `"c++"`  | Use the C++ ABI.


### @metatype

This function's [_fqname_] is added to type-definition keyword list.

This function is called when a type-definition is found. The arguments
to this functions are:

 * The list of type attributes
 * The template argument list
 * The type-inheritance list
 * The [_code-block_]

It returns a function that in-turn returns an actual type, which is
added to the overload-set as a type template.


### @public

This variable can be named outside of the module.


### @section(link-name)

In case of a global variable place it in a specific section of the executable.


### @shared(allocator)

Allocate the object on the heap, and make it sharable to other shared variables
using reference counting.


### @static

This function is a class memember function, instead of instance member function.


### @thread_local

Allocate the variable in the thread-local region.


### @unique(allocator)

Allocate the object on the heap, only one variable can own this object, but the
ownership can be moved to another variable


### @weak

A variable that references a shared variable. This variable becomes empty when
the shared variable's reference count becomes zero.
