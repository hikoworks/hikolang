# module-declaration

## Syntax

[_documentation_](documentation.md)__?__
`module` [_fqname_](fqname.md)
    [_version-literal_](version_literal.md)__?__
    [_build-guard_](build_guard.md)__?__
    `;`


## Semantics

A module has a [_fqname_](fqname.md), the name forms a tree structure. The
module tree structure does not have to follow the on-disk directory structure.

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

#### Nested anchors

Achors may be nested, a child anchor will own all its sub-modules. If the
child anchor is in a different repository, then the parent anchor may not
have a sub-module that matches the path of a child-anchor.

This will allow multiple repositories to each contribute part of a whole
project.


### Build guard (optional)

The optional [_guard_expression_](guard_expression.md) is evaluated during the
prologue-scan phase of compilation, this checks if the file should be compiled.
If no [_guard_expression_](guard_expression.md) is specified then the file
is compiled when all other [_guard_expression_](guard_expression.md) fail.

