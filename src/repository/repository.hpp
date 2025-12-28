
#pragma once

#include "parser/parse_context.hpp"
#include "error/error_list.hpp"
#include "utility/repository_url.hpp"
#include "utility/repository_flags.hpp"
#include "utility/generator.hpp"
#include "source.hpp"
#include "module_list.hpp"
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

    [[nodiscard]] friend bool operator==(repository const& lhs, repository const& rhs) noexcept
    {
        return lhs.remote == rhs.remote;
    }

    /** Scan the prologue of each *.hkm in the repository.
     * 
     */
    void scan_prologues(datum_namespace const& guard_namespace, module_list& modules);

    /** Recusively clone and scan repositories.
     * 
     * @param force Force scanning even on files that were already parsed.
     */
    void recursive_scan_prologues(datum_namespace const& guard_namespace, repository_flags flags);

    /** Get the remote repositories imported by this repository.
     * 
     * @pre `scan_prologues()` must be called first.
     */
    [[nodiscard]] generator<ast::import_repository_declaration_node*> remote_repositories() const;

    [[nodiscard]] std::vector<std::unique_ptr<repository>> const& child_repositories() const noexcept
    {
        return _child_repositories;
    }

    /** Get all the anchors declared in this repository.
     * 
     * @note Only sources that pass conditional compilation are returned.
     * @return Pointers to the source files.
     */
    [[nodiscard]] generator<source*> get_anchors() const;

    /** Get all the modules belonging to the anchor declared in this repository.
     * 
     * @note Only sources that pass conditional compilation are returned.
     * @param anchor The anchor that modules belong to.
     * @return Pointers to the source files.
     */
    [[nodiscard]] generator<source*> get_modules_by_anchor(fqname anchor) const;

private:
    /** sources.
     * 
     * @note sorted by path.
     */
    std::vector<std::unique_ptr<source>> _sources_by_path;

    /** sources.
     * 
     * @note sorted by name.
     */
    std::vector<source*> _sources_by_name;

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
     * @retval true The list of modules has changed.
     */
    bool gather_modules();

    /** Evaluate the build guards.
     */
    std::expected<void, hkc_error> evaluate_build_guard(datum_namespace const& ctx);

    /** Parse all the modules in a repository.
     * 
     * @pre `sort_modules()` may need to be called.
     */
    bool parse_prologues();

    /** Get or make a module based on the path.
     * 
     * @note It is UNDEFINED BEHAVIOR if @a path is not canonical or is not
     *       inside the repository.
     * @param path The path the module.
     */
    [[nodiscard]] source &get_module(std::filesystem::path const& path);

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
