# attributes

## Syntax

_attribute_ := `@` [_identifier_] __(__ `(` [_argument_list_] `)` __)?__

[_argument_list_]: argument_list.md
[_identifier_]: identifier.md


## Semantics

Attributes apear in front of different syntactical constructs:

 * [_function-definition_]
 * [_code-block_]
 * [_variable-definition_]


### Code Block Attributes

### Function Attributes

A function will consume the attributes early on, directly at the `fn` token.

  @no_return          | The function does not return.
  @construct          | This function will be called during start of the program
  @destruct           | This function will be called during exit of the program


#### @abi(string)

Use a specific abi for this function.

  abi      | Description
 :-------- |:---------------
  `"c"`    | Use the C ABI.
  `"c++"`  | Use the C++ ABI.


#### @condition(expr)

Only compile this function when the [_condition-expression_] is true.


#### @deprecated(message: string)

This function is deprecated. The compiler will emit a warning message
at the call site, including the `message` passed in the attribute.


#### @discard

The function's return value maybe discarded.


#### @doc(string)

Add documentation to this function. 

An `@doc()` attribute must be the first of a set of attributes.


#### @effects(id...)

_effects_ := _effect_ __(__ `,` _effect_ __)*__

_effect_ := __(__ `+` __|__ `-` __)__ [_identifier_]

Add or remove effects from a function. The [_identifier_]s for effects
are added using the [_syntax-effect_].

Functions that call a function that has an effect, inherits that effect.
An effect like this can be removed using the `-` [_identifier_] syntax.


#### @metadata

This function's [_fqname_] is added to type-definition keyword list.

This function is called when a type-definition is found. The arguments
to this functions are:

 * The list of type attributes
 * The template argument list
 * The type-inheritance list
 * The [_code-block_]

It returns a function that in-turn returns an actual type, which is
added to the overload-set as a type template.


#### @no_inline

The code of this function will not be inlined into the caller. This can possibly:

 * Reduces code size of the caller
 * Reduces register pressure
 * In turn improve chance for the caller to be inline into its caller.


#### @no_return

This function will not return, used for functions like `std.terminate()`.



### Variable Attributes




[_code-block_]: code_block.md
[_function-definition_]: function_definition.md
[_variable-definition_]: variable_definition.md


