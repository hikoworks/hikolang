# module-declaration

[_build-guard_]: build_guard.md
[_fqname_]: fqname.md
[_build-guard-expression_]: build_guard_expression.md
[_version-literal_]: version_literal.md

## Syntax

_module-declaration_ := `module` [_fqname_]
    [_version-literal_]__?__ [_build-guard_]__?__ `;`


## Semantics

A module has a [_fqname_], the name forms a tree structure. The package or
module tree structure does not have to follow the on-disk directory structure.

Each module will automatically open the namespace with the name of the
package.


### Build guard (optional)

The optional [_build-guard-expression_] is evaluated during the
prologue-scan phase of compilation, this checks if the file should be compiled.
If no [_build-guard-expression_] is specified then the file
is compiled when all other [_build-guard-expression_] fail.

