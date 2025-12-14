# struct-definition

## Syntax

Examples of struct definitions.

```
// struct-definition as an expression
var a = struct {
    var x: int
}

// struct-definition as a statement, syntactic sugar.
struct b {
    var x: int
}

// Merge members to the struct b.
b += struct {
    var y: int
}
```

### A struct-definition expression
An expression which returns a struct type.

`struct` __(__ `[` [_argument-list_](argument_list.md) `]` __)?__ __(__ `(` [_inheritance-list_](inheritance_list.md) `)` __)?__ `{` [_statement-list_](statement_list.md) `}` `;`

### A struct-definition statement
A statement which is syntactic sugar for a struct-definition and variable declaration expression.

`struct` [_identifier_](identifier.md)  __(__ `[` [_argument-list_](argument_list.md) `]` __)?__ __(__ `(` [_inheritance-list_](inheritance_list.md) `)` __)?__ `{` [_statement-list_](statement_list.md) `}` `;`


## Semantics

A struct describes a type with zero or more member variables each with their own type.

### Member functions
Member functions are defined outside of the struct and are resolved using the standard
overload resolution, where the first argument of the function is the reference to `self`.

The exception is virtual functions.

## Value-type

A `lang::struct_type` has the following members:
 - `__add__(a, b) -> c`: Add members from `b` to `a` creating a new struct `c`.
 - `__inplace_add__(a, b)`: Add members from `b` to `a`.
 - `__or__(a, b) -> c`: Merge members from `b` to `a` creating a new struct `c`.
 - `__inplace_or__(a, b)`: Merge members from `b` to `a`.
 - `__get_index__(a, args...)`: Create a new `lang:struct_type` that has filled in template
   arguments.
 - `__call__(a, args...)`: Create an instance and call the constructor with those arguments.
 