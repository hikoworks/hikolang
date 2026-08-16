# coerce-operator

## Syntax

_coerce-operator_ :=\
      [_expression_] `:` [_type-expression_]\
    __|__ [_expression_] `:!` [_type-expression_]\
    __|__ [_expression_] `:?` [_type-expression_]\
    __|__ [_expression_] `<-`\
    __|__ [_expression_] `<-` __(__ `&` __|__ `&&` __|__ `*`  __)__\
    __|__ [_expression_] `<-` [_type-expression_]\
    __|__ [_expression_] `<-!` [_type-expression_]\
    __|__ [_expression_] `<-?` [_type-expression_]\
    __|__ [_expression_] `<-=` [_type-expression_]

[_expression_]: expression.md
[_type-expression_]: type_expression.md

## Semantics
The _coerce-operator_ is used to:
 - Check if the value of the _expression_ matches the result of the
   type _expression_.
 - (optionally) Convert the value of the _expression_ to the result of the
   type _expression_.
 

If the left hand side is an expression, then it is as-if the expression is
coerced. If the match part of the coercion fails it is a reportable error. 

If the left hand side is a variable definition, then it is as-if the
initializer-object is first coerced and the resulting type is used as the type
of the variable. If the match part of the coercion fails it is a reportable
error. 

If the left hand side is an argument definition, then it is as-if the passed-in
argument is first coerced and the resulting type is used as the type of the
argument. If the match part of the coercion fails the function is not a
candidate of the overload set.

The type is generally used to select the correct overload of a function or
operator in the expression. After this the rules for each coerce-method are
applied.

 - `a : T`: Widen `a` to `T`.
 - `a :! T`: Truncate `a` to `T`.
 - `a :? T`: Narrow `a` to `T`.
 - `a <- T`: Check if `a` could be widened to `T`. Result is `a`
 - `a <-! T`: Check if `a` could be truncated to `T`. Result is `a`
 - `a <-? T`: Check if `a` could be narrowed to `T`. Result is `a`
 - `a <-= T`: Check if type of `a` is exactly `T`. Result is `a`

### Type Constraints `<-`, `<-!`, `<-?`, `<-=`

Type constraints check the type of the value on the LHS, matches the type
on the RHS. The RHS may include a binding selector:

 - `T`: Preserve the binding on the LHS
 - `*T`: Select/Prioritize the value representation
 - `&T`: Select/Prioritize the reference representation
 - `&&T`: Select/Prioritize the move-reference representation.

The type may include a `const`.

### Type Specification

In type specification `<-` may omit the type which
can be inferred from the expression passed in.

### Widen `<-`, `:`

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


### Truncate `<-!`, `:!`

Truncate is an explicit conversion of a value to a different type where
information may be lost.

Properties:
 - The conversion can not throw
 - Round toward zero, or stripping least significant information.
 - Clamp out-of-bound values

Examples (includes all Widen examples):
 - Add `mut` reference type of a reference-expression.
 - Convert the value to a different value with possible loss of information. 


### Narrow `<-?`, `:?`

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

### Equal `<-=`

The type of `a` is exactly `T`. This is used for argument in a
function-definition to match exactly with a specific type.

```
struct T {
  foo = fn(self <-= T) { ... }         // Preserves the binding of T.
  foo = fn(self <-= *T) { ... }        // 
  foo = fn(self <-= &T) { ... }        // 
  foo = fn(self <-= &const T) { ... }  // 
  foo = fn(self <-= && T) { ... }      // 
  foo = fn(self <-= &&const T) { ... } // 
}

```
