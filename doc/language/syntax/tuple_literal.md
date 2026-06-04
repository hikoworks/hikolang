# tuple-literal

## Syntax

_tuple-literal_ :=\
      `(` `,` `)`\
    __|__ `(` [_expression_] `,` `)`\
    __|__ `(` [_expression_] __(__ `,` [_expression_] __)+__ `,`__?__ `)`

[_expression_]: expression.md

## Semantics

A _tuple-literal_ creates a tuple of values. The values in a tuple may be of different types.

```
fn foo(a : float, b : int) -> (float, int)
{
    return (b, a)
}
```
