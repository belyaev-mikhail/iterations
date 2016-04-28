#ifndef TESTS_COMMON_H
#define TESTS_COMMON_H

#include "iterator_view.hpp"
#include <gtest/gtest.h>

#include <vector>
#include <type_traits>

#define LAM(X, ...) ([&](auto&& X) -> decltype(auto) { return __VA_ARGS__; })
#define LAM2(X, Y, ...) ([&](auto&& X, auto&& Y) -> decltype(auto) { return __VA_ARGS__; })


template<class It>
auto asValueVector(essentials::iterations::iterator_view<It> v) {
    using value_type = std::decay_t< decltype(v.head()) >;
    return v.template to<std::vector<value_type>>();
}

#endif /* TESTS_COMMON_H */