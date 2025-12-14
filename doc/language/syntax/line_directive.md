# line-directive

## Syntax

`#line` [_decimal-literal_](decimal_literal.md) __(__ [_string-literal_](string_literal.md) __)__

## Semantics
The `#line` directive is used to set the path, file name, and line number for
the next line of code. It must be located at the start of the line.

The directive can optionally include a string literal that specifies the file
name. If the string literal is not provided, the current file name is used.

The line number is specified as a decimal literal, which is a sequence of one or
more digits (0-9) that represents a non-negative integer value. The first line
of the file is considered to be line 1.

The `#line` directive is typically used in generated code to maintain the
original source file's context, allowing tools and debuggers to reference the
original source file and line number instead of the generated code's line
number.

> [!NOTE]
> The `#line` directive is not a token in the language but rather a
> directive for the tokenizer. It is completely hidden from the parser.

## Example

```
#line 42 "source_file.foo"
```

This example sets the line number to 42 and specifies the file name as "source_file.foo".


