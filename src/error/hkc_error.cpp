
#include "hkc_error.hpp"
#include <exception>
#include <utility>
#include <string>
#include <format>

namespace hk {

[[nodiscard]] std::string to_code(hkc_error const& rhs) noexcept
{
    auto const numeric_code = std::to_underlying(rhs);

    auto const prefix = [&]{
        if (numeric_code < 10000) {
            return 'I';
        } else if (numeric_code < 20000) {
            return 'W';
        } else if (numeric_code < 30000) {
            return 'E';
        } else if (numeric_code < 40000) {
            return 'F';
        } else if (numeric_code < 50000) {
            return 'S';
        } else {
            return 'U';
        }
    }();

    return std::format("{}{:05}", prefix, numeric_code);
}

std::string hkc_error_category::message(int code) const
{
    using namespace std::literals;

    switch (static_cast<hkc_error>(code)) {
    case hkc_error::none:
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
        return "Missing string literal containing the git revision."s;
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
    case hkc_error::missing_expression:
        return "Missing expression."s;
    case hkc_error::missing_rhs_of_binary_operator:
        return "Missing right-hand-side operator of this binary operator."s;
    case hkc_error::missing_closing_parenthesis:
        return "Missing closing paranthesis ')'."s;
    case hkc_error::invalid_fqname:
        return "Invalid syntax of a fully qualified name."s;
    case hkc_error::invalid_prologue_statement:
        return "A prologue statement has an invalid syntax."s;
    case hkc_error::invalid_operand_types:
        return "Types of operands are invalid"s;
    case hkc_error::unknown_build_guard_constant:
        return "Unknown build-guard constant."s;
    case hkc_error::duplicate_fallback_module:
        return "There a multiple fallback modules with the same name."s;
    case hkc_error::duplicate_module:
        return "There are multiple modules with the same name for which the build-guard evaluates to true."s;
    case hkc_error::missing_anchor_module:
        return "Module must be a sub-module of an anchor-module in the same repository."s;
    case hkc_error::imported_module_not_found:
        return "Imported module was not found in project"s;
    case hkc_error::could_not_clone_repository:
        return "Unable to clone a repository."s;
    case hkc_error::insecure_identifier:
        return "Insecure identifier; illegal Unicode characters or mixed scripts."s;
    }
    std::terminate();
}

}