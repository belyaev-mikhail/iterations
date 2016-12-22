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
#include <map>
#include <unordered_map>
#include <algorithm>

#include "abstract_iterator.hpp"
#include "bfs_iterator.hpp"
#include "dfs_iterator.hpp"
#include "group_iterator.hpp"
#include "map_iterator.hpp"
#include "flatten_iterator.hpp"
#include "filter_iterator.hpp"
#include "store_iterator.hpp"
#include "zip_iterator.hpp"
#include "sequence_iterator.hpp"
#include "count_iterator.hpp"
#include "itemize_iterator.hpp"
#include "cycle_iterator.hpp"
#include "product_iterator.hpp"
#include "memo_iterator.hpp"
#include "generate_iterator.hpp"


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

    template<class T>
    struct keyValue;

    template<class K, class V>
    struct keyValue<std::pair<K, V>> {
        using key = K;
        using value = V;
    };
}

template<class It>
struct iterator_view;

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

    auto cycle() const
            -> iterator_view<cycling_iterator<It>> {
        return create(
            cycle_iterator(begin_, end_, begin_),
            cycle_iterator(begin_, end_, end_)
        );
    }

    auto memo() const
            -> iterator_view<memoizing_iterator<It>> {
        return create(
            memo_iterator(begin_),
            memo_iterator(end_)
        );
    }

    auto reverse() const
            -> iterator_view<std::reverse_iterator<It>> {

        static_assert( std::is_same<iterator_category_for<It>, std::bidirectional_iterator_tag>::value
                     || std::is_same<iterator_category_for<It>, std::random_access_iterator_tag>::value,
                     "reverse() requires at least a bidirectional iterator" );
        return create(
            std::reverse_iterator<It>(end_),
            std::reverse_iterator<It>(begin_)
        );
    }

    template<class OtherIt, class ResFun>
    auto product(const iterator_view<OtherIt>& other, ResFun fun) const
            -> iterator_view<product_making_iterator<It, OtherIt, ResFun>> {
        return create(
            product_iterator(begin_, other.begin_, other.end_, other.begin_, fun),
            // other.begin_ here is on purpose:
            // unless we specify the end iterator as "outer.end_ x inner.begin_",
            // the exit condition will be unsatisfiable
            product_iterator(end_, other.begin_, other.end_, other.begin_, fun)
        );
    }

    template<class OtherIt>
    auto product(const iterator_view<OtherIt>& other) const {
        return create(
            product_iterator(begin_, other.begin_, other.end_, other.begin_),
            // other.begin_ here is on purpose:
            // unless we specify the end iterator as "outer.end_ x inner.begin_",
            // the exit condition will be unsatisfiable
            product_iterator(end_, other.begin_, other.end_, other.begin_)
        );
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

    template<class R, class F>
    std::enable_if_t<std::is_copy_constructible<value_type>::value && std::is_copy_constructible<F>::value, value_type>
    reduce(R&& deflt, F f) const {
        if(empty()) return value_type{ std::forward<R>(deflt) };
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

    template<class OtherView>
    void copyFrom(OtherView&& other) {
        auto sb = overloaded_begin(other);
        auto se = overloaded_end(other);
        auto db = begin();
        auto de = end();
        for(; sb != se && db != de; ++sb, ++db) {
            *db = *sb;
        }
    }

    template<class OtherView>
    void moveFrom(OtherView&& other) {
        auto sb = overloaded_begin(other);
        auto se = overloaded_end(other);
        auto db = begin();
        auto de = end();
        for(; sb != se && db != de; ++sb, ++db) {
            *db = std::move(*sb);
        }
    }

    template<class OtherView>
    void copyTo(OtherView&& other) {
        auto db = overloaded_begin(other);
        auto de = overloaded_end(other);
        auto sb = begin();
        auto se = end();
        for(; sb != se && db != de; ++sb, ++db) {
            *db = *sb;
        }
    }

    template<class OtherView>
    void moveTo(OtherView&& other) {
        auto db = overloaded_begin(other);
        auto de = overloaded_end(other);
        auto sb = begin();
        auto se = end();
        for(; sb != se && db != de; ++sb, ++db) {
            *db = std::move(*sb);
        }
    }

    template<class OtherView, class Equals = std::equal_to<value_type>>
    bool equals(OtherView&& other) {
        auto rb = overloaded_begin(other);
        auto re = overloaded_end(other);
        auto lb = begin();
        auto le = end();
        Equals eq;

        for(; rb != re && lb != le; ++rb, ++lb) {
            if(not eq(*lb, *rb)) return false;
        }
        return true;
    }

    iterator_view<abstracting_iterator<It>> abstract() const {
        return create(abstract_iterator(begin_), abstract_iterator(end_));
    }

    template<class Accessor>
    auto bfs(Accessor acc) {
        return create(bfs_iterator(*this, acc), bfs_iterator(create(end_, end_), acc));
    }

    template<class Accessor>
    auto dfs(Accessor acc) {
        return create(dfs_iterator(*this, acc), dfs_iterator(create(end_, end_), acc));
    }

    template<class KeyExtractor>
    auto group(KeyExtractor ex) {
        return create(
            group_iterator<It, KeyExtractor, iterator_view<grouping_iterator_child_by_ext<It, KeyExtractor>>>(begin_, end_, ex),
            group_iterator<It, KeyExtractor, iterator_view<grouping_iterator_child_by_ext<It, KeyExtractor>>>(end_, end_, ex)
        );
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
        class SFINAE = value_type,
        class Key = typename iterator_view_impl::keyValue<SFINAE>::key,
        class Value = typename iterator_view_impl::keyValue<SFINAE>::value,
        class Compare = std::less<Key>, 
        class Allocator = std::allocator< std::pair<const Key, Value> >
    >
    std::map<Key, Value, Compare, Allocator> toMap() const {
        return { begin_, end_ };
    }

    template<
        class SFINAE = value_type,
        class Key = typename iterator_view_impl::keyValue<SFINAE>::key,
        class Value = typename iterator_view_impl::keyValue<SFINAE>::value,
        class Compare = std::less<Key>, 
        class Allocator = std::allocator< std::pair<const Key, Value> >
    >
    std::multimap<Key, Value, Compare, Allocator> toMultiMap() const {
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

    template<
        class SFINAE = value_type,
        class Key = typename iterator_view_impl::keyValue<SFINAE>::key,
        class Value = typename iterator_view_impl::keyValue<SFINAE>::value,
        class Hash = std::hash<Key>,
        class KeyEqual = std::equal_to<Key>,
        class Allocator = std::allocator< std::pair<const Key, Value> >
    >
    std::unordered_map<Key, Value, Hash, KeyEqual, Allocator> toHashMap() const {
        return { begin_, end_ };
    }

    template<
        class SFINAE = value_type,
        class Key = typename iterator_view_impl::keyValue<SFINAE>::key,
        class Value = typename iterator_view_impl::keyValue<SFINAE>::value,
        class Hash = std::hash<Key>,
        class KeyEqual = std::equal_to<Key>,
        class Allocator = std::allocator< std::pair<const Key, Value> >
    >
    std::unordered_multimap<Key, Value, Hash, KeyEqual, Allocator> toHashMultiMap() const {
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
        class SFINAE = value_type,
        class Key = typename iterator_view_impl::keyValue<SFINAE>::key,
        class Value = typename iterator_view_impl::keyValue<SFINAE>::value,
        class Compare = std::less<Key>, 
        class Allocator = std::allocator< std::pair<const Key, Value> >
    >
    auto toMapView() const {
        return toContainerView< std::map<Key, Value, Compare, Allocator> >();
    }

    template<
        class SFINAE = value_type,
        class Key = typename iterator_view_impl::keyValue<SFINAE>::key,
        class Value = typename iterator_view_impl::keyValue<SFINAE>::value,
        class Compare = std::less<Key>, 
        class Allocator = std::allocator< std::pair<const Key, Value> >
    >
    auto toMultiMapView() const {
        return toContainerView< std::multimap<Key, Value, Compare, Allocator> >();
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

    template<
        class SFINAE = value_type,
        class Key = typename iterator_view_impl::keyValue<SFINAE>::key,
        class Value = typename iterator_view_impl::keyValue<SFINAE>::value,
        class Hash = std::hash<Key>,
        class KeyEqual = std::equal_to<Key>,
        class Allocator = std::allocator< std::pair<const Key, Value> >
    >
    auto toHashMapView() const {
        return toContainerView<std::unordered_map<Key, Value, Hash, KeyEqual, Allocator>>();
    }

    template<
        class SFINAE = value_type,
        class Key = typename iterator_view_impl::keyValue<SFINAE>::key,
        class Value = typename iterator_view_impl::keyValue<SFINAE>::value,
        class Hash = std::hash<Key>,
        class KeyEqual = std::equal_to<Key>,
        class Allocator = std::allocator< std::pair<const Key, Value> >
    >
    auto toHashMultiMapView() const {
        return toContainerView<std::unordered_multimap<Key, Value, Hash, KeyEqual, Allocator>>();
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

template<class LIt, class RIt>
auto operator*(const iterator_view<LIt>& self, const iterator_view<RIt>& other) {
    return self.product(other);
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

template<class E>
auto viewSingleReference(E& e) {
    return view(&e, &e + 1);
}

template<class T>
auto emptyView() {
    return view<std::decay_t<T>*>(nullptr, nullptr);
}

template<class ValueType, class Seed, class Generator, class Producer>
auto unfoldMap(Seed&& seed, Generator gen, Producer prod, size_t limit = std::numeric_limits<size_t>::max()) {
    auto begin_ = generate_iterator<ValueType>(std::forward<Seed>(seed), gen, prod);
    auto end_ = generate_iterator_limit<ValueType>(limit, gen, prod);
    return view( begin_, end_ );
}

template<class ValueType, class Seed, class Generator>
auto unfold(Seed&& seed, Generator gen, size_t limit = std::numeric_limits<size_t>::max()) {
    auto id = [](auto&& x)->decltype(auto){ return x; };
    auto begin_ = generate_iterator<ValueType>(std::forward<Seed>(seed), gen, id);
    auto end_ = generate_iterator_limit<ValueType>(limit, gen, id);
    return view( begin_, end_ );
}

template<class Callable>
auto generate(Callable callable, size_t limit = std::numeric_limits<size_t>::max()) {
    auto id = [](auto&& x)->decltype(auto){ return x; };
    auto call = [](auto&& f)->decltype(auto){ return f(); };
    auto begin_ = generate_iterator<Callable>(callable, id, call);
    auto end_ = generate_iterator_limit<Callable>(limit, id, call);
    return view( begin_, end_ );
}

template<class ValueType, class Category = std::forward_iterator_tag>
using generic_view = iterator_view<generic_iterator<ValueType, Category>>;

} /* namespace iterations */
} /* namespace essentials */

#undef ITER_VIEW_QRET

#endif /* ITERATOR_VIEW_HPP_ */
