# __meta_type__

## Definition

### Member functions
- `id(self) -> __idx__` - A unique identifier for each instance of `__meta_type__` and its subclasses.
- `name(self) -> str` - The name of the type.
- `set_name(ref self, name: str) -> ref type(self)` - Set the name of the type.

## Description

All instances of `__meta_type__` and its subclasses are interned, meaning that it is not possible to
have identifical instances of the same type; in that case the same instance is returned. This
is used when templates are instantiated, so that the same instance is used for the same type.

IDs are unique for each instance of `__meta_type__` and its subclasses. This is used to
compare types in the type system. The ID is not guaranteed to be the same between different
runs of the same program.

The IDs are ordered in such a way that types in a single inheritance hierarchy
can be range-compared. This is used to implement a high performance `isinstance[T](object)`
and `cast[T](object)` functions.


