# function-definition

## Syntax

_function-definition_ := \
      `fn` [_fqname_](fqname.md) `(` [_argument-declaration-list_](argument_declaration_list.md) `)` *attribute*__*__ `{` [_code_block_](code_block.md) `}`\
    __|__ `fn` `(` [_argument-declaration-list_](argument_declaration_list.md) `)` *attribute*__*__ `{` [_code_block_](code_block.md) `}`


## Semantics


### abi(x)
Make this function available for linking with an application written in a
different language; using a different ABI (Application Binary Interface).



### no_return
This function will not return.

An implementation must terminate the application if the function does
return. Or proof that the function will not return.

### meta_type

This function is used to add a type-template to the type overload set.

A function that has the `meta_type` must have a name, as it is needed
during parsing, before elaboration takes place. Therefor a `meta_type`
function can't be anonymous.

This function has as arguments:
`(name: fqname?, arguments: function_arguments, underlying : array[fqname], definition: type_definition)`



### pre(expression)
Pre-condition is checked before calling the function.


### post(expression)
Post-condition is checked after calling the function.

A post-condition on a type will run after an object is modified.


### constrain(expression)
A condition that is checked when matching if the template arguments
are valid for this specific overload in the overload-set.

Constrain can also contain guard conditions.


### std.no_inline
Calls to this function are never inlined.

This is a strong optimization lever:
 - It reduces code size by not inlining the code, which will reduce
   instruction-cache usage.
 - It significantly reduce register pressure, which will reduce the
   amount of reads/writes to memory. And may in turn reduce size of the
   code; which in turn may allow more inlining.


### std.depricated, std.depricated(msg)
This function is depricated print a warning. A reason for deprication can
be passed as a constant-string.


### std.discard, std.discard(msg)
The return value of this function is allowed to be discarded by the caller.

Discarding a return value is:
 - The return value is not assigned to a variable.
 - The return value is not passed to a function.
 - The return value is not part of a larger expression.
 - The return value is not used as an expression to a control-expression. 

If this attribute isn't set on a function, the compiler must give a warning
when the function's return value is discarded.
