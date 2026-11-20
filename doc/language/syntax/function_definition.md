# function-definition

[_attribute_]: attribute.md
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
[_integer-literal_]: integer_literal.md
[_expression_]: expression.md
[_qualifier_]: qualifier.md
[_string-literal_]: string_literal.md
[_positional-argument_]: positional_argument.md


## Syntax

_function-definition_ :=\
    `fn` [_fqname_]__?__ __(__ `(` [_argument-declaration-list_]__?__ `)` __)?__
      [_function-return-type_]__?__\
    *constraint*__*__\
    _specifier_

_constraint_ :=\
      `expect` `(` [_expression_] `)` \
    __|__ `build_guard` `(` [_build-guard-expression_] `)`\
    __|__ [_attribute_]__*__ `pre` `(` [_expression_] `)`\
    __|__ [_attribute_]__*__ `post` `(` [_expression_] `)`

_specifier_ :=\
      [_attribute_]__*__ [_code-block_]\
    __|__ `=` `default` `;`\
    __|__ `=` `delete` `;`\
    __|__ `=` `extern` `;`\
    __|__ `=` `abstract` `;`\
    __|__ `=` `builtin` `;`


## Semantics


### argument declaration

Inside a function, type or lambdas, the argument declarations are available as
variables as:
 - by their name,
 - positional arguments: `$0`, `$1`, etc,
 - positional arguments: `$[` [_integer-literal_] `]`,
 - hidden context arguments: `$`[_id_]
 - the number of arguments: `$#`,
 - result value as: `$_`


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

When the post-condition is evaluated as `false` at run-time it will throw
the `std.error_code.post_condition_failure` error or abort if uncaught.

### pre(expression)

Pre-condition is checked before calling the function, inside the caller.

available:
  * The arguments by name from the argument list
  * The arguments by index using the `$` [_integer_literal_]

When the pre-condition is evaluated as `false` at run-time it will throw
the `std.error_code.pre_condition_failure` error or abort if uncaught.

> [!note]
> If the compiler can proof the pre-condition then it may report and error or
> remove the check from run-time.

> [!note]
> The `@effect()` attribute on the `pre()` and `@post` clause will be evaluated
> if the condition cannot be proven to be `true`.

> [!note]
> The `@optional` attribute on the `pre()` and `post()` clause will be cause
> the check to be removed from the run-time for a *release* build. An error
> is reported if the condition is proven to be `false` at compile time.
