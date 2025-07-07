# storage-class

## Syntax

`static` __|__ `thread_local`

## Semantics

 - `static` : A mutable variable, with copy semantics, which is shared between
              invocations of a function or lambda, or shared between instances
              of a class.
 - `thread_local` : A mutable variable unique to the thread.

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

