
#pragma once

#include "error_code.hpp"

#define E(name, fmt) inline error_code_and_message<fmt> name = {};

namespace hk::error {
E(unimplemented, "E0001: Syntax has not been implemented.");

E(missing_semicolon, "E0102: Expected ';' after a import-declaration.");
E(missing_fqname, "E0103: Expecting a fully qualified name.");
E(missing_filename_stem, "E0104: Program- and library-declaration requires a filename stem for the executable.");
E(missing_git_url, "E0105: Expecting a URL string literal after 'git' keyword in a import-declaration.");
E(missing_git_rev, "E0108: Expecting a branch string literal after 'git' keyword in a import-declaration.");
E(missing_zip_path, "E0109: Expect a path string literal after 'zip' keyword in a import-declaration.");
E(missing_lib_path, "E0110: Expect a path string literal after 'lib' keyword in a import-declaration.");
E(missing_as_name, "E0111: Expected a name after 'as' keyword in a import-declaration.");
E(missing_module_name, "E0112: Expected fully qualified module name.");
E(missing_top_declaration, "E0113: Expected a 'module', 'program' or 'library' declaration at the top of the file.");

E(invalid_fqname, "E0201: Fully qualified name is invalid.");
E(invalid_prologue_statement, "E0202: Invalid prologue statement.");

E(could_not_clone_repository, "E0300: Could not clone repository '{}' rev '{}' into '{}': {}.");
}

namespace hk::warning {
}

#undef E
