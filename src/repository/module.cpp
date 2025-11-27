
#include "module.hpp"
#include "utility/read_file.hpp"
#include "parser/parse_top.hpp"
#include "parser/parse_context.hpp"
#include <cassert>

namespace hk {

module_t::module_t(std::filesystem::path path) : path(std::move(path))
{
    assert(std::filesystem::canonical(this->path) == this->path);
}

void module_t::reset()
{
    prologue_ast = nullptr;
    ast = nullptr;
    lines.clear();
    source_code.clear();
}

[[nodiscard]] std::expected<void, std::error_code> module_t::load()
{
    if (path.empty()) {
        // This module is not based on a file, instead it was source code
        // that was generated, so the text must already be filled in.
        return {};
    }

    // Loop until we read the file until the file hasn't changed on the disk.
    while (true) {
        auto ec = std::error_code{};
        auto const write_time = std::filesystem::last_write_time(path, ec);
        if (ec) {
            return std::unexpected{ec};
        }

        if (write_time == source_code_time) {
            // The file was loaded and it wasn't changed on disk.
            return {};
        }

        // Reset the parser, the file is being reloaded.
        reset();

        // Read the file and append 8 nul-bytes.
        if (auto optional_text = read_file(path, 8); not optional_text) {
            return std::unexpected{std::make_error_code(std::errc::io_error)};

        } else {
            source_code = std::move(optional_text).value();
            source_code_time = write_time;
            lines.add(source_code.data(), 0, path.string());
        }
    }
}

void module_t::parse_prologue()
{
    auto ctx = parse_context(errors, lines);
    auto p = const_cast<char const *>(source_code.data());
    if (auto optional_ast = parse_top(p, ctx, false)) {
        prologue_ast = std::move(optional_ast).value();
    }
}

}

