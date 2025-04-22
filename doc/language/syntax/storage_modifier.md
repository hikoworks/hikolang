# storage-modifier

## Syntax

`let` __|__ `var` __|__ `ref` __|__ `static` __|__ `thread_local`

## Semantics
The storage-modifier is used to specify the storage class of a variable or argument.

Storage-modifiers are mutually exclusive. The following storage-modifiers exist:
 - `var` : A mutable variable, with copy semantics.
 - `let` : A immutable variable, with copy semantics.
 - `ref` : An alias to a variable or temporary. Mutability is determined by the
          variable or temporary it refers to.
 - `static` : A mutable variable, with copy semantics, which is shared between invocations
              of a function or lambda, or shared between instances of a class.
 - `thread_local` : A mutable variable unique to the thread.

### var
A variable declared with `var` is mutable. It may appear in the following contexts:
 - In the declaration of a global variable.
 - In the declaration of an automatic variable in a function or lambda.
 - In the declaration of an instance member variable in a class.
 - In the argument declaration of a function or lambda.

A variable declared with `var` must be initialized before it is read. Initialization
can occur at the point of declaration, or in all code paths before the variable
is read.

The value of the variable can be changed. When the variable is changed, the
original value is copied, or potentially moved, to the new value.

It is recommended to use `let` over `var` unless the variable needs to be mutable.
The compiler will issue a warning if `var` is used when `let` would suffice.

### let
A variable declared with `let` is immutable. It may appear in the following contexts:
 - In the declaration of a global variable.
 - In the declaration of an automatic variable in a function or lambda.
 - In the argument declaration of a function or lambda.

A variable declared with `var` must be initialized before it is read. Initialization
can occur at the point of declaration, or in all code paths before the variable
is read.

The value of the variable cannot be changed. If an attempt is made to change the
variable, the compiler will issue an error.

### ref
A variable declared with `ref` is an alias to a variable or temporary-object.
It may appear in the following contexts:
 - In the declaration of a global variable.
 - In the declaration of an automatic variable in a function or lambda.
 - In the argument declaration of a function or lambda.

A variable declared with `ref` must be initialized at the point of declaration.
The reference itself cannot be changed. Any read or write to a reference will
be applied to the variable or temporary-object it refers to.

If the original variable is immutable, then a write to the reference will
result in a compiler error.

A ref for a function argument is used as a in/out parameter.

### static
A variable declared with `static` is mutable, and is shared between
invocations of a function or instances of a class. It may appear in the
following contexts:
 - In the declaration of an static variable in a function or lambda.
 - In the declaration of an instance member variable in a class.

In a function a `static` variable is initialized at the declaration.
When a function is called the first time, the variable is initialized
thread-savely exactly once. The initialization is done in the context
of the function, and can access the function's arguments and local variables.
The variable is destroyed when the program exits.

### thread_local
A variable declared with `thread_local` is mutable, and is shared between
invocations of a function or instances of a class within the same thread.
It may appear in the following contexts:
 - In the declaration of an static variable in a function or lambda.
 - In the declaration of an instance member variable in a class.
 - In the declaration of a global variable.

In a function a `thread_local` variable is initialized at the declaration.
When a function is called the first time on the current thread, the variable
is initialized once. The initialization is done in the context of the function,
and can access the function's arguments and local variables.
The variable is destroyed when the thread exits.

