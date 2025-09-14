
#pragma once

#include "ast/module_node.hpp"
#include "error/error_list.hpp"
#include "utility/repository_url.hpp"
#include "utility/repository_flags.hpp"
#include "utility/generator.hpp"
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
    void scan_prologues(repository_flags flags);

    /** Recusively clone and scan repositories.
     * 
     * @param force Force scanning even on files that were already parsed.
     */
    void recursive_scan_prologues(repository_flags flags);

    [[nodiscard]] generator<std::pair<repository_url, error_location>> remote_repositories() const;

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

        module_type(std::filesystem::path path);
    };

    struct child_repository_type {
        repository_url url;
        std::unique_ptr<repository> repository;
    };

    /** The path to the repository.
     */
    std::filesystem::path _path;

    /** modules, sorted by path.
     */
    std::vector<module_type> _modules;

    /** The root repository also has a list of child repositories.
     */
    std::vector<child_repository_type> _child_repositories;

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

    /** Get or make a child repository based on the repository_url.
     * 
     * @note It is UNDEFINED BEHAVIOR if @a path is not canonical or is not
     *       inside the repository.
     * @param path The path the module.
     */
    [[nodiscard]] child_repository_type &get_child_repository(repository_url const& url);

};


}
