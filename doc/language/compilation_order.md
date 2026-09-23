# Compilation Order

## Compilation Flags

The compilation cache for repositories will need to be distinct for different
compilation flags.

 - Release / Debug
 - Architecture

## Cross compilation

For cross compilation the program must be compiled twice, once for the current
machine and once for the target machine. This is needed so that compile-time
code can be properly executed in the JIT of the compiler.

The generated code for the current machine may need to include automatically
inserted endian swap instruction to match the endianess of the target machine.
Information about the current and the target machine are separately available
to the programmer; see: [_guard_expression_](syntax/guard_expression.md).

## Prologue Scan

 - Search all files in the current repository.
 - Update the previous prologue scan result for added, removed or updated files.
 - Read the prologue of added and updated files.
 - Checkout imported repositories
   - Recursively do the prologue scan for these imported repositories.
 - Mark old-version of anchor-modules and their children as "don't use".
 - Mark modules that fail conditional compilation as "don't use".
 - Mark any module that is imported as "used"
 - Mark any module that was added or updated as "to be compiled".
 - Mark any module the recursively imports a "to be compiled" module as
   "to be compiled".
 - Compute the default namespace of each module.


## 0. Tokenizer

 - Inserts missing semicolons
 - Replace superscript numbers with power operator `**` and normal numbers
 - Replace documentation-comments with `@doc()` attributes


## 2. Parsing

 - Update the current namespace based on the prologue and `namespace` statement
 - Add operators



The compiler treats the whole program as a whole, so libraries are being compiled
together with the program. Files in a program or library are called modules.
Modules are parsed depth-first.

A module is parsed statement by statement. Each statement at file scope is
directly evaluated after being parsed. The statement at file scopes are mostly
definitions of types, functions and global variables; but some can also modify
existing types. This execution at compile time is called elaboration. Statements
at other scopes, like function scopes, are not elaborated until such function
are used.

After all the modules are parsed and the file scope statements have been
elaborated; there will be a list of functions, like `main()` that need to be
written into the executable. The compiler is now in the translation phase.

As the compiler is translating these functions, the usage of
types in these functions may trigger more elaborations; types must be complete
when translating functions.

The compiler will aggressively constant fold. It expects by default that any
function can be evaluated during constant folding. Certain functions are
annotated with `@with_effect(phase_variant)`, these functions would work
differently during elaboration/constant-folding vs runtime, for example
functions that perform I/O. Functions that always work the same are called
phase-invariant.

 * elaboration: any function, including phase-variant functions, are always
   evaluated immediately, in the compiler's environment
 * const-folding:
   - phase-invariant functions are evaluated immediately in the compiler's
     environment.
   - calls to phase-variant functions are translated into the executable.
   - `@fold` annotated calls to phase-variant expressions are evaluated
     immediately in the compiler's environment.
 * runtime: any function that was translated, including phase-variant functions,
   are executed in the runtime's environment.

Expressions inside phase-variant functions are still optimized if those
expressions are phase-invariant or explicitly annotated with `@fold`.

During elaboration and constant-folding memory may be allocated. After
translation the compiler will move these allocations into the executable; as-if
the executable has an embedded allocation-arena. This embedded allocation-arena
acts like a normal allocation-arena, allowing deallocation and new allocations.
