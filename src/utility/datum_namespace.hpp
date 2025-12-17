
#pragma once

#include "datum.hpp"
#include "utility/semantic_version.hpp"
#include "utility/fqname.hpp"
#include <bit>

namespace hk {

class datum_namespace {
public:
    datum_namespace(datum_namespace const&) = delete;
    datum_namespace(datum_namespace&&) = delete;
    datum_namespace& operator=(datum_namespace const&) = delete;
    datum_namespace& operator=(datum_namespace&&) = delete;
    datum_namespace() = default;

    /** Get a value by name and target.
     * 
     * @param name The name of the value.
     * @return A pointer to the value, or nullptr if it does not exist.
     */
    [[nodiscard]] datum const* get(fqname const &name) const;

    /** Set a custom value from the environment / command-line.
     * 
     * @param name The name of the value
     * @param value The value
     * @return A reference to the stored value.
     */
    datum& set(fqname name, datum value);

    /** Remove a value.
     */
    void remove(fqname const& name);

private:
    struct item_type {
        fqname name;
        datum value;
    };
    std::vector<item_type> _items;
};


}
