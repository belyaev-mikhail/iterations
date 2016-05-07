#include "tests_common.h"

#include <deque>
#include <iostream>

namespace {

using namespace essentials::iterations;

TEST(generate, simple) {{
    auto g = unfoldMap<int>(0, LAM(i, i + 1), LAM(i, i));

    std::vector<int> expected { 0, 1, 2, 3, 4, 5 ,6, 7};
    ASSERT_EQ(g.take(8).toVector(), expected);
}}

TEST(generate, limited) {{
    auto g = unfoldMap<int>(0, LAM(i, i + 1), LAM(i, i), 7);

    std::vector<int> expected { 0, 1, 2, 3, 4, 5, 6};
    ASSERT_EQ(g.toVector(), expected);
}}

TEST(generate, empty) {{
    auto g = unfoldMap<int>(0, LAM(i, i + 1), LAM(i, i), 0);

    std::vector<int> expected {};
    ASSERT_EQ(g.toVector(), expected);
}}

TEST(generate, simple2) {{
    auto g = unfold<int>(1, LAM(i, i * 2));

    std::vector<int> expected { 1, 2, 4, 8, 16, 32, 64, 128 };
    ASSERT_EQ(g.take(8).toVector(), expected);
}}

TEST(generate, limited2) {{
    auto g = unfold<int>(1, LAM(i, i * 2), 7);

    std::vector<int> expected { 1, 2, 4, 8, 16, 32, 64};
    ASSERT_EQ(g.toVector(), expected);
}}

TEST(generate, empty2) {{
    auto g = unfold<int>(1, LAM(i, i * 2), 0);

    std::vector<int> expected {};
    ASSERT_EQ(g.toVector(), expected);
}}

TEST(generate, random) {{
    std::mt19937 one(42);
    std::mt19937 two(42);

    std::vector<uint64_t> expected(50);

    std::generate(expected.begin(), expected.end(), one);

    // this mostly checks that generators call function exactly once per step, as std::generate() does
    auto g = unfoldMap<std::mt19937>(two, LAM(i, i), LAM(r, r()), 50);

    ASSERT_EQ(expected, g.toVector());

}}

TEST(generate, random2) {{
    std::mt19937 one(0xDEADBEEF);
    std::mt19937 two(0xDEADBEEF);

    std::vector<uint64_t> expected(50);

    std::generate(expected.begin(), expected.end(), one);

    // this mostly checks that generators call function exactly once per step, as std::generate() does
    auto g = generate(two).take(50);

    ASSERT_EQ(expected, g.toVector());

}}

} /* namespace */
