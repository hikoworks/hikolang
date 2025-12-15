
#include "parse_build_guard_binary_operator.hpp"

namespace hk {

[[nodiscard]] parse_result_ptr<ast::build_guard_binary_operator_node>
parse_build_guard_binary_operator(token_iterator& it, parse_context& ctx)
{
    auto const first = it->begin();
    auto last = it->end();
    auto kind = ast::build_guard_binary_operator_node::kind_type::none;

    if (it[0] == "and") {
        kind = ast::build_guard_binary_operator_node::kind_type::_and;
    } else if (it[0] == "or") {
        kind = ast::build_guard_binary_operator_node::kind_type::_or;
    } else if (it[0] == "in") {
        kind = ast::build_guard_binary_operator_node::kind_type::in;
    } else if (it[0] == "not" and it[1] == "in") {
        kind = ast::build_guard_binary_operator_node::kind_type::not_in;
        ++it;
    } else if (it[0] == "==") {
        kind = ast::build_guard_binary_operator_node::kind_type::eq;
    } else if (it[0] == "!=") {
        kind = ast::build_guard_binary_operator_node::kind_type::ne;
    } else if (it[0] == "<") {
        kind = ast::build_guard_binary_operator_node::kind_type::lt;
    } else if (it[0] == ">") {
        kind = ast::build_guard_binary_operator_node::kind_type::gt;
    } else if (it[0] == "<=") {
        kind = ast::build_guard_binary_operator_node::kind_type::le;
    } else if (it[0] == ">=") {
        kind = ast::build_guard_binary_operator_node::kind_type::ge;
    } else {
        return tokens_did_not_match;
    }

    ++it;
    return std::make_unique<ast::build_guard_binary_operator_node>(first, last, kind);
}

} // namespace hk