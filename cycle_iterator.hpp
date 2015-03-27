/*
 * cycling_iterator.hpp
 *
 *  Created on: Mar 27, 2015
 *      Author: belyaev
 */

#ifndef CYCLE_ITERATOR_HPP_
#define CYCLE_ITERATOR_HPP_

#include "iterator_adapter.hpp"

namespace essentials {
namespace iterations {

template<class It>
struct cycling_iterator_simple {
    It begin_;
    It end_;
    It it_;
    size_t cycles_;

    cycling_iterator_simple(It begin, It end, It it):
        begin_(begin), end_(end), it_(it), cycles_{0} {};

    void next() {
        ++it_;
        if(it_ == end_) {
            ++cycles_;
            it_ = begin_;
        }
    }

    void prev() {
        if(it_ == begin_) {
            it_ = end_;
            --cycles_;
        }
        --it_;
    }

    auto value() const -> decltype(*it_) { return *it_; }

    bool equals(const cycling_iterator_simple& other) const {
        return it_ == other.it_ && cycles_ == other.cycles_;
    }
};

template<class It>
auto cycle_iterator(It begin, It end, It it) {
    return adapt_simple_iterator(
        cycling_iterator_simple<It>{begin, end, it},
        common_iterator_category<iterator_category_for<It>, std::bidirectional_iterator_tag>{}
    );
}

} /* namespace iterations */
} /* namespace essentials */

#endif /* CYCLE_ITERATOR_HPP_ */
