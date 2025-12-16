
#pragma once

#include "build_guard_value.hpp"
#include "utility/semantic_version.hpp"
#include "utility/fqname.hpp"
#include <bit>

namespace hk {

class build_guard_context {
public:
    build_guard_context(build_guard_context const&) = delete;
    build_guard_context(build_guard_context&&) = delete;
    build_guard_context& operator=(build_guard_context const&) = delete;
    build_guard_context& operator=(build_guard_context&&) = delete;
    build_guard_context() = default;

    /** Get a value by name and target.
     * 
     * @param name The name of the value.
     * @return A pointer to the value, or nullptr if it does not exist.
     */
    [[nodiscard]] build_guard_value const* get(fqname const &name) const;

    /** Set a custom value from the environment / command-line.
     * 
     * @param name The name of the value
     * @param value The value
     * @return A reference to the stored value.
     */
    build_guard_value& set(fqname name, build_guard_value value);

private:
    struct item_type {
        fqname name;
        build_guard_value value;
    };
    std::vector<item_type> _items;
};


}
