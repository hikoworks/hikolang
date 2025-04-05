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

