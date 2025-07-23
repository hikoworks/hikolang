
#pragma once

#include "ast/module_node.hpp"
#include "utility/file_cursor.hpp"

namespace hk {

/** Parse a module from the given file cursor.
 * 
 * Even when there are errors this function will return a module-node.
 * 
 * @note The file should be a module file with the `.hkm` extension.
 * @param c The file cursor to parse from.
 * @param only_prologue If true, only parse the prologue of the module (declaration and imports).
 * @return A parsed module, or what was parsed.
 */
[[nodiscard]] ast::module_node_ptr parse_module(hk::file_cursor& c, bool only_prologue);


}