/*
 * store_iterator.hpp
 *
 *  Created on: Mar 22, 2015
 *      Author: belyaev
 */

#ifndef STORE_ITERATOR_HPP_
#define STORE_ITERATOR_HPP_

#include <memory>

#include "iterator_adapter.hpp"

namespace essentials {
namespace iterations {

namespace storing_iterator_impl_ {
    template<class C>
    using iterator_type = decltype(overloaded_begin(std::declval<C&>()));
}

template<class Container>
struct storing_iterator_simple : simple_iterator_facade<storing_iterator_impl_::iterator_type<Container>> {
    std::shared_ptr<Container> theContainer;

    using base_it = storing_iterator_impl_::iterator_type<Container>;

    storing_iterator_simple(std::shared_ptr<Container> container, base_it base):
        simple_iterator_facade<base_it>(base),
        theContainer(container){}

};

template<class Container, class It>
auto store_iterator(std::shared_ptr<Container> container, It it) {
    return adapt_simple_iterator(
        storing_iterator_simple<Container>{container, it},
        typename std::iterator_traits<It>::iterator_category{}
    );
}


} /* namespace iterations */
} /* namespace essentials */

#endif /* STORE_ITERATOR_HPP_ */
