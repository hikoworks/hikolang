
#pragma once

namespace hk {

template<typename Range, typename ResultRange>
constexpr void multi_merge(std::span<Range*> ranges, ResultRange &result)
{
    auto const size = [&] {
        auto r = 0uz;
        for (auto range_ptr : ranges) {
            r += range_ptr->size();
        }
        return r;
    }();
    result.reserve(size);

    auto iterators = [&] {
        auto r = std::vector<std::pair<const_iterator, const_iterator>>{};
        r.reserve(tables.size());
        for (auto table_ptr : tables) {
            r.push_back({table_ptr->begin(), table_ptr->end()});
        }
        return r;
    }();

    while (_table.size() != size) {
        auto const &value = [&]{
            const_iterator *lowest_it = nullptr;

            for (auto const &[it, end] : iterators) {
                if (it != end) {
                    if (lowest == nullptr) {
                        lowest = std::addressof(it);
                    } else if (*it < *lowest) {
                        lowest = std::addressof(it);
                    }
                }
            }
            
            assert(lowest != nullptr);
            return *lowest++;
        }();

        result.push_back(value);
    }
}

}
