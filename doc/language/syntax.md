# Combined EBNF


_alternate-clauses_ := _alternate-clauses-1_ __|__ _alternate-clauses-2_

_alternate-clauses-1_ :=\
    [_catch-clauses_]\
    __(__ `elif` `(` [_condition-expression_] `)` `{` [_statement-list_ ]`}` [_catch-clauses_] __)*__\
    __(__ `else` `{` [_statement-list_] `}` __)?__\
    __(__ `empty` `{` [_statement-list_] `}` __)?__

_alternate-clauses-2_ :=\
    __(__ `else` `{` [_statement-list_] `}` __)?__\
    __(__ `empty` `{` [_statement-list_] `}` __)?__\
    [_catch-clauses_]

[_condition-expression_]: condition_expression.md
[_catch-expression_]: catch_expression.md
[_statement-list]: statement_list.md


_argument_ := __(__ `$`__?__ [_name_] `=` __)?__ [_expression_]

[_name_]: name.md
[_expression_]: expression.md


_argument-declaration_ :=\
      [_name_]__?__ [_type-annotation_]__?__ __(__ `=` [_expression_] __)?__\
    __|__ [_name_] `...` [_type-annotation_]__?__
    __|__ [_literal_] [_type-annotation_]__?__\
    __|__ `(` [_expression_] `)` [_type-annotation_]__?__\

[_type-annotation_]: type_annotation.md
[_expression_]: expression.md
[_literal_]: literal.md
[_name_]: name.md


_argument_declaration_list := [_argument-declaration_] __(__ `,` [_argument-declaration_] __)*__ `,`__?__
`)`

[_argument-declaration_]: argument_declaration.md


_argument_list := [_argument_] __(__ `,` [_argument_] __)*__

[_argument_]: argument.md


_attribute_ := `[[` [_fqname_] __(__ `(` [_argument-list_] `)` __)?__ `]]`

[_argument-list_]: argument_list.md
[_fqname_]: fqname.md


Bracketed text is a sequence of characters with proper nesting of brackets.

The parser will keep track of the following:
 - Nesting level of the following brackets:
   - Parentheses `(` `)`
   - Square brackets `[` `]`
   - Curly braces `{` `}`
 - It ignores brackets that are inside string literals, using the quotation
   marks `"`, `'` or `\``.
 - Quotation marks inside string literals can be escaped with a backslash `\`.
 - Brackets and quotes inside comments are ignored. Comments start with `//`,
   `;` or `#` and end at the end of the line.
 - Any valid Unicode character can be used inside the bracketed text.


_break_statement_ :=\
      `break` [_label_]**?** `;`\
    __|__ `break` [_label_]**?**  `with` [_expression_]__?__ `;`\
    __|__ `break` [_label_]**?**  `throw` `fatal`__?__ [_identifier_] `;`

[_expression_]: expression.md
[_identifier_]: identifier.md
[_label_]: label.md


_build-guard_ := `if` [_build-guard-expression_]

[_build-guard-expression_]: build_guard_expression.md


_call-operator_ := [_expression_] `(` [_argument-list_]__?__ `)`


[_argument-list_]: argument_list.md
[_expression_]: expression.md


_catch-clauses_ :=\
    __(__ `catch` `(` [_error-list_] `)` `{` [_statement-list_ ]`}` __)*__\
    __(__ `catch` `{` [_statement-list_] `}` __)?__

[_error-list_]: error_list.md
[_statement-list]: statement_list.md


_catch-operator_ :=\
      [_expression_] `catch` [_expression_]\
    __|__ [_expression_] `catch` `(` [_error-list_] `)` [_expression_]

[_error-list_]: error_list.md
[_expression_]: expression.md


_code-block_ := [_attribute_]__*__ `{` [_statement-list_] `}`

[_attribute_]: attribute.md
[_statement-list_]: statement_list.md


_coerce-operator_ :=\
      [_expression_] `:` [_type-expression_]\
    __|__ [_expression_] `:!` [_type-expression_]\
    __|__ [_expression_] `:?` [_type-expression_]\
    __|__ [_expression_] `<-`\
    __|__ [_expression_] `<-` __(__ `&` __|__ `&&` __|__ `*`  __)__\
    __|__ [_expression_] `<-` [_type-expression_]\
    __|__ [_expression_] `<-!` [_type-expression_]\
    __|__ [_expression_] `<-?` [_type-expression_]\
    __|__ [_expression_] `<-=` [_type-expression_]

[_expression_]: expression.md
[_type-expression_]: type_expression.md


_context_argument_ := `$` [_identifier_]

[_identifier_]: identifier.md


_decimal-literal_ := [0-9]+


_directive_ :=\
      [_namespace-directive_]\
    __|__ [_syntax-domain-directive_]\
    __|__ [_syntax-effect-directive_]\
    __|__ [_syntax-error-directive_]\
    __|__ [_syntax-operator-directive_]\
    __|__ [_syntax-unit-directive_]\


_do-clause_ := `do` `{` [_code-block_] `}`

[_code-block_]: code_block.md


_documentation_ := `/**` see below `*/`

In reality this is converted by the lexer to a `@doc()`


_effect-list_ := _effect_ __(__ `,` _effect_ __)*__

_effect := __(__ `+` __|__ `-`  __|__ `!` __|__ `=` __)__ [_identifier_]

[_identifier_]: identifier.md


_empty-operator_ :=\
      [_expression_] `empty` [_expression_]

[_expression_]: expression.md


_expression-list_ := [_expression_] __(__ `;` [_expression_] __)*__

[_expression_]: expression.md

> [!NOTE]
> For a discussion on the use of semicolons in expression lists see
> [tokenizer; Semi-colon and comma handling](../tokenizer.md#semi-colon-and-comma-handling). 


_file_ := [_prologue_](prologue.md) [_body_](body.md)


`.`__*__ [_identifier_](identifier.md) __(__ `.` [_identifier_](identifier.md) __)*__


_function-definition_ :=\
    [_attributes_]__*__\
    `fn` `(` [_argument-declaration-list_]__?__ `)` [_function-return-type_]__?__\
    __(__ `expect` `(` [_expression_] `)` __)*__\
    __(__ `pre` `(` [_expression_] `)` __)*__\
    __(__ `post` `(` [_expression_] `)` __)*__ `;`


[_argument-declaration-list_]: argument_declaration_list.md
[_code_block_]: code_block.md
[_function-return-type_]: function_return_type.md
[_fqname_]: fqname.md
[_expression_]: expression.md


_function-definition_ :=\
    [_documentation_]__*__\
    [_attribute_]__*__\
    [_qualifier_]__*__\
    `fn` `(` [_argument-declaration-list_]__?__ `)`
    [_function-return-type_]__?__\
    [_clause_]__*__\
    [_code-block_]


_attribute_ :=\
      `@` `condition` `(` [_condition-expression_] `)`\
    __|__ `@` `deprecated` `(` [_string-literal_] `)`\
    __|__ `@` `discard`\
    __|__ `@` `effect` `(` [_effect-list_] `)`\
    __|__ `@` `no_inline`\
    __|__ `@` `no_return`

_clause_ :=\
      `expect` `(` [_expression_] `)` \
    __|__ `pre` `(` [_expression_] `)`\
    __|__ `post` `(` [_expression_] `)`

_qualifier_ :=\
      `public`\
    __|__ `static`\
    __|__ `export` `(` [_string-literal_] `)`\
    __|__ `metatype`


[_argument-declaration-list_]: argument_declaration_list.md
[_attribute_]: attribute.md
[_clause_]: clause.md
[_code-block_]: code_block.md
[_condition-expression_]: condition_expression.md
[_documentation_]: documentation.md
[_effect-list_]: effect_list.md
[_function-return-type_]: function_return_type.md
[_fqname_]: fqname.md
[_expression_]: expression.md
[_qualifier_]: qualifier.md
[_string-literal_]: string_literal.md


_function-return-type_ :=\
      `->` __(__ `yield` __|__ `await` __)*__ `(` [_type-expression_] `)`\
      `->` __(__ `yield` __|__ `await` __)*__ [_type-tuple-literal_]\
      `->` __(__ `yield` __|__ `await` __)*__ [_fqname_]

[_type-expression_]: type_expression.md
[_type-tuple-literal_]: type_tuple_literal.md
[_fqname_]: fqname.md


Identifiers follow the Unicode UAX #31 (Unicode Identifier and Syntax)
rules for identifiers. Identifiers are case-sensitive. The following characters are added
as part of ID_Start:
 - `_` (UNDERSCORE)
 - `°` (DEGREE SIGN) This is used for units.


_if-control-expression_ :=\
    [_do-clause_]__?__\
    `if` `(` [_init_expression_]__?__ [_condition-expression_] `)` __(__ `{` [_code-block_] `}` __)?__\
    [_alternate-clauses_]


[_alternate-clauses_]: alternate_clauses.md
[_code-block_]: code_black.md
[_condition-expression_]: condition_expression.md
[_do-clause_]: do_clause.md
[_init_expression_]: init_expression.md


`import` `data` _file-path_ `in` _variable-name_ [_build-quard_](build_guard.md)__?__ `;`

file-path := [_string-literal_](string_literal.md)

variable-name := [_relative-fqname_](relative_fqname.md)


`import`
    `eval` [_expression_](expression.md)
    [_build-quard_](build_guard.md)__?__ `;`


`import` `git` _git-url_ *git-rev*__?__
    [_build-quard_](build_guard.md)__?__ `;`

git-url := [_string-literal_](string_literal.md)

git-rev := [_string-literal_](string_literal.md)


`import` `library` _lib-name_
    [_version-literal_](version_literal.md)__?__
    [_build-quard_](build_guard.md)__?__ `;`

lib-name := [_string-literal_](string_literal.md)


_import-module_ := `import` [_relative-fqname_] `public`__?__ [_build-quard_]__?__ `;`

[_build-quard_]: build_guard.md
[_relative-fqname_]: relative_fqname.md


`import` `zip` _zip-url_
    [_build-quard_](build_guard.md)__?__ `;`

zip-url := [_string-literal_](string_literal.md)


_init_expression_ := [_variable_definition_] `;`


__(__ `=` __|__ _inplace-operator_ __)__ [_expression_](expression.md)

### inplace-operator

Any operator that a inplace operator may be used to initialize a variable.
The following inplace operators exist by default:
`+=`, `-=`, `*=`, `/=`, `%=`, `|=`, `&=`, `^=`, `<<=`, `>>=`.


\[+-\]__?__ \[0-9\]__+__ __|__\
\[+-\]__?__ `0` \[bB\] \[0-1\]__+__ __|__\
\[+-\]__?__ `0` \[oO\] \[0-7\]__+__ __|__\
\[+-\]__?__ `0` \[dD\] \[0-9\]__+__ __|__\
\[+-\]__?__ `0` \[xX\] \[0-9a-fA-F\]__+__ __|__


_iterator-for-loop-control-expression_ :=\
    [_do-clause_]__?__\
    `for` `(` [_variable_definition_]__?__ `;`  [_condition_expression_]__?__ `;` [_expression_]__?__ `)` __(__ `{` [_code-block_] `}` __)?__\
    [_alternate-clauses_]


[_alternate-clauses_]: alternate_clauses.md
[_code-block_]: code_block.md
[_condition_expression_]: condition_expression.md
[_do-clause_]: do_clause.md
[_expression_]: expression.md
[_variable_definition_]: variable_definition.md


_label_ := [_identifier_]


_long-form-lambda_ __|__ _short-form-lambda_

### long-form-lambda
`{` *argument-declarations*__?__ [_result-type-declaration_](result_type_declaration.md)__?__\
[_attribute_](attribute.md)__*__
`in` [_statement-list_](statement_list.md) `}`

_argument-declarations_ := `(` [_argument-declaration-list_](argument_declaration_list.md) `)`

### short-form-lambda
`{` [_statement-list_](statement_list.md) `}`


_let-assignment_ := [_left-expression_] `=` [_expression_]

[_left-expression_]: left_expression.md
[_expression_]: expression.md
[_identifier_]: identifier.md


[_documentation_](documentation.md)__?__
`library` [_string-literal_](string_literal.md) [_version-literal_](version_literal.md)
    [_build-guard_](build_guard.md)__?__ `;`


`#line` [_decimal-literal_](decimal_literal.md) __(__ [_string-literal_](string_literal.md) __)__


_base_literal_ [_unit-expression_](unit_expression.md)__?__ [_tag_](tag.md)__*__

### base_literal
[_integer-literal_](integer_literal.md)  __|__\
[_float-literal_](float_literal.md) __|__\
[_string-literal_](string_literal.md) __|__\
[_boolean-literal_](boolean_literal.md) __|__\
[_null-literal_](null_literal.md) __|__\
[_character-literal_](character_literal.md) __|__\
[_version-literal_](version_literal.md) __|__\
[_tuple-literal_](tuple_literal.md) __|__\
[_array-literal_](array_literal.md)


`llvm`　`{` [_bracketed-text_](bracketed_text.md) `}`


[_expression_](expression.md) `and` [_expression_](expression.md) __|__
[_expression_](expression.md) `or` [_expression_](expression.md) __|__
`not` [_expression_](expression.md)


[_documentation_](documentation.md)__?__
`module` [_fqname_](fqname.md)
    [_version-literal_](version_literal.md)__?__
    [_build-guard_](build_guard.md)__?__
    `;`


namespace-directive :=\
      `namespace` [_fqname_] `;`\
    __|__ `namespace` `.`__+__ `;`

[_fqname_]: fqname.md


Operators are the longest sequence of Pattern Syntax characters defined by
Unicode UAX #31 (Unicode Identifier and Syntax). Excluding any characters
that are part of:
 - Brackets


`$` [_integer-literal_](integer_literal.md)


[_documentation_](documentation.md)__?__
`program` [_string_literal_](string_literal.md)
    [_version_literal_](version_literal.md)__?__
    [_build-guard_](build_guard.md)__?__ `;`


_qualifier_ := [_identifier_] __(__ `(` [_argument_list_] `)` __)?__

[_argument_list_]: argument_list.md
[_identifier_]: identifier.md


_ranged-for-loop-control-expression_ :=\
    [_do-clause_]__?__\
    `for` `(` [_left-expression_] `in` [_expression_] `)` __(__ `{` [_code-block_] `}` __)?__ \
    [_alternate-clauses_]


_range-expression_, _start-expression_, _condition-expression_, and
_increment-expression_ are all [_expression_]s.

[_alternate-clauses_]: alternate_clauses.md
[_code-block_]: code-block.md
[_do-clause_]: do_clause.md
[_expression_]: expression.md
[_left-expression_]: left_expression.md


_return-statement_ := `return` [_expression_]__?__ ';'

[_expression_]: expression.md


_statement_ :=\
      [_expression_](expression.md) `;`\
    __|__ [_return_statement_]\
    __|__ [_yield_statement_]\
    __|__ [_break_statement_] \
    __|__ [_continue_statement_]\
    __|__ [_goto_statement_]\
    __|__ [_throw_statement_]\
    __|__ [_namespace_](namespace.md)\


_statement-list_ := __(__ __(__ [_label_](label.md) `:` __)?__ [_statement_](statement.md) __)*__

> [!NOTE]
> For a discussion on the use of semicolons in statement lists see
> [tokenizer; Semicolon and comma handling](../tokenizer.md#semicolon-and-comma-handling). 


`static` __|__ `thread_local`


`(` [_expression-list_](expression_list.md) `)`


_syntax-error-directive_ := `syntax` `error` [_identifier_] `auto_rethrow`__?__


_syntax-operator-directive_ :=\
      `syntax` _associativity_ 
    __(__ [_operator_](operator.md) __|__ [_identifier_](identifier.md) __)__
    _precedence_ _function_name_ `;`

associativity := __(__ `prefix` __|__ `suffix` __|__ `ltr` __|__ `rtl` __)__

precedence := [_integer-literal_](integer_literal.md)

function_name := [_identifier_](identifier.md)


_throw-statement_ := `throw` [_identifier_]__?__ `;`

[_identifier_]: identifier.md


`try` `{` [_statement-list_](statement_list.md) `}` [_catch_clauses_](catch_clauses.md)__?__


`try` [_expression_](expression.md)


_tuple-literal_ :=\
      `(` `,` `)`\
    __|__ `(` [_expression_] `,` `)`\
    __|__ `(` [_expression_] __(__ `,` [_expression_] __)+__ `,`__?__ `)`

[_expression_]: expression.md


_type_definition_ :=\
      _meta_type_ _name_ __(__ `[` [_argument_declaration_list_](argument_declaration_list.md) `]` __)?__ _underlying_**?** [_attributes_]__*__ `{` [_block_](block.md) `}`\
    __|__ _meta_type_ __(__ `[` [_argument_declaration_list_](argument_declaration_list.md) `]` __)?__ _underlying_**?** [_attributes_]__*__ `{` [_block_](block.md) `}`

_underlying_ := `:` [_type_expression_](type_expression.md) __|__ `(` [_type_list_](type_list.md) `)`

_name_ := [_identifier_](identifier.md)

_meta_type_ := [_fqname_](fqname.md)


[_type-expression_](type_expression.md) __(__ `,` [_type_expression_](type_expression.md) __)*__ `,`__?__


_tuple_literal_ :=\
      `(` `,` `)`\
    __|__ `(` [_type-expression_] `,` `)`\
    __|__ `(` [_type-expression_] __(__ `,` [_type-expression_] __)+__ `,`__?__ `)`

[_type-expression_]: type_expression.md


_let-assignment_ := [_left-expression_] `:=` [_expression_]

[_left-expression_]: left_expression.md
[_expression_]: expression.md


_variable-definition_ :=\
    [_attribute_]__*__ [_qualifier_](qualifier.md)__*__\
    [_fqname_](fqname.md)\
    __(__ `[` [_argument_declaration_list_](argument_declaration_list.md) `]` __)?__
    _type-declaration_**?** [_initializer_](initializer.md)__?__


[_attribute_]: attribute.md


_qualifier_ :=\
      `thread_local`\
    __|__ `static`\
    __|__ `shared` __(__ `(` [_expression_](expression.md) `)` __)__\
    __|__ `weak`\
    __|__ `unique` __(__ `(` [_expression_](expression.md) `)` __)__\
    __|__ `alloc` __(__ `(` [_expression_](expression.md) `)` __)__\
    __|__ `public`\
    __|__ `section` `(` [_expression_](expression.md) `)`\
    __|__ `alignas` `(`[_expression_](expression.md) `)`


### type_declaration

_type-declaration_ __:=__ [_coerce-operator_](coerce_operator.md) [_type-expression_](type_expression.md)


\[0-9\]__+__ `.` __(__ \[0-9\]__+__ __|__ `*` __)__ `.` __(__ \[0-9\]__+__ __|__ `*` __)__


_while-control-expression_ :=\
    [_do-clause_]__?__\
    `while` `(` [_init_expression_]__?__ [_condition-expression_] `)` __(__ `{` [_code-block_] `}` __)?__\
    [_alternate-clauses_]


[_alternate-clauses_]: alternate_clauses.md
[_code-block_]: code_block.md
[_condition-expression_]: condition_expression.md
[_do-clause_]: do_clause.md
[_init_expression_]: init_expression.md


[_alternate-clauses_]: alternate_clauses.md
[_argument-declaration-list_]: argument_declaration_list.md
[_argument-declaration_]: argument_declaration.md
[_argument-list_]: argument_list.md
[_argument_]: argument.md
[_argument_list_]: argument_list.md
[_attribute_]: attribute.md
[_attributes_]: attributes.md
[_binding-selector_]: binding_selector.md
[_boolean-literal_]: boolean_lite
[_build-guard-expression_]: build_guard_expression.md
[_build-quard_]: build_guard.md
[_catch-expression_]: catch_expression.md
[_catch-operator_]: catch_operator.md
[_clause_]: clause.md
[_code-block_]: code_block.md
[_code_block_]: code_block.md
[_coerce-operator_]: coerce_operator.md
[_condition-expression_]: condition_expression.md
[_condition_expression_]: condition_expression.md
[_directive_]: directive.md
[_do-clause_]: do_clause.md
[_documentation_]: documentation.md
[_effect-list_]: effect_list.md
[_error-list_]: error_list.md
[_expression-list_]: expression_list.md
[_expression_]: expression.md
[_float-literal_]: float_literal.md
[_for-control-expression_]: for_control_expression.md
[_fqname_]: fqname.md
[_function-definition_]: function_definition.md
[_function-return-type_]: function_return_type.md
[_identifier_]: identifier.md
[_if-control-expression_]: if_control_expression.md
[_import-git_]: import_git.md
[_import-lib_]: import_lib.md
[_import-module_]: import_module.md
[_import-zip_]: import_zip.md
[_init_expression_]: init_expression.md
[_integer-literal_]: integer_literal.md
[_label_]: label.md
[_left-expression_]: left_expression.md
[_library-declaration_]: library_declaration.md
[_literal_]: literal.md
[_module-declaration_]: module_declaration.md
[_name_]: name.md
[_program-declaration_]: program_declaration.md
[_qualifier_]: qualifier.md
[_relative-fqname_]: relative_fqname.md
[_shortcut-operator_]: shortcut_operator.md
[_simple-unit-expression_]: unit_expression.md
[_statement-list]: statement_list.md
[_statement-list_]: statement_list.md
[_statement_]: statement.md
[_string-literal_]: string_literal.md
[_syntax-effect_]: syntax_effect.md
[_try-operator_]: try_operator.md
[_tuple-literal_]: tuple_literal.md
[_type-annotation_]: type_annotation.md
[_type-expression_]: type_expression.md
[_type-tuple-literal_]: type_tuple_literal.md
[_unit-expression_]: unit_expression.md
[_variable-definition_]: variable_definition.md
[_variable_definition_]: variable_definition.md
[_version-literal_]: version_literal.md
[_while-control-expression_]: while_control_expression.md
[_with_effect_]: with_effect.md
