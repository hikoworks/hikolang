# Tokenizer

## Character Set
A source code file is encoded in UTF-8. The following limitations apply and
violations are reported as errors:
 - The source code file must only contain valid UTF-8 sequences.
 - The encoded characters must be valid Unicode characters.
 - The source code file must not contain any null characters (U+0000).

## Tokens
The following is a list of all the tokens that are used in the language:
 - *identifier* - A name that is used to identify a variable, function, namespace, type or keyword.
 - *string* - A string literal, enclosed in double quotes. The string can contain escape sequences.
 - *char* - A character literal, enclosed in single quotes. The character can be an escape sequence.
 - *integer* - An integer literal, which can be in binary, octal, decimal or hexadecimal format.
 - *float* - A floating point literal, which can be in decimal or hexadecimal format.
 - *bracket* - A bracket, which can be a parenthesis `(` `)`, square bracket `[` `]` or curly brace `{` `}`.
 - *operator* - An operator a sequence of non-letter symbols that represents an operation.
 - *comment* - Comments are not considered tokens, but the comment text is stored
               with a nearby token in the AST.
 - *end-of-statement* - A virtual token that is used to mark the end of a statement.
 - *whitespace* - A character used to separate tokens. Whitespace are not considered tokens.

### Identifiers
Identifiers are used to name variables, functions, namespaces, types and
keywords.

Identifiers follow the Unicode UAX #31 (Unicode Identifier and Syntax)
rules for identifiers. This languages adds the underscore `_` character as part
of ID_Start. Identifiers are case sensitive.

For security purposes, it is a reportable error when an identifier violates the
following rules:
 - Characters of identifiers must be part of the UAX #31 Recommended Scripts.
 - After splitting an identifier by characters that are part of the Common or
   Inherited scripts; all characters of each part must be from the same script.
   This is to prevent confusion between identifiers that are visually similar
   but are from different scripts.

Internally identifiers are stored in NFC (Normalization Form C) form, and
identifiers are compared in NFC form.

Identifier that include a double underscore `__` are reserved for the language
implementation.

### Strings
Strings are enclosed in double quotes `"` and can contain escape sequences.
Strings can be empty, and can contain any valid Unicode character except for
a non-escaped `"` character.

A raw-string is a string that starts with `r"` and ends with `"` and can contain
any valid Unicode character except for a non-escaped `"` character.
Raw-strings do not support escape sequences. To include a double quote in a
raw-string, you must escape it using quote doubling `""`.
Raw-strings are useful for writing regular expressions, or when you want to
include a lot of backslashes in the string without having to escape them.
For example:
 - `r""` - A raw string that is empty.
 - `r"he\lo"` - A raw string that contains the text `he\lo`.
 - `r"""hello"` - A raw string that contains the text `"hello`.
 - `r"hello"""` - A raw string that contains the text `hello"`.
 - `r"he""o"` - A raw string that contains the text `he"o`.
 - `r""""` - A raw string that contains the text `"`.

### Characters
Characters are enclosed in single quotes `'` it contains a single character,
this single character may be an escape sequence. See the section on escape
sequences in Strings for more information.

### Escape Sequences
Escape sequences are used to represent characters that are not easily typed.

The following escape sequences are supported:
 - `\'` - The single quote character `'`.
 - `\"` - The double quote character `"`.
 - `\\` - The backslash character `\`.
 - `\a` - The alert (bell) character.
 - `\b` - The backspace character.
 - `\f` - The form feed character.
 - `\n` - The newline character.
 - `\r` - The carriage return character.
 - `\t` - The tab character.
 - `\v` - The vertical tab character.
 - `\0` - The null character.
 - `\u{n...}` - A Unicode character, where `n...` is one or more hexdecimal
                digits. The unicode character must be a valid Unicode code point.
 - `\N{name}` - A Unicode character, where `name` is the official Unicode name of the
                character. The name must result in a valid Unicode code point.

Strings are stored in UTF-8 format without normalization, after unescaping the
escape sequences.

### Integers
Integers are whole numbers that can be positive or negative. Integers can be
of the following formats:
 - [+-]?[0-9']+ - Decimal format.
 - [+-]?0[bB][01']+ - Binary format.
 - [+-]?0[oO][0-7']+ - Octal format.
 - [+-]?0[dD][0-9']+ - Decimal format.
 - [+-]?0[xX][0-9a-fA-F']+ - Hexadecimal format.

The length of the integer literal is unlimited.

### Floats
Floats are numbers that can be positive or negative. Floats can be of the
following formats:
 - [+-]?[0-9]+[.][0-9]*([eE][+-]?[0-9]+)? - Decimal format.
 - [+-]?[.][0-9]+([eE][+-]?[0-9]+)? - Decimal format starting with decimal point.

The length of the float literal is unlimited.

### Brackets
Brackets are used to group expressions and to define the scope of blocks of code.
Brackets can be of the following types:
 - Parentheses `(` `)` - Used to group expressions.
 - Square brackets `[` `]` - Used to group expressions.
 - Curly braces `{` `}` - Used to group statements.

It is a syntax error when brackets are not properly nested and if a bracket is
not closed properly before the end of the file.

The tokenizer will keep track of the nesting level of brackets for determining
when to insert the end-of-statement token.

### Operators
Operators are used to perform operations on values.

Operators are the maximum sequence of Pattern_Syntax characters from
Unicode UAX #31 (Unicode Identifier and Syntax).

As part of the syntax operators may also be keywords, but not as part of the
tokenizer.

### Comments
Comments are used to add notes to source code, or to document functionality.
Comments are not considered tokens. But the text from documentation comments
is stored with a nearby token in the AST, so that it becomes easy to generate
documentation.

Comments come in several forms:
 - Block-documentation comments start with `/**` and end with `*/`.
   The text of the comment is stored with the next token.
 - Line-documentation comments start with `///` and end at the end of the line.
   The text of the comment is stored with the next token.
 - Back-documentation comments start with `//<` and end at the end of the line.
   The text of the comment is stored with the preceding token. 
 - Single line comments start with `//` and end at the end of the line.
   The text of the comment is ignored.
 - Multi-line comments start with `/*` and end with `*/`.
   The text of the comment is ignored.

The comment text itself has a specific format, this is documented in
documentation.md.

### Whitespace
Whitespace is used to separate tokens. Whitespace is not considered a token.

Whitespace characters are defined by Pattern_White_Space from Unicode UAX #31
(Unicode Identifier and Syntax).

### End-of-statement
The end-of-statement token is a virtual token that is used to mark the end of a
statement. The end-of-statement token is inserted by the tokenizer when:

 - When a semicolon `;` is found.
 - When a newline is found and:
   + Tokens have been generated since the last end-of-statement was generated.
   + When the top of the bracket stack is a brace `{` or empty.

The generated end-of-statement token generated is the semicolon `;` token.
