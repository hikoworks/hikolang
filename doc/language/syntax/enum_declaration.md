# enum-declaration

## Syntax

_enum-declaration_ := `enum` [_fqname_](fqname.md) __(__ `:` _buildin_int_
__)?__ `{` _enum_names_**?** `}` `;`

_enum_names_ :=
[_identifier_](identififier)
__(__ `;` [_identifier_](identififier) __)*__
`;`__?__

_enum-name-declaration_ := `enum` [_fqname_](fqname.md) `+`
[_identifier_](identififier)


## Semantics

An enum is a type of which a value is that of a named member.

An enum is a type which is internally an integer number, the named members
of an enum are assigned an unique number. Even though internally an enum is
implemented as 

An enum is kind of special for a type as new named members may be added to an
enum even after it has been frozen. This is because the bit-layout of an enum
remains unmodified when named members are added.

Named members can be added using `enum <type> + <name>` declaration, even
after the enum-type has been frozen. This capability is useful when implementing
`error_code` which may become frozen early when a function that throws an error
is used during compilation.

Because an enum creates a symbol table of named members, the enum needs to be
declared with a name. This is why there is no _enum-declaration-expression_
variant.  

By default the underlying type is a 32-bit integer, but you can specify the
underlying builtin-integer type. It is an error if the underlying integer
overflows when adding new member names.

Some of the enums are directly managed by the language itself:
 - Member names for `error_code` are introduced with the `throw` statement.
 - Member names for `type_tag` are introduced when using a tag in a type expression.

The value zero is not assigned to a member name, and zero is the niche value
used for optimized storage inside a variant like `optional[error_code]`.
