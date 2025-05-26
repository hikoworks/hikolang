
#ifndef HL_MODULE_HPP
#define HL_MODULE_HPP

namespace hl {

class module {
public:
    /** Path to the module file.
     *
     * This is the canonical path to the module file. This is done
     * so that we can match module files parsed from different locations.
     */
    [[nodiscard]] std::string relative_path() const;

};


[[nodiscard]] module const& get_module(std::size_t module_id) noexcept;

}

#endif // HL_MODULE_HPP