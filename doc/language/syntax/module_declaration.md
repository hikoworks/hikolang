# module-declaration

## Syntax

`module` [_fully_qualified_name_](fully_qualified_name.md) [_compile_condition_](compile_condition.md)__?__ `;`


## Semantics

A module has a fully qualified unique name, the fully qualified names form a
tree structure. The module tree structure does not have to follow the on-disk
directory structure.

A `module` can only be imported by other files in the same repository; unless
the module is a child of a `package`, in which case the module can be
imported by a file in another repository.

The optional [_compile_condition_](compile_condition.md) is evaluated during the
prologue-scan phase of compilation, this checks if the file should be compiled.
If no condition is specified, the following default condition is used:
 - normal-module: `if true`
 - package-module: `if true`
 - program-module: `if build`
 - library-module: `if build`

Multiple files may have the same name, only if the conditional
compilation is mutually exclusive. If a file has a `fallback` condition, it will
be used if no other file with the same name is compiled.
