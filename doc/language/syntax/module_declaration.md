# module-declaration

## Syntax

`module` [_module_name_](module_name.md) _module_type_**?** [_compile_condition_](compile_condition.md)__?__ `;`

### module_type

`application` [_string_literal_](string_literal.md) __|__
`library` [_string_literal_](string_literal.md) __|__
`package` [_version_literal_](version_literal.md)

## Semantics

Each `.hkm` file must have a `module` declaration to give the file a
module-name. A module declaration must appear at the top of a file,
before any other declarations.

The _module_name_ is a unique identifier for the module. Every module must have
a _module_name_ that is a sub-name of a `package`, `application` or `library`
declarated in the same repository. We call `package`, `application` or `library`
an anchor for other modules.

The optional [_compile_condition_](compile_condition.md) is evaluated during the
prologue-scan phase of compilation, this checks if the file should be compiled.
If no condition is specified, the following default condition is used:
 - normal-module: `if true`
 - package-module: `if true`
 - application-module: `if build`
 - library-module: `if build`

Multiple files may have the same _module_name_, only if the conditional
compilation is mutually exclusive. If a file has a `fallback` condition, it will
be used if no other file with the same _module_name_ is compiled.


