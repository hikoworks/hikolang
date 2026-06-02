# function-declaration

## Syntax

_function-declaration_ :=
    `fn` [_fqname_] `(` [_argument-declaration-list_]__?__ `)`
    `->` [_expression_] `;`


[_argument-declaration-list_]: argument_declaration_list.md
[_code_block_]: code_block.md
[_fqname_]: fqname.md
[_expression_]: expression.md


## Semantics

This statement declares an external function to link against from a library,
i.e. external from the program.

The `@abi()` attribute must match the ABI of the function in the library.
