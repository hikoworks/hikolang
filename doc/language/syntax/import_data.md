# import-data

## Syntax

`import` `data` _file-path_ `in` _variable-name_ [_build_guard_](build_guard.md)__?__ `;`

file-path := [_string_literal_](string_literal.md)

variable-name := [_fully_qualified_name_](fully_qualified_name.md)

## Semantics

Import the data in a file into a variable of a byte array type.
This byte array can be accessed at compile time and runtime.

The byte array will be aligned to a 16 byte boundary, so that it is possible
to directly access the data as a native data structure.
