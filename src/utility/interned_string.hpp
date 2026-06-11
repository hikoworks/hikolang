
#pragma once

#include <set>
#include <string>
#include <string_view>
#include <mutex>

namespace hk {
    
template<typename CharT>
class basic_interned_string {
public:
    using value_type = std::basic_string<CharT>::value_type;
    using size_type = std::basic_string<CharT>::size_type;
    using difference_type = std::basic_string<CharT>::difference_type;
    using const_reference = std::basic_string<CharT>::const_reference;
    using const_pointer = std::basic_string<CharT>::const_pointer;
    using const_iterator = std::basic_string<CharT>::const_iterator;

    basic_interned_string()
        : _ptr(basic_interned_string::intern(""))
    {}

    basic_interned_string(std::string_view sv)
        : _ptr(basic_interned_string::intern(sv))
    {}

    void clear() noexcept
    {
        _ptr = basic_interned_string::intern("");
    }

    [[nodiscard]] std::basic_string<CharT> const& string() const noexcept
    {
        return *_ptr;
    }

    operator std::basic_string<CharT> const&() const noexcept
    {
        return this->string;
    }

    [[nodiscard]] std::basic_string_view<CharT> string_view() const noexcept
    {
        return *_ptr;
    }

    operator std::basic_string_view<CharT>() const noexcept
    {
        return this->string_view();
    }

    [[nodiscard]] const_pointer data() const noexcept
    {
        return _ptr->data();
    }

    [[nodiscard]] const_pointer c_str() const noexcept
    {
        return _ptr->c_str();
    }

    [[nodiscard]] size_type size() const noexcept
    {
        return _ptr->size();
    }

    [[nodiscard]] bool empty() const noexcept
    {
        return _ptr->empty();
    }

    [[nodiscard]] const_iterator begin() const noexcept
    {
        return _ptr->begin();
    }

    [[nodiscard]] const_iterator end() const noexcept
    {
        return _ptr->end();
    }

    [[nodiscard]] const_iterator cbegin() const noexcept
    {
        return _ptr->cbegin();
    }

    [[nodiscard]] const_iterator cend() const noexcept
    {
        return _ptr->cend();
    }

    [[nodiscard]] const_reference operator[](size_type index) const
    {
        return (*_ptr)[index];
    }

    [[nodiscard]] const_reference at(size_type index) const
    {
        return _ptr->at(index);
    }

    [[nodiscard]] const_reference front() const noexcept
    {
        return _ptr->front();
    }
   
    [[nodiscard]] const_reference back() const noexcept
    {
        return _ptr->back();
    }
   
    [[nodiscard]] bool starts_with(std::basic_string_view<CharT> sv) const noexcept
    {
        return _ptr->starts_with(sv);
    }
   
    [[nodiscard]] bool ends_with(std::basic_string_view<CharT> sv) const noexcept
    {
        return _ptr->ends_with(sv);
    }
   
    [[nodiscard]] bool contains(std::basic_string_view<CharT> sv) const noexcept
    {
        return _ptr->contains(sv);
    }
   
    [[nodiscard]] friend bool operator==(basic_interned_string lhs, basic_interned_string rhs)
    {
        return lhs._ptr == rhs._ptr;
    }

    [[nodiscard]] friend auto operator<=>(basic_interned_string lhs, basic_interned_string rhs)
    {
        return lhs.string_view() <=> rhs.string_view();
    }

    [[nodiscard]] friend bool operator==(basic_interned_string lhs, std::basic_string_view<CharT> rhs)
    {
        return lhs.string_view() == rhs;
    }

    [[nodiscard]] friend auto operator<=>(basic_interned_string lhs, std::basic_string_view<CharT> rhs)
    {
        return lhs.string_view() <=> rhs;
    }

    [[nodiscard]] friend bool operator==(std::basic_string_view<CharT> lhs, basic_interned_string rhs)
    {
        return lhs == rhs.string_view();
    }

    [[nodiscard]] friend auto operator<=>(std::basic_string_view<CharT> lhs, basic_interned_string rhs)
    {
        return lhs <=> rhs.string_view();
    }


private:
    static inline std::set<std::basic_string<CharT>> _strings;
    static inline std::mutex _mutex;

    const std::basic_string<CharT>* _ptr;

    static const std::basic_string<CharT>* intern(std::basic_string_view<CharT> sv) {
        auto const _= std::scoped_lock(_mutex);

        auto [it, inserted] = _strings.emplace(sv);
        return std::addressof(*it);
    }
};

using interned_string = basic_interned_string<char>;

}
