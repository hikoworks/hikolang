# import

## Syntax

`import` [_relative_fqname_](relative_fqname.md)
    `public`__?__
    __(__ `if` [_guard_expression_](guard_expression.md) __)?__ `;`


## Semantics

Imports a module, so that its symbols can be used from the current module.

Searching for modules in the following order:
 - Treat the name as relative to the current module, in the current repository.
 - Search the module in the compiler's include-paths.
 - Search the module in the current repository.
 - Search the module in imported repositories by highest module version number.

If the module is located in a different repository, then that repository must be
imported using a `import git` or `import zip`, by at least one file in the
current repository.

### Public (optional)

If the `public` attribute is added, all the symbols of the imported module
will be available by subsequent imports of the current module. 

### Build guard (optional)

The optional [_guard_expression_](guard_expression.md) is evaluated during the
prologue-scan phase of compilation, this checks if the `import` statment
should be processed or ignored.
