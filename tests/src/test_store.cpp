#include "tests_common.h"

namespace {

using namespace essentials::iterations;

TEST(store, simple) {{
    std::vector<std::string> v {"hello", "world"};

    auto&& w = viewContainer(v) >> viewContainer(v);
    auto&& res = w.toListView();

    std::vector<std::string> expected {"hello", "world", "hello", "world"}; 

    ASSERT_EQ(res.toVector(), expected);
}}

TEST(store, sets) {{
    std::vector<int> v { 1, 15, 4, 1, 8 };

    auto&& w = viewContainer(v) >> viewContainer(v);
    auto&& res = w.toSetView();

    std::vector<int> expected { 1, 4, 8, 15 };  // SUDDENLY!

    ASSERT_EQ(res.toVector(), expected);
}}

TEST(store, access) {{
    std::vector<std::string> v {"hello", "world"};

    auto&& w = viewContainer(v) >> viewContainer(v);
    auto&& res = w.toVectorView();

    *(std::next(res.begin(), 2)) = "Wupwup";

    std::vector<std::string> expected {"hello", "world", "Wupwup", "world"}; 

    ASSERT_EQ(res.toVector(), expected);
}}

} /* namespace */