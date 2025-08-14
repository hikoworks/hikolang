# Lambda

## Syntax

`{` [_attribute_](attribute.md)__*__ `(` [_argument-list_](argument_list.md)__?__ `)`\
[_result-type-declaration_](result_type_declaration.md)__?__\
`in` [_statement-list_](statement_list.md) `}` __|__

`{` [_attribute_](attribute.md)__*__ [_argument-list_](argument_list.md)\
`in` [_statement-list_](statement_list.md) `}` __|__

`{` [_attribute_](attribute.md)__*__ [_statement-list_](statement_list.md) `}`


## Semantics
A lambda expression's result is a instance of functor.

It is implemented as a anonymous class with a single templated `__call__()`
member function. The captured variables are stored as private members of the
class, with the same name as the variable in the lambda. During creation of the
instance, the captured variables are copied into the instance.

### Capture
Any variable that is used inside the lambda is automatically captured by copy.

### Result
The result of the lambda is the value returned by the `return` statement. If
there is no `return` statement, the result of the lambda is the value of the
last executed expression in the [_statement-list_](statement_list.md). Or `void`
if there was no such expression.

It is a **static error** if in multiple branches the last executed expression
are not of the same type.


## Example

```
var a = {(x, y :*int) :^int[5..=6] : x + y }
var b = {(x, y :*int) : x + y }
var c = {:^int[5..=6] : $0 + $1 }
var d = { $0 + $1 }
```


