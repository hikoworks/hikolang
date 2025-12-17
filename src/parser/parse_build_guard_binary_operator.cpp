
#include "parse_build_guard_binary_operator.hpp"

namespace hk {

[[nodiscard]] parse_result_ptr<ast::build_guard_binary_operator_node>
parse_build_guard_binary_operator(token_iterator& it, parse_context& ctx)
{
    using enum ast::build_guard_binary_operator_node::op_type;

    auto const first = it->begin();
    auto last = it->end();
    auto kind = _and;

    if (it[0] == "and") {
        kind = _and;
    } else if (it[0] == "or") {
        kind = _or;
    } else if (it[0] == "in") {
        kind = in;
    } else if (it[0] == "not" and it[1] == "in") {
        kind = not_in;
        ++it;
    } else if (it[0] == "==") {
        kind = eq;
    } else if (it[0] == "!=") {
        kind = ne;
    } else if (it[0] == "<") {
        kind = lt;
    } else if (it[0] == ">") {
        kind = gt;
    } else if (it[0] == "<=") {
        kind = le;
    } else if (it[0] == ">=") {
        kind = ge;
    } else {
        return tokens_did_not_match;
    }

    ++it;
    return std::make_unique<ast::build_guard_binary_operator_node>(first, last, kind);
}

} // namespace hk