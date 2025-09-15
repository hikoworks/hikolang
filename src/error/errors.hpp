
#pragma once

#include "error_code.hpp"

#define E(name, fmt) inline error_code_and_message<fmt> name = {};

namespace hk::error {
E(missing_module_declaration_name, "E0001: Module declaration requires fqname.");
E(missing_module_application_declaration_exe, "E0002: Module application declaration requires an executable name.");
E(missing_module_library_declaration_bin, "E0003: Module library declaration requires a binary name.");
E(missing_module_package_declaration_version, "E0004: Module package declaration requires a version number.");
E(unimplemented_module_declaration_if, "E0005: module-if expression is not implemented.");
E(missing_module_declaration_semicolon, "E0006: Expecting ';' at end of module declaration.");
E(missing_import_git_declaration_url, "E0007: Expecting a URL string literal after 'git' keyword in a import-declaration.");
E(missing_import_git_declaration_branch, "E0008: Expecting a branch string literal after 'git' keyword in a import-declaration.");
E(missing_import_zip_declaration_path, "E0009: Expect a path string literal after 'zip' keyword in a import-declaration.");
E(missing_import_lib_declaration_path, "E0010: Expect a path string literal after 'lib' keyword in a import-declaration.");
E(missing_import_mod_declaration_as_name, "E0011: Expected a name after 'as' keyword in a import-declaration.");
E(missing_import_mod_declaration_name, "E0012: Expected fully qualified name after 'import' keyword in a import-declaration.");
E(missing_import_declaration_semicolon, "E0013: Expected ';' after a import-declaration.");
E(missing_fqname_identifier_after_dot, "E0014: Expected identifier after '.' in fully qualified name.");
E(could_not_clone_repository, "E0015: Could not clone repository '{}' rev '{}' into '{}': {}.");
}

namespace hk::warning {
}

#undef E
