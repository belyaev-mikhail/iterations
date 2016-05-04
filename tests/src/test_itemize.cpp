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

TEST(itemize, different_types) {{
    auto view = itemize( (char)'a', (short)2 );
    std::vector<int> expected { 97, 2 };

    ASSERT_FALSE(IS_ASSIGNABLE(*std::begin(view)));
    // *std::begin(view) = 'b'; does not compile: expression is not assignable

    ASSERT_EQ(view.toVector(), expected);
}}

TEST(itemize, references) {{
    std::vector<int> input { 1, 2, 3 };
    auto view = itemize(input[0], input[1], input[2]);

    ASSERT_TRUE(IS_ASSIGNABLE(*std::begin(view)));

    *std::begin(view) = 42;

    std::vector<int> expected { 42, 2, 3 };
    ASSERT_EQ(input, expected);
}}

TEST(itemize, random_access) {{
    auto view = itemize(1,2,3,4,5,6,7);

    ASSERT_EQ(view.size(), 7);

    ASSERT_EQ(*(std::begin(view) + 4), 5);
}}

} /* namespace */