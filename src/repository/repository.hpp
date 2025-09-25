
#pragma once

#include "module_list.hpp"
#include "parser/parse_context.hpp"
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
    /** The remote url to the repository.
     */
    repository_url remote = {};

    /** The path to the repository.
     */
    std::filesystem::path path = {};

    /** A flag for algorithms to check if a repository was visited.
     */
    bool mark = false;

    /** Construct a repository.
     * 
     * @param path The disk location of the repository.
     * @param remote The remote location of the repository.
     */
    repository(std::filesystem::path path, repository_url remote = repository_url{});

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

    [[nodiscard]] std::vector<std::unique_ptr<repository>> const& child_repositories() const noexcept
    {
        return _child_repositories;
    }

private:
    

    /** modules.
     * 
     * @note sorted by path.
     */
    module_list _modules;

    /** The root repository also has a list of child repositories.
     * 
     * @note sorted by url.
     */
    std::vector<std::unique_ptr<repository>> _child_repositories;

    /** Gather all modules in the repository.
     * 
     * This finds all the files with the `.hkm` extension.
     * It excludes any files and directories that start with a dot `.` or
     * underscore `_`.
     * 
     * @retval true The list of modules have changed; the list of modules
     *              must be sorted.
     */
    bool gather_modules();

    /** Sort modules in compilation order.
     */
    void sort_modules();

    /** Parse all the modules in a repository.
     * 
     * @pre `sort_modules()` may need to be called.
     * @param context The parse context.
     * @param new_state Parse the modules upto this state.
     */
    bool parse_modules(parse_context &context, module_t::state_type new_state, repository_flags flags);

    /** Get or make a module based on the path.
     * 
     * @note It is UNDEFINED BEHAVIOR if @a path is not canonical or is not
     *       inside the repository.
     * @param path The path the module.
     */
    [[nodiscard]] module_t &get_module(std::filesystem::path const& path);

    /** Get or make a child repository based on the remote.
     * 
     * @param remote URL of the remote repository.
     * @param child_path The local path to the repository.
     * @return A reference to the repository object.
     */
    [[nodiscard]] repository& get_child_repository(repository_url const& remote, std::filesystem::path child_path);

    /** Remove the child repository based on the remote.
     * 
     * @param remote URL of the remote repository.
     */
    void erase_child_repository(repository_url const& remote);

};


}
