# Expression

## Primary Expression

   Expression                                        | Description
  :---------------                                   |:-------------------------
   `(` _expression_ `)`                              | Sub expression
   [_integer-literal_]                               | Integer literal
   [_float-literal_] [_simple-unit-expression_]__?__ | Floating point literal
   [_string-literal_]                                | String literal
   [_tuple-literal_]                                 |
   [_if-control-expression_]                         |
   [_for-control-expression_]                        |
   [_while-control-expression_]                      |


## Standard Operators

 * `a`: Left hand side value.
 * `b`: Right hand side value.
 * `T`: [_type-expression_]

   Operator     | Precedence  | Function                       | Description
  :----------   |:----------- |:------------------------------ |:----------
   `a++`        |  2000 L     | `__post_increment__(a)`
   `a--`        |  2000 L     | `__post_decrement__(a)`
   `a(...)`     |  2000 R     | 
   `a[...]`     |  2000 R     | 
   `a.<name>`   |  2000 R     | 
   `++b`        |  3000 R     | `__increment__(a)`
   `--b`        |  3000 R     | `__decrement__(a)`
   `+b`         |  3000 R     | `__pos__(a)`
   `-b`         |  3000 R     | `__neg__(a)`
   `~b`         |  3000 R     | `__inv__(a)`
   `not b`      |  3000 R     | `__not__(a)`
   `await b`    |  3000 R     |
   `try b`      |  3000 R     |                                | [_try_operator_](try_operator.md)
   `a * b`      |  5000 L     | `__mul__(a, b)`
   `a / b`      |  5000 L     | `__div__(a, b)`
   `a % b`      |  5000 L     | `__rem__(a, b)`
   `a &* b`     |  5000 L     | `__mod_mul__(a, b)`
   `a &/ b`     |  5000 L     | `__mod_div__(a, b)`
   `a + b`      |  6000 L     | `__add__(a, b)`
   `a - b`      |  6000 L     | `__sub__(a, b)`
   `a &+ b`     |  6000 L     | `__mod_add__(a, b)`
   `a &- b`     |  6000 L     | `__mod_sub__(a, b)`
   `a << b`     |  7000 L     | `__sal__(a, b)`
   `a >> b`     |  7000 L     | `__sar__(a, b)`
   `a &>> b`    |  7000 L     | `__slr__(a, b)`
   `a <=> b`    |  8000 L     | `__cmp__(a, b)`
   `a < b`      |  9000 L     | `__lt__(a, b)`
   `a > b`      |  9000 L     | `__gt__(a, b)`
   `a <= b`     |  9000 L     | `__le__(a, b)`
   `a >= b`     |  9000 L     | `__ge__(a, b)`
   `a == b`     | 10000 L     | `__eq__(a, b)`
   `a != b`     | 10000 L     | `__ne__(a, b)`
   `a in b`     | 10000 L     | `__in__(a, b)`
   `a not in b` | 10000 L     | `__not_in__(a, b)`
   `a & b`      | 11000 L     | `__bit_and__(a, b)`
   `a ^ b`      | 12000 L     | `__bit_xor__(a, b)`
   `a \| b`     | 13000 L     | `__bit_or__(a, b)`
   `a and b`    | 14000 L     |
   `a or b`     | 15000 L     |
   `a catch b`  | 15100 R     |                            | [_catch_operator_](catch_operator.md)
   `a := T`     | 15500 L     | `__cast_exact__(a, T)`  
   `a : T`      | 15500 L     | `__cast_widen__(a, T)`  
   `a :! T`     | 15500 L     | `__cast_truncate__(a, T)`
   `a :? T`     | 15500 L     | `__cast_narrow__(a, T)`
   `a ~ T`      | 15500 L     |                            | Checks if `a : T` is valid
   `a ~! T`     | 15500 L     |                            | Checks if `a :! T` is valid
   `a ~? T`     | 15500 L     |                            | Checks if `a :? T` is valid
   `a = b`      | 16000 R     | `__assign__(a, b)`
   `a += b`     | 16000 R     | `__inplace_add__(a, b)`
   `a -= b`     | 16000 R     | `__inplace_sub__(a, b)`
   `a *= b`     | 16000 R     | `__inplace_mul__(a, b)`
   `a /= b`     | 16000 R     | `__inplace_div__(a, b)`
   `a %= b`     | 16000 R     | `__inplace_mod__(a, b)`
   `a <<= b`    | 16000 R     | `__inplace_sll__(a, b)`
   `a >>= b`    | 16000 R     | `__inplace_sar__(a, b)`
   `a &>>= b`   | 16000 R     | `__inplace_slr__(a, b)`
   `a &= b`     | 16000 R     | `__inplace_and__(a, b)`
   `a ^= b`     | 16000 R     | `__inplace_xor__(a, b)`
   `a \|= b`    | 16000 R     | `__inplace_or__(a, b)`

## Type expression

_type-expression_ := *type-modifier*__*__ __(__ `[` [_unit-expression_] `]` __)?__ _expression_**?**

_type-modifier_ := `&` __|__ `?` __|__ `const` __|__ `mut`


## Semantics

An _expression_ is a sequence of symbols that can be evaluated to produce a
value. When the result of the expression is not used, the restriction on the
type of the result is relaxed.

For parsing expressions an algorithm like [Shunting
Yard](https://en.wikipedia.org/wiki/Shunting-yard_algorithm) is used.
The other expressions listed in this document are terminal expressions which
are parsed outside of the shunting yard.


[_float-literal_]: float_literal.md
[_integer-literal_]: integer_literal.md
[_string-literal_]: string_literal.md
[_tuple-literal_]: tuple_literal.md
[_type-expression_]: type_expression.md
[_simple-unit-expression_]: unit_expression.md
[_unit-expression_]: unit_expression.md
[_if-control-expression_]: if_control_expression.md
[_for-control-expression_]: for_control_expression.md
[_while-control-expression_]: while_control_expression.md
