#include "tests_common.h"

#include <deque>
#include <iostream>
#include <limits>

namespace {

using namespace essentials::iterations;

TEST(abstract, simple) {{
    auto view = range(1, 11);
    
    generic_view<int, std::random_access_iterator_tag> gen = view.abstract();

    ASSERT_EQ(view.toVector(), gen.toVector());

}}

TEST(abstract, references) {{
    std::vector<int> data{ 1,2,3,4,5,6,7,8 };
    
    generic_view<int&, std::random_access_iterator_tag> gen = viewContainer(data).abstract();

    ASSERT_TRUE(IS_ASSIGNABLE(*gen.begin()));

    *std::next(gen.begin(), 5) = 42;

    std::vector<int> expected{ 1,2,3,4,5,42,7,8 };

    ASSERT_EQ(data, expected);

}}

} /* namespace */
