# tuple-literal

## Syntax

_tuple_literal_ :=\
      `(` `,` `)`\
    __|__ `(` [_expression_] `,` `)`\
    __|__ `(` [_expression_] __(__ `,` [_expression_] __)+__ `,`__?__ `)`

[_expression_]: expression.md

## Semantics

A tuple literal creates a tuple of values. The values in a tuple may be of different types.

A tuple may also be used to declare the types of a tuple for type coercion and return type.

```
fn foo(a : float, b : int) -> (float, int)
{
    return (b, a)
}
```
