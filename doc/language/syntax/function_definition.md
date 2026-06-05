# function-definition

## Syntax

_function-definition_ :=\
    [_documentation_]__*__\
    [_attribute_]__*__\
    [_qualifier_]__*__\
    `fn` [_fqname_]__?__ `(` [_argument-declaration-list_]__?__ `)` [_function-return-type_]__?__\
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


## Semantics

### export(abi: string)

The function will be available in the executable using the `abi`
specified.

  abi      | Description
 :-------- |:---------------
  `"c"`    | Use the C ABI.
  `"c++"`  | Use the C++ ABI.


### metatype

This function's [_fqname_] is added to type-definition keyword list.

This function is called when a type-definition is found. The arguments
to this functions are:

 * The list of type attributes
 * The template argument list
 * The type-inheritance list
 * The [_code-block_]

It returns a function that in-turn returns an actual type, which is
added to the overload-set as a type template.

### static

This function is a class memember function, instead of instance member function. 


### expect(expression)

Although not part of the signature, the [_expression_] is checked during the
overload resolution. The overload is dropped without error if the expression
result is false.


### post(expression)

Post-condition is checked after calling the function, inside the caller.

available:
  * The arguments by name from the argument list
  * The arguments by index using the `$` [_integer_literal_]
  * The return variable using `$?`.


### pre(expression)

Pre-condition is checked before calling the function, inside the caller.

available:
  * The arguments by name from the argument list
  * The arguments by index using the `$` [_integer_literal_]


## Attributes

A function definitions moves the list of attributes from the parser context into
the function's AST, when `fn` is parsed.



### @condition(expr)

Only compile this function when the [_condition-expression_] is true.


### @deprecated(message: string)

This function is deprecated. The compiler will emit a warning message
at the call site, including the `message` passed in the attribute.


### @discard

The function's return value maybe discarded.


### @effects(effect-list)

Add, Remove and Check for effects to be available in the function.


### @no_inline

The code of this function will not be inlined into the caller. This can possibly:

 * Reduces code size of the caller
 * Reduces register pressure
 * In turn improve chance for the caller to be inline into its caller.


### @no_return

This function will not return, used for functions like `std.terminate()`.
Meaning code after this function call will never execute.



