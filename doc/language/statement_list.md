# statement-list

## Syntax
<style type="text/css">
strong {
    color: green !important;
    font-weight: normal !important;
}
</style>

_statement_ __(__ `;` _statement_ __)*__ `;`__?__

## Semicolon insertion
Statements are normally separated by semi-colons, however the [tokenizer](../tokenizer.md) will insert
semi-colons automatically at the end of a line, when:
 - Tokens have been generated since the last semicolon `;` was generated.
 - When the top of the bracket stack is a brace `{` or empty.

