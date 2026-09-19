# init-expression

## Syntax

_init_expression_ := [_variable_definition_] `;`

## Semantics

If the [_initializer_] is the result of a function call, then the following
may happen based on the result:
 - **Value**: the variable is initialized with the value. Then the condition expression
   is evaluated.
 - **Empty**: the `empty` clause is executed.
 - **Error**: the matching `catch` clause is executed.
