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

#include "map_iterator.hpp"
#include "flatten_iterator.hpp"
#include "filter_iterator.hpp"
#include "store_iterator.hpp"
#include "zip_iterator.hpp"
#include "sequence_iterator.hpp"
#include "count_iterator.hpp"

namespace essentials {
namespace iterations {

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
    auto map(Mapping m) const {
        return create(map_iterator(begin(), m), map_iterator(end(), m));
    }

    template<class Pred>
    auto filter(Pred p) const {
        return create(
            filter_iterator(begin(), end(), p), 
            filter_iterator(end(), end(), p) 
        );
    }

    auto flatten() const {
        return create( 
            flatten_iterator(begin(), end()), 
            flatten_iterator(end(), end()) 
        );
    }

    template<class OtherView>
    auto zipWith(OtherView&& other) const {
        return create(
            zip_iterator(begin(), overloaded_begin(std::forward<OtherView>(other))),
            zip_iterator(end(), overloaded_end(std::forward<OtherView>(other)))
        );
    }

    template<class OtherView, class Zipper>
    auto zipWith(OtherView&& other, Zipper zipper) const {
        return create(
            zip_iterator(begin(), overloaded_begin(std::forward<OtherView>(other)), zipper),
            zip_iterator(end(), overloaded_end(std::forward<OtherView>(other)), zipper)
        );
    }

    template<class OtherView>
    friend auto operator^(const iterator_view& self, OtherView&& other) {
        return self.zipWith(std::forward<OtherView>(other));
    }

    template<class OtherView>
    auto seq(OtherView&& other) const {
        return create(
            sequence_iterator(begin(), end(), overloaded_begin(std::forward<OtherView>(other))),
            sequence_iterator(end(), end(), overloaded_end(std::forward<OtherView>(other)))
        );
    }

    template<class OtherView>
    friend auto operator>>(const iterator_view& self, OtherView&& other) {
        return self.seq(std::forward<OtherView>(other));
    }

    /* terminating operations */

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

template<class It>
iterator_view<It> view(It begin, It end) {
    return { begin, end };
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

} /* namespace iterations */
} /* namespace essentials */

#endif /* ITERATOR_VIEW_HPP_ */
