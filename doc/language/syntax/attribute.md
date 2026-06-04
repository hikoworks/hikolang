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


