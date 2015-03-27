/*
 * flatten_iterator.hpp
 *
 *  Created on: Mar 22, 2015
 *      Author: belyaev
 */

#ifndef FLATTEN_ITERATOR_HPP_
#define FLATTEN_ITERATOR_HPP_

#include "iterator_adapter.hpp"
#include "copy_assignable.hpp"

namespace essentials {
namespace iterations {

template<class It>
struct flattened_iterator_simple {
    It parent;
    It parentEnd;

    using ChildIt = decltype(overloaded_begin(*parent));

    copy_assignable_value<ChildIt> child; // option<ChildIt> ???

    flattened_iterator_simple(It begin, It end):
            parent(begin), parentEnd(end), child() {
        if(parent != parentEnd) child = overloaded_begin(*parent);
        restore_invariant();
    }

    bool invariant() const {
        return parent == parentEnd || child.get() != overloaded_end(*parent);
    }

    void restore_invariant() {
        while (!invariant()) {
            ++parent;
            if (parent != parentEnd)
                child = overloaded_begin(*parent);
        }
    }

    auto value() const -> decltype(*child.get()) { return *child.get(); }
    void next() {
        ++child.get();
        restore_invariant();
    }

    bool equals(flattened_iterator_simple other) const { 
        if(parent != other.parent || parentEnd != other.parentEnd) return false;
        if(parent == parentEnd) return true;
        return child.get() == other.child.get();
    }

};

template<class It>
auto flatten_iterator(It it, It itEnd) {
    return adapt_simple_iterator(
        flattened_iterator_simple<It>{it, itEnd},
        std::forward_iterator_tag{}
    );
}

template<class It>
using flattened_iterator = forward_iterator_adapter<flattened_iterator_simple<It>>;

} /* namespace iterations */
} /* namespace essentials */

#endif /* FLATTEN_ITERATOR_HPP_ */
