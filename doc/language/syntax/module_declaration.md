# module-declaration

## Syntax

`module` [_fully_qualified_name_](fully_qualified_name.md)
    [_version_literal_](version_literal.md)__?__
    [_compile_condition_](compile_condition.md)__?__ `;`


## Semantics

A module has a fully qualified unique name, the fully qualified names form a
tree structure. The module tree structure does not have to follow the on-disk
directory structure.


### Version (optional)

If a version number is given, then all sub-modules within the same repository
will get the same version.

When a module has a version number, then the module of the same name may appear
in two different repositories. Only the module with the highest version will be
compiled. If there are two modules with an equal highest version, then the module
with the repository-directory that alphabetically first is selected.


### Compile condition (optional)

The optional [_compile_condition_](compile_condition.md) is evaluated during the
prologue-scan phase of compilation, this checks if the file should be compiled.

Multiple files may have the same name, only if the conditional
compilation is mutually exclusive. If a file has a `fallback` condition, it will
be used if no other file with the same name is compiled.
