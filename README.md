# hikolang

 * type inference
 * everything is constexpr / elaboration phase
 * language syntax is extendable
 * inline assembler LLVM
 * compile time reflection
 * hidden dependency injection, both functions and structs.

## Hidden dependency injection
### Function calls
On top of normal arguments, functions also include hidden arguments called dependencies,
which are accessed through the `deps` object. The type of the `deps` object is implementation defined.

When calling a function, the named dependencies in the `deps` object of the current function are copied
automatically into the `deps` object of the called function. There is a special syntax to explicitly pass
dependencies in a function call. It is a reportable error when one or more dependencies that a function
requires is not passed.

The compiler will automatically track which dependencies a function requires based on:
 * The names that are accessed through the `deps` object.
 * Recursively the names that are used by other functions that are called.
 * Excluding the names that are explicitly passed to another function.

It is a reportable error if the type of a named-dependency is different between the caller and callee, when
this named-dependenct=y is automatically copied.

### Structs
Structs may also have hidden dependencies. vtable, allocator.
