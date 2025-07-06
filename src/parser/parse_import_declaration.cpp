
#include "parsers.hpp"

namespace hk {

[[nodiscard]] parse_result_ptr<ast::import_declaration_node> parse_import_declaration(token_iterator& it, error_list& e)
{
    auto const first = it->first;

    if (*it != "import") {
        return tokens_did_not_match;
    }
    ++it;

    auto r = std::make_unique<ast::import_declaration_node>(first);

    if (auto fqname = parse_fqname(it, e)) {
        r->name = std::move(fqname).value();
    } else if (fqname.error()) {
        return std::unexpected{fqname.error()};
    } else {
        return e.add<"E0007: Expected fully qualified name after 'import' keyword in import-declaration.">(first, it->last);
    }
    ++it;

    auto const last = it->first;
    if (*it != ';') {
        return e.add<"E0008: Expected ';' after import-declaration.">(first, it->last);
    }
    ++it;

    r->last = last;
    return r;
}

}