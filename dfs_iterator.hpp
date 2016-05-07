/*
 * dfs_iterator.hpp
 *
 *  Created on: May 6, 2016
 *      Author: belyaev
 */

#ifndef DFS_ITERATOR_HPP
#define DFS_ITERATOR_HPP

#include <iostream>

#include "iterator_adapter.hpp"
#include "shared_queue.hpp"

namespace essentials {
namespace iterations {

struct dfs_identity {
    template<class T>
    T&& operator()(T&& value) const {
        return value;
    }
};

template<class BaseIt, class Accessor = dfs_identity>
struct dfs_iterator_simple {
    using inner_view = decltype(std::declval<Accessor>()(*std::declval<BaseIt>()));

    // we need two stacks because iterators get mutated much more frequently
    shared_stack<inner_view> viewStack;
    shared_stack<BaseIt> iteratorStack;

    Accessor accessor;

    void down() {
        auto it = iteratorStack.top();
        iteratorStack = iteratorStack.pop();
        iteratorStack = iteratorStack.push(std::next(it));

        viewStack = viewStack.push(accessor(*it));
        iteratorStack = iteratorStack.push(overloaded_begin(viewStack.top()));
    }

    void up() {
        iteratorStack = iteratorStack.pop();
        viewStack = viewStack.pop();
    }

    BaseIt current() const {
        return iteratorStack.top();
    }

    dfs_iterator_simple() = default;
    dfs_iterator_simple(inner_view inner, Accessor accessor):
            viewStack{}, iteratorStack{}, accessor(accessor) {
        viewStack = viewStack.push(inner);
        iteratorStack = iteratorStack.push(overloaded_begin(viewStack.top()));
        restore_invariant();
    };

    bool invariant() const {
        return viewStack.empty() || (!viewStack.empty() && current() != overloaded_end(viewStack.top()));
    }

    void restore_invariant() {
        while(!invariant()) {
            if(!viewStack.empty()) {
                up();
            }
        }
    }

    auto value() const -> decltype(*current()) { return *current(); }

    void next() {
        if(viewStack.empty()) return;
        down();
        restore_invariant();
    }

    bool equals(const dfs_iterator_simple& that) const {
        if(viewStack.empty() != that.viewStack.empty())
            return false;
        return (viewStack.empty() && that.viewStack.empty()) 
            || current() == that.current();
    }
};

template<class BaseIt, class Accessor = dfs_identity>
using dfs_ap_iterator = iterator_adapter<dfs_iterator_simple<BaseIt, Accessor>, std::forward_iterator_tag>;

template<class View, class Accessor = dfs_identity>
auto dfs_iterator(View from, Accessor acc) -> dfs_ap_iterator<decltype(overloaded_begin(from)), Accessor> {
    using It = decltype(overloaded_begin(from));
    return adapt_simple_iterator(
        dfs_iterator_simple<It, Accessor>{from, acc},
        std::forward_iterator_tag{}
    );
}

} /* namespace iterations */
} /* namespace essentials */

#endif //DFS_ITERATOR_HPP
