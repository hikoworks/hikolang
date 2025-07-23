
#pragma once

#include "utility/fqname.hpp"
#include "utility/semantic_version.hpp"
#include "ast/module_node.hpp"
#include <filesystem>
#include <map>
#include <memory>

namespace hk {

enum class repository_kind {
    /** The host repository.
     * 
     * This is the repository where the compiler is starting from.
     */
    host,

    /** A repository that is in a local directory.
     * 
     * This repository is likely a development repository that is being
     * worked on locally. It will always win over remote repositories
     * when scanning for packages.
     */
    local,

    /** A remote git-repository.
     */
    git_remote,

    /** A remote zip-file
     */
    zip_remote,
};


struct repository {
    /** Filesystem path of a potential cloned repository.
     */
    std::filesystem::path path;

    repository_kind kind = repository_kind::host;

    /** URL to the remote repository.
     * This is used for both git and zip repositories.
     */
    std::string url;

    /** Branch of the repository to use.
     * Used for git repositories.
     */
    std::string branch;

    /** All modules in the repository. 
     * 
     * This modules may be: modules, packages, applications, or libraries.
     */
    std::map<fqname, std::unique_ptr<ast::module_node>> module_by_name;

    [[nodiscard]] friend bool operator==(repository const& lhs, repository const& rhs) noexcept
    {
        return lhs.path == rhs.path;
    }
    [[nodiscard]] friend auto operator<=>(repository const& lhs, repository const& rhs) noexcept
    {
        return lhs.path <=> rhs.path;
    }

};


/** Scan the prologue of all files in a repository.
 * 
 * This function will scan all files in the given path and extract the prologue
 * from each file. The prologue is the first few lines of the file that contain
 * metadata about the file, such as the module name, version, and dependencies.
 * 
 * @param path The path to the repository to scan.
 * @return A module for each .hkm file found.
 */
[[nodiscard]] std::vector<ast::module_node_ptr> prologue_scan_repository(std::filesystem::path const& path);


}
