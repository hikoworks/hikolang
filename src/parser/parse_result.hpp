
#pragma once

#include "ast/node.hpp"
#include "error/hkc_error.hpp"
#include <memory>
#include <expected>
#include <concepts>

namespace hk {

template<typename T>
using parse_result = std::expected<T, hkc_error>;

template<std::derived_from<ast::node> T>
using parse_result_ptr = parse_result<std::unique_ptr<T>>;

constexpr std::unexpected<hkc_error> tokens_did_not_match = std::unexpected{hkc_error{}};

}
