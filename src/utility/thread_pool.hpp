
#pragma once

#include <future>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <stop_token>
#include <cassert>

namespace hk {

class thread_pool_base {
public:
    virtual ~thread_pool_base() = default;
    virtual void call() = 0;

private:
};

template<typename F, typename... Args>
class thread_pool_item : public thread_pool_base {
public:
    using return_type = std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>;

    thread_pool_item(F&& f, Args&&... args) : _f(std::forward<F>(f)), _args{std::forward<Args>(args)...} {}

    [[nodiscard]] std::future<return_type> get_future()
    {
        return _promise.get_future();
    }

    void call() override
    {
        try {
            _promise.set_value(std::apply(_f, _args));
        } catch (std::exception const& e) {
            _promise.set_exception(std::current_exception());
        }
    }

private:
    F _f;
    std::tuple<std::decay_t<Args>...> _args;
    std::promise<return_type> _promise;
};

class thread_pool {
public:
    ~thread_pool() = default;

    thread_pool(size_t max_num_threads);

    void schedule(std::unique_ptr<thread_pool_base> work);

    template<typename F, typename... Args>
    std::future<std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>> operator()(F&& f, Args&&... args)
    {
        using item_type = thread_pool_item<F, Args...>;
        auto work = std::make_unique<item_type>(std::forward<F>(f), std::forward<Args>(args)...);
        auto future = work->get_future();
        schedule(std::move(work));
        return future;
    }

private:
    struct thread_item {
        std::jthread thread;
        std::unique_ptr<thread_pool_base> work;
        std::condition_variable new_work_condition;

        ~thread_item();

        thread_item(thread_pool* pool);
    };

    [[nodiscard]] std::pair<thread_item*, std::unique_lock<std::mutex>> idle_thread(std::unique_lock<std::mutex> lock);

    static void runner(std::stop_token token, thread_pool::thread_item* item, thread_pool* pool);

    std::mutex _mutex;
    std::condition_variable _idle_thread_condition;
    size_t _max_num_threads;
    std::vector<std::unique_ptr<thread_item>> _threads;
};

[[nodiscard]] inline size_t num_cpus()
{
    if (auto _ = std::thread::hardware_concurrency()) {
        return _;
    } else {
        return 1;
    }
}

inline std::atomic<size_t> _max_num_threads = num_cpus();

[[nodiscard]] inline size_t max_num_threads()
{
    return _max_num_threads.load(std::memory_order::relaxed);
}

inline size_t set_max_num_threads(size_t new_max_threads)
{
    assert(new_max_threads != 0);
    return _max_num_threads.exchange(new_max_threads, std::memory_order::relaxed);
}

template<typename F, typename... Args>
std::future<std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>> async_on_pool(F&& f, Args&&... args)
{
    static std::unique_ptr<thread_pool> pool = std::make_unique<thread_pool>(max_num_threads());

    return (*pool)(std::forward<F>(f), std::forward<Args>(args)...);
}

} // namespace hk
