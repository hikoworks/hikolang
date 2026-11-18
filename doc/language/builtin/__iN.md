# LLVM native types

These are the native integer and floats available in LLVM. In LLVM the size of the
integers can be any bit width from 1 to 8,388,608 bits. For performance reasons
a builtin LLVM integer type only exists when it is used in the program.

   Type          | # bits                     | Description
  :------------- |:-------------------------- |:----------------
   `__ptr`       |                            | An opaque pointer.
   `__i<N>`      | N                          | A integer of N bits.
   `__ip`        | N                          | A integer with the size of a pointer.
   `__is`        | N                          | A integer with the size of a size / offset.
   `__ia`        | N                          | A arbitrary sized integer limb.
   `__i<N>x<M>`  | N x M                      | A vector of M integers of N bits.
   `__f<N>`      | N                          | A float of N bits.
   `__fd<N>`     | N                          | A double-double of N bits.
   `__fq<N>`     | N                          | A quad-double of N bits.
   `__f<N>x<M>`  | N x M                      | A vector of M floats of N bits.


## Opaque pointer

A `__ptr` is an opaque value of unknown size. It allows adding an index. But the
actual value or how the adding works is implementation specific and may be
cryptographically-signed for security.


## Integers

Integers are stored as 2's compliment in the endian order of the target machine.
If the target machine has a different endian order from the source machine, then
the source machine may emulate the target's endian order for the purpose of
compile-time execution.

A `__ip` is an integer sized to hold a pointer, it can be used for pointer
tagging. However casting between `__ip` and `__ptr` may not work on all
operating systems and the casting implies the loss of pointer-providence.

A `__is` is an integer sized to hold an offset compatible with
pointer-arithmetic.

A `__ia` is an integer that can be used as the operands on a wide multiply CPU
instruction, for which the result is lo/hi split into two registers. This is for
the implementation of arbitrary sized integers. If the CPU does not have a wide
multiply instruction it will be emulated, and the `__ia` size is instead based
on the add/sub instructions which implements a carry chain.


## Floats

A `__f<N>` is a IEEE 754 binary floating point number.

A `__fd<N>` uses the double-double algorithm with IEEE 754 binary floating point
numbers to increase precision.

A `__fq<N>` uses the quad-double algorithm with IEEE 754 binary floating point
numbers to increase precision.


## Vectors

A `__i<N>x<M>` is a vector of `<M>` members of `__i<N>` integers.

A `__f<N>x<M>` is a vector of `<M>` members of `__f<N>` floating point numbers.


## Constants

Since integer/floating pointer literals are used with higher level types, this
language uses special identifiers. These special identifiers don't exist,
they are actualized when they are parsed.

   Type                       | Description
  :-------------              |:----------------
   `__i<N>_<C>`               | A constant `<C>` of `__i<N>` type.
   `__i<N>x<M>_<C>`           | A broadcast of `<C>` to all elements of the vector `__i<N>x<M>`.
   `__i<N>x<M>_<C0>_<C1>...`  | The constants `<C0>`, `<C1>`, etc; for each elements of the vector `__i<N>x<M>`.
   `__f<N>_<C>`               | A constant `<C>` of `__f<N>` type.
   `__f<N>x<M>_<C>`           | A broadcast of `<C>` to all elements of the vector `__f<N>x<M>`.
   `__f<N>x<M>_<C0>_<C1>...`  | The constants `<C0>`, `<C1>`, etc; for each elements of the vector `__f<N>x<M>`.

The syntax of `<C>` in `__i` integers are uses `p` for plus `+` and `m` for
minus `-`:
 - `[pm]?[0-9]+`
 - `0[bB][0-1]+`
 - `0[oO][0-7]+`
 - `0[dD][0-9]+`
 - `0[xX][0-9a-fA-F]+`

The syntax of `<C>` in `__f` floating point numbers uses `d` for the decimal dot
`.`, `p` for plus `+` and `m` for minus `-`:
 - `[pm]?[0-9]+d[0-9]*([eE][pm]?[0-9]+)?`
 - `[pm]?d[0-9]+([eE][pm]?[0-9]+)?`

The `<C0>`, `<C1>` syntax requires every element to be represented.

