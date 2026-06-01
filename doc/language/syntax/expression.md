# Expression

## Primary Expression

   Expression                                        | Description
  :---------------                                   |:-------------------------
   `(` _expression_ `)`                              | Sub expression
   [_integer_literal_]                               | Integer literal
   [_float_literal_] [_simple_unit_expression_]__?__ | Floating point literal
   [_string_literal_]                                | String literal
   [_tuple_literal_]                                 |
   [_if_control_expression_]                         |
   [_for_control_expression_]                        |
   [_while_control_expression_]                      |


[_float_literal_]: float_literal.md
[_integer_literal_]: integer_literal.md
[_string_literal_]: string_literal.md
[_tuple_literal_]: tuple_literal.md
[_simple_unit_expression_]: unit_expression.md

## Standard Operators

   Operator     | Precedence  | Function                       | Description
  :----------   |:----------- |:------------------------------ |:----------
   `a++`        |  2000 L     | `__post_increment__(a)`
   `a--`        |  2000 L     | `__post_decrement__(a)`
   `a(...)`     |  2000 R     | 
   `a[...]`     |  2000 R     | 
   `a.<name>`   |  2000 R     | 
   `++a`        |  3000 R     | `__increment__(a)`
   `--a`        |  3000 R     | `__decrement__(a)`
   `+a`         |  3000 R     | `__pos__(a)`
   `-a`         |  3000 R     | `__neg__(a)`
   `~a`         |  3000 R     | `__inv__(a)`
   `not a`      |  3000 R     | `__not__(a)`
   `await a`    |  3000 R     |
   `try a`      |  3000 R     |                                | [_try_operator_](try_operator.md)
   `&T`         |  3000 R     | `__make_reference__(T)`
   `mut T`      |  3000 R     | `__has_mut__(T)`
   `const T`    |  3000 R     | `__remove_mut__(T)`
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


## Primary expressions

 * Identifier
 * Integer literal: `42`
 * Floating point literal: `42.0`
 * String literal `"hello world"`
 * literal dimension suffix: `42.0 u`
 * literal dimension expression: `42.0 (u)`
 * Compound Expression: `(a)`




## Semantics

An _expression_ is a sequence of symbols that can be evaluated to produce a
value. When the result of the expression is not used, the restriction on the
type of the result is relaxed.

For parsing expressions an algorithm like [Shunting
Yard](https://en.wikipedia.org/wiki/Shunting-yard_algorithm) is used.
The other expressions listed in this document are terminal expressions which
are parsed outside of the shunting yard.

