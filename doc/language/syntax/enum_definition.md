# enum-definition

## Syntax

`enum` [_relative-fqname_](relative_fqname.md)
    __(__ `[` [_argument-list_](argument_list.md) `]` __)?__
    __(__ `(` [_type-list_](type_list.md) `)` __)?__
    `{` [_enum-name-list_](enum_name_list.md) `}` `;`



## Semantics


### Examples

The following is the optional type.

```
enum optional[T : type] {
    none;
    some(T);
};

interface nonable {
    fn __none__(self : nonable);
    fn __set_none__(self : nonable);
}

class ref[T : type] (nonable) {
    __builtin_ptr _p;

    fn __none__(self := ref[T]) {
        return self._p == 0
    }

    fn __set_none__(self := ref[T]) {
        self._p = 0
    }
};


enum optional[T : nonable] {
    none(T) {if __none__($0); set __set_none__($0)};
    some(T);
};

```

```
enum result [T : type] {
    error(error_code)
    some(T),
}
```
