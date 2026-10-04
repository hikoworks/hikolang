# binding-operator

[_expression_]: expression.md

## Syntax

_binding-operator_ :=\
      `&` __(__ `move` __|__ `const` __)?__ [_expression_]\
    __|__ `&&` [_expression_]\
    __|__ `*` [_expression_]


## Semantics

 * `&` - Borrow a reference, preserving the optional const-qualifier.
 * `&const` - Borrow a const-qualified reference.
 * `&move` - Explicitly borrow a move-qualified reference.
 * `&&` - Explicitly forward a reference preserving the qualifier.
 * `*` - Dereference creating a temporary value.


