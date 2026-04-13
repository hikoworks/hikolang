Tutorial - Values
=================

Literals
--------

```
// file: literals.hkm
import std
program "literals"

std.print("string: {}", "Hello World")           // string: Hello World
std.print("float: {}", 42.0)                     // float: 42.0
std.print("int: {}", 42)                         // int: 42
```


Basic Arithmetic
----------------

```
// file: arithmetic.hkm
import std
program "arithmetic"

std.print("string: {}", "Hello" + " " + "World") // string: Hello World
std.print("float: {}", (10.0 + 0.5) * 4)         // float: 42.0
std.print("int: {}", 21 * 2)                     // int: 42
```


Printing Values
---------------

```
// file: printing.hkm
import std
program "printing"

std.print("{}", "Hello World")                   // Hello World
std.print("{}", 42.0)                            // 42.0
std.print("{}", 42)                              // 42
std.repr("Hello World")                          // "Hello World"
std.repr(42.0)                                   // 42.0
std.repr(42)                                     // 42
```
