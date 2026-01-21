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

Function and type definitions are always templates in this language.


## Overload sets




## Compilation Order

The compilation order is important when we are describing how the compiler
knows which types are visible and complete.

Compilation starts after the prologue-scan which will read every source file
in the repository and each imported repository.

Each file is then compiled in random order while satisfying that all imported
modules of a file are compiled first. In incremental/language server mode,
files may trigger recompilation of files and their dependent files.

Each file is compiled statement-by-statement, as-if they executed in a
interpreter, for example:
 - type-template definition.
 - function-template definition.
 - global variable definition.
 - executable statements

> [!NOTE]
> All types and function declarations are templates in this language.

At this time function and type declarations remain in their "modifiable" state,
this means:
 - Other versions of type and functions can be added to the overload set.
 - It is possible to modify types by removing and adding members to types,
   as-if types are mutable values.

During the statement-by-statement compilation there may also be normal code
statments at file scope that will be executed that for example can make these
modifications of types. These code statements can cause types and functions
to transition to "frozen" and "instantiated" state.



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

