# function-definition

## Syntax

`function` [_name_](name.md) `(` [_argument-list_](argument_list.md) `)`\
[_function-attributes_](function_attributes.md)\
[_type_declaration_](type_declaration.md)\
`{` [_statement-list_](statement_list.md) `}`

## Semantics

### Hidden / Inferred attributes
Since the compiler can see the program as a whole, it can infer a lot of information
about the function, which does not need to manually entered.

#### Thrown errors
The compiler will automatically keep track of each thrown or rethrown error.
On a call to this function each error needs to be caught or rethrown.

#### Injected variables
The compiler will automatically keep track of which [_injected_variables_](injected_variables.md)
are used in the function. These variables have the syntax `$` [_name_](name.md) and are automatically
passed as hidden arguments to the function.

Injected variables of functions that are called are also added to the list of
injected variables. Excluded are explicitly injected variables.
See [_call_](call.md) for more information.


