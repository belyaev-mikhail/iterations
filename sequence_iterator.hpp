/*
 * sequence_iterator.hpp
 *
 *  Created on: Mar 22, 2015
 *      Author: belyaev
 */

#ifndef SEQUENCE_ITERATOR_HPP_
#define SEQUENCE_ITERATOR_HPP_

#include "iterator_adapter.hpp"

namespace essentials {
namespace iterations {

template<class ItLhv, class ItRhv>
struct sequencing_iterator_simple {

    ItLhv lhv;
    ItLhv lhvEnd;
    ItRhv rhv;

    sequencing_iterator_simple(ItLhv lhv, ItLhv lhvEnd, ItRhv rhv):
        lhv(lhv), lhvEnd(lhvEnd), rhv(rhv) {}

    bool isFirstPart() const { return !(lhv == lhvEnd); }

    auto value() const -> decltype(false? *lhv : *rhv) {
        return isFirstPart()? *lhv : *rhv;
    }

    void next() { 
        if(isFirstPart()) ++lhv; 
        else ++rhv; 
    }

    bool equals(const sequencing_iterator_simple& other) const {
        return lhv == other.lhv && rhv == other.rhv;
    }
};


template<class LIt, class RIt>
auto sequence_iterator(LIt lit, LIt litEnd, RIt rit) {
    return adapt_simple_iterator(
        sequencing_iterator_simple<LIt, RIt>{lit, litEnd, rit},
        std::forward_iterator_tag{}
    );
}

} /* namespace iterations */
} /* namespace essentials */

#endif /* SEQUENCE_ITERATOR_HPP_ */
