# module-declaration

## Syntax

`module` [_module_name_](module_name.md) __(__ `if` [_compile_condition_](compile_condition.md) __|__ `fallback` __)?__

## Semantics

Each file must have a `module` (or [`application`](application_declaration.md))
declaration to give the file a module-name.

The optional compile-condition is evaluated during the prologue-scan phase of compilation,
this checks if the file should be compiled.

Multiple files may have the same module-name, only if the conditional compilation is
mutually exclusive. If a file has a `fallback` condition, it will be used if no other
file with the same module-name is compiled.
