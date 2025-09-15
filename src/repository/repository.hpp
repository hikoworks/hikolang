
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

    /** modules.
     * 
     * @note sorted by path.
     */
    std::vector<module_type> _modules;

    /** The root repository also has a list of child repositories.
     * 
     * @note sorted by url.
     */
    std::vector<std::unique_ptr<repository>> _child_repositories;

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
