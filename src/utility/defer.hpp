

#pragma once

#include <concepts>
#include <functional>

namespace hk {

class defer {
public:
    ~defer()
    {
        if (_f) {
            _f();
        }
    }

    [[nodiscard]] defer(std::function<void()> && arg) : _f(std::move(arg)) {}
    [[nodiscard]] defer(std::function<void()> const& arg) : _f(arg) {}

    void cancel()
    {
        _f = nullptr;
    }

private:
    std::function<void()> _f;
};


}
