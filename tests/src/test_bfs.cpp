#include "tests_common.h"

#include <iostream>

namespace {

using namespace essentials::iterations;

struct tree {
    tree* left = nullptr;
    tree* right = nullptr;

    int value = 0;
};

TEST(bfs, simple) {{
    tree root;
    tree* null = nullptr;
    root.value = 1;

    root.left = new tree();
    root.right = new tree();

    root.left->value = 2;
    root.left->right = new tree();
    root.left->right->value = 4;

    root.right->value = 3;

    root.right->left = new tree();
    root.right->left->value = 5;
    root.right->left->right = new tree();
    root.right->left->right->value = 7;
    root.right->right = new tree();
    root.right->right->value = 6;
    root.right->right->right = new tree();
    root.right->right->right->value = 8;

    //     1
    //    / \
    //   2   3
    //  /|   |\
    // x 4   5 6
    //  /|  /| |\
    // x x x 7 x 8
    //       |\  |\
    //       x x x x

    auto bfs = itemize(static_cast<tree*>(&root), static_cast<tree*>(null))
              .filter()
              .bfs(LAM(tr, itemize(static_cast<tree*>(tr->left), static_cast<tree*>(tr->right)).filter()))
              .map(LAM(tr, tr->value));

    std::vector<int> expected { 1,2,3,4,5,6,7,8 };

    ASSERT_EQ(bfs.toVector(), expected);

}}


} /* namespace */
