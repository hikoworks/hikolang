# enum-definition

## Syntax

### A enum-definition expression
An expression which returns a enum type.

`enum` __(__ `[` [_argument_list_](argument_list.md) `]` __)?__ __(__ `(` [_enum_name_list_](enum_name_list.md) `)` __)?__ `{` [_statement-list_](statement_list.md) `}`

### A enum-definition statement
A statement which is syntactic sugar for a enum-definition and variable declaration expression.

`enum` [_identifier_](identifier.md)  __(__ `[` [_argument_list_](argument_list.md) `]` __)?__ __(__ `(` [_enum_name_list_](enum_name_list.md) `)` __)?__ `{` [_statement-list_](statement_list.md) `}`



```
// struct-definition as an expression
let a = enum (
    x 
)

// struct-definition as a statement, syntactic sugar.
enum b (
    x
)
```

## Semantics

### enum-type-object

An enum type is an instance of the `__enum_meta_type__` type.
