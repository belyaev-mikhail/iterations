#include "tests_common.h"

namespace {

using namespace essentials::iterations;

TEST(general, simple) {{
    std::vector<std::string> v {"hello", "world"};

    ASSERT_EQ(viewContainer(v).toVector(), v);

}}

TEST(general, references) {{
    std::vector<std::string> v {"hello", "world"};

    auto&& view = viewContainer(v);
    *view.begin() = "alloha";

    ASSERT_EQ(v[0], "alloha");

}}

TEST(general, empty) {{
    std::vector<std::string> v {};

    auto&& view = viewContainer(v);
    for(auto&& e: view) ASSERT_TRUE(false);

    ASSERT_EQ(view.size(), 0);

}}

TEST(general, const_) {{
    const std::vector<std::string> v {"hello", "world"};

    auto&& view = viewContainer(v);
    // *view.begin() = "alloha"; should not compile

    ASSERT_EQ(*view.begin(), "hello");

}}

TEST(general, size) {{
    std::vector<int> v0 {};
    std::vector<int> v1 {1,2};

    ASSERT_EQ(viewContainer(v0).size(), 0);
    ASSERT_EQ(viewContainer(v1).size(), 2);
    ASSERT_EQ(viewContainer(v1).drop(1).size(), 1);

}}


} /* namespace */