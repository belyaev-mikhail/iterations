#include "tests_common.h"

#include <deque>
#include <iostream>

namespace {

using namespace essentials::iterations;

TEST(seq, simple) {{

    auto&& v = std::vector<int>{1,2,3,4};
    auto&& w = std::list<int>{3,4,5,6};

    auto&& z = viewContainer(v) >> viewContainer(w);

    auto&& have = z.toVector();
    auto&& expected = std::vector<int>{ 1,2,3,4,3,4,5,6 };

    ASSERT_TRUE(std::is_reference<decltype(*z.begin())>::value);

    ASSERT_EQ(have, expected);

}}

TEST(seq, different_types) {{

    auto&& v = std::vector<char>{1,2,3,4};
    auto&& w = std::list<short>{3,4,5,6};

    auto&& z = viewContainer(v) >> viewContainer(w);

    auto&& have = z.toVector();
    auto&& expected = std::vector<int>{ 1,2,3,4,3,4,5,6 };

    ASSERT_TRUE(!std::is_reference<decltype(*z.begin())>::value);

    ASSERT_EQ(have, expected);

}}

TEST(seq, references) {{

    auto&& v = std::vector<unsigned>{1,2,3,4};
    auto&& w = std::deque<unsigned>{3,4,5,6};

    auto&& z = viewContainer(v) >> viewContainer(w);

    auto&& zx = z.drop(3);
    *zx.begin() = 0xAAAAAAAA;
    
    auto&& zy = zx.drop(2);
    *zy.begin() = 0xCAFEBABE;

    ASSERT_TRUE(std::is_reference<decltype(*z.begin())>::value);

    ASSERT_EQ(0xAAAAAAAA, v[3]);
    ASSERT_EQ(0xCAFEBABE, w[1]);

}}

} /* namespace */