/*
 * generate_iterator.hpp
 *
 *  Created on: May 4, 201
 *      Author: belyaev
 */

#ifndef GENERATE_ITERATOR_HPP_
#define GENERATE_ITERATOR_HPP_

#include "iterator_adapter.hpp"
#include "copy_assignable.hpp"

namespace essentials {
namespace iterations {

template<class ValueType, class Generator, class Producer>
struct generating_iterator_simple {
    using GeneratorStorage = copy_assignable_function<Generator>;
    using ProducerStorage = copy_assignable_function<Producer>;
    using CurrentStorage = std::shared_ptr<ValueType>;

    compact_tuple<GeneratorStorage, ProducerStorage, CurrentStorage> data_;
    size_t generation_;

    template<class Seed>
    generating_iterator_simple(Seed&& startingValue, Generator generator, Producer producer):
        data_(generator, producer, std::make_shared<ValueType>(std::forward<Seed>(startingValue))), generation_(0) {};

    struct limiting_iterator{};

    generating_iterator_simple(limiting_iterator, size_t lastGeneration, Generator generator, Producer producer):
        data_(generator, producer, nullptr), generation_(lastGeneration) {};

    void next() {
        auto&& generator = data_.template get<GeneratorStorage>();
        auto&& current = data_.template get<CurrentStorage>();
        current = std::make_shared<ValueType>(generator(*current));
        ++generation_;
    }

    auto value() const -> decltype(auto) {
        auto&& producer = data_.template get<ProducerStorage>();
        auto&& current = data_.template get<CurrentStorage>();
        return producer(*current);
    }

    bool equals(const generating_iterator_simple& other) const {
        return generation_ == other.generation_;
    }
};

template<class ValueType, class Generator, class Producer>
using generating_iterator = iterator_adapter<
    generating_iterator_simple<ValueType, Generator, Producer>,
    std::forward_iterator_tag
>;

template<class ValueType, class Seed, class Generator, class Producer>
auto generate_iterator(Seed&& seed, Generator generator, Producer producer) -> generating_iterator<ValueType, Generator, Producer> {
    return adapt_simple_iterator(
        generating_iterator_simple<ValueType, Generator, Producer>{ std::forward<Seed>(seed), generator, producer},
        std::forward_iterator_tag{}
    );
}

template<class ValueType, class Generator, class Producer>
auto generate_iterator_limit(size_t genLimit, Generator generator, Producer producer) -> generating_iterator<ValueType, Generator, Producer> {
    using rIt = generating_iterator_simple<ValueType, Generator, Producer>;
    return adapt_simple_iterator(
        rIt{ typename rIt::limiting_iterator{}, genLimit, generator, producer},
        std::forward_iterator_tag{}
    );
}

} /* namespace iterations */
} /* namespace essentials */

#endif
