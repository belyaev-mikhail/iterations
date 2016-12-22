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

TEST(general, reverse) {{
    std::vector<int> us {1,2,3,4,5};
    auto view = viewContainer(us).reverse();

    std::vector<int> expected {5,4,3,2,1};
    ASSERT_EQ(std::vector<int>(std::begin(view), std::end(view)), expected);

    std::vector<int> empty{};

    auto emptyView = viewContainer(empty).reverse();
    ASSERT_EQ(std::vector<int>(std::begin(emptyView), std::end(emptyView)), empty);
}}


TEST(general, copyFrom) {{
    std::vector<int> v0 {1,2,3,4,5,6,7};
    
    auto view0 = viewContainer(v0).filter(LAM(i, i%2 == 0));
    view0.copyFrom(range(42, 45));

    std::vector<int> expected { 1, 42, 3, 43, 5, 44, 7 };

    ASSERT_EQ(v0, expected);
}}

TEST(general, copyTo) {{
    std::vector<int> v0 {1,2,3,4,5,6,7};

    auto view0 = viewContainer(v0).filter(LAM(i, i%2 == 0));
    range(42, 45).copyTo(view0);

    std::vector<int> expected { 1, 42, 3, 43, 5, 44, 7 };

    ASSERT_EQ(v0, expected);
}}

} /* namespace */
