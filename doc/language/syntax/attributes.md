# attributes

## Syntax

[_fqname_](fqname.md) __(__ `(` [_argument_list_](argument_list.md) `)` __)?__


## Semantics

If the [_fqname_](fqname.md) is a single name,
then this attribute is required. A required attribute must be handled by the compiler.
If the compiler does not know how to handle the attribute it is an error.

If the [_fqname_](fqname.md) has the `std.` prefix
it is a well-known optional attribute. These attributes don't change the semantic
meaning of the program, instead they are directives to the compiler to create
better code.

Other prefixes are attributes that are handled by specific compilers. Compilers
should ignore any attributes that do not match their own prefix.




