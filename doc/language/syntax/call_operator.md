# call-operator

[_argument-list_]: argument_list.md
[_code-block_]: code_block.md
[_expression_]: expression.md
[_id_]: id.md

## Syntax

_call-operator_ :=\
      [_expression_] `(` [_argument-list_] `)` [_code-block_]__?__\
    __|__ [_expression_] `.` [_id_] `(` [_argument-list_] `)` [_code-block_]__?__

_index-operator_ :=\
      [_expression_] `[` [_argument-list_] `]` [_code-block_]__?__\
    __|__ [_expression_] `.` [_id_] `[` [_argument-list_] `]` [_code-block_]__?__



## Semantics

The `()` operator is used to:
 * Call a function.
 * Call a method.
 * Call a functor:
   - An object with the `__call__()` method
   - An type with the `@type` `__call__()` type-method
 * Call a lambda.
 * Instantiate a type.

The `[]` operator is used to:
 * Index into an array.
 * Index a functor:
   - An object with the `__index__()` method
   - An type with the `@type` `__index__()` type-method
 * Instantiate a type-template.

The optional [_code-block_] after the argument list is syntactic sugar for
passing the code-block as a simplified lambda with variable number of arguments
as the last argument in the call.

```
a = foo(1, 2, 3) {
  $0 + $1
}

// Desugared as:
a = foo(1, 2, 3, fn { $0 + $1 }) 
```