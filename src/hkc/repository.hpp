
#pragma once

#include "ast/module_node.hpp"
#include "error/error_list.hpp"
#include <filesystem>
#include <memory>
#include <chrono>
#include <vector>

namespace hk {

class repository {
public:
    repository(std::filesystem::path path);

    /** Scan the prologue of each *.hkm in the repository.
     * 
     * @param force Force scanning even on files that were already parsed.
     */
    void scan_prologues(bool force);



private:
    struct module_type {
        std::filesystem::path path;

        /** This is the timestamp of the file when it was parsed.
         */
        std::filesystem::file_time_type time;

        std::unique_ptr<ast::module_node> ast;

        /** This flag is used to determine if a module has been removed from
         * the filesystem.
         */
        bool touched = false;

        error_list errors;

        module_type(std::filesystem::path path);
    };

    std::filesystem::path _path;

    /** modules, sorted by path.
     */
    std::vector<module_type> _modules;

    /** Unset the touch flag on all modules.
     * 
     * @param remove Remove modules if already not touched.
     */
    void untouch(bool remove);

    /** Get or make a module based on the path.
     * 
     * @note It is UNDEFINED BEHAVIOR if @a path is not canonical or is not
     *       inside the repository.
     * @param path The path the module.
     */
    [[nodiscard]] module_type &get_module(std::filesystem::path const& path);
};


}
