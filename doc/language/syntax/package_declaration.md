# package-declaration

## Syntax

`package` [_module_name_](module_name.md) [_version_literal_](version_literal.md) `;`

## Semantics
The `package` declaration is used to declare a package. A package is the root
of a module hierarchy and is used to group related modules together.
A package is itself a module and may contain code.

The package declaration must appear at the top of a file, before any other
declarations.

Multiple packages may be declared in a single repository.
