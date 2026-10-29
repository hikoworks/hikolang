# module-declaration

[_annotation_]: annotation.md
[_build-guard_]: build_guard.md
[_fqname_]: fqname.md
[_build-guard-expression_]: build_guard_expression.md
[_version-literal_]: version_literal.md

## Syntax

_module-declaration_ := [_annotation_]__*__ `module` [_fqname_]
    [_version-literal_]__?__
    [_build-guard_]__?__
    `;`


## Semantics

A module has a [_fqname_], the name forms a tree structure. The
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

The optional [_build-guard-expression_] is evaluated during the
prologue-scan phase of compilation, this checks if the file should be compiled.
If no [_build-guard-expression_] is specified then the file
is compiled when all other [_build-guard-expression_] fail.

