# function-definition

[_argument-declaration-list_]: argument_declaration_list.md
[_attribute_]: attribute.md
[_build-guard-expression_]: build_guard_expression.md
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
[_positional-argument_]: positional_argument.md


## Syntax

_function-definition_ :=\
    `fn` __(__ `(` [_argument-declaration-list_]__?__ `)` __)?__
      [_function-return-type_]__?__\
    *constraint*__*__\
    _specifier_

_constraint_ :=\
      `expect` `(` [_expression_] `)` \
    __|__ `build_guard` `(` [_build-guard-expression_] `)`\
    __|__ `pre` `(` [_expression_] `)`\
    __|__ `post` `(` [_expression_] `)`

_specifier_ :=\
      [_code-block_]\
    __|__ `=` `default` `;`\
    __|__ `=` `delete` `;`\
    __|__ `=` `extern` `;`\
    __|__ `=` `abstract` `;`\
    __|__ `=` `builtin` `;`


## Semantics

### argument declaration

### lambda

A lambda is just a function definition; most of a function definition is
optional, including the argument declaration and return type declaration. The
function simply becomes a templated function with variable number of arguments.

Inside the function you have access to the [_positional-argument_]s.

```
a = foo(fn { $0 + $1 })
```




### build_guard(expression)

Conditional compilation of the function.


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




## annotations

A function definition consumes the pending-annotations and apply it to the function
definition.

### @export(abi: string)

The function will be available in the executable using the `abi`
specified.

  abi      | Description
 :-------- |:---------------
  `"c"`    | Use the C ABI.
  `"c++"`  | Use the C++ ABI.


### @metatype

This function's [_fqname_] is added to type-definition keyword list.

This function is called when a type-definition is found. The arguments
to this functions are:

 * The list of type attributes
 * The template argument list
 * The type-inheritance list
 * The [_code-block_]

It returns a function that in-turn returns an actual type, which is
added to the overload-set as a type template.


### @static

This function is a class memember function, instead of instance member function.


### @public
