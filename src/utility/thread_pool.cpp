
#include "thread_pool.hpp"

namespace hk {

thread_pool::thread_pool(size_t max_num_threads) : _max_num_threads(max_num_threads) {}

void thread_pool::schedule(std::unique_ptr<thread_pool_base> work)
{
    auto lock = std::unique_lock{_mutex};
    while (true) {
        auto [ti_ptr, lock_] = idle_thread(std::move(lock));
        lock = std::move(lock_);

        if (ti_ptr) {
            ti_ptr->work = std::move(work);
            ti_ptr->new_work_condition.notify_one();
            return;
        }
        _idle_thread_condition.wait(lock);
    }
}

thread_pool::thread_item::~thread_item()
{
    assert(work == nullptr);
    thread.request_stop();
    new_work_condition.notify_one();
}

void thread_pool::runner(std::stop_token token, thread_pool::thread_item* item, thread_pool* pool)
{
    assert(item != nullptr);
    assert(pool != nullptr);

    auto lock = std::unique_lock(pool->_mutex);
    while (not token.stop_requested()) {
        auto work_ptr = item->work.get();
        if (work_ptr) {
            lock.unlock();
            work_ptr->call();
            lock.lock();

            item->work = nullptr;
            pool->_idle_thread_condition.notify_one();
        }

        item->new_work_condition.wait(lock);
    }
}

thread_pool::thread_item::thread_item(thread_pool* pool) : thread{thread_pool::runner, this, pool} {}

[[nodiscard]] std::pair<thread_pool::thread_item*, std::unique_lock<std::mutex>>
thread_pool::idle_thread(std::unique_lock<std::mutex> lock)
{
    for (auto& _ : _threads) {
        if (_->work == nullptr) {
            return {_.get(), std::move(lock)};
        }
    }

    if (_threads.size() < _max_num_threads) {
        auto item = std::make_unique<thread_item>(this);
        auto item_ptr = item.get();
        _threads.push_back(std::move(item));
        return {item_ptr, std::move(lock)};
    }

    return {nullptr, std::move(lock)};
}

} // namespace hk
