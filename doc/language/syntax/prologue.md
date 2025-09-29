# Prologue

## Syntax

__(__
[_module_declaration_](module_declaration.md) __|__
[_program_declaration_](program_declaration.md) __|__
[_library_declaration_](library_declaration.md)
__)__
__(__
[_import_module_](import_module.md) __|__
[_import_git_](import_git.md) __|__
[_import_lib_](import_lib.md) __|__
[_import_zip_](import_zip.md)
__)*__

## Semantic

These declarations must appear at the top of a file, before any other
declarations. The prologues are scanned before the compilation phase, to
determine the module hierarchy and the dependencies. It in turn is used to
determine the compilation order of the files.
