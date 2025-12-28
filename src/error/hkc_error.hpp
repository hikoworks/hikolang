
#pragma once

#include "utility/fixed_string.hpp"
#include <system_error>
#include <format>

namespace hk {

enum class hkc_errc : int {
    none = 0,

    /** An error that will not stop compilation.
     */
    warning,

    /** An error that causes compilation to fail eventually.
     */
    error,

    /** An unrecoverable error.
     */
    fatal,

    /** A security error.
     */
    security,
};

enum class hkc_error : int {
    none = 0,

    // Warnings start at 10xxx
    could_not_clone_repository = 10000,

    // Errors start at 20xxx
    unimplemented = 20010,

    // Error: missing syntax 21xxx
    missing_semicolon = 21001,
    missing_fqname = 21002,
    missing_filename_stem = 21003,
    missing_git_url = 21004,
    missing_git_rev = 21005,
    missing_zip_path = 21006,
    missing_lib_path = 210070,
    missing_as_name = 21008,
    missing_module_name = 21009,
    missing_top_declaration = 21010,
    missing_expression = 21011,
    missing_rhs_of_binary_operator = 21012,
    missing_closing_parenthesis = 21013,

    // Error: syntax error 22xxx
    invalid_fqname = 22001,
    invalid_prologue_statement = 22002,

    // Error: compile time evaluation 23xxx
    invalid_operand_types = 23001,
    unknown_build_guard_constant = 23002,
    duplicate_fallback_module = 23003,
    duplicate_module = 23004,
    missing_anchor_module = 23005,
    
    // Fatal: 30xxx

    // Security: 40xxx
    insecure_identifier = 40001,
};

[[nodiscard]] constexpr bool to_bool(hkc_error const& rhs) noexcept
{
    return rhs != hkc_error::none;
}

[[nodiscard]] std::string to_code(hkc_error const& rhs) noexcept;

[[nodiscard]] constexpr bool operator!(hkc_error const& rhs) noexcept
{
    return not to_bool(rhs);
}

} // namespace hk

template<>
struct std::is_error_condition_enum<hk::hkc_errc> : std::true_type {};

template<>
struct std::is_error_code_enum<hk::hkc_error> : std::true_type {};

namespace hk {

struct hkc_error_category : std::error_category {
    char const* name() const noexcept override
    {
        return "hkc";
    }

    std::string message(int code) const override;
};

inline auto global_hkc_error_category = hkc_error_category{};

[[nodiscard]] inline std::error_code make_error_code(hkc_error code) noexcept
{
    return {static_cast<int>(code), global_hkc_error_category};
}

} // namespace hk

template<>
struct std::formatter<hk::hkc_error, char> : public std::formatter<std::string, char> {
    template<class FmtContext>
    FmtContext::iterator format(hk::hkc_error e, FmtContext& ctx) const
    {
        auto e_ = make_error_code(e);
        return std::formatter<std::string, char>::format(e_.message(), ctx);
    }
};

