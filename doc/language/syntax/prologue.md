# Prologue

## Syntax

__(__
[_module_declaration_](module_declaration.md) __|__
[_application_declaration_](application_declaration.md) __|__
[_package_declaration_](package_declaration.md)
__)__ [_import_statement_](import_statement.md)__*__

## Semantic

The prologue of a file contains only `module`, `application`, `package` and `import` statements, this makes it easy
to make a parser to quickly scan all the source files to create a dependency tree.

During prologue scan the parser can exit early.

