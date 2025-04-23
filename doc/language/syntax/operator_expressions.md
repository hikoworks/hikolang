# operator-expression

## Syntax

[_expression_](expression.md) _operator_ [_expression_](expression.md) __|__
[_expression_](expression.md) _operator_ __|__
_operator_ [_expression_](expression.md)

### operator

Here is a table of the standard operators:

 | Precedence | Operator  | Associativity | Function                           |
 | ----------:|:--------- |:------------- |:---------------------------------- |
 |          2 | `a++`     | left-to-right | `__post_increment__(a)`            |
 |          2 | `a--`     | left-to-right | `__post_decrement__(a)`            |
 |          2 | `a(...)`  | left-to-right | `__call__(a, ...)`                 |
 |          2 | `a[...]`  | left-to-right | `__index__(a, ...)`                |
 |          2 | `a.b`     | left-to-right | [_member_access_](member_acces.md) |
 |          3 | `a++`     | right-to-left | `__pre_increment__(a)`             |
 |          3 | `a--`     | right-to-left | `__pre_decrement__(a)`             |
 |          3 | `+a`      | right-to-left | `__plus__(a)`                      |
 |          3 | `-a`      | right-to-left | `__minus__(a)`                     |
 |          3 | `~a`      | right-to-left | `__bitwise_not__(a)`               |
 |          3 | `not a`   | right-to-left | [_not_](not.md)                    |    
 |          3 | `await a` | right-to-left | [_await_](await.md)                |    
 |          5 | `a * b`   | left-to-right | `__mul__(a, b)`                    |
 |          5 | `a / b`   | left-to-right | `__div__(a, b)`                    |
 |          5 | `a % b`   | left-to-right | `__mod__(a, b)`                    |
 |          6 | `a + b`   | left-to-right | `__add__(a, b)`                    |
 |          6 | `a - b`   | left-to-right | `__sub__(a, b)`                    |
 |          7 | `a << b`  | left-to-right | `__left_shift__(a, b)`             |
 |          7 | `a >> b`  | left-to-right | `__right_shift__(a, b)`            |
 |          8 | `a <=> b` | left-to-right | `__cmp__(a, b)`                    |
 |          9 | `a < b`   | left-to-right | `__lt__(a, b)`                     |
 |          9 | `a <= b`  | left-to-right | `__le__(a, b)`                     |
 |          9 | `a >= b`  | left-to-right | `__ge__(a, b)`                     |
 |          9 | `a > b`   | left-to-right | `__gt__(a, b)`                     |
 |         10 | `a == b`  | left-to-right | `__eq__(a, b)`                     |
 |         10 | `a != b`  | left-to-right | `__ne__(a, b)`                     |
 |         11 | `a & b`   | left-to-right | `__bitwise_and__(a, b)`            |
 |         12 | `a ^ b`   | left-to-right | `__bitwise_xor__(a, b)`            |
 |         13 | `a \| b`  | left-to-right | `__bitwise_or__(a, b)`             |
 |         14 | `a and b` | left-to-right | [_and_](and.md)                    |
 |         15 | `a or b`  | left-to-right | [_or_](or.md)                      |
 |         16 | `a = b`   | right-to-left | `__assign__(a, b)`                 |
 |         16 | `a += b`  | right-to-left | `__inplace_add__(a, b)`            |
 |         16 | `a -= b`  | right-to-left | `__inplace_sub__(a, b)`            |
 |         16 | `a *= b`  | right-to-left | `__inplace_mul__(a, b)`            |
 |         16 | `a /= b`  | right-to-left | `__inplace_div__(a, b)`            |
 |         16 | `a %= b`  | right-to-left | `__inplace_mod__(a, b)`            |
 |         16 | `a <<= b` | right-to-left | `__inplace_left_shift__(a, b)`     |
 |         16 | `a >>= b` | right-to-left | `__inplace_right_shift__(a, b)`    |
 |         16 | `a &= b`  | right-to-left | `__inplace_bitwise_and__(a, b)`    |
 |         16 | `a ^= b`  | right-to-left | `__inplace_bitwise_xor__(a, b)`    |
 |         16 | `a \|= b`  | right-to-left | `__inplace_bitwise_or__(a, b)`    |


     
