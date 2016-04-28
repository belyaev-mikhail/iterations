#include "tests_common.h"

#include <deque>
#include <iostream>

namespace {

using namespace essentials::iterations;

TEST(filter, simple) {{
    std::vector<int> v1 {1,2,3,4,5,6,7,8,9,10};

    auto&& view = viewContainer(v1).filter(LAM(x, x % 2 == 0));
    std::vector<int> expected{2,4,6,8,10};

    ASSERT_TRUE(std::is_reference<decltype(*view.begin())>::value); // filter does return references
    ASSERT_EQ(view.toVector(), expected);
}}

TEST(filter, simpleNoArg) {{
    std::vector<int> v1 {0, 2, 0, 4, 0, 0, 6, 8, 19};

    auto&& view = viewContainer(v1).filter();
    std::vector<int> expected{2,4,6,8,19};

    ASSERT_TRUE(std::is_reference<decltype(*view.begin())>::value); // filter does return references
    ASSERT_EQ(view.toVector(), expected);
}}

TEST(filter, references) {{
    std::vector<int> v1 {1,2,3,4,5,6,7,8,9,10};

    auto&& view = viewContainer(v1).filter(LAM(x, x % 2 == 0));

    *view.drop(2).begin() = 13;

    ASSERT_EQ(v1[5], 13);
}}

TEST(filter, referencesNoArg) {{
    std::vector<int> v1 {0, 2, 0, 4, 0, 0, 6, 8, 19};

    auto&& view = viewContainer(v1).filter();

    // set third non-null element to 13
    *view.drop(2).begin() = 13;

    ASSERT_EQ(v1[6], 13);
}}

} /* namespace */
