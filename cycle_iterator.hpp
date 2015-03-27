/*
 * product_iterator.hpp
 *
 *  Created on: Mar 27, 2015
 *      Author: belyaev
 */

#ifndef PRODUCT_ITERATOR_HPP_
#define PRODUCT_ITERATOR_HPP_

#include "iterator_adapter.hpp"

namespace essentials {
namespace iterations {

template<class It>
struct cycling_iterator_simple : simple_iterator_facade<It>{
    It begin_;
    It end_;
    It it_;

    cycling_iterator_simple(It begin, It end, It it):
        begin_(begin), end_(end), it_(it) {};

    void next() {
        ++it_;
        if(it_ == end_) it_ = begin_;
    }

    void prev() {
        if(it_ == begin_) it_ = end_;
        --it_;
    }
};

template<class It>
auto cycle_iterator(It begin, It end, It it) {
    return adapt_simple_iterator(
        cycling_iterator_simple<It>{begin, end, it},
        typename std::iterator_traits<It>::iterator_category{}
    );
}

} /* namespace iterations */
} /* namespace essentials */

#endif /* LIB_ITERATIONS_PRODUCT_ITERATOR_HPP_ */
