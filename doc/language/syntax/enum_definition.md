# enum-definition

## Syntax

_enum-definition_ := `enum` [_fqname_](fqname.md) __(__ `:` _buildin_int_
__)?__ `{` [_code_block_](code_block.md) `}` `;`


_enum-name-add_ := `enum` [_fqname_](fqname.md) `add` [_identifier_](identififier) `;`


## Semantics

An enum defines a symbolic type consisting exclusively of its declared member
names. A value of an enum type can never exist outside this set.

Enums are not integer aliases. While the implementation stores enum values
using an integer representation internally, the underlying numeric value is not
part of the language-visible semantics and cannot be accessed or converted to
directly.

Unlike most types, an enum may be extended with additional member names after
the type has been frozen. An enum type is therefore considered complete once it
has been declared or frozen, allowing it to be used by functions during
compilation while still permitting additional members to be introduced later.

Because an enum creates a symbol table containing its member names, every enum
must have a declared name. For this reason, there is no anonymous
_enum-declaration-expression_ form.

By default, the underlying type is a 32-bit signed integer. A different builtin
integer type may be specified explicitly in the declaration. It is an error if
adding additional members would overflow the range of the underlying integer
type.

Some enums are managed directly by the compiler:
 - Members of `error_code` are introduced with the `throw` statement.
 - Members of `type_tag` are introduced when using a tag in a type expression.

The integer value `0` is never assigned to a named enum member. Instead, `0`
is reserved as a niche value for layout optimizations in types such as
optional[error_code].
