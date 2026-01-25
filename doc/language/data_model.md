# data model

This chapter describes how functions, types, templates and values work in this
language.

Here are a couple of rules in this language:
 - Types are first class values, and can be passed to functions or type
   templates.
 - All function and type definitions are always templates.
 - A type definition is an alternate representation of a function that returns a
   type.
 - Multiple type and function definition with the same name form a overload set.
 - Type and function definitions may be modified until the whole overload set is
   frozen.
 - An overload set is frozen when a type or function is instantiated.
 - Functions and types are lazilly evaluated to delay freezing; functions and
   global variables are recursively evaluated when they are exported, after
   processing the last statement of a file.


## Function Definition

All functions definition are templates of functions, for which partial or full
specializations are generated depending the arguments that are passed to the
function in a call.

A function definition consist of the following components:
 - Name
 - Argument declarations
 - Return type declaration
 - Attributes
 - Code-Block

The name of the function is recorded in the context where the function is
defined. One or more functions with the same name form an overload-set.

The signature of a function differentiates between different functions within an
overload-set. The signature of a function includes: argument types and
`constrain()`-attributes.

When a function-call is compiled the following is done:
 - Collect all overload-sets based on the name in:
   + In the current context, surrounding contexts, up to the root namespace.
   + In the context of the type definitions of any of the call's arguments;
     ADR (Argument Depended Lookup)
 - Each function in the collected overload-sets is matched with the function's
   argument and `constraint()` expressions. This will cause these overload-sets
   to be frozen.
 - The best matching function is selected, and this function is instantiated
   and compiled.
 - A (partial) specialization of that function is generated, at which time the
   code-block is compiled.
 - As an optimization functions are de-duplicated; which may cause multiple
   functions to have the same address.

> [!NOTE]
> Because the return type of functions are not part of the signature
> it delays freezing of return types. This could delay freezing of important
> types like `error_code`.


### Best matching function

The best function of a collection of overload-sets is chosen in the following
priority order:
 1. The longest start sequence of perfect matching argument types.
 2. The largest number of perfect matching argument types.
 3. The longest start sequence of derived argument types.
 4. The largest number of derived argument types.
 5. The rest.

If at each step there are functions that have an equal number of arguments
that match, the rest of the arguments are matched using the same priority order.

If after this there is more than one function with the same priority then
the compiler will emit an "ambiguous function" call error.


### Freezing

When a function call is compiled the full overload-sets that match that function
call is frozen. This means that from this point forward no new functions can
be added to the overload-set, nor can the signatures of these functions be
modified.

During this freeze the types used in every signature of the overload-set needs
to recursively be frozen as well.

### (Partial) Specialization

A second level of freezing happens when one of the templates in the overload-set
gets specialized/instantiated, at this point the code-block of this template can
no longer be modified, and the code-block gets compiled and triggers freezing
from any call and type usage in this code-block.


## Type Definition

All type definition are templates of types, for which partial or full
specializations are generated depending the arguments that are passed when
creating a concrete type.

```
struct foo[T : type] (barrable) constrain(T : numeric) {
  var x : T

  fn bar(y : T) {
    return x + y
  }
}
```

A type definition consists of the following:
 - meta-type `struct`,
 - name `foo`,
 - template-arguments `[T : type]`,
 - inheritance list `(barrable)`,
 - attributes `constrain(T : numeric)`,
 - code-block

A type definition is a different syntactic form of a function definition (a
function returning a type); hence a type definition is part of an overload set
of this function definition.

A meta-type is a function that returns a type template, for example `struct` and
`enum` are meta-types. The functions `struct()`, `enum()` are called to return a
function-template with template-arguments, which in turn returns a concrete
type.






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

