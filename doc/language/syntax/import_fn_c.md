# import-fn

## Syntax

`import` `fn` `c` [_name_](name.md) `(` [_argument-list_](argument_list.md) `)` [_result-type-declaration_](result_type_declaration.md)
    __(__ `if` [_guard_expression_](guard_expression.md) __)?__ `;`

### Build guard (optional)

The optional [_guard_expression_](guard_expression.md) is evaluated during the
prologue-scan phase of compilation, this checks if the `import` statement
should be processed or ignored.
