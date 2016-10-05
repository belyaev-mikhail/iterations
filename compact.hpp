/*
 * compact.hpp
 *
 *  Created on: Oct 4, 2016
 *      Author: belyaev
 */

#ifndef COMPACT_HPP_
#define COMPACT_HPP_

namespace essentials {
namespace iterations {

template<
    size_t Ix,
    class T, 
    bool IsTrivial =
        std::is_trivial<std::remove_cv_t<T>>::value,
    bool IsCompactible = 
        std::is_empty<std::remove_cv_t<T>>::value
        && !std::is_final<std::remove_cv_t<T>>::value
> struct compacter;

template<size_t Ix, class T, bool Trivial>
struct compacter<Ix, T, Trivial, false> {
    T value_;

    constexpr compacter(const T& value): value_(value){}
    constexpr compacter(T&& value): value_(std::move(value)){}

    constexpr compacter() = default;
    constexpr compacter(const compacter&) = default;
    constexpr compacter(compacter&&) = default;
    constexpr compacter& operator=(const compacter&) = default;
    constexpr compacter& operator=(compacter&&) = default;

    T& value() { return value_; }
    const T& value() const { return value_; }
};

template<size_t Ix, class T>
struct compacter<Ix, T, false, true>: T {
    constexpr compacter(const T& value): T(value){}
    constexpr compacter(T&& value): T(std::move(value)){}

    constexpr compacter() = default;
    constexpr compacter(const compacter&) = default;
    constexpr compacter(compacter&&) = default;
    constexpr compacter& operator=(const compacter&) = default;
    constexpr compacter& operator=(compacter&&) = default;

    T& value() { return *this; }
    const T& value() const { return *this; }
};

template<size_t Ix, class T>
struct compacter<Ix, T, true, true> {
    constexpr compacter(const T&){}
    constexpr compacter(T&&){}

    constexpr compacter() = default;
    constexpr compacter(const compacter&) = default;
    constexpr compacter(compacter&&) = default;
    constexpr compacter& operator=(const compacter&) = default;
    constexpr compacter& operator=(compacter&&) = default;
    
    T& instance() const {
        static T instance;
        return instance;
    }

    T& value() { return instance; }
    const T& value() const { return instance; }
};

template<class A, class B>
struct compact_pair: compacter<0, A>, compacter<1, B> {
    using First = compacter<0, A>;
    using Second = compacter<1, B>;

    constexpr compact_pair() = default;
    constexpr compact_pair(const A& c, const B& d): First(c), Second(d) {}
    template<class C, class D>
    constexpr compact_pair(C&& c, D&& d): First(std::forward<C>(c)), Second(std::forward<D>(d)) {}
    template<class C, class D>
    constexpr compact_pair(const compact_pair<C, D>& other): First(other.first()), Second(other.second()) {}
    template<class C, class D>
    constexpr compact_pair(compact_pair<C, D>&& other): First(std::move<C>(other.first())), Second(std::move<D>(other.second())) {}

    A& first() { return First::value(); }
    B& second() { return Second::value(); }

    const A& first() const { return First::value(); }
    const B& second() const { return Second::value(); }
};

template<class Seq, class ...Elements>
struct compact_tuple_impl;

template<class ...Elements, size_t ...Ixs>
struct compact_tuple_impl<std::index_sequence<Ixs...>, Elements...>
    : compacter<Ixs, Elements>... {

    template<size_t Ix>
    using type_at = std::tuple_element_t<Ix, std::tuple<Elements...>>;

    template<size_t Ix>
    using Component = compacter<Ix, type_at<Ix>>;

    constexpr compact_tuple_impl() = default;
    constexpr compact_tuple_impl(const Elements&... elements): compacter<Ixs, Elements>(elements)... {}
    template<class ...OtherElements>
    constexpr compact_tuple_impl(OtherElements&&... elements): compacter<Ixs, Elements>(std::forward<Elements>(elements))... {}
    template<class ...OtherElements>
    constexpr compact_tuple_impl(
        const compact_tuple_impl<std::index_sequence<Ixs...>, OtherElements...>& elements
    ): compacter<Ixs, Elements>(elements.template get<Ixs>())... {}
    template<class ...OtherElements>
    constexpr compact_tuple_impl(
        compact_tuple_impl<std::index_sequence<Ixs...>, OtherElements...>&& elements
    ): compacter<Ixs, Elements>(std::move(elements.template get<Ixs>()))... {}

    template<size_t Ix>
    type_at<Ix>& get() { return Component<Ix>::value(); }

    template<size_t Ix>
    const type_at<Ix>& get() const { return Component<Ix>::value(); }

};

template<class ...Elements>
using compact_tuple = compact_tuple_impl<std::index_sequence_for<Elements...>, Elements...>;

} /* namespace iterations */
} /* namespace essentials */

#endif /* COMPACT_HPP_ */
