#include "tests_common.h"

#include <deque>
#include <iostream>
#include <limits>

namespace {

using namespace essentials::iterations;

TEST(product, simple) {{
    std::vector<int> v1 { 1, 2, 3 };
    std::vector<int> v2 { 4, 5 };

    auto&& res = viewContainer(v1) * viewContainer(v2);

    std::vector<std::pair<int, int>> have( res.begin(), res.end() );
    std::vector<std::pair<int, int>> expected { {1, 4}, {1, 5}, {2, 4}, {2, 5}, {3, 4}, {3, 5} };

    ASSERT_EQ(have, expected);
}}

TEST(product, references) {{
    std::vector<int> v1 { 1, 2, 3 };
    std::vector<int> v2 { 4, 5 };

    auto&& res = viewContainer(v1) * viewContainer(v2);

    std::next(res.begin(), 3)->first = 0xAA;
    std::next(res.begin(), 3)->second = 0xBB;

    std::vector<std::pair<int, int>> have( res.begin(), res.end() );
    // it's a kinda magic...
    std::vector<std::pair<int, int>> expected { {1, 4}, {1, 0xBB}, {0xAA, 4}, {0xAA, 0xBB}, {3, 4}, {3, 0xBB} };

    ASSERT_EQ(have, expected);
}}

TEST(product, values) {{
    auto&& res = range(1,4) * range(4,6);

    std::next(res.begin(), 3)->first = 0xAA;
    std::next(res.begin(), 3)->second = 0xBB;

    std::vector<std::pair<int, int>> have( res.begin(), res.end() );
    // it's a kinda non-magic.
    std::vector<std::pair<int, int>> expected { {1, 4}, {1, 5}, {2, 4}, {2, 5}, {3, 4}, {3, 5} };

    ASSERT_EQ(have, expected);
}}

TEST(product, empty) {{
    std::vector<int> v1 { 1, 2, 3 };
    std::vector<int> v2{};

    std::vector<std::pair<int&, int&>> empty { };

    auto&& res0 = viewContainer(v2) * viewContainer(v1);
  
    ASSERT_TRUE(res0.empty());
    ASSERT_EQ(res0.toVector(), empty);

    auto&& res1 = viewContainer(v1) * viewContainer(v2);
  
    ASSERT_TRUE(res1.empty());
    ASSERT_EQ(res1.toVector(), empty);

    auto&& res2 = viewContainer(v2) * viewContainer(v2);
  
    ASSERT_TRUE(res2.empty());
    ASSERT_EQ(res2.toVector(), empty);
}}

TEST(product, oneOnTwo) {{
    std::vector<int> v1 { 1 };
    std::vector<int> v2 { 2, 3 };
    {
        auto&& res0 = viewContainer(v1) * viewContainer(v2);
        std::vector<std::pair<int, int>> res(res0.begin(), res0.end());
        std::vector<std::pair<int, int>> expected{ {1, 2}, {1, 3} };

        ASSERT_EQ(res, expected);
    }
    {
        auto&& res0 = viewContainer(v2) * viewContainer(v1);
        std::vector<std::pair<int, int>> res(res0.begin(), res0.end());
        std::vector<std::pair<int, int>> expected{ {2, 1}, {3, 1} };

        ASSERT_EQ(res, expected);
    }
}}

TEST(product, oneOnOne) {{
    std::vector<int> v1 { 1 };
    std::vector<int> v2 { 2 };
    {
        auto&& res0 = viewContainer(v1) * viewContainer(v2);
        std::vector<std::pair<int, int>> res(res0.begin(), res0.end());
        std::vector<std::pair<int, int>> expected{ {1, 2} };

        ASSERT_EQ(res, expected);
    }
}}

} /* namespace */