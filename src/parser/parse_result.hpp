
#pragma once

#include "ast/node.hpp"
#include <memory>
#include <expected>
#include <concepts>

namespace hk {

template<typename T>
using parse_result = std::expected<T, error_code>;

template<std::derived_from<ast::node> T>
using parse_result_ptr = parse_result<std::unique_ptr<T>>;

constexpr std::unexpected<error_code> tokens_did_not_match = std::unexpected{error_code{}};

}
