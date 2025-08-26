# import

## Syntax

`import` [_fully_qualified_name_](fully_qualified_name.md) __(__ `as` [_identifier_](identifier.md) __)?__
[_compile_condition_](compile_condition.md)__?__ `;`

## Semantics

Imports a module, so that its functions, types and variables can be used in the
current file.

The [_fully_qualified_name_](fully_qualified_name.md) is the name of the module
to import. The optional `as` clause allows the module to be made available under
a shorter name to make the code more readable.

If the module is located in a different repository, then the repository must be
imported using a `git` or `zip` by at least one file in the current repository.
Likely this in done in a `application` or `package` file.

When you import a module all sub-modules are imported as well; accessible through
the sub-names. This holds when the module is renamed through the `as` part.

