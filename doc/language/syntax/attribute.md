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

  @construct          | This function will be called during start of the program
  @destruct           | This function will be called during exit of the program


#### @abi(string)

Use a specific abi for this function.

  abi      | Description
 :-------- |:---------------
  `"c"`    | Use the C ABI.
  `"c++"`  | Use the C++ ABI.


#### @class

This function is a class memember function, instead of instance member function. 


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

See [block.effects](#block-effects).


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
Meaning code after this function call will never execute.

### Code Block


#### @effects(id...) <a id="block-effects"></a> 

id... := _effect_ __(__ `,` _effect_ __)*__

_effect_ := __(__ `+` __|__ `-` __)__ [_identifier_]

Add or remove effects from this code-block. The [_identifier_]s for effects
are added using the [_syntax-effect_].

Functions that call a function that has an effect, inherits that effect.
An effect like this can be removed using the `-` [_identifier_] syntax.

The default set of effects are:

  Effect    | Description
 :--------  |:--------------
  io        | May performs I/O.
  block     | May cause progress to stop on this thread.
  allocate  | May allocate and free memory.
  unsafe    | May cause unexpected behavior.
  terminate | May cause the end of the thread or program.


#### @fast_path

Used on a block of a flow-control expression, indicating the path to optimize.
This will:
 * Order code to reduce latency for this fast-path.
 * Prefer calling, instead of inlining, on the slow-path.


#### @slow_path

Used on a block of a flow-control expression, indicating the path to not
optimize. This is the opposite of `@fast_path`.


#### @without_effects(id...)

id... := [_identifier_] __(__ `,` [_identifier_] __)*__

Make sure none of the effects listed will be executed in this block.



### Variable



[_code-block_]: code_block.md
[_function-definition_]: function_definition.md
[_variable-definition_]: variable_definition.md


