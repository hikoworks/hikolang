# attributes

## Syntax

[_fqname_](fqname.md) __(__ `(` [_argument_list_](argument_list.md) `)` __)?__


## Semantics

If the [_fqname_](fqname.md) is a single name,
then this attribute is required. A required attribute must be handled by the compiler.
If the compiler does not know how to handle the attribute it is an error.

If the [_fqname_](fqname.md) has the `std.` prefix
it is a well-known optional attribute. These attributes don't change the semantic
meaning of the program, instead they are directives to the compiler to create
better code.

Other prefixes are attributes that are handled by specific compilers. Compilers
should ignore any attributes that do not match their own prefix.

### abi(x)
Make this function available for linking with an application written in a
different language; using a different ABI (Application Binary Interface).

On a type this will change how the memory layout is for an object.

 - "c": Use the C ABI. (same as aligned)
 - "c++": Use the C++ ABI. (same as aligned)
 - "aligned": Keep members in a struct in given order, and align members.
 - "reorder": Reorder members in a struct, pack while maintaining alignment. (default)
 - "pack": Keep members in a struct in given order, ignore natural alignment of members. This will
           cause member access to use possible slower, but valid, unaligned access.
 - "compress": Use niche-mask to further compress members in a struct. Access to members may
               require shifts and masks. 

### no_return
This function will not return.

An implementation must terminate the application if the function does
return. Or proof that the function will not return.

### pre()
Pre-condition is checked before calling the function.

### post()
Post-condition is checked after calling the function.

A post-condition on a type will run after an object is modified.


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

