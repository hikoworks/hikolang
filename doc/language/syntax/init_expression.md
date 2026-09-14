# init-expression

## Syntax

_init_expression_ := [_variable_definition_] `;`

## Semantics

An init expression may me a function call, and this may result in
the following:
 - **Value**: the variable is initialized with the value. Then the condition expression
   is evaluated.
 - **Empty**: the `empty` clause is executed.
 - **Error**: the matching `catch` clause is executed.
