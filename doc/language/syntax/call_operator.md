# call-operator

[_argument-chain_]: argument_chain.md
[_expression_]: expression.md
[_identifier_]: identifier.md

## Syntax

_call-operator_ :=
      [_expression_] `(` [_argument-chain_] `)`\
    __|__ [_expression_] `.` [_identifier_] `(` [_argument-chain_] `)`

_index-operator_ :=
      [_expression_] `[` [_argument-chain_] `]`\
    __|__ [_expression_] `.` [_identifier_] `[` [_argument-chain_] `]`



## Semantics

The `()` operator is used to:
 * call a function
 * call a method
 * call a functor:
   - An object with the `__call__()` method
   - An type with the `__call__()` type-method
 * call a lambda
 * instantiate a type-template
 * instantiate a type

The `[]` operator is used to:
 * Index into an array
 * Index a functor
   - An object with the `__index__()` method
   - An type with the `__index__()` type-method
 
If the [_argument-chain_] contain semicolons `;` it is a separator for
chained calls. For example:

```
// 0...10 range of the integer which creates a specific int type.
// Follows by instantiating int(0...10) with a constructor passing in 5.
a = int(0...10; 5)

// Desugars into
a = int(0...10)(5)
```

