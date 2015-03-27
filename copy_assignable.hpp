/*
 * copy_assignable_function.hpp
 *
 *  Created on: Mar 22, 2015
 *      Author: belyaev
 */

#ifndef COPY_ASSIGNABLE_FUNCTION_HPP
#define COPY_ASSIGNABLE_FUNCTION_HPP

namespace essentials {
namespace iterations {

template<class F>
struct copy_assignable_function {

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

    template<class ...Args>
    auto operator()(Args&&... args) const {
        return f(std::forward<Args>(args)...);
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
