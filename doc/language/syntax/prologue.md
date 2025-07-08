# Prologue

## Syntax

[_module_declaration_](module_declaration.md) [_import_declaration_](import_declaration.md)__*__

## Semantic

These declarations must appear at the top of a file, before any other
declarations. The prologues are scanned before the compilation phase, to
determine the module hierarchy and the dependencies. It in turn is used to
determine the compilation order of the files.
