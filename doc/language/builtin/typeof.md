# type() function

## Declaration

`typeof(object) -> __meta_type__`

> [NOTE!]
> This function must be evaluated at compile time.

## Description
The `typeof()` function returns the type of the given object.
The type is an instance of a sub-class of the `__meta_type__` type.

If you use `type()` on a type object, it will return the instance
of `__meta_struct_type__`, which represent the meta-type object.
Recursively calling `type()` on the meta-type object will return
the same meta-type object.
