# enum-definition

## Syntax

A statement which is syntactic sugar for a enum-definition and variable declaration expression.

`enum` [_relative_fqname_](relative_fqname.md)
    __(__ `[` [_argument_list_](argument_list.md) `]` __)?__
    __(__ `(` [_type_list_](type_list.md) `)` __)?__
    `{` [_enum_name_list_](enum_name_list.md) `}` `;`



## Semantics


### Examples

The following is the optional type.

```
enum optional [T : type] {
    none,
    some(T)
};
```

```
enum result [T : type] {
    error(error_code)
    some(T),
}
```
