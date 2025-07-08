# module-declaration

## Syntax

`module` [_module_name_](module_name.md)
__(__ `if` [_compile_condition_](compile_condition.md) __|__ `fallback` __)?__ `;`

`module` `application` [_module_name_](module_name.md) [_string_literal_](string_literal.md)
__(__ `if` [_compile_condition_](compile_condition.md) __|__ `fallback` __)?__ `;`

`module` `package` [_module_name_](module_name.md) [_version_literal_](version_literal.md)
__(__ `if` [_compile_condition_](compile_condition.md) __|__ `fallback` __)?__ `;`

## Semantics

Each `.hkm` file must have a `module` ([`package`](package_declaration.md)
or [`application`](application_declaration.md)) declaration to give the file a
module-name. A module declaration must appear at the top of a file,
before any other declarations.

The _module_name_ is a unique identifier for the module. Every module must have
a _module_name_ that is a sub-name of a package or application declarated in the
same repository.

The optional _compile_condition_ is evaluated during the prologue-scan phase of
compilation, this checks if the file should be compiled.

Multiple files may have the same _module_name_, only if the conditional
compilation is mutually exclusive. If a file has a `fallback` condition, it will
be used if no other file with the same _module_name_ is compiled.


