#include "tests_common.h"

#include <deque>
#include <iostream>
#include <limits>

namespace {

using namespace essentials::iterations;

TEST(itemize, simple) {{
    auto view = itemize(1,2,3,4,5,6);
    std::vector<int> expected {1,2,3,4,5,6};

    ASSERT_EQ(view.toVector(), expected);
}}

} /* namespace */