#include "tests_common.h"

namespace {

using namespace essentials::iterations;

TEST(memo, simple) {{
    std::vector<int> v { 1, 2, 3, 4 };

    auto&& v2 = viewContainer(v).memo();

    ASSERT_EQ(v2.toVector(), v);

}}


} /* namespace */