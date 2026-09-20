# annotation

## Syntax

annotation :=\
      [_documentation_]\
    __|__ _attribute_\
    __|__ _modifier_


_attribute_ := `[[` [_fqname_] __(__ `(` [_argument-list_] `)` __)?__ `]]`

_modifier_ := `@` [_fqname_] __(__ `(` [_argument-list_] `)` __)?__

[_argument-list_]: argument_list.md
[_fqname_]: fqname.md
[_code-block_]: code_block.md
[_documentation_]: documentation.md
[_expression_]: expression.md
[_function-definition_]: function_definition.md
[_variable-definition_]: variable_definition.md


## Semantics

Attributes apear in front of different syntactical constructs:

 * [_function-definition_]
 * [_expression_]
 * [_code-block_]
 * [_variable-definition_]

### call-operator: [[call]] 

Use a `call` instruction; do not inline the function being called.


### code-block: [[effects(id...)]]

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


### code-block: [[fast_path]]

Used on a block of a flow-control expression, indicating the path to optimize.
This will:
 * Order code to reduce latency for this fast-path.
 * Prefer calling, instead of inlining, on the slow-path.


### code-block: [[slow_path]]

Used on a block of a flow-control expression, indicating the path to not
optimize. This is the opposite of `@fast_path`.


### code-block: [[without_effects(id...)]]

id... := [_identifier_] __(__ `,` [_identifier_] __)*__

Make sure none of the effects listed will be executed in this block.


### type definition code-block: [[abi(x)]]

On a type this will change how the memory layout is for an object.
 
 abi         | Description
 :---------- | :---------
 `c`         | Use the C ABI, keeping members aligned and in original order.
 `reorder`   | Reorder members from large to small struct, maintaining alignment. (default)
 `pack`      | Keep members in original order, but ignore alignment. Access to members is valid.
 `compress`  | Use niche-mask to further compress members in a struct. Access to members may require shifts and masks. 


### definition: [[doc(str)]]

See : [_documentation_](documentation.md)


### function: [[deprecated(message: string)]]

This function is deprecated. The compiler will emit a warning message
at the call site, including the `message` passed in the attribute.


### function: [[discard]]

The function's return value maybe discarded.


### function: [[effects(effect-list)]]

Add, Remove and Check for effects to be available in the function.


### function: [[no_return]]

This function will not return, used for functions like `std.terminate()`.
Meaning code after this function call will never execute.
