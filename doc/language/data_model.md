# data model

This chapter describes how functions, types, templates and values work in this
language.

Here are a couple of rules in this language:
 - Types are first class values, and can be passed to functions or type templates.
 - All function and type definitions are always templates.
 - A type definition is an alternate representation of a function that returns a type.
 - Multiple type and function definition with the same name form a overload set.
 - Type and function definitions may be modified until the whole overload set is frozen.
 - An overload set is frozen when a type or function is instantiated.
 - Functions and types are lazilly evaluated to delay freezing; functions and
   global variables are recursively evaluated when they are exported, after
   processing the last statement of a file.

## Templates

All function-, type- and (global) variables-definitions are templates. One or more
definitions with different signatures with the same name form a overload-set.

Each of the following functions form the `foo` overload-set:
```
fn foo(x: int) { return x + 42 }
fn foo(x) { return x + 1 }
fn foo() { return 0 }
```

Both of the following type-templates form the `bar` overload-set:
```
struct bar[T: type] { var x : T }
struct bar { var x : int }
```

Both of the following global variable-templates form the `qux` overload-set.
`qux_init()` is evaluated at compile time if possible:
```
var[T: type] qux : T = qux_init(T)
var qux : int = qux_init(int)
```

An overload set is lazilly evaluated, which means they may be modified after the
definition. This makes it possible to add members to a type after its definition
and even by a different module or repository. After a overload set is frozen
it can no longer be modified.

### Freezing

These are the main ways that a overload-set becomes frozen:
 - Code-statements at file level will cause overload-sets that are used
   to be frozen.
 - After a `program` is parsed the `main()` function is compiled and
   overload-sets that are used will be frozen.
 - After a `library` is parsed all the exported functions and
   global-variables are compiled and overload-sets that are used will be frozen.
 - Recursively all type annotations used in the signatures of an overload
   set are frozen.




## Functions

A function is a piece of code to execute, its identity is the memory address to
the start of a function, together with a function's signature.

It is possible for code to be shared by multiple functions, by de-duplication of
identical code being generated.

> [!NOTE]
> A function address is not unique between functions, 


### Function Definition

A function definition consists of:
 - A fully qualified name
 - A function signature
 - Attributes
 - A block of code

A function definition creates a function-template which is part of an overload set
which shares the fully qualified name with other function definitions.

### Compilation Order

A function definition during compilation has the following states:
 1. Modifiable: each function definition is modifiable during compilation.
 2. Frozen: all function definitions of a overload set is frozen when the
    first reference to a function by that name is being compiled. This will
    in turn freeze each type-template mentioned in the signature of each
    function definition of that overload set.
 3. Instantiation: when a function call is compiled the proper template in
    the overload set is matched, and then the function is uniquely instantiated
    for this call, with potential constant values. Only when a function is
    instantiated will calls in the code block be compiled and trigger freeze
    and instantiation of other functions.

The compiler is guaranteed to lazy freeze or instantiate functions only when
those functions are:
 - Called directly in the file-scope, in-order from top to bottom.
 - When functions are explicitly exported to be part of the executable linkage.
   This happens after the last line of the file-scope is processed.
 - When a global variable is explicitly exported to be part of the executable linkage.
   This happens after the last line of the file-scope is processed.
 - A function called `main()` is implicitly exported, this happens as-if
   This happens after the last line of the file-scope is processed.

