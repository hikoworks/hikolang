

#pragma once

#include <concepts>
#include <functional>
#include <cassert>

namespace hk {

/** Defer execution till destruction.
 * 
 * Delay execution to the end of a function/block; useful for cleaning non-scoped variables
 * with older C-style APIs.
 */
class defer {
public:
    /** If not cancelled, execute the deferred callable object.
     */
    ~defer()
    {
        if (_f) {
            _f();
        }
    }

    /** Construct with a callable object.
     */
    [[nodiscard]] defer(std::function<void()> && arg) : _f(std::move(arg)) {}

    /** Construct with a callable object.
     */
    [[nodiscard]] defer(std::function<void()> const& arg) : _f(arg) {}

    /** Cancel execution of the deferred execution.
     */
    void cancel()
    {
        assert(_f != nullptr);
        _f = nullptr;
    }

private:
    std::function<void()> _f;
};


}
