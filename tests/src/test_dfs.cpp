#include "tests_common.h"

#include <iostream>

namespace {

using namespace essentials::iterations;

struct tree {
    tree* left = nullptr;
    tree* right = nullptr;

    int value = 0;
};

TEST(dfs, simple) {{
    tree root;
    tree* null = nullptr;
    root.value = 1;

    root.left = new tree();
    root.right = new tree();

    root.left->value = 2;
    root.left->right = new tree();
    root.left->right->value = 3;

    root.right->value = 4;

    root.right->left = new tree();
    root.right->left->value = 5;
    root.right->left->right = new tree();
    root.right->left->right->value = 6;
    root.right->right = new tree();
    root.right->right->value = 7;
    root.right->right->right = new tree();
    root.right->right->right->value = 8;

    //     1
    //    / \
    //   2   4
    //  /|   |\
    // x 3   5 7
    //  /|  /| |\
    // x x x 6 x 8
    //       |\  |\
    //       x x x x

    auto dfs = itemize(static_cast<tree*>(&root), static_cast<tree*>(null))
              .filter()
              .dfs(LAM(tr, itemize(static_cast<tree*>(tr->left), static_cast<tree*>(tr->right)).filter()))
              .map(LAM(tr, tr->value));

    std::vector<int> expected { 1,2,3,4,5,6,7,8 };

    ASSERT_EQ(dfs.toVector(), expected);

}}

TEST(dfs, empty) {{
    
    auto dfs = emptyView<int>().dfs(LAM(i, emptyView<int>()));

    std::vector<int> expected {};

    ASSERT_EQ(dfs.toVector(), expected);

}}

} /* namespace */
