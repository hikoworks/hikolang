
#pragma once

#include "ast/module_node.hpp"
#include <filesystem>
#include <memory>

namespace hk {

struct module_t {
    enum class state_type {
        /** The file has not been parsed yet.
         * 
         * When it is detected that the module's file has been modified this
         * is the value the state is set to.
         */
        out_of_date,

        /** The prologue is scanned
         *
         * After prologue scanning the full repository it is possible to
         * calculate the compilation order.
         */
        prologue,

        /** The full module file is parsed.
         */
        parsed,
    };

    /** The location of the module's file.
     */
    std::filesystem::path path;

    /** This is the timestamp of the file when it was parsed.
     * 
     * This is used to detect if the file has been modified since it was last
     * parsed.
     */
    std::filesystem::file_time_type parse_time;

    /** State denoting the progress of parsing and compiling the module.
     */
    state_type state = state_type::out_of_date;

    /** The abstract syntax tree of the module.
     */
    std::unique_ptr<ast::module_node> ast;

    module_t(std::filesystem::path path);
};

}
