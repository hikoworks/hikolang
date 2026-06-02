# function-definition

## Syntax

_function-definition_ :=\
      [_attributes_]__*__
      `fn` [_fqname_]__?__ `(` [_argument-declaration-list_]__?__ `)`\
      _function_return_type_**?** `{` [_code_block_] `}`

_function-return-type_ := `->` __(__ `yield` __|__ `await` __)*__ [_expression_]

[_argument-declaration-list_]: argument_declaration_list.md
[_code_block_]: code_block.md
[_fqname_]: fqname.md
[_expression_]: expression.md
[_attributes_]: attributes.md


## Semantics

### Attributes

A function definitions moves the list of attributes from the parser context into
the function's AST, when `fn` is parsed.

The following attributes are supported by function definitions:

  attribute           | description
 :------------------- | :------------------------------------------------------
  @doc(string)        | Set the documentation of the function (must be the first attribute)
  @no_inline          | Do not inline the code of the function into the caller.
  @no_return          | The function does not return.
  @deprecated(string) | This function is deprectated, argument is a message.
  @discard            | The return value may be discarded by the caller.
  @abi(string)        | The ABI to use for this function.
  @metatype           | The function adds the fqname of the function as a type keyword.
  @construct          | This function will be called during start of the program
  @destruct           | This function will be called during exit of the program
  @condition(expr)    | This function is conditionally compiled.
  @effects(id list)   | Add/Remove/Set effects on this function.
  @extern             | This function declaration will link against a function external to the program (in a library)



#### pre(expression)
Pre-condition is checked before calling the function.


#### post(expression)
Post-condition is checked after calling the function.

A post-condition on a type will run after an object is modified.


#### constrain(expression)
A condition that is checked when matching if the template arguments
are valid for this specific overload in the overload-set.

Constrain can also contain guard conditions.

