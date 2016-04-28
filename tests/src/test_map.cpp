#include "tests_common.h"

#include <deque>
#include <iostream>
#include <sstream>

namespace {

using namespace essentials::iterations;

TEST(map, simple) {{
    std::vector<int> v1 {1,2,3,4};

    auto&& view = viewContainer(v1).map(LAM(x, x + 2));
    std::vector<int> expected{3,4,5,6};

    ASSERT_TRUE(!std::is_reference<decltype(*view.begin())>::value); // map does not return a reference unless the mapper does
    ASSERT_EQ(view.toVector(), expected);
}}

TEST(map, chain) {{
    auto toString = [](int x) {
        std::ostringstream oss;
        oss << x;
        return oss.str();
    };

    std::vector<int> v1 {1,2,3,4};

    auto&& view = viewContainer(v1).map(LAM(x, x + 2)).map(toString);

    std::vector<std::string> expected{ "3", "4", "5", "6" };

    ASSERT_TRUE(!std::is_reference<decltype(*view.begin())>::value); // map does not return a reference unless the mapper does
    ASSERT_EQ(view.toVector(), expected);
}}

} /* namespace */
