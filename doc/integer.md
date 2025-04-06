# Integer

## Base Integers
Base integers are based on CPU registers and memory layout of records. The language
defined the following base integers:
 - `__u1__` - 1-bit unsigned integer (register only)
 - `__s8__` - 8-bit signed integer
 - `__u8__` - 8-bit unsigned integer
 - `__s16__` - 16-bit signed integer
 - `__u16__` - 16-bit unsigned integer
 - `__s32__` - 32-bit signed integer
 - `__u32__` - 32-bit unsigned integer
 - `__s64__` - 64-bit signed integer
 - `__u64__` - 64-bit unsigned integer
 - `__ptr__` - signed integer pointer
 - `__idx__` - signed integer index


### Safe implicit conversions
Integer types are implicitly converted when the following conditions are met:
 - The source and target types are base integers.
 - The target type is larger than the source type.
 - The source type is signed and the target type is signed.
 - The source type is unsigned and the target type is signed.
 - The source type is unsigned and the target type is unsigned.


### Safe explicit conversions
The function: `T:to(x) -> result(T)` converts the value `x` to the type `T`.
This function will return an error-result of `std::overflow_error` if the conversion fails.

The function: `T:truncate(x) -> T` converts the value `x` to the type `T`.
This function will bitwise-truncate the value `x` to the type `T`. If the type of the
result is larger than the argument type, the result will be zero-extended. The signness of
the argument or result type do not matter.


### Simple implicit safe operations
Some simple integer math operators are safe, they will automatically return a type that will
fully represent the result of the operation, so that it is impossible to overflow.

 - `a + b` - addition.
   If one of the operands is signed, the result will be signed. If both operands are unsigned,
   the result will be unsigned. The result type will be next higher size of the largest operand.

 - `a - b` - subtraction.
   The result type is signed. The result type will be next higher size of the largest operand.

 - `a * b` - multiplication.
   If one of the operands is signed, the result will be signed. If both operands are unsigned,
   the result will be unsigned. The result type will be double the size of the largest operand.

 - `a & b` - bitwise and.
   The result will be unsigned. The size of the result will be the size of the smallest operand.

 - `a | b` - bitwise or.
   The result will be unsigned. The size of the result will be the size of the largest operand.

 - `a ^ b` - bitwise or.
   The result will be unsigned. The size of the result will be the size of the largest operand.

 - `a << b` - left shift.
   The result will be the same type as the left operand. The right operand is unsigned. The
   right operand may be larger than the size of the left operand, in this case the result will be
   zero.

 - `a >> b` - right shift.
   The result will be the same type as the left operand. The right operand is unsigned. The
   right operand may be larger than the size of the left operand, in this case the result will be
   zero or -1 depending on the sign of the left operand. If the left operand is signed the shift
   will be arithmetic, if the left operand is unsigned the shift will be logical.

 - `a += b`, `a -= b`, `a *= b`, `a &= b`, `a |= b`, `a ^= b`, `a <<= b`, `a >>= b` - inplace operators.
   The result type is the same as the left operand. Overflow is not checked, and the result value is
   modulo the size of the left operand. For the `|=` and `^=` operators the right operand must be
   equal or smaller than the left operand. For the `<<=` and `>>=` operators the right operand must be
   unsigned.

 - `a == b`, `a != b`, `a < b`, `a <= b`, `a > b`, `a >= b`, `a <=> b` - comparison operators.
   The comparison is done correctly, as if both operands are converted to signed integers large
   enough to hold the operands without overflow.


## Large integers
There are two types of large integers:
 - `signed(nr_bits: __u16__)` - signed integer with `nr_bits` bits.
 - `unsigned(nr_bits: __u16__)` - unsigned integer with `nr_bits` bits.

The literal syntax are:
 - `__integer_literal__ "U"` - unsigned integer that is converted to `unsigned(nr_bits)`, with the correct
   number of bits.
 - `__integer_literal__ "S"` - signed integer that is converted to `signed(nr_bits)`, with the correct
   number of bits.

## Integer interval

 - `integer_interval(lo: signed, hi: signed)` - integer interval with a low and high value.

Literal syntax for an integer interval is `<lo>...<hi>` where `<lo>` and `<hi>` are
`__integer_literal__`, it is automatically converted to `integer_interval` type.

## Ranged integers
The ranged integer are the default integer type in the language.

The ranged integer has the following features:
 - An integer with with a range, values outside the range are not allowed.
 - The possible range of values is not limited by the size of the cpu registers.
   Up to 65536 bits of range is supported.
 - Compile time range checking.
 - Runtime range checking only if the range can't be checked at compile time.
 - The size of the integer is based on the range:
    - If the range is between 0 and 1, the size is 1 bit.
    - If the range is between -128 and 127, the size is 8 bits.
    - If the range is between -32768 and 32767, the size is 16 bits.
    - If the range is between -2147483648 and 2147483647, the size is 32 bits.
    - If the range is between -9223372036854775808 and 9223372036854775807, the size is 64 bits.
    - Ranges beyond this use multiple 64-bit integers.

 - `integer(range: integer_interval)` - integer with a range.
