
#include "token.hpp"
#include <charconv>
#include <cassert>

namespace hk {

[[nodiscard]] std::expected<std::string, utf8_security_error> token::identifier_value() const
{
    auto normalized_text = normalize_utf8_string(string());
    if (not normalized_text) {
        return std::unexpected{utf8_security_error::unknown_error};
    }

    if (auto const check_result = security_check_utf8_string(*normalized_text); not check_result) {
        return std::unexpected{check_result.error()};
    }

    return std::move(normalized_text).value();
}

[[nodiscard]] std::expected<std::string, utf8_security_error> token::operator_value() const
{
    return identifier_value();
}

[[nodiscard]] long long token::integer_value() const
{
    assert(_kind == integer_literal);

    long long x = 0;
    auto const fcr = std::from_chars(begin(), end(), x, 10);
    assert(fcr.ptr == end());

    return x;
}

[[nodiscard]] std::pair<size_t, std::string> token::line_value() const
{
    assert(_kind == line_directive);

    auto line = 0uz;
    auto const fcr = std::from_chars(begin(), end(), line, 10);
    if (fcr.ptr == begin()) {
        // fcr.ec contains an errc that we do not care about, it is not possible
        // to check the errc for no-error.
        return {0, std::string{}};
    }

    auto p = fcr.ptr;
    while (p != end() and p[0] == ' ') {
        ++p;
    }

    auto file_name = std::string{};
    if (p != end() and p[0] == '"') {
        ++p;

        auto escape = false;
        for (; p != end(); ++p) {
            if (escape) {
                escape = false;
                file_name += *p;
            } else if (p[0] == '\\') {
                escape = true;
            } else if (p[0] == '"') {
                break;
            } else {
                file_name += *p;
            }
        }

        if (p == end() or p[0] != '"') {
            return {0, std::string{}};
        }
    }

    return {line, file_name};
}

[[nodiscard]] std::string token::raw_string_value() const
{
    assert(_kind != simple);

    auto const s = this->string_view();

    auto r = std::string{};
    r.reserve(s.size());

    auto i = 0uz;
    while (true) {
        auto const j = s.find('\\', i);
        if (j == std::string_view::npos) {
            r += s.substr(i);
            return r;

        } else {
            r += s.substr(i, j - i);
            i = j + 1;
        }
    }
}


}
