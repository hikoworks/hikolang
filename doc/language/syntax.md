# Syntax

[_file_] := [_prologue_] [_body_]

[_prologue_] :=\
    __(__ [_import-module_] __|__ [_import-git_] __|__ [_import-lib_] __|__ [_import-zip_] __)*__\
    __(__ [_module-declaration_] __|__ [_program-declaration_] __|__ [_library-declaration_] __)__

[_import-module_] := `import` [_relative-fqname_] `public`__?__ [_build-quard_]__?__ `;`





[_body_]: syntax/body.md
[_build-quard_]: syntax/build_guard.md
[_file_]: syntax/file.md
[_import-module_]: syntax/import_module.md
[_import-git_]: syntax/import_git.md
[_import-lib_]: syntax/import_lib.md
[_import-zip_]: syntax/import_zip.md
[_library-declaration_]: syntax/library_declaration.md
[_module-declaration_]: syntax/module_declaration.md
[_program-declaration_]: syntax/program_declaration.md
[_prologue_]: syntax/prologue.md
[_relative-fqname_]: syntax/relative_fqname.md
