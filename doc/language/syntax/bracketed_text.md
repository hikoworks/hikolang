# bracketed-text

## Syntax

Bracketed text is a sequence of characters with proper nesting of brackets.

The parser will keep track of the following:
 - Nesting level of the following brackets:
   - Parentheses `(` `)`
   - Square brackets `[` `]`
   - Curly braces `{` `}`
 - It ignores brackets that are inside string literals, using the quotation marks `"`, `'` or `\``.
 - Quotation marks inside string literals can be escaped with a backslash `\`.
 - Brackets and quotes inside comments are ignored. Comments start with `//`, `;` or `#`
   and end at the end of the line.
 - Any valid Unicode character can be used inside the bracketed text.

## Semantics

