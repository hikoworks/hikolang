# variable-definition

## Syntax

_variable-definition_ :=\
    *qualifier*__*__\
    [_fqname_]\
    __(__ `[` [_argument_declaration_list_](argument_declaration_list.md) `]` __)?__
    [_type-specifier_]__?__ _initializer_


_qualifier_ :=\
      `thread_local`\
    __|__ `static`\
    __|__ `shared` __(__ `(` [_expression_](expression.md) `)` __)?__\
    __|__ `weak`\
    __|__ `unique` __(__ `(` [_expression_](expression.md) `)` __)?__\
    __|__ `alloc` __(__ `(` [_expression_](expression.md) `)` __)?__\
    __|__ `public`\
    __|__ `section` `(` [_expression_](expression.md) `)`\
    __|__ `alignas` `(`[_expression_](expression.md) `)`

_initializer_ :=\
      `=` [_expression_]\
    __|__ `:=` [_expression_]\
    __|__ `:=` `unintialized`\
    __|__ `:=` `extern`
    __|__ `:=` `default`


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


## Semantics

Declares a variable with a [_fqname_] which can be used in the scope or child
scopes where the variable is declared.

Variable declarations may appear in the following contexts:
 - In the [_body_] of a [_file_]; which makes it a global variable.
 - In a [_code-block_].
 - In a [_init-expression_] of a control-expression; the variable is then
   declared in the scope of all following [_condition-expression_]s and
   [_code-block_]s of that control-expression.

If the type is not explicitly set, the exact type of the first assigned value is
used as the type of the variable.


Examples:

```
a : T := default            // Variable default value of specific type
a := b                      // Variable type is inferred
a :~+ T := b                // Value is converted to type T.
a :~+ T := b :~+ U          // ((b :~+ U) :~+ T)
thread_local a : T := b     // A thread_local variable
shared a : T := b           // A variable on the heap managed by ref-counting
static(.rwdata) a : T := b  // static, in an explicit data segment.
```

### Qualifier

#### shared / weak / unique / alloc

These variables manage values on the heap; they are managed as follows:
 * `shared`: Reference counted allocation. Assigning a `shared var` to another
   will assign the underlying address of the reference.
 * `weak`: A weak copy of a `shared` reference, becomes _empty_ when
   ref-count is zero. And creates a `shared` when assigned.
 * `unique`: Only one reference can point to the managed allocation. The
   reference may be moved or swapped.
 * `alloc`: Memory is allocated, but must be manually deallocated.

The optional argument is the allocator to use. Otherwise it is the current
or default allocator.



### initializer

 Initializer          | Description
 :------------------- | :------------
 `=` [_expression_]   | Create an immutable variable.
 `:=` [_expression_]  | Create a variable.
 `:=` `extern`        | Link to an external variable of the same name.
 `:=` `default`       | Initialize the variable to the default value.
 `:=` `uninitialized` | The variable is not initialized, you must initialize it later before reading from it.


### assignment operator

If a second variable declaration of the same name exists, then this becomes a
assignment operator. The assignment operator depending on the initializer symbol
will call different methods on the value.

 Initializer          | Method         | Description
 :------------------- | :------------- | :----------
 `:=` [_expression_]  | `__assign__()` | Assigns to a variable
 `=` [_expression_]   | `__merge__()`  | Merge function/type definitions

Special types, like functions, have overload sets that may be merged.

> [!caution]
> Technically any type can implement the `__merge__()` method,
> but it should be extremely rare. As we want `=` to mean immutable.
