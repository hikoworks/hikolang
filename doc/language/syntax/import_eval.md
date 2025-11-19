# import

## Syntax

`import`
    `eval` [_expression_](expression.md)
    __(__ `if` [_guard_expression_](guard_expression.md) __)?__ `;`


## Semantics

Imports a module from a generated string, returned from the
[_expression_](expression.md). The generated string is compiled and its symbols
are imported into the current module.

The [_expression_](expression.md) is evaluated in the default namespace of
the module, after all `import module` statements in the prologue of the current
module have been processed.

The returned string must be a full module's text, including a prologue with a
basic `module` declaration and optional `import` statements.

A generated module cannot:
 - import repositories, or
 - set the module's version number.

> [!NOTE]
> It is the generator function's responsibility to cache results of an expensive
> generation.


If the module is located in a different repository, then that repository must be
imported using a `import git` or `import zip`, by at least one file in the
current repository.



### Build guard (optional)

The optional [_guard_expression_](guard_expression.md) is evaluated during the
prologue-scan phase of compilation, this checks if the `import` statment
should be processed or ignored.
