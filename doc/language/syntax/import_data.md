# import-data

## Syntax

`import` `data` _file-path_ `in` _variable-name_ __(__ `if` [_guard_expression_](guard_expression.md) __)?__ `;`

file-path := [_string_literal_](string_literal.md)

variable-name := [_relative_fqname_](relative_fqname.md)

## Semantics

Import the data from a file into a variable of a byte array type.
This byte array may be accessed at compile time and runtime.

The byte array will be aligned to a 16 byte boundary, so that it is possible
to directly access the data as a native data structure.


### Build guard (optional)

The optional [_guard_expression_](guard_expression.md) is evaluated during the
prologue-scan phase of compilation, this checks if the `import` statment
should be processed or ignored.
