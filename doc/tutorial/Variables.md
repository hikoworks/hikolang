Tutorial - Variables
====================

## var

```
// file: var.hkm
import std
program "var"

var a = 40.0
a = a + 2.0
std.repr(a)                                      // 42.0
```

The expression's value is copied into the value maintaned by the variable
declaration.


## let

```
// file: let.hkm
import std
program "let"

let a = 40.0
let b = a + 2.0
std.repr(b)                                      // 42.0
```
