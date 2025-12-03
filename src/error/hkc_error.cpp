
#include "hkc_error.hpp"
#include <exception>

namespace hk {


std::string hkc_error_category::message(int code) const
{
    using namespace std::literals;

    switch (static_cast<hkc_error>(code)) {
    case hkc_error::no_error:
        return "<No error>"s;
    case hkc_error::unimplemented:
        return "Not implement syntax."s;
    case hkc_error::missing_semicolon:
        return "Missing semicolon after statement."s;
    case hkc_error::missing_fqname:
        return "Missing fully qualified name."s;
    case hkc_error::missing_filename_stem:
        return "Missing string literal containing the filename stem."s;
    case hkc_error::missing_git_url:
        return "Missing string literal containing the git remote URL."s;
    case hkc_error::missing_git_rev:
        return "Missing string literal containing the git revission."s;
    case hkc_error::missing_zip_path:
        return "Missing string literal containing the path to a zip file."s;
    case hkc_error::missing_lib_path:
        return "Missing string literal containing the name or path to a systen library file."s;
    case hkc_error::missing_as_name:
        return "Missing a identifier for the 'as'-name."s;
    case hkc_error::missing_module_name:
        return "Missing a name for a module."s;
    case hkc_error::missing_top_declaration:
        return "Missing the top-declaration in the prologue."s;
    case hkc_error::invalid_fqname:
        return "Invalid syntax of a fully qualified name."s;
    case hkc_error::invalid_prologue_statement:
        return "A prologue statement has an invalid syntax."s;
    case hkc_error::could_not_clone_repository:
        return "Unable to clone a repository."s;
    case hkc_error::insecure_identifier:
        return "Insecure identifier; illigal Unicode characters or mixed scripts."s;
    }
    std::terminate();
}

}