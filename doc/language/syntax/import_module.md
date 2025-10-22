# import

## Syntax

`import` [_fully_qualified_name_](fully_qualified_name.md)
    [_compile_condition_](compile_condition.md)__?__ `;`


## Semantics

Imports a module, so that its symbols can be used from the current file.

Searching for modules in the following order:
 - Treat the name as relative to the current module, in the current repository.
 - Search the module in the compiler's include-paths.
 - Search the module in the current repository.
 - Search the module in imported repositories by highest module version number.

If the module is located in a different repository, then that repository must be
imported using a `import git` or `import zip`, by at least one file in the
current repository.

When you import a module all sub-modules are imported as well; accessible
through the sub-names. This holds when the module is renamed with `in`.

### Compile condition (optional)

The optional [_compile_condition_](compile_condition.md) is evaluated during the
prologue-scan phase of compilation, this checks if the file should be compiled.

Multiple files may have the same name, only if the conditional
compilation is mutually exclusive. If a file has a `fallback` condition, it will
be used if no other file with the same name is compiled.
