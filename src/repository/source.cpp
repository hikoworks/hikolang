
#include "source.hpp"
#include "repository.hpp"
#include "utility/read_file.hpp"
#include "parser/parse_top.hpp"
#include "parser/parse_context.hpp"
#include <cassert>

namespace hk {

source::source(hk::repository &parent, std::filesystem::path path) :
    _parent(gsl::make_not_null(&parent)), _source_filename(std::move(path))
{
    assert(std::filesystem::canonical(this->path()) == this->path());
}

source::source(hk::repository &parent, fqname generating_module, size_t lineno) :
    _parent(gsl::make_not_null(&parent)), _source_filename(std::pair{std::move(generating_module), lineno})
{

}

[[nodiscard]] bool source::is_generated() const noexcept
{
    return not std::holds_alternative<std::filesystem::path>(_source_filename);
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
    assert(std::holds_alternative<std::filesystem::path>(_source_filename));
    return std::get<std::filesystem::path>(_source_filename);
}

[[nodiscard]] ast::top_declaration_node& source::file_declaration() const
{
    return top().declaration();
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
        _prologue_ast->fixup_top(this);
    } else if (to_bool(optional_ast.error())) {
        return std::unexpected{optional_ast.error()};
    } else {
        std::unreachable();
    }

    _ast = nullptr;
    return true;
}

std::expected<void, hkc_error> source::evaluate_build_guard(datum_namespace const& ctx)
{
    auto last_error = hkc_error::none;

    if (_prologue_ast != nullptr) {
        if (auto r = _prologue_ast->evaluate_build_guard(ctx); not r) {
            last_error = r.error();
        }
    }

    if (_ast != nullptr) {
        if (auto r = _ast->evaluate_build_guard(ctx); not r) {
            last_error = r.error();
        }
    }
    
    if (last_error != hkc_error::none) {
        return std::unexpected{last_error};
    }

    return {};
}

[[nodiscard]] generator<ast::import_repository_declaration_node *> source::remote_repositories() const
{
    if (_prologue_ast == nullptr) {
        // The prologue failed to parse. 
        co_return;
    }

    for (auto &node : _prologue_ast->remote_repositories) {
        if (node->enabled()) {
            co_yield node.get();
        }
    }
}

[[nodiscard]] std::strong_ordering cmp_sources(source const& lhs, source const& rhs) noexcept
{
    if (auto r = lhs.source_filename().index() <=> rhs.source_filename().index(); r != std::strong_ordering::equal) {
        return r;
    } else if (lhs.source_filename().index() == 0) {
        return std::get<std::filesystem::path>(lhs.source_filename()) <=> std::get<std::filesystem::path>(rhs.source_filename());
    } else {
        return std::get<std::pair<fqname, size_t>>(lhs.source_filename()) <=> std::get<std::pair<fqname, size_t>>(rhs.source_filename());
    }
}

[[nodiscard]] std::partial_ordering cmp_names(source const& lhs, source const& rhs) noexcept
{
    if (lhs.name().index() == 0 or rhs.name().index() == 0) {
        return std::partial_ordering::unordered;
    } else if (auto _ = lhs.name().index() <=> rhs.name().index(); _ != std::strong_ordering::equal) {
        return _;
    } else {
        switch (lhs.name().index()) {
        case 1:
            if (auto _ = std::get<1>(lhs.name()) <=> std::get<1>(rhs.name()); _ == std::strong_ordering::equal) {
                return lhs.repository().path <=> rhs.repository().path;
            } else {
                return _;
            }
        case 2:
            if (auto _ = std::get<2>(lhs.name()) <=> std::get<2>(rhs.name()); _ == std::strong_ordering::equal) {
                return lhs.repository().path <=> rhs.repository().path;
            } else {
                return _;
            }
        case 3:
            if (auto _ = std::get<3>(lhs.name()) <=> std::get<3>(rhs.name()); _ == std::strong_ordering::equal) {
                return lhs.repository().path <=> rhs.repository().path;
            } else {
                return _;
            }
        }
        std::unreachable();
    }
}

[[nodiscard]] std::strong_ordering cmp_versions(source const& lhs, source const& rhs) noexcept
{
    if (auto _ = lhs.kind() <=> rhs.kind(); _ != std::strong_ordering::equal) {
        return _;
    }

    if (auto _ = lhs.version() <=> rhs.version(); _ == std::strong_ordering::equal) {
        return lhs.repository().path <=> rhs.repository().path;
    } else {
        return _;
    }
}

[[nodiscard]] bool is_child_of(source const& child, source const& parent) noexcept
{
    return is_child_of(child.module_name(), parent.module_name());
}

} // namespace hk
