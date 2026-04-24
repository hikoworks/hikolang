Tutorial - Ranged Integers
==========================

There are three type of integers in this language:
 - Ranged Integers; integers that fall within a lower and upper bound.
 - Long Integers; integers that are dynamically sized.
 - LLVM Integers; low-level integers used for standard library implementation. 

Integer literals and integers in the standard library public API are
ranged-integers.

By using ranged integers we can fully prevent non-checked overflow conditions.
This primarily improves safety and correctness without strongly impacting
run-time performance.

> [!NOTE]
> By implementing ranged integers as arbitrarily sized integers, we can do
> overflow checks at the end of a multi-step calculation instead of before each
> sub-expression. Reducing the number of bound checks significantly.

Integer Type
------------

An `int` is a type template, with as argument a range. For example `int[4..=10]`
is an integer that may have the values: `4`, `5`, `6`, `7`, `8`, `9` or `10`.

The operator `..=` creates a closed-range, while the `..<` operator creates a
half-open-range. The range must be a compile time constant.


Integer Literal
---------------

The type of an integer literal is an integer with the range set to only being
able to hold that integer value. So for example the integer literal `42` has
the type `int[42..=42]`.


Range Inference
---------------

Integer expressions result in a type that encapsulate the full range of values.
In this example the type of `x` and `y` are converted to a larger integer range,
to show how the type of `z` is inferred. 

```
// file: let_int_range
import std
program "let_int_range"

let a = 2 + 3             // int[2..=2] + int[3..=3] -> int[5..=5]
std.repr(a)               // displays: 5

let x = 2 : int[2..=4]
let y = 7 : int[3..=10]
let z = x * y             // int[2..=4] * int[3..=10] -> int[6..=40]
std.repr(z)               // displays: 14
```

Range Mismatch
--------------

There are three ways of converting an integer from one range to another:

 - Widening: the target range can hold ALL the values from the source range.
 - Narrowing: the target range can hold SOME of the values from the source
   range.
 - Non-overlapping: the target range can hold NONE of the values from the source
   range.


### Widening

When widening ALL values can be converted, that means no bound checking will
be done when assigning the value.

```
// file: int_widen
import std
program "int_narrow"

var a = 0 : int[0..=10]
a = 2                     // int[2..=2] is fully in range of int[0..=10]
std.repr(a)
```

### Narrowing

When narrowing SOME values can be converted. A bound check will be done to
determine if the value is within bounds of the target type. A `bound_error`
will be thrown from the operation, which is required to be locally checked.
In this case `try` is used to throw the `bound_error` up one level,
causing a trap in this case.

```
// file: int_narrow
import std
program "int_narrow"

var a = 0 : int[0..=10]
try a = a + 2             // int[2..=12], values 11 and 12 cause overflow
std.repr(a)
```

### Non-overlapping range

When NONE of the values can be converted a compile time error will be reported.

```
var a = 0     // int[0..=0]
// a = a + 2  error: non-overlapping range (int[0..=0] and int[2..=2])
```
