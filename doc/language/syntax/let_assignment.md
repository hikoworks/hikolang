# let-assignment

## Syntax

_let-assignment_ := [_left-expression_] `=` [_expression_]

[_left-expression_]: left_expression.md
[_expression_]: expression.md
[_identifier_]: identifier.md

## Semantics

```
a = 42   // A definition for a.
// a = 55;  Redefinition is not allowed in the same block
{
    a = a + 3 // Redefinition inside block shadows outside definition.
}

b := 4  // A variable
c = mut &b  // A mutable reference to b, c itself is constant.
c := 3  // Modifies b.
```

### Dummy
If the [_left-expression_] may contain `_` as a [_identifier_]. `_` is a dummy
variable or definition. It acts like a normal variable or definition, with the
exception that the `_` may be redefined (both value and type) in the same block.

When `_` is redefined, it is not possible to use it in an expression.
