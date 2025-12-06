
#pragma once

#include <utility>
#include <coroutine>
#include <concepts>
#include <optional>
#include <memory>
#include <type_traits>
#include <cassert>
#include <exception>
#include <iterator>

namespace hk {

/** Implementation of std::generator.
 */
template<typename T>
class generator {
public:
    using value_type = T;

    static_assert(not std::is_reference_v<value_type>);
    static_assert(not std::is_const_v<value_type>);

    class promise_type {
    public:
        generator get_return_object()
        {
            return generator{handle_type::from_promise(*this)};
        }

        value_type const& value() const noexcept
        {
            assert(_value);
            return *_value;
        }

        static std::suspend_never initial_suspend() noexcept
        {
            return {};
        }

        static std::suspend_always final_suspend() noexcept
        {
            return {};
        }

        std::suspend_always yield_value(value_type const& value) noexcept
        {
            _value = value;
            return {};
        }

        std::suspend_always yield_value(value_type&& value) noexcept
        {
            _value = std::move(value);
            return {};
        }

        void return_void() noexcept {}

        // Disallow co_await in generator coroutines.
        void await_transform() = delete;

        void unhandled_exception() noexcept
        {
            _exception = std::current_exception();
        }

        void rethrow()
        {
            if (auto ptr = std::exchange(_exception, nullptr)) {
                std::rethrow_exception(ptr);
            }
        }

    private:
        std::optional<value_type> _value = {};
        std::exception_ptr _exception = nullptr;
    };

    using handle_type = std::coroutine_handle<promise_type>;

    class value_proxy {
    public:
        value_proxy(value_type const& value) noexcept : _value(value) {}

        value_type const& operator*() const noexcept
        {
            return _value;
        }

    private:
        value_type _value;
    };

    /** A forward iterator which iterates through values co_yieled by the generator-function.
     */
    class const_iterator {
    public:
        using difference_type = ptrdiff_t;
        using value_type = std::decay_t<value_type>;
        using pointer = value_type const *;
        using reference = value_type const&;
        using iterator_category = std::input_iterator_tag;

        explicit const_iterator(handle_type coroutine) : _coroutine{coroutine}
        {
            _coroutine.promise().rethrow();
        }

        /** Resume the generator-function.
         */
        const_iterator& operator++()
        {
            assert(not at_end());
            _coroutine.resume();
            _coroutine.promise().rethrow();
            return *this;
        }

        value_proxy operator++(int)
        {
            auto tmp = value_proxy(_coroutine.promise().value());
            assert(not at_end());
            _coroutine.resume();
            _coroutine.promise().rethrow();
            return tmp;
        }

        /** Retrieve the value co_yielded by the generator-function.
         */
        decltype(auto) operator*() const
        {
            assert(not at_end());
            return _coroutine.promise().value();
        }

        pointer *operator->() const noexcept
        {
            assert(not at_end());
            return std::addressof(_coroutine.promise().value());
        }

        [[nodiscard]] bool at_end() const noexcept
        {
            return not _coroutine or _coroutine.done();
        }

        /** Check if the generator-function has finished.
         */
        [[nodiscard]] bool operator==(std::default_sentinel_t) const noexcept
        {
            return at_end();
        }

    private:
        handle_type _coroutine;
    };

    explicit generator(handle_type coroutine) : _coroutine(coroutine) {}

    generator() = default;
    ~generator()
    {
        if (_coroutine) {
            _coroutine.destroy();
        }
    }

    generator(const generator&) = delete;
    generator& operator=(const generator&) = delete;

    generator(generator&& other) noexcept : _coroutine{std::exchange(other._coroutine, {})} {}

    generator& operator=(generator&& other) noexcept
    {
        if (this == &other) {
            return *this;
        }

        if (_coroutine) {
            _coroutine.destroy();
        }
        _coroutine = std::exchange(other._coroutine, {});
        return *this;
    }

    /** Start the generator-function and return an iterator.
     */
    const_iterator begin() const
    {
        return const_iterator{_coroutine};
    }

    /** Start the generator-function and return an iterator.
     */
    const_iterator cbegin() const
    {
        return const_iterator{_coroutine};
    }

    /** Return a sentinel for the iterator.
     */
    std::default_sentinel_t end() const
    {
        return {};
    }

    /** Return a sentinel for the iterator.
     */
    std::default_sentinel_t cend() const
    {
        return {};
    }

private:
    handle_type _coroutine;
};

}
