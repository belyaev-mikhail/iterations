/*
 * filter_iterator.hpp
 *
 *  Created on: Mar 22, 2015
 *      Author: belyaev
 */

#ifndef FILTER_ITERATOR_HPP_
#define FILTER_ITERATOR_HPP_

#include "copy_assignable.hpp"
#include "compact.hpp"

namespace essentials {
namespace iterations {

template<class It, class Pred>
struct filtered_iterator_simple: simple_iterator_facade<It> {
    compact_tuple<copy_assignable_function<Pred>, It> data_;

    inline const Pred& predicate() const { return data_.template get<0>(); }
    inline const It& baseEnd() const { return data_.template get<1>(); }

    filtered_iterator_simple() = default;
    filtered_iterator_simple(It it, It end, Pred pred):
            simple_iterator_facade<It>{it}, data_(pred, end) {
        restore_invariant();
    }

    bool has_next() const { return !(this->base == this->baseEnd()); }
    bool invariant() const { 
        auto&& pred = predicate();
        return !has_next() || pred(*this->base); 
    }
    void restore_invariant() {
        while(!invariant()) simple_iterator_facade<It>::next();
    }
    void next() {
        if(has_next()) simple_iterator_facade<It>::next();
        restore_invariant();
    }
};

template<class It, class Pred>
using filtered_iterator = forward_iterator_adapter<filtered_iterator_simple<It, Pred>>;

template<class It, class Pred>
auto filter_iterator(It it, It itEnd, Pred pred) -> filtered_iterator<It, Pred> {
    return adapt_simple_iterator(
        filtered_iterator_simple<It, Pred>{it, itEnd, pred},
        std::forward_iterator_tag{}
    );
}


} /* namespace iterations */
} /* namespace essentials */

#endif /* FILTER_ITERATOR_HPP_ */
