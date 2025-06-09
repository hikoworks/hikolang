
#include "cursor.hpp"
#include "file.hpp"
#include <cassert>

namespace hl {

constexpr void cursor::set_line(std::string file_name, size_t line)
{
    auto &file = get_file(file_name);
    _source_file_id = file.id();
}

}