# type-operator

 
 Operator | Description
 :------- | :-----------
 `:~+`    | Convert to a wider type, which always succeeds
 `:~!`    | Convert by truncating to a narrower type.
 `:~?`    | Convert to a narrower type, this may fail.
 `:+`     | Constrain the type, for which `:~+` is valid.
 `:!`     | Constrain the type, for which `:~!` is valid.
 `:?`     | Constrain the type, for which `:~?` is valid.
 `:`      | Constrain by exact type.



## Type Constraints `:+`, `:!`, `:?`, `:`

Type constraints check the type of the value, matches the type. The type may
include a binding selector:

 - `T`: Preserve the binding of the value
 - `*T`: Select/Prioritize the value representation
 - `&T`: Select/Prioritize the reference representation
 - `&&T`: Select/Prioritize the move-reference representation.

The type may include a `const`.


## Widen `:+`, `:~+`

Widen is an (implicit) conversion of a value to a different type, where
information is never lost.

Properties:
 - The conversion can not throw
 - When doing round-trip: widen followed by narrow; the result is equal to
   the original, and the narrow conversion didn't throw.

Examples:
 - Same type as expression
 - The super-class reference type of a reference-expression.
 - Convert the value to a different value without loss of information.


## Truncate `:!`, `:~!`

Truncate is an explicit conversion of a value to a different type where
information may be lost.

Properties:
 - The conversion can not throw
 - Round toward zero, or stripping least significant information.
 - Clamp out-of-bound values

Examples (includes all Widen examples):
 - Remove `const` reference type of a reference-expression.
 - Convert the value to a different value with possible loss of information. 


## Narrow `:?`, `:~?`

Narrow is an explicit conversion of a value to a different type where
precision may be lost and which may fail with an error.

Properties:
 - Throws `out_of_bound` error, if the value does not fit.
 - Round to nearest value (not an error).

Examples (includes all Truncate examples):
 - Convert the value to a different value with possible loss of information.
 - The conversion may fail with an `out_of_bound` error.
 - Convert a reference of a super-class to a reference of a sub-class, this
   requires run-time checking of the vtable-pointer.


## Equal `:`

The type of is exactly the same. This is used for argument in a
function-definition to match exactly with a specific type.

```
struct T {
  foo = fn(self : T) { ... }         // Preserves the binding of T.
  foo = fn(self : *T) { ... }        // 
  foo = fn(self : &T) { ... }        // 
  foo = fn(self : &const T) { ... }  // 
  foo = fn(self : && T) { ... }      // 
  foo = fn(self : &&const T) { ... } // 
}
```
