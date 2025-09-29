# import

## Syntax

`import` [_fully_qualified_name_](fully_qualified_name.md)
  __(__ `in` __(__ [_fully_qualified_name_](fully_qualified_name.md) __|__ `*` __))?__
[_compile_condition_](compile_condition.md)__?__ `;`


## Semantics

Imports a module, so that its symbols can be used from the current file.

The [_fully_qualified_name_](fully_qualified_name.md) is the name of the module
to import:
 - if prefixed with `.` then it is an absolute named-module.
 - relative to the current module, or
 - relative to root, when not found.

The optional `in` clause allows the module to be made available under
a different name:
 - relative to the current module,
 - if prefixed with `.` then it is an absolute named-module,
 - if `*` then all symbols are imported into the current module.

If there are no conflicts you may import multiple modules `in`-to the same
module.

If the module is located in a different repository, then that repository must be
imported using a `import git` or `import zip`, by at least one file in the current
repository.

When you import a module all sub-modules are imported as well; accessible
through the sub-names. This holds when the module is renamed with `in`.

### Compile condition (optional)

The optional [_compile_condition_](compile_condition.md) is evaluated during the
prologue-scan phase of compilation, this checks if the file should be compiled.

Multiple files may have the same name, only if the conditional
compilation is mutually exclusive. If a file has a `fallback` condition, it will
be used if no other file with the same name is compiled.
