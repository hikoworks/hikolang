Tutorial - Program Structure
============================

Smallest program
----------------

The minimum of a program is a single file with the `program` statement
which ends the prologue.

Example (small.hkm):

```
program "small"
```

That’s the smallest valid program header.
Nothing runs yet—we need code.

Hello World
-----------

Example (hello.hkm):

```
import std
program "hello"

std.print("Hello World")
```
