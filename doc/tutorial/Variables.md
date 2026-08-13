Tutorial - Variables
====================

## variables

```
// file: variable.hkm
import std
program "variable"

a := 40.0
a := a + 2.0
std.repr(a)                                      // 42.0
```

The expression's value is copied into the value maintaned by the variable
declaration.


## immutables

```
// file: immutable.hkm
import std
program "let"

a = 40.0
b = a + 2.0
std.repr(b)                                      // 42.0
```
