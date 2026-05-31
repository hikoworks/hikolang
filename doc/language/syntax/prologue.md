# Prologue

## Syntax

prologue :=\
    __(__ [_import-module_] __|__ [_import-git_] __|__ [_import-lib_] __|__ [_import-zip_] __)*__\
    __(__ [_module-declaration_] __|__ [_program-declaration_] __|__ [_library-declaration_] __)__

[_import-module_]: import_module.md
[_import-git_]: import_git.md
[_import-lib_]: import_lib.md
[_import-zip_]: import_zip.md
[_module-declaration_]: module_declaration.md
[_program-declaration_]: program_declaration.md
[_library-declaration_]: library_declaration.md

## Semantic

These declarations must appear at the top of a file, before any other
declarations. The prologues are scanned before the compilation phase, to
determine the module hierarchy and the dependencies. It in turn is used to
determine the compilation order of the files.
