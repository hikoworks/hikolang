# function-definition

## Syntax

`function` [_name_](name.md) `(` [_argument-list_](argument_list.md) `)`\
*function-attribute*__*__ \
[_result-type-declaration_](result_type_declaration.md)__?__\
`{` [_statement-list_](statement_list.md) `}`

### function-attribute
`pre` `(` [_expression_list_](expression_list.md) `)` __|__\
`post` `(` [_expression_list_](expression_list.md) `)` __|__\

## Semantics

### Hidden / Inferred attributes
Since the compiler can see the program as a whole, it can infer a lot of
information about the function, which does not need to manually entered.

#### Thrown errors
The compiler will automatically keep track of each thrown or rethrown error.
On a call to this function each error needs to be caught or rethrown.

#### Injected variables
The compiler will automatically keep track of which
[_injected_variable_](injected_variable.md) are used in the function. These
variables have the syntax `$` [_name_](name.md) and are automatically passed as
hidden arguments to the function.

Injected variables of functions that are called are also added to the list of
injected variables. Excluded are explicitly injected variables.
See [_call_](call.md) for more information.


