# import

## Syntax

`import`\
    `eval` _generator_call_\
    `from` [_fully_qualified_name_](fully_qualified_name.md)\
    [_build_guard_](build_guard.md)__?__ `;`

_generator_call_ := [_fully_qualified_name_](fully_qualified_name.md)\
    `(` *generator_argument-list*__?__ `)`

_generator_argument-list_ := _generator_argument_ __(__ `,` _generator_argument_ __)*__

_generator_argument := [_expression_](expression.md)

## Semantics

Imports a module from a generated string, returned from the `eval`-call. The
generated string is compiled and its symbols are imported into the current
module.

The returned string must be a full module's text, including a prologue with a
basic `module` declaration and optional `import` statements. A generated module
cannot: import repositories, or set the module's version number.

> [!NOTE]
> It is the generator function's responsibility to cache results of an expensive
> generation.

The `eval`-call's arguments are expressions with the same limitations as the
expression of a [_build_guard_](build_guard.md).

The `from` module is temporarily imported (its symbols are only available during
evaluation of the current statement) to get access to the generator function.
The `eval`-call's namespace is the default namespace of the `from` module; if
the `from` module switches namespace the `eval`-call needs to be fully
qualified.

Searching for the `from`-module in the following order:
 - Treat the name as relative to the current module, in the current repository.
 - Search the module in the compiler's include-paths.
 - Search the module in the current repository.
 - Search the module in imported repositories by highest module version number.

If the module is located in a different repository, then that repository must be
imported using a `import git` or `import zip`, by at least one file in the
current repository.



### Build guard (optional)

The optional [_build_guard_](build_guard.md) is evaluated during the
prologue-scan phase of compilation, this checks if the file should be compiled.

Multiple files may have the same name, only if the conditional
compilation is mutually exclusive. If a file has a `fallback` condition, it will
be used if no other file with the same name is compiled.
