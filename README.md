# hikolang

 * type inference
 * elaboration phase
 * language syntax is extendable
 * inline assembler
 * compile time reflection, types are values.
 * hidden dependency injection, both functions and structs.
 * native ranged arbitrary sized integer type.
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

## Extensible syntax
### Custom operators
The language allows you to define custom operators. This is done by registering a
a keyword or pattern-syntax, arity, precedence and associativity, and a function that
will be called when the operator is used.

### Custom literals
The language allows you to define custom literals. This is done by registering a
suffix-keyword and a function that will be called when the suffix is used with a literal.

The literal is passed to the function as a string, and the function determines the
returned value and type. Since the function is called at compile time, it can
dynamically create both the value and the type.

## Inline assembly
Inline assembly is supported through the `llvm` directive. This allows you to write low-level
LLVM assembly code directly in your source code. The inline assembly may even be executed during
the elaboration phase.

```
function foo(a : __i32__, b : __i32__) -> __i32__
{
    var c = a &+ b
    var r = 0 : __i32__

    llvm {
        %tmp = mul i32 %c, %a
        store i32 %tmp, i32* %r.addr
    }

    return r
}
```

## Compile time reflection
The language treats types as if they are first-class values.
This means types themselves are of a meta-type, recursively.

Since types are values, you can interrigate and manipulate types at compile time.
This allows you to write generic code that can work with any type, and even
create new types at compile time.

Meta-types are defined as built-in, by the standard library, and can be extended
by the user.


## Hidden dependency injection
### Function calls
On top of normal arguments, functions also include hidden arguments called dependencies.
Dependencies are identifiers prefixed with `$`, such as `$foo`. A function that uses
a dependency automatically declares that dependency as a hidden argument recursively through
the function call chain.

Explicitly a function may be called with a dependency by using the syntax `$foo = variable`. This
dependency is then passed to each function in the call chain that requires this dependency. It
also breaks the requirement for this dependency toward the caller. A dependency is always a
reference to a variable, it is not possible to pass a value as a dependency.

```
function foo(a)
{
    return a + $injected_variable
}

function bar(a)
{
    return foo(a)
}

function main()
{
    var b = 10;
    return bar(42, $injected_variable = b)
}
```

### Structs
I am hoping to implement vtable and allocator, and possible other special pointers as
hidden depedencies as well. This would allow an `$allocator` to be implicitly passed to
constructors called from a member-function. `$this` could be used as pointer to
object/vtable.

Another one of those hidden dependencies could be a mutex, maybe even passed to child
objects like the allocator. I would also like a synchronize attribute on methods so
that the mutex is automatically acquired and released when the method is called.
But I want this synchronize attribute to be generic also for other things that
can be acquired and released.

I am not done thinking about this yet.

## Ranged arbitrary sized integers
The language and standard library natively supports ranged arbitrary sized integers.

Using type inference, the compiler will automatically select the appropriate integer type
for the range of the value. For most operations this will mean that it will
not be possible to overflow the value. Certain operations like AND and modulo operations
allows wrapping behavior to reduce the range of the value.

Long integers are stored in the native endianness of the target architecture. Integers
can be both unsigned or 2's complement signed. Small integers are stored in the smallest
possible type, including, 64, 32, 16, 8 and 1 bit (CPU flag) integers. This means that
they can be used predictably in FFI calls.

Most of the functionality is implemented in the standard library, except that the default
integer literal returns a ranged integer, through the standard library.

## Universal call syntax

Functions and member functions may be called in two different ways:
 * `foo(a, b)`
 * `a.foo(b)`

## Extending syntax
There is macro system that allows you to match with tokens and replace it with a different expression.

