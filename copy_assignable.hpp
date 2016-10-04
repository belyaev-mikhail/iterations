/*
 * copy_assignable_function.hpp
 *
 *  Created on: Mar 22, 2015
 *      Author: belyaev
 */

#ifndef COPY_ASSIGNABLE_FUNCTION_HPP
#define COPY_ASSIGNABLE_FUNCTION_HPP

#include <type_traits>

namespace essentials {
namespace iterations {

template<class F, 
         bool EmptyOptimizable = 
               std::is_empty<std::remove_cv_t<F>>::value 
            && !std::is_final<std::remove_cv_t<F>>::value>
struct copy_assignable_function;

template<class F>
struct copy_assignable_function<F, true>: F {
    copy_assignable_function(F f): F(f){}
    copy_assignable_function(const copy_assignable_function&) = default;
    copy_assignable_function(copy_assignable_function&&) = default;

    // Function is stateless, assigning by reconstruction has no real sense
    copy_assignable_function& operator= (const copy_assignable_function&) {
        return *this;
    }

    F& get_function() { return *this; }
    const F& get_function() const { return *this; }

    template<class ...Args>
    auto call(Args&&... args) const -> decltype(std::declval<F>()(std::forward<Args>(args)...)) {
        return get_function()(std::forward<Args>(args)...);
    }
};

template<class F>
struct copy_assignable_function<F, false> {

    union { F f; };

    copy_assignable_function(F f):
        f(f){}
    copy_assignable_function(const copy_assignable_function& other): 
        f(other.f) {}

    ~copy_assignable_function() {
        f.~F();
    }

    copy_assignable_function& operator=(const copy_assignable_function& other) {
        f.~F();
        new(&f)F(other.f);
        return *this;
    }

    F& get_function() { return f; }
    const F& get_function() const { return f; }

    template<class ...Args>
    auto operator()(Args&&... args) const -> decltype(std::declval<F>()(std::forward<Args>(args)...)) {
        return f(std::forward<Args>(args)...);
    }

    template<class ...Args>
    auto call(Args&&... args) const -> decltype(std::declval<F>()(std::forward<Args>(args)...)) {
        return f(std::forward<Args>(args)...);
    }

    operator const F&() const {
        return f;
    }
};

template<class T>
struct copy_assignable_value {
    union { T v; char dummy; };
    bool full;

    copy_assignable_value(const T& v):
        v(v), full(true){}
    copy_assignable_value(T&& v):
        v(std::move(v)), full(true){}
    copy_assignable_value(): 
        dummy(0), full(false){}
    copy_assignable_value(const copy_assignable_value& other): full(false) {
        *this = other;
    }
    copy_assignable_value(copy_assignable_value&& other): full(false) {
        *this = std::move(other);
    }

    ~copy_assignable_value() {
        if(full) v.~T();
    }

    copy_assignable_value& operator=(const copy_assignable_value& other) {
        if(full) v.~T();
        if(other.full) new(&v)T(other.v);
        full = other.full;
        return *this;
    }

    copy_assignable_value& operator=(copy_assignable_value&& other) {
        if(full) v.~T();
        if(other.full) new(&v)T(std::move(other.v));
        full = other.full;
        return *this;
    }

    copy_assignable_value& operator=(const T& other) {
        if(full) v.~T();
        new(&v)T(other);
        full = true;
        return *this;
    }

    copy_assignable_value& operator=(T&& other) {
        if(full) v.~T();
        new(&v)T(std::move(other));
        full = true;
        return *this;
    }

    T& get() { return v; }
    const T& get() const { return v; }
};

} /* namespace iterations */
} /* namespace essentials */

#endif /* COPY_ASSIGNABLE_FUNCTION_HPP */
