# package-declaration

[_build-guard_]: build_guard.md
[_fqname_]: fqname.md
[_build-guard-expression_]: build_guard_expression.md
[_version-literal_]: version_literal.md

## Syntax

_package-declaration_ := `package` [_fqname_]
   [_version-literal_] [_build-guard_]__?__ `;`


## Semantics

A package has a [_fqname_], the name forms the root of a module tree structure.
The package or module tree structure does not have to follow the on-disk
directory structure.

Each module will automatically open the namespace with the name of the
package.

A package should import the modules it intends to export. Only the symbols in
those modules that are marked with `@public` are available outside of a module
tree. Imports are recursive; so a package may only need to import a few modules.


### Version

A package with the same name may appear in multiple repositories the
package with the largest version number will be selected when imported, if
there are two packages with an equal highest version, then the module with the
repository-directory that is alphabetically first is selected.

If the package is part of the compiler's include-path, such as `std`, then its
version number is treated as-if it is higher than any other.

#### Nested packages

Packages may be nested, a child package will own all its modules. If the
child package is in a different repository, then the parent package may not
have a module that matches the path inside child package.

Multiple repositories can thus contribute modules as part of larger package.

### Build guard (optional)

The optional [_build-guard-expression_] is evaluated during the
prologue-scan phase of compilation, this checks if the file should be compiled.
If no [_build-guard-expression_] is specified then the file
is compiled when all other [_build-guard-expression_] fail.

