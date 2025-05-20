# ismutable

## Declaration

 - `instance_of[T](object) -> bool`
 - `is_const(object) -> bool`
 - `is_mutable(object) -> bool`
 - `is_ref(object) -> bool`

## Description

### instance_of
The `instance_of` function is used to check the type of `object` and compare it to type `T`; it returns `true` if:
 - If type of `object` is the same as type `T`,
 - If type of `object` is a subclass of type `T`, or
 - If `object` has a virtual table and:
   - `T` is a subclass of type of `object`, and
   - The actual type of `object` is a subclass of type `T` at runtime.

### is_const
Is the object a constant, either a variable or argument declared using `let`.

### is_mutable
Is the object a mutable variable, either a variable or argument declared using `var` or `ref`.

### is_ref
Is the object a reference, either a variable or argument declared using `ref`.
