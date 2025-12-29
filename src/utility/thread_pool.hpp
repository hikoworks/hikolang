
#pragma once

#include <future>
#include <thread>
#include <condition_variable>
#include <mutex>

namespace hk {

class thread_pool_base {
public:
    virtual void call();

private:
};

template<typename F, typename... Args>
class thread_pool_item : thread_pool_base {
public:
    using return_type = std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>;

    thread_pool_item(F&& f, Args&&... args) : f(std::forward<F>(f)), args{std::forward<Args>(args)...} {}

    [[nodiscard]] std::future<return_type> get_future()
    {
        return _promise.get_future();
    }

    void call() override
    {
        try {
            _promise.set_value(std::apply(f, args));
        } catch (std::exception const &e) {
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
    thread_pool(size_t num_threads) : _num_threads(num_threads) {}

    void schedule(std::unique_ptr<thread_pool_base> work)
    {
        auto lock = std::unique_lock{_mutex};
        while (true) {
            if (auto ti_ptr = idle_thread()) {
                ti_ptr->work = std::move(work);
                ti_ptr->empty.notify_one();
                return;
            }
            _full.wait(lock);
        }
    }

    template<typename F, typename... Args>
    std::future<std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>> operator(F&& f, Args&&... args)
    {
        using item_type = thread_pool_item<F, Args...>;
        auto work = std::make_unique<item_type>(std::forward<F>(f), std::forward<Args>(args)...);
        auto future = work.get_future();

        
    }

private:
    struct thread_item {
        std::jthread thread;
        std::unique_ptr<thread_pool_base> work;
        std::condition_variable empty;
    };

    static void thread_runner(thread_pool &pool, size_t thread_index)
    {
        auto &info = pool._threads[thread_index];

        auto lock = std::unique_lock(_mutex);
        while (true) {
            if (info.work) {
                lock.unlock();
                info.work.call();
                lock.lock();
                info.work = nullptr;
                pool._full.notify_one();
            }

            info.empty.wait(lock);
        }
    }

    thread_item *idle_thread()
    {
        for 
    }

    mutable std::mutex _mutex;
    size_t _num_threads;
    std::vector<thread_item> _threads;
    std::condition_variable _full;
};

inline thread_pool pool;

}
