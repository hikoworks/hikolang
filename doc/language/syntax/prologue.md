# Prologue

## Syntax

__(__
[_package_declaration_](package_declaration.md) __|__
[_module_declaration_](module_declaration.md) __|__
[_application_declaration_](application_declaration.md) __|__
__)__
__(__
[_import_statement_](import_statement.md) __|__
[_repository_declaration_](repository_declaration.md) __|__
[_library_declaration_](library_declaration.md)
__)*__

## Semantic

The prologue of a file contains only the following declarations:
 - `module`,
 - `application`,
 - `package`
 - `import`
 - `repository`
 - `library`

These declarations must appear at the top of a file, before any other
declarations. The prologues are scanned before the compilation phase, to
determine the module hierarchy and the dependencies. It in turn is used to
determine the compilation order of the files.
