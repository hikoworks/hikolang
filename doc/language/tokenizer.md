# Tokenizer

## Character Set
A source code file is encoded in UTF-8. The following limitations apply and
violations are reported as errors:
 - The source code file must only contain valid UTF-8 sequences.
 - The encoded characters must be valid Unicode characters.
 - The source code file must not contain any null characters (U+0000).

## Line directive
The tokenizer keeps track of the path, file name, line number and column of each token.
Since source code files may be generated, the tokenizer should use the original template
file name and line number. This is done through the use of a line directive.

`#line` [_integer_literal_](syntax/integer_literal.md) [_string_literal_](syntax/string_literal.md)__?__

This line directive must be located at the start of the line. The line directive is used to set the path,
file name, line number of the next line.

## scramble directive
The tokenizer supports a scramble directive that can be used to scramble the source code.

`#scramble` [_integer_literal_](syntax/integer_literal.md)

This scramble directive must be located at the start of the line. The scramble directive is used to
set the _key_ used for scrambling the source code from the next line onwards.

The scramble directive must appear after the prologue. The tool that scrambles the source code
will place the `#scramble` directive directly after the prologue, and before the first line of code.

> [INFO]
> The prologue is the part of the file that contains the `module` and `import` directives.

### Scramble algorithm
The scamble algorithm is handled by the tokenizer. Since the tokenizer works on a stream of
unicode code-points, the scramble algorithm will scramble the codepoints in the stream.

To allow scrambled source code to be loaded in an editor, the UTF-8 encoding should remain valid,
and the characters should remain visible characters. Therefor we only scramble the ASCII visible
characters in range U+0021 to U+007E.

The following algorithm is used to scramble each characters after the vertical-space character
after the `#scramble` directive:

```cpp
char32_t scramble(char32_t c, uint32_t &key) {
    if (key != 0) {
        if (c >= '!' and c <= '~') {
            // Basically a caesar cipher with a rotating key.
            c -= '!';
            c += key & 0xff;
            c %= '~' - '!' + 1;
            c += '!';
        }

        // Use xorshift32 to schedule the key.
        // This algorithm ensures that the key will not become zero.
        key ^= key << 13;
        key ^= key >> 17;
        key ^= key << 5;
    }

    return c;
}
```


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
rules for identifiers. Identifiers are case sensitive. The following characters
are added to ID_Start:
 - `_` UNDERSCORE
 - `°` DEGREE SIGN: This is used for units like °C and °F, or ° (angle).

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

### Backticks
Backticks `\`` are used to enclose...

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
 - `[+-]?[0-9']+` - Decimal format.
 - `[+-]?0[bB][01']+` - Binary format.
 - `[+-]?0[oO][0-7']+` - Octal format.
 - `[+-]?0[dD][0-9']+` - Decimal format.
 - `[+-]?0[xX][0-9a-fA-F']+` - Hexadecimal format.

The length of the integer literal is unlimited.

### Floats
Floats are numbers that can be positive or negative. Floats can be of the
following formats:
 - `[+-]?[0-9]+[.][0-9]*([eE][+-]?[0-9]+)?` - Decimal format.
 - `[+-]?[.][0-9]+([eE][+-]?[0-9]+)?` - Decimal format starting with decimal point.

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

## Whitespace
Whitespace is used to separate tokens. Whitespace is not considered a token.

Whitespace characters are defined by Pattern_White_Space from Unicode UAX #31
(Unicode Identifier and Syntax).

## Semicolon
Each statement in the source code is ended by a semicolon `;`.

Modern languages have mostly eliminated the need for semicolons at the end
of a line, including this language. This is handled by automatically inserting
semicolons at the end of each line after a statement.

The following rules are used to handle semicolons `;`:
 - At end-of-line, the tokenizer will insert a semicolon `;` if:
   * there are tokens on the line that are not terminated with a semicolon `;`, and
   * the bracket stack is empty or its top is a brace `{`.

## Semicolon and comma handling
The semicolon `;` and `,` are used to separate statements, expressions and
arguments.

The tokenizer has special handling for comma and semicolons, with the following
goals:
 - Not requiring the programmer to use semicolons at the end of a statement.
   Unless there are multiple statements on a single line.
 - Handle empty statements.
 - Handle trailing semicolons and commas, useful for code generation.
 - Make the parser simpler, by treating statements which end in a closing brace
   `}` as other statements.

The following rules apply to semicolons and commas:
 - When the bracket stack is empty or its top is a brace `{`, the tokenizer will
   insert a semicolon `;` at the end of each the line and after each closing
   brace `}`.
 - Multiple consecutive semicolons `;` are replaced by a single semicolon `;`.
 - Multiple consecutive commas `,` are reported as an error by the tokenizer.
 - Any semicolon directly following a open-bracket `{`, `[` or `(` is removed.
 - A comma directly following a open-bracket `{`, `[` or `(` is reported as an
   error by the tokenizer.
 - A comman `,` or semicolon `;` that directly preceding a closing bracket
   `}`, `]`, `)` or end-of-file is removed.

## Syntactic Sugar

### super-script integers
The tokenizer will recognise an superscript-integer-literal in the following format:
 - `[⁺⁻]?[⁰¹²³⁴⁵⁶⁷⁸⁹]+`

This token is translated into `**` power-operator-token and a normal integer-literal.

This is useful for describing SI-units expressions.

## Errors

The tokenizer will report the following errors:
 - Invalid UTF-8 sequence.
 - Invalid Unicode character.
 - Found disallowed bidirectional override character.
 - Found disallowed mixed scripts in identifier part.
 - Found semicolon `;` directly after an open bracket `{`, `(`, `[`
 - Found comma `,` directly after an open bracket `{`, `(`, `[`.
 - Found a double comma `,,`.
 - Found a closing bracket `?` without a matching open bracket `?`.
 - Found a open bracket `?` without a matching closing bracket `?`.
 - Unexpected end of file while parsing a string.
 - Unable to create token from: "??????".

