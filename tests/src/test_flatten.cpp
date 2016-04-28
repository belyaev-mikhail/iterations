#include "tests_common.h"

#include <deque>
#include <iostream>

namespace {

using namespace essentials::iterations;

TEST(flatten, simple) {{
    std::vector<std::vector<int>> v1 {
        { 1, 2, 3 },
        { 4, 5, 6 },
        { 7, 8, 9 }
    };

    auto&& view = viewContainer(v1).flatten();
    auto expected = range(1,10).toVector();

    ASSERT_TRUE(std::is_reference<decltype(*view.begin())>::value); // flatten keeps references intact
    ASSERT_EQ(view.toVector(), expected);
}}

TEST(flatten, references) {{
    std::vector<std::vector<int>> v1 {
        { 1, 2, 3 },
        { 4, 5, 6 },
        { 7, 8, 9 }
    };

    auto&& view = viewContainer(v1).flatten();
    *std::next(view.begin(), 4) = 0xBBBBBB;

    auto expected = range(1,10).toVector();
    expected[4] = 0xBBBBBB;

    ASSERT_TRUE(std::is_reference<decltype(*view.begin())>::value); // flatten keeps references intact
    ASSERT_EQ(view.toVector(), expected);
}}

TEST(flatten, values) {{
    auto view = range(0, 4).map(LAM(i, range(i, i + 2))).flatten();

    auto expected = std::vector<int>{0, 1, 1, 2, 2, 3, 3, 4};
    
    ASSERT_TRUE(!std::is_reference<decltype(*view.begin())>::value); // original had values
    ASSERT_EQ(view.toVector(), expected);

    // BUT: don't do this : .map(LAM(i, std::vector<int>{i, i+1})), it WON'T work as you expect it to

}}

} /* namespace */