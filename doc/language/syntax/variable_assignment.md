# variable-assignment

## Syntax

_let-assignment_ := [_left-expression_] `:=` [_expression_]

[_left-expression_]: left_expression.md
[_expression_]: expression.md

## Semantics

```
a := 42  // A variable
a := 3   // Change the value of variable a.
b := &a  // A reference to a
b := 5   // Change the value of variable a.
c := b   // Make a copy of b
d := &b  // A reference to a.
e := &b : &T // A reference to a, casted to a base-class.
```

