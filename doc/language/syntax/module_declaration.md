# module-declaration

## Syntax

`module` [_fully_qualified_name_](fully_qualified_name.md)
    [_version_literal_](version_literal.md)__?__
    [_build_guard_](build_guard.md)__?__ `;`


## Semantics

A module has a fully qualified unique name, the fully qualified names form a
tree structure. The module tree structure does not have to follow the on-disk
directory structure.

Each module will automatically open the namespace with the name of the
anchor-module, i.e. the ancestor module with a version number.


### Version (optional)

Every sub-module must have an ancestor module with a version number. A module
with a version number is called an anchor-module. In each repository there may
be zero or more anchor-modules.

A anchor-module with the same name may appear in multiple repositories the
anchor-module with the largest version number will be selected when imported, if
there are two modules with an equal highest version, then the module with the
repository-directory that is alphabetically first is selected.

If the module is part of the compiler's include-path, such as `std`, then the
version number is treated as-if it is higher than any other.


### Build guard (optional)

The optional [_build_guard_](build_guard.md) is evaluated during the
prologue-scan phase of compilation, this checks if the file should be compiled.

Multiple files may have the same name, only if the conditional
compilation is mutually exclusive. If a file has a `fallback` condition, it will
be used if no other file with the same name is compiled.
