#include "tests_common.h"

#include <deque>
#include <iostream>
#include <limits>

namespace {

using namespace essentials::iterations;

TEST(group, simple) {{
    auto have = itemize(1,1,2,2,3,3,3,4)
                .group(LAM(it, it))
                .map(LAM(v, v.toVector()))
                .toVector();

    auto expected = std::vector<std::vector<int>> {
        {1, 1},
        {2, 2},
        {3, 3, 3},
        {4}
    };

    ASSERT_EQ(have, expected);

}}

TEST(group, empty) {{
    auto have = itemize(0)
                .filter()
                .group(LAM(it, it))
                .map(LAM(v, v.toVector()))
                .toVector();

    auto expected = std::vector<std::vector<int>> { };

    ASSERT_EQ(have, expected);

}}

TEST(group, maps) {{
    auto have = 
        itemize(
            std::make_pair(2, std::string("hello")),
            std::make_pair(2, std::string("world")),
            std::make_pair(3, std::string("")),
            std::make_pair(4, std::string("yadayada")),
            std::make_pair(3, std::string("whatever"))
        )
        .toMultiMapView()
        .group(LAM(kv, kv.first))
        .map(LAM(view, view.map(LAM(kv, kv.second)).reduce(std::string{}, LAM2(A, B, A + B))))
        .toVector();

    auto expected = std::vector<std::string> { 
        { "helloworld" },
        { "whatever" },
        { "yadayada" }
    };

    ASSERT_EQ(have, expected);

}}

} /* namespace */
