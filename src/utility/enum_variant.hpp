
#pragma once

#include <variant>
#include <utility>

namespace hk {

/** A variant type that holds an enum tag.
 * 
 * This class is a wrapper around std::variant that allows you to use an enum type
 * as the tag for the variant. It provides a way to access the current tag and
 * to manipulate the variant using the enum values.
 * 
 * @tparam E The enum type used as the tag.
 * @tparam Types The types that can be held by the variant.
 */
template<typename E, typename... Types>
class enum_variant {
public:
    using enum_type = E;

    /** Default constructor.
     */
    constexpr enum_variant() 
        requires std::is_default_constructible_v<std::variant<Types...>> = default;

    /** Constructs an enum_variant from a specific type, with the first enum value.
     * 
     * @param arg The value to construct the variant with.
     */
    template<typename Arg>
    constexpr enum_variant(Arg&& arg)
        requires std::is_constructible_v<std::variant<Types...>, Arg>
        : _value(std::forward<Arg>(arg)) {}
        
    [[nodiscard]] constexpr friend bool operator==(enum_variant const&, enum_variant const&) noexcept = default;
    [[nodiscard]] constexpr friend auto operator<=>(enum_variant const&, enum_variant const&) noexcept = default;

    /** Get the current tag of the variant. 
     */
    [[nodiscard]] constexpr enum_type tag() const noexcept
    {
        return static_cast<enum_type>(_value.index());
    }

    /** Get the current tag of the variant as an enum value.
     * 
     * This allows you to use the enum value directly in comparisons or other operations.
     */
    [[nodiscard]] constexpr operator enum_type() const noexcept
    {
        return tag();
    }

    /** Get the current tag of the variant as an enum value.
     * 
     * This allows you to use the enum value directly in comparisons or other operations.
     */
    [[nodiscard]] constexpr enum_type operator*() noexcept
    {
        return tag();
    }

    /** Emplace a new value into the variant.
     * 
     * @tparam Tag The enum value that corresponds to the type being emplaced.
     * @param Args The arguments to construct the value with.
     * @return A reference to the emplaced value.
     */
    template<enum_type Tag, typename... Args>
    [[nodiscard]] constexpr auto& emplace(Args&&... args)
    {
        return this->_value.template emplace<static_cast<std::size_t>(Tag)>(std::forward<Args>(args)...);
    }

    template<typename T>
    [[nodiscard]] constexpr friend T& get(enum_variant& rhs)
    {
        return std::get<T>(rhs._value);
    }

    template<typename T>
    [[nodiscard]] constexpr friend T const& get(enum_variant const& rhs)
    {
        return std::get<T>(rhs._value);
    }

    template<enum_type Tag>
    [[nodiscard]] constexpr friend auto& get(enum_variant& rhs)
    {
        return std::get<static_cast<std::size_t>(Tag)>(rhs._value);
    }

    template<enum_type Tag>
    [[nodiscard]] constexpr friend auto const& get(enum_variant const& rhs)
    {
        return std::get<static_cast<std::size_t>(Tag)>(rhs._value);
    }

    template<enum_type Tag>
    [[nodiscard]] constexpr friend bool holds_alternative(enum_variant const& rhs) noexcept
    {
        return std::holds_alternative<static_cast<std::size_t>(Tag)>(rhs._value);
    }

    template<enum_type Tag>
    [[nodiscard]] constexpr friend auto *get_if(enum_variant *rhs) noexcept
    {
        return std::get_if<static_cast<std::size_t>(Tag)>(std::addressof(rhs->_value));
    }

private:
    using variant_type = std::variant<Types...>;
    variant_type _value = {};
};

} // namespace hk
