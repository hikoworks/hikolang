
#pragma once

#include <vector>
#include <filesystem>

namespace hk {

class tokenizer_context
{
public:
    constexpr tokenizer_context() = default;

    /** Add a new path to the context.
     * 
     * This function returns a index of the path in the context.
     * 
     * This index together with the line number and column number are used to
     * create a start and end location for each node in the AST.
     * To reduce memory usage the path is reduced to an index. 
     * 
     * @param path The path to add.
     * @return The index of the path in the context.
     */
    size_t new_path(std::filesystem::path const& path)
    {
        auto const it = std::find(_paths.begin(), _paths.end(), path);
        if (it != _paths.end()) {
            return std::distance(_paths.begin(), it);
        }
        _paths.push_back(path);
        return _paths.size() - 1;
    }

    /** Get the path at the given index.
     * 
     * @param index The index of the path to get.
     * @return The path at the given index.
     */
    [[nodiscard]] std::filesystem::path get_path(size_t index) const
    {
        if (index < _paths.size()) {
            return _paths[index];
        }
        throw std::out_of_range("Invalid path index");
    }

private:
    std::vector<std::filesystem::path> _paths;
};

}
