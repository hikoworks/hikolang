
#pragma once

#include "parse_result.hpp"
#include "ast/node.hpp"
#include "utility/file_cursor.hpp"

namespace hl {


[[nodiscard]] parse_result<ast::node> parse(hl::file_cursor& c);

}
