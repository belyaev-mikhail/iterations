/*
 * zip_iterator.hpp
 *
 *  Created on: Mar 22, 2015
 *      Author: belyaev
 */

#ifndef ZIP_ITERATOR_HPP_
#define ZIP_ITERATOR_HPP_

#include "iterator_adapter.hpp"

namespace essentials {
namespace iterations {

template<class ItLhv, class ItRhv, class Zipper>
struct zipping_iterator_simple {

    ItLhv lhv;
    ItRhv rhv;
    Zipper zipper;

    zipping_iterator_simple(ItLhv lhv, ItRhv rhv, Zipper zipper):
        lhv(lhv), rhv(rhv), zipper(zipper) {}

    auto value() const -> decltype(zipper(*lhv, *rhv)) {
        return zipper(*lhv, *rhv);
    }

    void next() { ++lhv; ++rhv; }
    void next(size_t diff) { lhv += diff; rhv += diff; }
    void prev() { --lhv; --rhv; }
    void prev(size_t diff) { lhv -= diff; rhv -= diff; }
    bool equals(const zipping_iterator_simple& other) const {
        return lhv == other.lhv || rhv == other.rhv; // this is done on purpose
    }
    size_t distance(const zipping_iterator_simple& other) const {
        return std::min(std::distance(lhv, other.lhv), std::distance(rhv, other.rhv));
    }
    int compare(const zipping_iterator_simple& other) const {
        return (lhv < other.lhv || rhv < other.rhv) ? (-1) :
               (lhv > other.lhv || rhv > other.rhv) ?   1 :
                                                        0;
    }

};

namespace zipping_iterator_impl {
    std::forward_iterator_tag unify_tags(std::forward_iterator_tag, std::forward_iterator_tag) { return {}; }
    std::forward_iterator_tag unify_tags(std::bidirectional_iterator_tag, std::forward_iterator_tag){ return {}; }
    std::forward_iterator_tag unify_tags(std::forward_iterator_tag, std::bidirectional_iterator_tag){ return {}; }
    std::forward_iterator_tag unify_tags(std::random_access_iterator_tag, std::forward_iterator_tag){ return {}; }
    std::forward_iterator_tag unify_tags(std::forward_iterator_tag, std::random_access_iterator_tag){ return {}; }
    std::bidirectional_iterator_tag unify_tags(std::bidirectional_iterator_tag, std::bidirectional_iterator_tag){ return {}; }
    std::bidirectional_iterator_tag unify_tags(std::bidirectional_iterator_tag, std::random_access_iterator_tag){ return {}; }
    std::bidirectional_iterator_tag unify_tags(std::random_access_iterator_tag, std::bidirectional_iterator_tag){ return {}; }
    std::random_access_iterator_tag unify_tags(std::random_access_iterator_tag, std::random_access_iterator_tag){ return {}; }
}

template<class LIt, class RIt, class Zipper>
auto zip_iterator(LIt lit, RIt rit, Zipper zipper) {
    return adapt_simple_iterator(
        zipping_iterator_simple<LIt, RIt, Zipper>{lit, rit, zipper},
        zipping_iterator_impl::unify_tags(
            typename std::iterator_traits<LIt>::iterator_category{},
            typename std::iterator_traits<RIt>::iterator_category{}
        )
    );
}

#define zip_iterator_FWD(X) std::forward<decltype(X)>(X)

template<class LIt, class RIt>
auto zip_iterator(LIt lit, RIt rit) {
    return zip_iterator(lit, rit, [&](auto&& lhv, auto&& rhv){ 
        using LType = std::remove_rvalue_reference_t<decltype(lhv)>;
        using RType = std::remove_rvalue_reference_t<decltype(rhv)>;
        return std::pair<LType, RType>(std::forward<LType>(lhv), std::forward<RType>(rhv));
    });
}

#undef zip_iterator_FWD

} /* namespace iterations */
} /* namespace essentials */

#endif /* ZIP_ITERATOR_HPP_ */
