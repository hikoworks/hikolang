# function-declaration

## Syntax

_function-definition_ :=\
    [_attributes_]__*__\
    `fn` [_fqname_]__?__ `(` [_argument-declaration-list_]__?__ `)` [_function-return-type_]__?__\
    __(__ `expect` `(` [_expression_] `)` __)*__\
    __(__ `pre` `(` [_expression_] `)` __)*__\
    __(__ `post` `(` [_expression_] `)` __)*__ `;`


[_argument-declaration-list_]: argument_declaration_list.md
[_code_block_]: code_block.md
[_function-return-type_]: function_return_type.md
[_fqname_]: fqname.md
[_expression_]: expression.md


## Semantics

This statement declares an external function to link against from a library,
i.e. external from the program.

## Attributes

The `@abi()` attribute must match the ABI of the function in the library.
