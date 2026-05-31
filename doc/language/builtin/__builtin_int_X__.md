# LLVM native integers __i\<N\>

These are the native integers available in LLVM. In LLVM the size of the
integers can be any bit width from 1 to 8,388,608 bits. For performance reasons
an builtin LLVM integer type only exists when it is used in the program.

   Type          | # bits                     | Description
  :------------- |:-------------------------- |:----------------
   `__i<N>`      | N                          | A integer of N bits.
   `__i<N>x<M>`  | N x M                      | A vector of M integers of N bits.
   `__i<N>c`     | `{ v : __i<N>, c : __i1 }` | Result of add/sub with carry.
   `__i<N>w`     | `__i<N>x2`                 | Result of wide multiplication.
   `__f<N>`      | N                          | A float of N bits.
   `__f<N>`      | N                          | A float of N bits.
   `__f<N>_<M>`  | N . M                      | A float of N bits. M bits of mantissa.
   `__f<N>x<M>`  | N x M                      | A vector of M floats of N bits.
   `__f<N>d`     | `__f<N>x2`                 | double precision algorithm
   `__f<N>q`     | `__f<N>x4`                 | quad precision algorithm

## Variable sized scalars

   Type          | Description
  :------------- |:----------------
   `__ptr`       | An opaque pointer.
   `__size`      | Register holding an array size or index.
   `__limb`      | Register holding an limb of a large integer.

A `__ptr` is an opaque value of unknown size. It allows adding an index. But the
actual value or how the adding works is implementation specific and may be
cryptographically-signed for security.

A `__size` is an alias to one of `__i<N>` that can be added to a `__ptr`, and
subtracting two `__ptr` will result in a value of `__size`. It is used unsigned
as a size, and signed as index. It is expected that `__size` is the same
size as the number of bytes of the segment that `__ptr` points into, so modulo
math will work properly as-if it is a signed number. 

A `__limb` is an unsigned integer that can be used as the operands on a wide
multiply CPU instruction, for which the result is lo/hi split into two
registers. This allows implementation of arbitrary sized integers.

If the CPU does not have a wide multiply instruction it will be emulated, and
the `__limb` size is instead based on the add/sub instructions which implements
a carry chain.




