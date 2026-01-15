
#pragma once

#include <memory>

namespace hk {

struct symbol_type;

struct symbol_value_base {
    virtual ~symbol_value_base() = default;

    std::unique_ptr<symbol_type> type;
};

template<typename T>
struct symbol_value : public symbol_value_base {
    T value;
};

template<typename T>
struct symbol_type : public symbol_value<T> {

};

struct symbol_instance {
    symbol_type type;
    symbol_value_base value;
};

}