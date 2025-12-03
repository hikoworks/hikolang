
#pragma once

#include "utility/fixed_string.hpp"
#include <system_error>

namespace hk {

enum class hkc_error : int {
    no_error = 0,

    unimplemented = 30001,
    missing_semicolon = 30102,
    missing_fqname = 30103,
    missing_filename_stem = 30104,
    missing_git_url = 30105,
    missing_git_rev = 30108,
    missing_zip_path = 30109,
    missing_lib_path = 30110,
    missing_as_name = 30111,
    missing_module_name = 30112,
    missing_top_declaration = 30113,
    invalid_fqname = 30201,
    invalid_prologue_statement = 30202,
    could_not_clone_repository = 30300,

    insecure_identifier = 50001,
};

}

template<>
struct std::is_error_code_enum<hk::hkc_error> : std::true_type {};

namespace hk {

struct hkc_error_category : std::error_category {
    char const *name() const noexcept override
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

}
