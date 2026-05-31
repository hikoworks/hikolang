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
