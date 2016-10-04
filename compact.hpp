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
    class T, 
    bool IsCompactible = 
        std::is_empty<std::remove_cv_t<T>>::value
        && !std::is_final<std::remove_cv_t<T>>::value
> struct compacter;

template<class T>
struct compacter<T, false> {
    T value_;

    compacter(const T& value): value_(value){}
    compacter(T&& value): value_(std::move(value)){}
    compacter(const compacter&) = default;
    compacter(compacter&&) = default;

    T& value() { return value_; }
    const T& value() const { return value_; }
};

template<class T>
struct compacter<T, true>: T {
    compacter(const T& value): T(value){}
    compacter(T&& value): T(std::move(value)){}
    compacter(const compacter&) = default;
    compacter(compacter&&) = default;

    T& value() { return *this; }
    const T& value() const { return *this; }
};

template<
    class A, 
    class B, 
    bool IsACompactible = 
        std::is_empty<std::remove_cv_t<A>>::value
        && !std::is_final<std::remove_cv_t<A>>::value,
    bool IsBCompactible = 
        std::is_empty<std::remove_cv_t<B>>::value
        && !std::is_final<std::remove_cv_t<B>>::value
>    
struct compact_pair;

template<class A, class B>
struct compact_pair<A, B, true, false>: A {
    B second_;

    constexpr compact_pair() = default;
    constexpr compact_pair(const A& a, const B& b): A(a), second_(b) {}
    template<class C, class D>
    constexpr compact_pair(C&& c, D&& d): A(std::forward<C>(c)), second_(std::forward<D>(d)) {}
    template<class C, class D>
    constexpr compact_pair(const compact_pair<C, D>& other): A(other.first()), second_(other.second()) {}
    template<class C, class D>
    constexpr compact_pair(compact_pair<C, D>&& other): A(std::move<C>(other.first())), second_(std::move<D>(other.second())) {}

    A& first() { return *this; }
    B& second() { return second_; }
    const A& first() const { return *this; }
    const B& second() const { return second_; }
};

template<class A, class B>
struct compact_pair<A, B, false, true>: B {
    A first_;

    constexpr compact_pair() = default;
    constexpr compact_pair(const A& a, const B& b): B(b), first_(a) {}
    template<class C, class D>
    constexpr compact_pair(C&& c, D&& d): B(std::forward<D>(d)), first_(std::forward<C>(c)) {}
    template<class C, class D>
    constexpr compact_pair(const compact_pair<C, D>& other): B(other.second()), first_(other.first()) {}
    template<class C, class D>
    constexpr compact_pair(compact_pair<C, D>&& other): B(std::move<D>(other.second())), first_(std::move<C>(other.first())) {}

    A& first() { return first_; }
    B& second() { return *this; }
    const A& first() const { return first_; }
    const B& second() const { return *this; }
};

template<class A, class B>
struct compact_pair<A, B, true, true>: A, B {
    constexpr compact_pair() = default;
    constexpr compact_pair(const A& a, const B& b): A(a), B(b) {}
    template<class C, class D>
    constexpr compact_pair(C&& c, D&& d): A(std::forward<C>(c)), B(std::forward<D>(d)) {}
    template<class C, class D>
    constexpr compact_pair(const compact_pair<C, D>& other): A(other.first()), B(other.second()) {}
    template<class C, class D>
    constexpr compact_pair(compact_pair<C, D>&& other): A(std::move<C>(other.first())), B(std::move<D>(other.second())) {}

    A& first() { return *this; }
    B& second() { return *this; }
    const A& first() const { return *this; }
    const B& second() const { return *this; }
};

template<class A>
struct compact_pair<A, A, true, true>: A {
    constexpr compact_pair() = default;
    constexpr compact_pair(const A& a, const A&): A(a) {}
    template<class C, class D>
    constexpr compact_pair(C&& c, D&& d): A(std::forward<C>(c)) {}
    template<class C, class D>
    constexpr compact_pair(const compact_pair<C, D>& other): A(other.first()) {}
    template<class C, class D>
    constexpr compact_pair(compact_pair<C, D>&& other): A(std::move<C>(other.first())) {}

    A& first() { return *this; }
    A& second() { return *this; }
    const A& first() const { return *this; }
    const A& second() const { return *this; }
};

template<class A, class B>
struct compact_pair<A, B, false, false> {
    A first_;
    B second_;

    constexpr compact_pair() = default;
    constexpr compact_pair(const A& a, const B& b): first_(a), second_(b) {}
    template<class C, class D>
    constexpr compact_pair(C&& c, D&& d): first_(std::forward<C>(c)), second_(std::forward<D>(d)) {}
    template<class C, class D>
    constexpr compact_pair(const compact_pair<C, D>& other): first_(other.first()), second_(other.second()) {}
    template<class C, class D>
    constexpr compact_pair(compact_pair<C, D>&& other): first_(std::move<C>(other.first())), second_(std::move<D>(other.second())) {}

    A& first() { return first_; }
    B& second() { return second_; }
    const A& first() const { return first_; }
    const B& second() const { return second_; }
};

} /* namespace iterations */
} /* namespace essentials */

#endif /* COMPACT_HPP_ */
