# operator-expression

## Syntax

[_expression_](expression.md) _operator_ [_expression_](expression.md) __|__
[_expression_](expression.md) _operator_ __|__
_operator_ [_expression_](expression.md)

### operator

Here is a table of the standard operators:

 | Precedence | Operator  | Associativity | Function                           |
 | ----------:|:--------- |:------------- |:---------------------------------- |
 |         20 | `a++`     | left-to-right | `__post_increment__(a)`            |
 |         20 | `a--`     | left-to-right | `__post_decrement__(a)`            |
 |         20 | `a(...)`  | left-to-right | [_call_](call.md)                  |
 |         20 | `a[...]`  | left-to-right | `__index__(a, ...)`                |
 |         20 | `a.b`     | left-to-right | [_member_access_](member_acces.md) |
 |         30 | `++a`     | right-to-left | `__increment__(a)`                 |
 |         30 | `--a`     | right-to-left | `__decrement__(a)`                 |
 |         30 | `+a`      | right-to-left | `__plus__(a)`                      |
 |         30 | `-a`      | right-to-left | `__neg__(a)`                       |
 |         30 | `~a`      | right-to-left | `__bitwise_not__(a)`               |
 |         30 | `not a`   | right-to-left | [_not_](not.md)                    |    
 |         30 | `await a` | right-to-left | [_await_](await.md)                |
 |         40 | `a : b`   | left-to-right | [_coerce_](coerce.md)              |
 |         40 | `a :^ b`  | left-to-right | [_coerce_](coerce.md)              |
 |         40 | `a :! b`  | left-to-right | [_coerce_](coerce.md)              |
 |         40 | `a := b`  | left-to-right | [_coerce_](coerce.md)              |
 |         49 | `a ** b`  | left-to-right | `__pow__(a, b)`                    |   
 |         50 | `a * b`   | left-to-right | `__mul__(a, b)`                    |
 |         50 | `a / b`   | left-to-right | `__div__(a, b)`                    |
 |         50 | `a % b`   | left-to-right | `__mod__(a, b)`                    |
 |         60 | `a + b`   | left-to-right | `__add__(a, b)`                    |
 |         60 | `a - b`   | left-to-right | `__sub__(a, b)`                    |
 |         70 | `a << b`  | left-to-right | `__left_shift__(a, b)`             |
 |         70 | `a >> b`  | left-to-right | `__right_shift__(a, b)`            |
 |         75 | `a ..< b` | left-to-right | `__range__(a, b)`                  |
 |         75 | `a ..= b` | left-to-right | `__closed_range__(a, b)`           |
 |         80 | `a <=> b` | left-to-right | `__cmp__(a, b)`                    |
 |         90 | `a < b`   | left-to-right | `__lt__(a, b)`                     |
 |         90 | `a <= b`  | left-to-right | `__le__(a, b)`                     |
 |         90 | `a >= b`  | left-to-right | `__ge__(a, b)`                     |
 |         90 | `a > b`   | left-to-right | `__gt__(a, b)`                     |
 |        100 | `a == b`  | left-to-right | `__eq__(a, b)`                     |
 |        100 | `a != b`  | left-to-right | `__ne__(a, b)`                     |
 |        110 | `a & b`   | left-to-right | `__bitwise_and__(a, b)`            |
 |        120 | `a ^ b`   | left-to-right | `__bitwise_xor__(a, b)`            |
 |        130 | `a \| b`  | left-to-right | `__bitwise_or__(a, b)`             |
 |        140 | `a and b` | left-to-right | [_and_](and.md)                    |
 |        150 | `a or b`  | left-to-right | [_or_](or.md)                      |
 |        160 | `a = b`   | right-to-left | `__assign__(a, b)`                 |
 |        160 | `a += b`  | right-to-left | `__inplace_add__(a, b)`            |
 |        160 | `a -= b`  | right-to-left | `__inplace_sub__(a, b)`            |
 |        160 | `a *= b`  | right-to-left | `__inplace_mul__(a, b)`            |
 |        160 | `a /= b`  | right-to-left | `__inplace_div__(a, b)`            |
 |        160 | `a %= b`  | right-to-left | `__inplace_mod__(a, b)`            |
 |        160 | `a <<= b` | right-to-left | `__inplace_left_shift__(a, b)`     |
 |        160 | `a >>= b` | right-to-left | `__inplace_right_shift__(a, b)`    |
 |        160 | `a &= b`  | right-to-left | `__inplace_bitwise_and__(a, b)`    |
 |        160 | `a ^= b`  | right-to-left | `__inplace_bitwise_xor__(a, b)`    |
 |        160 | `a \|= b`  | right-to-left | `__inplace_bitwise_or__(a, b)`    |


Beyond these standard set of operators, the language allows for user-defined
operators. These are defined using the [_operator_](operator.md) statement.

