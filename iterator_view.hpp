/*
 * map_iterator.hpp
 *
 *  Created on: Mar 22, 2015
 *      Author: belyaev
 */

#ifndef ITERATOR_VIEW_HPP_
#define ITERATOR_VIEW_HPP_

#include <vector>
#include <list>
#include <set>
#include <unordered_set>
#include <algorithm>

#include "map_iterator.hpp"
#include "flatten_iterator.hpp"
#include "filter_iterator.hpp"
#include "store_iterator.hpp"
#include "zip_iterator.hpp"
#include "sequence_iterator.hpp"
#include "count_iterator.hpp"
#include "itemize_iterator.hpp"

#define ITER_VIEW_QRET(...) ->decltype(__VA_ARGS__){ return __VA_ARGS__; }

namespace essentials {
namespace iterations {

namespace iterator_view_impl {
    template<class It>
    It next_with_limit(It it, It limit, size_t advance, std::random_access_iterator_tag) {
        size_t dist = std::distance(it, limit);
        if(dist < advance) return limit;
        else return std::next(it, advance);
    }
    template<class It, class Whatever>
    It next_with_limit(It it, It limit, size_t advance, Whatever) {
        for(size_t i = 0; i < advance; ++i) {
            if(it == limit) break;
            ++it;
        }
        return it;
    }
    template<class It>
    It next_with_limit(It it, It limit, size_t advance) {
        return next_with_limit(it, limit, advance, typename std::iterator_traits<It>::iterator_category{});
    }
}

template<class It>
struct iterator_view {
    It begin_;
    It end_;

    iterator_view(It begin, It end):
        begin_(begin), end_(end) {};

    It begin() const { return begin_; }
    It end() const { return end_; }

    using reference = decltype(*begin_);
    using value_type = std::remove_cv_t<std::remove_reference_t<reference>>;

    template<class OtherIt>
    static iterator_view<OtherIt> create(OtherIt b, OtherIt e) {
        return { b, e };
    }

    template<class Mapping>
    auto map(Mapping m) const ITER_VIEW_QRET(
        create(
            essentials::iterations::map_iterator(begin_, m),
            essentials::iterations::map_iterator(end_, m)
        )
    )

    template<class Pred>
    auto filter(Pred p) const ITER_VIEW_QRET(
        create(
            filter_iterator(begin_, end_, p),
            filter_iterator(end_, end_, p)
        )
    )

    iterator_view<filtered_iterator<It, std::add_pointer_t<bool(reference)>>> filter() const {
        std::add_pointer_t<bool(reference)> pred = [](reference v) -> bool { return !!v; };
        return filter(pred);
    }

    iterator_view<flattened_iterator<It>> flatten() const {
        return create(
            flatten_iterator(begin_, end_),
            flatten_iterator(end_, end_)
        );
    }

    template<class OtherView>
    auto zipWith(OtherView&& other) const {
        return create(
            zip_iterator(begin_, overloaded_begin(std::forward<OtherView>(other))),
            zip_iterator(end_, overloaded_end(std::forward<OtherView>(other)))
        );
    }

    template<class OtherView, class Zipper>
    auto zipWith(OtherView&& other, Zipper zipper) const {
        return create(
            zip_iterator(begin_, overloaded_begin(std::forward<OtherView>(other)), zipper),
            zip_iterator(end_, overloaded_end(std::forward<OtherView>(other)), zipper)
        );
    }

    template<class OtherView>
    auto seq(OtherView&& other) const ITER_VIEW_QRET(
        create(
            sequence_iterator(begin_, end_, overloaded_begin(std::forward<OtherView>(other))),
            sequence_iterator(end_, end_, overloaded_end(std::forward<OtherView>(other)))
        )
    )

    iterator_view take(size_t howmany) const {
        using namespace iterator_view_impl;
        return { begin_, next_with_limit(begin_, end_, howmany) };
    }

    iterator_view drop(size_t howmany) const {
        using namespace iterator_view_impl;
        return { next_with_limit(begin_, end_, howmany), end_ };
    }

    /* terminating operations */

    template<class F>
    void foreach(F f) const {
        for(auto&& e : *this) f(std::forward<decltype(e)>(e));
    }

    template<class R, class F>
    R fold(const R& initial, F f) const {
        R holder = initial;
        for(auto&& e : *this) {
            holder = f(holder, std::forward<decltype(e)>(e));
        }
        return std::move(holder);
    }

    template<class F>
    std::enable_if_t<std::is_copy_constructible<value_type>::value && std::is_copy_constructible<F>::value, value_type> reduce(F f) const {
        return drop(1).fold(*begin_, f);
    }

    template<class Pred>
    bool all_of(Pred p) const {
        return std::all_of(begin_, end_, p);
    }

    template<class Pred>
    bool any_of(Pred p) const {
        return std::any_of(begin_, end_, p);
    }

    template<class Pred>
    bool none_of(Pred p) const {
        return std::none_of(begin_, end_, p);
    }

    template<class Con>
    bool starts_with(Con&& c) const {
        auto cb = overloaded_begin(c);
        auto ce = overloaded_end(c);
        auto sb = begin_;
        auto se = end_;
        for(; cb != ce && sb != se; ++cb, ++sb) {
            if(*sb != *cb) return false;
        }
        return true;
    }

    bool empty() const {
        return begin_ == end_;
    }

    size_t size() const {
        return std::distance(begin_, end_);
    }

    template<class R>
    auto first_or(R&& def) const {
        return empty() ? std::forward<R>(def) : *begin_;
    }

    template<class Container>
    Container to() const {
        return Container{ begin_, end_ };
    }

    template<class Allocator = std::allocator<value_type>>
    std::vector<value_type, Allocator> toVector() const {
        return { begin_, end_ };
    }

    template<class Allocator = std::allocator<value_type>>
    std::list<value_type, Allocator> toList() const {
        return { begin_, end_ };
    }

    template<
        class Compare = std::less<value_type>, 
        class Allocator = std::allocator<value_type>
    >
    std::set<value_type, Compare, Allocator> toSet() const {
        return { begin_, end_ };
    }

    template<
        class Compare = std::less<value_type>, 
        class Allocator = std::allocator<value_type>
    >
    std::multiset<value_type, Compare, Allocator> toMultiSet() const {
        return { begin_, end_ };
    }

    template<
        class Hash = std::hash<value_type>,
        class KeyEqual = std::equal_to<value_type>,
        class Allocator = std::allocator<value_type>
    >
    std::unordered_set<value_type, Hash, KeyEqual, Allocator> toHashSet() const {
        return { begin_, end_ };
    }

    template<
        class Hash = std::hash<value_type>,
        class KeyEqual = std::equal_to<value_type>,
        class Allocator = std::allocator<value_type>
    >
    std::unordered_multiset<value_type, Hash, KeyEqual, Allocator> toHashMultiSet() const {
        return { begin_, end_ };
    }

    /* semi-terminating operations */
    
    template<class Container>
    auto toContainerView() const {
        auto sh = std::make_shared<Container>(begin_, end_);
        return create(
            store_iterator(sh, overloaded_begin(*sh)),
            store_iterator(sh, overloaded_end(*sh))
        );
    }

    template<class Allocator = std::allocator<value_type>>
    auto toVectorView() const {
        return toContainerView< std::vector<value_type, Allocator> >();
    }

    template<class Allocator = std::allocator<value_type>>
    auto toListView() const {
        return toContainerView< std::list<value_type, Allocator> >();
    }

    template<
        class Compare = std::less<value_type>, 
        class Allocator = std::allocator<value_type>
    >
    auto toSetView() const {
        return toContainerView< std::set<value_type, Compare, Allocator> >();
    }

    template<
        class Compare = std::less<value_type>, 
        class Allocator = std::allocator<value_type>
    >
    auto toMultiSetView() const {
        return toContainerView< std::multiset<value_type, Compare, Allocator> >();
    }

    template<
        class Hash = std::hash<value_type>,
        class KeyEqual = std::equal_to<value_type>,
        class Allocator = std::allocator<value_type>
    >
    auto toHashSetView() const {
        return toContainerView< std::unordered_set<value_type, Hash, KeyEqual, Allocator> >();
    }

    template<
        class Hash = std::hash<value_type>,
        class KeyEqual = std::equal_to<value_type>,
        class Allocator = std::allocator<value_type>
    >
    auto toHashMultiSetView() const {
        return toContainerView< std::unordered_multiset<value_type, Hash, KeyEqual, Allocator> >();
    }

};

template<class LIt, class RIt>
auto operator^(const iterator_view<LIt>& self, const iterator_view<RIt>& other) {
    return self.zipWith(other);
}


template<class LIt, class RIt>
auto operator>>(const iterator_view<LIt>& self, const iterator_view<RIt>& other) {
    return self.seq(other);
}

template<class It>
iterator_view<It> view(It begin, It end) {
    return { begin, end };
}

template<class It>
iterator_view<It> view(const std::pair<It, It>& pr) {
    return { pr.first, pr.second };
}

template<class Container>
auto viewContainer(Container&& con) {
    return view(
        overloaded_begin(std::forward<Container>(con)), 
        overloaded_end(std::forward<Container>(con))
    );
}

template<class ValueType = size_t>
auto range(ValueType from, ValueType to) {
    return view(count_iterator(from), count_iterator(to));
}

template<class ...Elements>
auto itemize(Elements&&... elements) {
    auto storage = std::make_shared<std::tuple<Elements...>>(std::forward<Elements>(elements)...);
    return view(itemize_iterator(storage, 0), itemize_iterator(storage, sizeof...(Elements)));
}

} /* namespace iterations */
} /* namespace essentials */

#undef ITER_VIEW_QRET

#endif /* ITERATOR_VIEW_HPP_ */
