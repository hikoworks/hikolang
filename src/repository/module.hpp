
#pragma once

#include "ast/top_node.hpp"
#include "utility/semantic_version.hpp"
#include "error/error_list.hpp"
#include "tokenizer/line_table.hpp"
#include <expected>
#include <system_error>
#include <filesystem>
#include <memory>

namespace hk {

struct module_t {
    /** The location of the module's file.
     */
    std::filesystem::path path;

    /** The full name of the module.
     */
    fqname name;

    /** The version of the module.
     * 
     */
    semantic_version version;


    /** List of errors found.
     */
    mutable error_list errors;

    /** The modules source code.
     * 
     * The source code must have 8 nul characters at the end of the text.
     * 
     * - Empty when the source code is out-of-date.
     */
    std::string source_code;

    /** This is the timestamp when the source_code was read from disk.
     * 
     * This is used to detect if the file has been modified since it was last
     * parsed.
     */
    std::filesystem::file_time_type source_code_time = {};

    /** This is the table of line synchronization points.
     * 
     * This is used to query where a character is located.
     */
    line_table lines;

    /** The abstract syntax tree of just the prologue.
     * 
     * - Empty when the source code is out-of-data.
     */
    std::unique_ptr<ast::top_node> prologue_ast;

    /** The abstract syntax tree of the module.
     * 
     * - Empty when the source code is out-of-data.
     */
    std::unique_ptr<ast::top_node> ast;

    module_t(std::filesystem::path path);

private:
    /** Reset compilation state when the file has been loaded.
     * 
     */
    void reset();

    /** Load the file if it has changed on disk.
     * 
     */
    std::expected<void, std::error_code> load();

    void parse_prologue();
};

}
