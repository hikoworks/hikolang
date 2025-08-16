# lambda

## Syntax

_long-form-lambda_ __|__ _short-form-lambda_

### long-form-lambda
`{` [_argument-declarations_](argument_declarations.md)__?__ [_result-type-declaration_](result_type_declaration.md)__?__\
[_specifiers_](specifiers.md)__*__
[_attribute_](attribute.md)__*__
`in` [_statement-list_](statement_list.md) `}`

### short-form-lambda
`{` [_statement-list_](statement_list.md) `}`


## Semantics
A lambda expression is an instance of an anonymous type. The anonymous type
itself is an instance of `lambda_metatype`.

The anonymous type implements an overloaded `__call__(self, ...)`, and it
has member variables for each captured variables. When the lambda-expression is
evaluated the anonymous type is instantiated with copies/refs of the captured
variables. 

It is implemented as a anonymous class with a single `__call__()`
member function. The captured variables are stored as members of the
instance, with the same name as the variable in the lambda. During creation of
the instance, the captured variables are copied into the instance.

### Capture
Any variable that is used inside the lambda is automatically captured by copy.

### Result
The result of the lambda is the value returned by the `return` statement. If
there is no `return` statement, the result of the lambda is the value of the
last executed expression in the [_statement-list_](statement_list.md). Or `void`
if there was no such expression.

It is a **static error** if in multiple branches the last executed expression
are not of the same type.

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


## Example

```
var a = {(x, y :*int) -> int[5..=6] in x + y }
var b = {(x, y :*int) in x + y }
var c = {-> int[5..=6] in $0 + $1 }
var d = { $0 + $1 }
```

Larger example:

```
var foo += {(x: float, y: float) -> float
    pre(x >= 0.0)
    pre($2 > 0.0)
    post($0 > 0.0)
    [[noinline]]
in
    x += y
    x *= y
    return x
}

// Syntactic Sugar identical to the previous declaration.
fn foo(x: int, y: float) -> string
    pre(x >= 0.0)
    pre($2 > 0.0)
    post($0 > 0.0)
    [[noinline]]
{
    x += y
    x *= y
    return x
}
```

Lambda passed in call:

