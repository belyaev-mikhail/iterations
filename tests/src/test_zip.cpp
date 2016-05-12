#include "tests_common.h"

namespace {

using namespace essentials::iterations;

TEST(zip, simple) {{

    auto&& v = std::vector<int>{1,2,3,4};
    auto&& w = std::list<int>{3,4,5,6};

    auto&& z = viewContainer(v) ^ viewContainer(w);

    auto&& have = std::vector<std::pair<int, int>>( std::begin(z), std::end(z) );
    auto&& expected = std::vector<std::pair<int, int>>{ {1,3}, {2,4}, {3,5}, {4,6} };

    ASSERT_EQ(have, expected);

}}

TEST(zip, reference) {{

    auto&& v = std::vector<int>{1,2,3,4};
    auto&& w = std::list<int>{3,4,5,6};

    auto&& z = viewContainer(v) ^ viewContainer(w);

    std::begin(z)->first = 6;

    auto&& have = std::vector<std::pair<int, int>>( std::begin(z), std::end(z) );
    auto&& expected = std::vector<std::pair<int, int>>{ {6,3}, {2,4}, {3,5}, {4,6} };

    ASSERT_EQ(have, expected);
    ASSERT_EQ(v[0], 6);

}}

TEST(zip, empty) {{

    auto&& v = std::vector<int>{};
    auto&& w = std::list<int>{};

    auto&& z = viewContainer(v) ^ viewContainer(w);

    ASSERT_TRUE(z.toVector().empty());

}}

TEST(zip, firstBigger) {{

    auto&& v = std::vector<int>{ 1, 2, 3, 4 };
    auto&& w = std::list<int>{ 33, 34, 35};

    auto&& z = viewContainer(v) ^ viewContainer(w);

    auto&& have = std::vector<std::pair<int, int>>( std::begin(z), std::end(z) );
    auto&& expected = std::vector<std::pair<int, int>>{ {1,33}, {2,34}, {3,35} };

    ASSERT_EQ(have.size(), 3);
    ASSERT_EQ(have, expected);

}}

TEST(zip, secondBigger) {{

    auto&& v = std::vector<int>{ 1, 2 };
    auto&& w = std::list<int>{ 33, 34, 35, 48};

    auto&& z = viewContainer(v) ^ viewContainer(w);

    auto&& have = std::vector<std::pair<int, int>>( std::begin(z), std::end(z) );
    auto&& expected = std::vector<std::pair<int, int>>{ {1,33}, {2,34} };

    ASSERT_EQ(have.size(), 2);
    ASSERT_EQ(have, expected);

}}

TEST(zip, infinite) {{
    auto&& v = std::vector<int>{ 1, 2 };
    auto&& vinf = viewContainer(v).cycle();

    auto&& z = vinf ^ vinf;

    auto&& startz = z.drop(1).take(8);
    auto&& have = std::vector<std::pair<int, int>>( std::begin(startz), std::end(startz) );
    auto&& expected = std::vector<std::pair<int, int>>{ {2,2}, {1,1}, {2,2}, {1,1}, {2,2}, {1,1}, {2,2}, {1,1} };

    ASSERT_EQ(have, expected);
}}

TEST(zip, gccIssue) {{

    struct nonCopyable {
        nonCopyable(const nonCopyable&) = delete;
        nonCopyable() {}
    } nc;

    auto&& v = std::vector<nonCopyable>(2);
    
    auto&& view = viewContainer(v);

    auto&& z = view ^ view;

    auto&& y = z.toVector();

}}

} /* namespace */
