# Object and Lifetime Semantics


## Concepts

 * **Object** - An instance of a type with storage and a lifetime. An object may
   be associated with a named variable or created by materializing a temporary.
 * **Value** - The data or state represented by an object, or produced by
   evaluating an  expression. A value has no independent storage, identity, or
   lifetime. An object may own resources associated with its value.


## Temporaries

A temporary is a value produced by an expression rather than accessed through an
existing binding. Its initial expression type is `T`.

A temporary can initialize a variable binding directly and can be consumed
without an explicit move operator. When a reference binding requires
materialization, the temporary is materialized in the code-block scope where the
expression requiring the reference is evaluated.

The materialized temporary remains alive until that scope exits. A reference to
it is valid only while the temporary's object remains alive; binding a
reference does not extend the temporary's lifetime.

```text
r <- &make_value()    // Materializes the result in the current scope.
use(r)                // Valid while that scope remains active.
```

When the return-binding conversion requires materialization, the temporary is
materialized in the caller's code-block. This applies even when an explicit
binding operator changes the effective expression type to a reference type, as
in `return &make_value()`. The binding operator does not itself materialize the
temporary; the return-binding conversion determines whether materialization
occurs.

This rule does not apply to a reference to an object that already exists in the
callee's code-block, such as a local variable. Returning a reference to an
object whose lifetime ends when the callee's code-block exits is invalid.

Temporaries therefore support direct initialization and consumption while
allowing references to them only for the lifetime of their materialized
object.

## Lifetime

An object has a lifetime that begins when it is defined and ends when its
lifetime-ending conditions are met. The object's destructor is called directly
when its lifetime ends. Values do not have independent lifetimes; they are
represented by objects or produced by evaluating expressions.

The lifetime of a local object begins at the point of its variable definition
and ends at the end of its enclosing code-block. When a code-block exits, its
local objects are destroyed in reverse order of their definitions. The lifetime
of a global object begins when its definition is processed during compilation.
Its initialized state and lifetime are transferred to the generated executable,
where its lifetime ends at program termination. Global objects are destroyed in
reverse order of their definitions.

Updating an existing variable does not, by itself, end the lifetime of its
associated object. The `<-` operator calls `__assign__()`, while `=` calls
`__merge__()`. These operations determine how the object's value is changed;
their implementations are responsible for managing the lifetimes of any objects
they replace or destroy.

References do not extend the lifetimes of the objects they refer to. A reference
is valid only while its referent remains alive. Reseating a reference does not
change the lifetime of either the previously referenced object or the newly
referenced object.

A temporary is a value produced by an expression rather than accessed through an
existing binding. A temporary value can initialize a destination object directly
without creating a separate temporary object. When a reference binding requires
materialization, an anonymous object is created and initialized with the
temporary value, as if it were an anonymous variable defined in the code-block
where the expression requiring materialization is evaluated. The object remains
alive until that code-block exits, and its destructor is called when its
lifetime ends.

Temporaries materialized while evaluating function arguments belong to the
code-block where the function call occurs. The argument list does not introduce
a separate scope. When a function's return binding requires materializing a
temporary value, the object is materialized in the code-block containing the
function call. However, if the return expression produces a reference to a
temporary object created in the callee's code-block, returning that reference is
invalid because the object's lifetime ends when the callee's code-block exits.

When the compiler can determine that a reference is used after the lifetime of
its referent has ended, compilation fails.

