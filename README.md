# hikolang

 * type inference
 * elaboration phase
 * language syntax is extendable
 * inline assembler
 * compile time reflection, types are values.
 * hidden dependency injection, both functions and structs.
 * ranged integer types.
 * enum only has named-values, it is not an integer.
 * function overloading, on both arguments (type and non-type) and return type.
 * universal call syntax.

## Elaboration Phase
Certain languages have a separate elaboration phase during compilation.

In this phase the compiler will fill in all:
 * type and non-type arguments of templates.
 * global variables.
 * Guaranteed full constant folding and function elimination.

The process basically does full constant folding by executing functions until no
more functions can be executed.

It is a reportable error if any expression is left of which the value needs to
be known for the next compilation phase.

Note: all functions by default can be executed at compile time. 
Note: full constant folding allows generic/templated functions without a special syntax.
Note: since low level functions are written as inline-assembly, inline-assembly must be
      executable during compile time, possibly using an interpreter/JIT.

## Hidden dependency injection
### Function calls
On top of normal arguments, functions also include hidden arguments called dependencies,
which are accessed through the `deps` object. The type of the `deps` object is implementation defined.
The `deps` object is a struct-like object containing named-dependencies. Each named-dependency is a
reference to an object in memory.

When calling a function, the named dependencies in the `deps` object of the current function are copied
automatically into the `deps` object of the called function. There is a special syntax to explicitly pass
dependencies in a function call. It is a reportable error when one or more dependencies that a function
requires is not passed.

The compiler will automatically track which dependencies a function requires based on:
 * The names that are accessed through the `deps` object.
 * Recursively the names that are used by other functions that are called.
 * Excluding the names that are explicitly passed to another function.

It is a reportable error if the type of a named-dependency is different between the caller and callee, when
this named-dependency is automatically copied.

### Structs
Structs may also have hidden dependencies. The reference to the 'vtable' for example is implemented as a
hidden dependency. Another idea is to add a reference to an 'allocator' an allocator that is passed through
`deps` would allow child objects to be automatically allocated with the same allocator.

A constructor would explicitly, but hiddenly, set the 'vtable' hidden dependency of an object, it will not
be automatically copied from `deps` of the constructor. However the 'allocator' would be copied from `deps`
to `deps` into the object automatically.

## Inline assembly
You can create functions with inline assembly using the `asmfunc` keyword. The syntax within the
braces are compatible with the assembly of the host compiler. Here is an example when using
LLVM as a host compiler.

```
asmfunc mul(arg1 : double, arg2 : double) -> double
{
    %tmp = fadd double %arg1, %arg2
    ret double %tmp
}
```

Low level types can also be made in assembly:

```
asmtype complex {
    double r
    double i
}
```

note: Extensible syntax allows the syntax be different from assembly and other parts.

## Universal call syntax

Functions may be called in two different ways:
 * `foo(a, b)`
 * `a.foo(b)`

