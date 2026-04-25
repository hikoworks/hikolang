# type-coerce-operators

## Syntax

[_expression_](expression.md) `:=` [_type-expression_](type_expression.md) __|__\
[_expression_](expression.md) `:` [_type-expression_](type_expression.md) __|__\
[_expression_](expression.md) `:!` [_type-expression_](type_expression.md) __|__\
[_expression_](expression.md) `:?` [_type-expression_](type_expression.md) __|__\
[_expression_](expression.md) `~` [_type-expression_](type_expression.md) __|__\
[_expression_](expression.md) `~!` [_type-expression_](type_expression.md) __|__\
[_expression_](expression.md) `~?` [_type-expression_](type_expression.md)

## Semantics
The _type-coerce-operator_ is used to:
 - Check if the value of the _expression_ matches the result of the
   _type-expression_.
 - (optionally) Convert the value of the _expression_ to the result of the
   _type-expression_.
 

If the left hand side is an expression, then it is as-if the expression is
coerced. If the match part of the coercian fails it is a reportable error. 

If the left hand side is a variable definition, then it is as-if the
initializer-object is first coerced and the resulting type is used as the type
of the variable. If the match part of the coercian fails it is a reportable
error. 

If the left hand side is an argument definition, then it is as-if the passed-in
argument is first coerced and the resulting type is used as the type of the
argument. If the match part of the coercian fails the function is not a
candidate of the overload set.

The type is generally used to select the correct overload of a function or
operator in the expression. After this the rules for each coerce-method are
applied.

 - `a ~ T`: Check if `a` could be widened to `T`. Result is `a`
 - `a ~! T`: Check if `a` could be truncated to `T`. Result is `a`
 - `a ~? T`: Check if `a` could be narrowed to `T`. Result is `a`
 - `a := T`: Check if type of `a` is exactly `T`. Result is `a`
 - `a : T`: Widen `a` to `T`.
 - `a :! T`: Truncate `a` to `T`.
 - `a :? T`: Narrow `a` to `T`.

> [!NOTE]
> `:=` and `~=` would have identical semantics, we choose `:=` because of how
> it visual looks with function arguments; ADL (Argument Depended Lookup).


### Widen `a ~ T`, `a : T`

Widen is an (implicit) conversion of a value to a different type, where
information is never lost.

Properties:
 - The conversion can not throw
 - When doing round-trip: widen followed by narrow; the result is equal to
   the original, and the narrow conversion didn't throw.

Examples:
 - Same type as expression
 - The `const` or `mut` type of the expression
 - The `const` reference type of a reference-expression
 - The super-class reference type of a reference-expression.
 - Automatically take the reference or `const` reference of the expression.
 - Convert the value to a different value without loss of information.


### Truncate `a ~! T`, `a :! T`

Truncate is an explicit conversion of a value to a different type where
information may be lost.

Properties:
 - The conversion can not throw
 - Round toward zero, or stripping least significant information.
 - Clamp out-of-bound values

Examples (includes all Widen examples):
 - Add `mut` reference type of a reference-expression.
 - Convert the value to a different value with possible loss of information. 


### Narrow `a ~? T`, `a :? T`

Narrow is an explicit conversion of a value to a different type where
precision may be lost and which may fail with an error.

Properties:
 - Throws on out-of-bound / out-of-domain values
 - Round to nearest value

Examples (includes all Truncate examples):
 - Convert the value to a different value with possible loss of information that
   may fail with an error.
 - Convert a reference of a super-class to a reference of a sub-class, this
   requires run-time checking of the vtable-pointer.

### Equal `a := T`

The type of `a` is exactly `T`. This is used for argument in a
function-definition to match exactly with a specific type.

```
struct T {
  fn foo(self := T) { ... }      // Matches with T, const T, &T, &const T
  fn foo(var self := T) { ... }  // Matches with T, const T
  fn foo(ref self := T) { ... }  // Matches with &T
  fn foo(view self := T) { ... } // Matches with &T, &const T
}

```
