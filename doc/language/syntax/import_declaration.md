# import-declaration

## Syntax

`import` [_module_name_](module_name.md)

`import` `package` [_module_name_](module_name.md) [_version_literal_](version_literal.md)
__(__
`git` [_string_literal_](string_literal.md) __(__ `branch` [_string_literal_](string_literal.md) __)?__ __|__
`zip` [_string_literal_](string_literal.md)
__)__

## Semantics

An `import` declaration is used to import a module into the current file.
