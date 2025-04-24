# Lambda

## Syntax

`{` __(__ [_argument-list_](argument_list.md) __?__ __(__ `->` [_type_declaration_](type_declaration.md) __)?__ `:` __)?__ [_statement-list_](statement_list.md) `}`

A _type_expression_ is a [_expression_](expression.md) which is evaluted to a type,
type-template, interface or interface-template at compile time.

```
var f = {x, y :*int -> :^int[5..=6] : x + y }
let my_int = f(1, 2)

class lambda_uih4rih3495ry3294 {
    let __call__(x, y :*int[5..=6]) -> ^[int] {
        return x + y
    }
}

template(I :=interval) class int {
    static iv = I
    var value :=I.value_type

    let __add__(let a :=int[lo,hi], let b :=int) {
        int[a.lo + b.lo, a.hi + b.hi](a.value + b.value)
    }
}
```

## Semantics
A lambda expression's result is a instance of functor.

It is implemented as a anonymous class with a single templated `__call__()` member function.
The captured variables are stored as private members of the class, with the same name
as the variable in the lambda. During creation of the instance, the captured
variables are copied into the instance.

### Capture
Any variable that is used inside the lambda is automatically captured by copy.

### Result
The result of the lambda is the value returned by the `return` statement. If
there is no `return` statement, the result of the lambda is the value of the
last executed expression in the [_statement-list_](statement_list.md). Or `void` if
there was no such expression.

It is a **static error** if in multiple branches the last executed expression
are not of the same type.




