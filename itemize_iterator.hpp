/*
 * itemize.hpp
 *
 *  Created on: Mar 22, 2015
 *      Author: belyaev
 */

#ifndef ITEMIZE_ITERATOR_HPP_
#define ITEMIZE_ITERATOR_HPP_

#include <tuple>
#include <memory>

#include "iterator_adapter.hpp"

namespace essentials {
namespace iterations {

namespace itemizing_iterator_impl {
    template<size_t Ix, class ...Elements>
    struct peek_tuple_element {
        static auto doit(size_t realIx, const std::tuple<Elements...>& tp) -> decltype(auto) {
            return realIx == Ix? std::get<Ix>(tp) : peek_tuple_element<Ix-1, Elements...>::doit(realIx, tp);
        }
    };

    template<class ...Elements>
    struct peek_tuple_element<0, Elements...> {
        static auto doit(size_t, const std::tuple<Elements...>& tp) -> decltype(auto) {
            return std::get<0>(tp); 
        }
    };

}

template<class ...Elements>
struct itemizing_iterator_simple {
    std::shared_ptr<const std::tuple<Elements...>> data;
    size_t position;

    itemizing_iterator_simple(std::shared_ptr<std::tuple<Elements...>> data, size_t position):
            data(data), position(position) {}

    auto value() const -> decltype(auto) {
        return itemizing_iterator_impl::peek_tuple_element<sizeof...(Elements)-1, Elements...>::doit(position, *data);
    }

    void next() { ++position; }
    void prev() { --position; }
    void next(size_t diff) { position += diff; }
    void prev(size_t diff) { position -= diff; }

    size_t distance(const itemizing_iterator_simple& other) {
        return position - other.position;
    }

    bool compare(const itemizing_iterator_simple& other) {
        return position > other.position? 1:
               position == other.position? 0:
                                           -1;
    }

    bool equals(itemizing_iterator_simple other) const {
        return data == other.data && position == other.position;
    }

};

template<class ...Elements>
using itemizing_iterator = random_access_iterator_adapter<itemizing_iterator_simple<Elements...>>;

template<class ...Elements>
auto itemize_iterator(std::shared_ptr<std::tuple<Elements...>> data, size_t position) {
    return adapt_simple_iterator(
        itemizing_iterator_simple<Elements...>{ data, position },
        std::random_access_iterator_tag{}
    );
}

} /* namespace iterations */
} /* namespace essentials */

#endif /* ITEMIZE_ITERATOR_HPP_ */
