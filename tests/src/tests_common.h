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

template<class T>
struct is_assignable {
    static const constexpr auto value = false;
};

template<class T>
struct is_assignable<const T&> {
    static const constexpr auto value = false;
};

template<class T>
struct is_assignable<T&> {
    static const constexpr auto value = true;
};

#define IS_ASSIGNABLE(...) is_assignable<decltype(__VA_ARGS__)>::value

#endif /* TESTS_COMMON_H */