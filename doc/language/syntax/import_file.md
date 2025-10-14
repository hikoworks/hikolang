# import-file

## Syntax

`import` `file` _file-path_ `in` _variable-name_ [_compile_condition_](compile_condition.md)__?__ `;`

file-path := [_string_literal_](string_literal.md)

variable-name := [_fully_qualified_name_](fully_qualified_name.md)

## Semantics

Import the data in a file into a variable of a byte array type.
This byte array can be accessed at compile time and runtime.
