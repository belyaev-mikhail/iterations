#include "tests_common.h"

#include <deque>
#include <iostream>
#include <limits>

namespace {

using namespace essentials::iterations;

TEST(cycle, simple) {{
    auto view = range(1, 4).cycle().drop(1).take(7);
    std::vector<int> expected {2,3,1,2,3,1,2};

    ASSERT_TRUE(!std::is_reference<decltype(*view.begin())>::value); // range produces non-references
    ASSERT_EQ(view.toVector(), expected);
}}

TEST(cycle, reference) {{

    std::vector<int> contents { 1,2,3 };
    auto view = viewContainer(contents).cycle().drop(1).take(7);
    
    ASSERT_TRUE(std::is_reference<decltype(*view.begin())>::value);

    *std::next(view.begin()) = 42; // assign element 2 of _every_ cycle in motion

    std::vector<int> expected {2,42,1,2,42,1,2};

    ASSERT_EQ(view.toVector(), expected);
}}

} /* namespace */