#include "tests_common.h"

#include <deque>
#include <iostream>
#include <limits>

namespace {

using namespace essentials::iterations;

TEST(count, simple) {{
    auto view = range(1, 11);
    std::vector<int> expected {1,2,3,4,5,6,7,8,9,10};

    ASSERT_TRUE(!std::is_reference<decltype(*view.begin())>::value);
    ASSERT_EQ(view.toVector(), expected);
}}

TEST(count, empty) {{
    auto view = range(1, 1);
    std::vector<int> expected {};

    ASSERT_TRUE(!std::is_reference<decltype(*view.begin())>::value);
    ASSERT_TRUE(view.empty());
    ASSERT_EQ(view.toVector(), expected);
}}

TEST(count, infinite) {{
    // well, not _technically_ infinite
    auto view = range(0, std::numeric_limits<int>::max());

    ASSERT_TRUE(!std::is_reference<decltype(*view.begin())>::value);

    auto view2 = view.zipWith(itemize(0) >> view, LAM2(x, y, x + y));

    auto us = view2.take(6).toVector();
    auto them = std::vector<int>{ 0, 1, 3, 5, 7, 9 };

    ASSERT_EQ(us, them);
}}

TEST(count, non_numbers) {{
    int x[] = { 1, 2, 3 };

    auto&& rng = range(x, x + 2);
    ASSERT_EQ(x, *rng.begin());

    auto&& us = rng.map(LAM(ptr, *ptr)).toVector();
    auto&& them = std::vector<int>(x, x+2);
    ASSERT_EQ(us, them);
}}

} /* namespace */
