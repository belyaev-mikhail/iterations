/*
 * count_iterator.hpp
 *
 *  Created on: Nov 10, 2015
 *      Author: belyaev
 */

#ifndef MEMO_ITERATOR_HPP_
#define MEMO_ITERATOR_HPP_

#include <unordered_map>

#include "iterator_adapter.hpp"

namespace essentials {
namespace iterations {

template<class It>
struct memo_iterator_simple: simple_iterator_facade<It> {
    using facade = simple_iterator_facade<It>;
    using key_t = size_t;
    using elem_t = std::decay_t<decltype(*std::declval<It>())>;

    key_t key;

    std::shared_ptr<std::unordered_map<key_t, elem_t>> storage;

    memo_iterator_simple(It it):
        simple_iterator_facade<It>{it}, storage(std::make_shared<std::unordered_map<key_t, elem_t>>()) {}

    void next() { ++key; facade::next(); }
    void next(size_t diff) { key += diff; facade::next(diff); }
    void prev() { --key; facade::prev(); }
    void prev(size_t diff) { key -= diff; facade::prev(diff); }

    auto value() const -> decltype(facade::value()) {
        auto fit = storage->find(key);
        if(fit == std::end(*storage)) {
            auto emplace_result = storage->emplace(key, facade::value());
            return emplace_result.first->second;
        } else {
            return fit->second;
        }
    }
};

template<class It>
using memoizing_iterator = iterator_adapter<memo_iterator_simple<It>, iterator_category_for<It>>;

template<class It>
auto memo_iterator(It it) -> memoizing_iterator<It> {
    return adapt_simple_iterator(
        memo_iterator_simple<It>{it},
        iterator_category_for<It>{}
    );
}

} /* namespace iterations */
} /* namespace essentials */

#endif /* MEMO_ITERATOR_HPP_ */
