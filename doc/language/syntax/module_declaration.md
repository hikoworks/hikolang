# module-declaration

## Syntax

`module` [_module_name_](module_name.md) [_version_literal_](version_literal.md)__?__
__(__ `if` [_compile_condition_](compile_condition.md) __|__ `fallback` __)?__ `;`

## Semantics

Each `.hkm` file must have a `module` (or [`application`](application_declaration.md))
declaration to give the file a module-name.

The optional _version_literal_ specifies the version of the module. Having a version
makes the module a package. A repository may contain multiple packages.
Every module in a repository must be a submodule of one of the packages in the repository.

The optional _compile_condition_ is evaluated during the prologue-scan phase of compilation,
this checks if the file should be compiled.

Multiple files may have the same _module_name_, only if the conditional compilation is
mutually exclusive. If a file has a `fallback` condition, it will be used if no other
file with the same _module_name_ is compiled.
