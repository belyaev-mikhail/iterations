/*
 * count_iterator.hpp
 *
 *  Created on: Mar 22, 2015
 *      Author: belyaev
 */

#ifndef COUNT_ITERATOR_HPP_
#define COUNT_ITERATOR_HPP_

#include "iterator_adapter.hpp"

namespace essentials {
namespace iterations {

template<class ValueType = size_t>
struct counting_iterator_simple {

    ValueType val;

    counting_iterator_simple(ValueType start):
        val(start) {};

    auto value() const {
        return val;
    }
    void next() { ++val; }
    void next(size_t diff) { val += diff; }
    void prev() { --val; }
    void prev(size_t diff) { val -= diff; }

    bool equals(const counting_iterator_simple& other) const {
        return val == other.val;
    }
    ValueType distance(const counting_iterator_simple& other) const {
        return val - other.val;
    }
    int compare(const counting_iterator_simple& other) const {
        return (val < other.val) ? (-1) :
               (val > other.val) ?   1 :
                                     0;
    }
};

template<class ValueType = size_t>
auto count_iterator(ValueType from) {
    return adapt_simple_iterator(
        counting_iterator_simple<ValueType>{from},
        std::random_access_iterator_tag{}
    );
}

} /* namespace iterations */
} /* namespace essentials */

#endif /* COUNT_ITERATOR_HPP_ */