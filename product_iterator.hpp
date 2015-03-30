/*
 * product_iterator.hpp
 *
 *  Created on: Mar 27, 2015
 *      Author: belyaev
 */

#ifndef PRODUCT_ITERATOR_HPP_
#define PRODUCT_ITERATOR_HPP_

#include "iterator_adapter.hpp"
#include "copy_assignable.hpp"

namespace essentials {
namespace iterations {

template<class OuterIt, class InnerIt, class ResFun>
struct product_making_iterator_simple {
    OuterIt outerIt_;
    InnerIt innerIt_;
    InnerIt innerBegin_;
    InnerIt innerEnd_;
    copy_assignable_function<ResFun> finalizer_;

    product_making_iterator_simple(OuterIt outerIt, InnerIt begin, 
                                   InnerIt end, InnerIt it, ResFun finalizer):
        outerIt_(outerIt), innerIt_(it), innerBegin_(begin), 
        innerEnd_(end), finalizer_(finalizer) {};

    void next() {
        ++innerIt_;
        if(innerIt_ == innerEnd_) {
            ++outerIt_;
            innerIt_ = innerBegin_;
        }
    }

    void prev() {
        if(innerIt_ == innerBegin_) {
            --outerIt_;
            innerIt_ = innerEnd_;
        }
        --innerIt_;
    }

    auto value() const -> decltype(finalizer_(*outerIt_,*innerIt_)) { 
        return finalizer_(*outerIt_,*innerIt_); 
    }

    bool equals(const product_making_iterator_simple& other) const {
        return outerIt_ == other.outerIt_ && innerIt_ == other.innerIt_;
    }
};

template<class OuterIt, class InnerIt, class ResFun>
using product_making_iterator = iterator_adapter<
        product_making_iterator_simple<OuterIt, InnerIt, ResFun>,
        common_iterator_category<common_iterator_category_for<OuterIt, InnerIt>, std::bidirectional_iterator_tag>
    >;

template<class OuterIt, class InnerIt, class ResFun>
auto product_iterator(OuterIt outerIt, InnerIt begin, 
                      InnerIt end, InnerIt it, ResFun finalizer)
        -> product_making_iterator<OuterIt, InnerIt, ResFun> {
    return adapt_simple_iterator(
        product_making_iterator_simple<OuterIt, InnerIt, ResFun>{outerIt, begin, end, it, finalizer},
        common_iterator_category<common_iterator_category_for<OuterIt, InnerIt>, std::bidirectional_iterator_tag>{}
    );
}

} /* namespace iterations */
} /* namespace essentials */

#endif /* PRODUCT_ITERATOR_HPP_ */
