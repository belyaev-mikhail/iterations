/*
 * filter_iterator.hpp
 *
 *  Created on: Mar 22, 2015
 *      Author: belyaev
 */

#ifndef FILTER_ITERATOR_HPP_
#define FILTER_ITERATOR_HPP_

namespace essentials {
namespace iterations {

template<class It, class Pred>
struct filtered_iterator_simple: simple_iterator_facade<It> {
    It baseEnd;
    Pred pred;

    filtered_iterator_simple(It it, It end, Pred pred):
            simple_iterator_facade<It>{it}, baseEnd(end), pred(pred) {
        restore_invariant();
    }

    bool has_next() const { return !(this->base == this->baseEnd); }
    bool invariant() const { return !has_next() || pred(*this->base); }
    void restore_invariant() {
        while(!invariant()) simple_iterator_facade<It>::next();
    }
    void next() {
        if(has_next()) simple_iterator_facade<It>::next();
        restore_invariant();
    }
};

template<class It, class Pred>
auto filter_iterator(It it, It itEnd, Pred pred) {
    return adapt_simple_iterator(
        filtered_iterator_simple<It, Pred>{it, itEnd, pred},
        std::forward_iterator_tag{}
    );
}

} /* namespace iterations */
} /* namespace essentials */

#endif /* FILTER_ITERATOR_HPP_ */
