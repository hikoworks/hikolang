# struct-definition

## Syntax

### A struct-definition expression
An expression which returns a struct type.

`struct` __(__ `[` [_argument_list_](argument_list.md) `]` __)?__ __(__ `(` [_inheritance_list_](inheritance_list.md) `)` __)?__ `{` [_statement-list_](statement_list.md) `}`

### A struct-definition statement
A statement which is syntactic sugar for a struct-definition and variable declaration expression.

`struct` [_identifier_](identifier.md)  __(__ `[` [_argument_list_](argument_list.md) `]` __)?__ __(__ `(` [_inheritance_list_](inheritance_list.md) `)` __)?__ `{` [_statement-list_](statement_list.md) `}`

```
// struct-definition as an expression
let a = struct {
    var x: int
 }

// struct-definition as a statement, syntactic sugar.
struct b {
    var x: int
}
```

## Semantics


