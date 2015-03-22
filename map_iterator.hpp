/*
 * map_iterator.hpp
 *
 *  Created on: Mar 22, 2015
 *      Author: belyaev
 */

#ifndef MAP_ITERATOR_HPP_
#define MAP_ITERATOR_HPP_

#include "iterator_adapter.hpp"

namespace essentials {
namespace iterations {

template<class It, class Mapping>
struct mapped_iterator_simple: simple_iterator_facade<It> {
    Mapping mapping;

    mapped_iterator_simple(It it, Mapping mapping):
        simple_iterator_facade<It>{it}, mapping(mapping) {}

    auto value() const -> decltype(mapping(*this->base)) { return mapping(*this->base); }
};

template<class It, class Mapping>
auto map_iterator(It it, Mapping mapping) {
    return adapt_simple_iterator(
        mapped_iterator_simple<It, Mapping>{it, mapping},
        typename std::iterator_traits<It>::iterator_category{}
    );
}

} /* namespace iterations */
} /* namespace essentials */

#endif
