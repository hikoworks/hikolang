# argument-declaration-list

## Syntax

`(` `)` __|__

`(` [_return-declaration_](return_declaration.md) `,`__?__ `)` __|__

`(` [_argument-declaration_](argument_declaration.md)
__(__ `,` [_argument-declaration_](argument_declaration.md) __)*__ 
__(__ `,` [_result-declaration_](result_declaration.md) __)?__ `,`__?__
`)`

## Semantics
A list of argument declarations, used for: function, type and lambda declarations.

Inside a function, type or lambdas, the argument declarations are available as
variables as:
 - by their name,
 - positional arguments: `$0`, `$1`, etc,
 - positional arguments: `$[` integer `]`,
 - the number of arguments: `$#`,
 - result value as: `$_`

### Result
Functions and lambdas may include a
[_result-declaration_](result_declaration.md) in the argument declaration.

 - _none_ : see `return`.
 - `return` : This is a function with an inferred return-type. All
   return-statements in the body of the function are required to return the
   exact same type.
 - `return` [_type_expression_](type_expression.md) : This is a function with
   a given result type. Values of other types that are returned in the
   function's body may be converted.
 - `yield` : This is a generator-function with an inferred yield-type. All
   yield-statements in the body of the generator-function are required to return
   the exact same type.
 - `yield` [_type_expression_](type_expression.md) : This is a
   generator-function with a given result type. Values of other types that are
   yielded from the function's body may be converted.
 - `await` : This is a task with an inferred await-type. All await-statements
   in the body of the task are required to return the exact same type.
 - `await` [_type_expression_](type_expression.md) : This is a task with a
   given result type. Values of other type that are awaited in the task's body
   may be converted.

The variable `$_` points to the result value, but since the result value has no
meaning inside the function you may only use `$_` to query its type, if that
type is known at the time of querying it.
