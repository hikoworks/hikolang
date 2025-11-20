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


## 1. Prologue Scan

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

## 2. Compilation

Compilation should be done in parralel using a pool of compiler threads.

 - Compile every "used" module that is "to be compiled" depth first.
   - Recursivelly collect the imported symbols in the context for compilation.
   - Recursivelly collect the imported `syntax` symbols in the context for
     compilation.
   - If there are `import eval` statements:
     - Call the generators, cache the text, read their prologues, update the
       prologue scan result.
     - If there are dependencies to be compiled, then cancel the compilation
       of the current module and continue with the depth-first compilation.
     - Compile the cached result of the generator, and import the generated
       module into the current module.
   - Generate the AST of the current module, using the `syntax` symbols from
     any imported modules.
   - Specialize and generate IR-code for the current module.
   - Execute all code with constant arguments and replace calls and expression
     with the resulting value.
   - If the current module is a program or library generate the executable.
   - Save the compiled repository, once all modules of a repository have been
     compiled. For incremental compilation.
   - Remove the "to be compiled" mark.
