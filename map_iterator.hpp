/*
 * map_iterator.hpp
 *
 *  Created on: Mar 22, 2015
 *      Author: belyaev
 */

#ifndef MAP_ITERATOR_HPP_
#define MAP_ITERATOR_HPP_

#include "iterator_adapter.hpp"
#include "copy_assignable.hpp"

namespace essentials {
namespace iterations {

template<class It, class Mapping>
struct mapped_iterator_simple: simple_iterator_facade<It>, copy_assignable_function<Mapping> {
    using copy_assignable_function<Mapping>::call;

    mapped_iterator_simple(It it, Mapping mapping):
        simple_iterator_facade<It>{it}, copy_assignable_function<Mapping>(mapping) {}

    auto value() const -> decltype(call(*this->base)) { return call(*this->base); }
};

template<class It, class Mapping>
using mapped_iterator = iterator_adapter<mapped_iterator_simple<It, Mapping>, iterator_category_for<It>>;

template<class It, class Mapping>
auto map_iterator(It it, Mapping mapping) -> mapped_iterator<It, Mapping> {
    return adapt_simple_iterator(
        mapped_iterator_simple<It, Mapping>{it, mapping},
        iterator_category_for<It>{}
    );
}

} /* namespace iterations */
} /* namespace essentials */

#endif
