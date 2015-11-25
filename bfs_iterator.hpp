/*
 * bfs_iterator.hpp
 *
 *  Created on: Nov 25, 2015
 *      Author: belyaev
 */

#ifndef BFS_ITERATOR_HPP
#define BFS_ITERATOR_HPP

#include "iterator_adapter.hpp"
#include "shared_queue.hpp"

namespace essentials {
namespace iterations {

struct bfs_identity {
    template<class T>
    T&& operator()(T&& value) const {
        return value;
    }
};

template<class BaseIt, class Accessor = bfs_identity>
struct bfs_iterator_simple: simple_iterator_facade<BaseIt> {

    using inner_view = decltype(std::declval<Accessor>()(*std::declval<BaseIt>()));

    shared_queue<inner_view> que;
    Accessor accessor;

    bfs_iterator_simple() = default;
    bfs_iterator_simple(inner_view inner): simple_iterator_facade(), que{}, accessor{} {
        que = que.push(inner);
        base = overloaded_begin(que.top());
    };
    bfs_iterator_simple(inner_view inner, Accessor accessor):
            simple_iterator_facade(), que{}, accessor(accessor) {
        que = que.push(inner);
        base = overloaded_begin(que.top());
    };

    bool invariant() const {
        return que.empty() || (!que.empty() && base != overloaded_end(que.top()));
    }

    void next() {
        if(que.empty()) return;

        que = que.push(accessor(value()));
        simple_iterator_facade::next();

        while(!invariant()) {
            que = que.pop();
            if(!que.empty())
                base = overloaded_begin(que.top());
        }
    }
};

template<class BaseIt, class Accessor = bfs_identity>
using bfs_ap_iterator = iterator_adapter<bfs_iterator_simple<BaseIt, Accessor>, std::forward_iterator_tag>;

template<class View, class Accessor = bfs_identity>
auto bfs_iterator(View from) -> bfs_ap_iterator<decltype(overloaded_begin(from)), Accessor> {
    using It = decltype(overloaded_begin(from));
    return adapt_simple_iterator(
        bfs_ap_iterator<It, Accessor>{from},
        std::forward_iterator_tag{}
    );
}

template<class View, class Accessor = bfs_identity>
auto bfs_iterator(View from, Accessor acc) -> bfs_ap_iterator<decltype(overloaded_begin(from)), Accessor> {
    using It = decltype(overloaded_begin(from));
    return adapt_simple_iterator(
        bfs_ap_iterator<It, Accessor>{from, acc},
        std::forward_iterator_tag{}
    );
}

} /* namespace iterations */
} /* namespace essentials */

#endif //BFS_ITERATOR_HPP
