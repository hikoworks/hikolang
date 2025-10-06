
#include "parse_top.hpp"
#include "parse_import_module_declaration.hpp"
#include "parse_import_repository_declaration.hpp"
//#include "parse_import_library_declaration.hpp"
#include "parse_module_declaration.hpp"
#include "parse_program_declaration.hpp"
#include "parse_library_declaration.hpp"
#include "consume.hpp"
#include "ast/module_node.hpp"
#include "ast/program_node.hpp"
#include "ast/library_node.hpp"
#include "error/errors.hpp"
#include "tokenizer/tokenizer.hpp"
#include "utility/fixed_fifo.hpp"

namespace hk {


[[nodiscard]] parse_result_ptr<ast::top_node> parse_top(token_iterator& it, parse_context &ctx, bool only_prologue)
{
    auto const first = it->first;
    auto r = std::unique_ptr<ast::top_node>{};
    if (auto module_node = parse_module_declaration(it, ctx)) {
        auto r_ = std::make_unique<ast::module_node>(first);
        r_->declaration = std::move(module_node).value();
        r = std::move(r_);

    } else if (module_node.error()) {
        return std::unexpected{module_node.error()};

    } else if (auto program_node = parse_program_declaration(it, ctx)) {
        auto r_ = std::make_unique<ast::program_node>(first);
        r_->declaration = std::move(program_node).value();
        r = std::move(r_);

    } else if (program_node.error()) {
        return std::unexpected{program_node.error()};

    } else if (auto library_node = parse_library_declaration(it, ctx)) {
        auto r_ = std::make_unique<ast::library_node>(first);
        r_->declaration = std::move(library_node).value();
        r = std::move(r_);

    } else if (library_node.error()) {
        return std::unexpected{library_node.error()};

    } else {
        return ctx.e.add(first, it->last, error::missing_top_declaration);
    }

    while (*it == "import" or *it == "syntax") {
        auto const first = it->first;

        // parse_import_repository_declaration() must be executed before
        // parse_import_module_declaration() so that it can handle modules
        // that start with `zip` or `git`.
        if (auto remote_repository = parse_import_repository_declaration(it, ctx)) {
            r->remote_repositories.push_back(std::move(remote_repository).value());

        } else if (remote_repository.error()) {
            consume_rest_of_statement(it);

        } else if (auto import_module = parse_import_module_declaration(it, ctx)) {
            r->module_imports.push_back(std::move(import_module).value());

        } else if (import_module.error()) {
            consume_rest_of_statement(it);

        } else {
            return ctx.e.add(first, it->last, error::invalid_prologue_statement);
        }
    }

    if (only_prologue) {
        // If we are only parsing the prologue, we stop here.
        auto const last = it->first;
        r->last = last;
        return r;
    }

    auto const last = it->first;
    r->last = last;
    return r;
}

[[nodiscard]] parse_result_ptr<ast::top_node> parse_top(hk::file_cursor& c, parse_context &ctx, bool only_prologue)
{
    auto token_generator = hk::tokenize(c);
    auto lazy_tokens = lazy_vector{token_generator.cbegin(), token_generator.cend()};

    auto it = lazy_tokens.cbegin();
    if (auto top = parse_top(it, ctx, only_prologue)) {
        //top->upstream_paths = c.upstream_paths();
        return top;
    } else if (top.error()) {
        return std::unexpected{top.error()};
    } else {
        std::unreachable();
    }
}

}
