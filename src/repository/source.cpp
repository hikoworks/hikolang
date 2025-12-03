
#include "source.hpp"
#include "utility/read_file.hpp"
#include "parser/parse_top.hpp"
#include "parser/parse_context.hpp"
#include <cassert>

namespace hk {

source::source(repository &parent, std::filesystem::path path) :
    _parent(gsl::make_not_null(parent)), _source(std::move(path))
{
    assert(std::filesystem::canonical(this->path()) == this->path());
}

source::source(repository &parent, fqname generating_module, size_t lineno) :
    _parent(gsl::make_not_null(parent)), _source(std::pair{std::move(generating_module), lineno})
{

}

[[nodiscard]] bool source::is_generated() const noexcept
{
    return not std::holds_alternative<std::filesystem::path>(_source);
}

[[nodiscard]] source::kind_type source::kind() const noexcept
{
    switch (_name.index()) {
    case 0:
        return kind_type::unknown;
    case 1:
        return kind_type::module;
    case 2:
        return kind_type::program;
    case 3:
        return kind_type::unknown;
    }
    std::unreachable();
}

[[nodiscard]] std::filesystem::path const& source::path() const
{
    assert(std::holds_alternative<std::filesystem::path>(_source));
    return std::get<std::filesystem::path>(_source);
}

[[nodiscard]] std::expected<bool, std::error_code> source::load()
{
    auto modified = false;

    if (is_generated()) {
        return modified;
    }

    // Loop until we read the file until the file hasn't changed on the disk.
    while (true) {
        auto ec = std::error_code{};
        auto const write_time = std::filesystem::last_write_time(path(), ec);
        if (ec) {
            return std::unexpected{ec};
        }

        if (write_time == _source_code_time) {
            // The file was loaded and it wasn't changed on disk.
            return modified;
        }

        _source_code.clear();
        _lines.clear();
        _errors.clear();
        modified = true;

        // Read the file and append 8 nul-bytes.
        if (auto optional_text = read_file(path(), 8); not optional_text) {
            return std::unexpected{std::make_error_code(std::errc::io_error)};

        } else {
            _source_code = std::move(optional_text).value();
            _source_code_time = write_time;
            _lines.add(_source_code.data(), 0, path().string());
        }
    }
}

std::expected<bool, std::error_code> source::parse_prologue()
{
    if (auto optional_modified = load(); not optional_modified) {
        return std::unexpected{optional_modified.error()};
    } else if (*optional_modified == false) {
        return false;
    }

    _prologue_ast = nullptr;

    auto errors = error_list{};
    auto ctx = parse_context(errors, _lines);
    auto p = const_cast<char const*>(_source_code.data());
    if (auto optional_ast = parse_top(p, ctx, false)) {
        _prologue_ast = std::move(optional_ast).value();
    } else if (optional_ast.error()) {
        return std::unexpected{std::make_error_code(optional_ast.error())};
    } else {
        std::unreachable();
    }

    _ast = nullptr;
    return true;
}

} // namespace hk
