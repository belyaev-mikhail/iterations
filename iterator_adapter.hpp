#include <utility>
#include <type_traits>
#include <iterator>

namespace essentials
{

namespace impl_
{
    template<class T> const T& ensure_const(T&);
} /* namespace impl_ */

template<class T>
struct iterator_pseudo_ptr {
    T value;
    
    T& operator*() const { return value; }
    T* operator->() const { return &value; }
};

namespace impl_
{

template<class T>
iterator_pseudo_ptr<T> take_ptr(T&& value) {
    return iterator_pseudo_ptr<T>{ std::move(value) };
}

template<class T>
T* take_ptr(T& value) {
    return &value;
}

template<class T>
T&& return_ptr(iterator_pseudo_ptr<T>& p) {
    return std::move(p.value);
}

template<class T>
T& return_ptr(T* p) {
    return *p;
}

template<class F>
using invoke = typename F::type;

} /* namespace impl_ */

template<class Simple>
struct forward_iterator_adapter {
    Simple derived;
    
    using reference = decltype(derived.value());
    using value_type = impl_::invoke<std::remove_reference<reference>>;
    using pointer = impl_::invoke<
        std::conditional<
            std::is_reference<reference>::value, 
            impl_::invoke<
                std::add_pointer<reference>
            >, 
            iterator_pseudo_ptr<value_type>
        >
    >;
    using iterator_category = std::forward_iterator_tag;
    using difference_type = size_t;

    friend bool operator==(forward_iterator_adapter lhv, forward_iterator_adapter rhv) {
        return lhv.derived.equals(rhv.derived);
    }
    
    friend bool operator!=(forward_iterator_adapter lhv, forward_iterator_adapter rhv) {
        return !lhv.derived.equals(rhv.derived);
    }
    
    reference operator*() const {
        return derived.value();
    }
    
    pointer operator->() const {
        return take_ptr(**this);
    }
    
    forward_iterator_adapter operator++(int) {
        auto copy = *this;
        derived.next();
        return copy;
    }
    
    forward_iterator_adapter& operator++() {
        derived.next();
        return *this;
    }
    
};

template<class Simple>
struct bidir_iterator_adapter {
    Simple derived;
    
    using reference = decltype(derived.value());
    using value_type = impl_::invoke<std::remove_reference<reference>>;
    using pointer = impl_::invoke<
        std::conditional<
            std::is_reference<reference>::value, 
            impl_::invoke<std::add_pointer<reference>>, 
            iterator_pseudo_ptr<value_type>
        >
    >;
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = size_t;
        
    friend bool operator==(bidir_iterator_adapter lhv, bidir_iterator_adapter rhv) {
        return lhv.derived.equals(rhv.derived);
    }
    
    friend bool operator!=(bidir_iterator_adapter lhv, bidir_iterator_adapter rhv) {
        return !lhv.derived.equals(rhv.derived);
    }
    
    reference operator*() const {
        return derived.value();
    }
    
    pointer operator->() const {
        return take_ptr(**this);
    }
    
    bidir_iterator_adapter operator++(int) {
        auto copy = *this;
        derived.next();
        return copy;
    }
    
    bidir_iterator_adapter& operator++() {
        derived.next();
        return *this;
    }
    
    bidir_iterator_adapter operator--(int) {
        auto copy = *this;
        derived.prev();
        return copy;
    }
    
    bidir_iterator_adapter& operator--() {
        derived.prev();
        return *this;
    }

};

template<class Simple>
struct random_access_iterator_adapter {
    Simple derived;
    
    using reference = decltype(derived.value());
    using value_type = impl_::invoke<std::remove_reference<reference>>;
    using pointer = impl_::invoke<
        std::conditional<
            std::is_reference<reference>::value, 
            impl_::invoke<
                std::add_pointer<reference>
            >, 
            iterator_pseudo_ptr<value_type>
        >
    >;
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = decltype(derived.distance(derived));
        
    friend bool operator==(random_access_iterator_adapter lhv, random_access_iterator_adapter rhv) {
        return lhv.derived.equals(rhv.derived);
    }
    
    friend bool operator!=(random_access_iterator_adapter lhv, random_access_iterator_adapter rhv) {
        return !lhv.derived.equals(rhv.derived);
    }
    
    reference operator*() const {
        return derived.value();
    }
    
    pointer operator->() const {
        return take_ptr(**this);
    }
    
    random_access_iterator_adapter operator++(int) {
        auto copy = *this;
        derived.next();
        return copy;
    }
    
    random_access_iterator_adapter& operator++() {
        derived.next();
        return *this;
    }
    
    random_access_iterator_adapter operator--(int) {
        auto copy = *this;
        derived.prev();
        return copy;
    }
    
    random_access_iterator_adapter& operator--() {
        derived.prev();
        return *this;
    }

    random_access_iterator_adapter& operator+=(difference_type diff) {
        derived.next(diff);
        return *this;
    }
    
    random_access_iterator_adapter& operator-=(difference_type diff) {
        derived.prev(diff);
        return *this;
    }

    reference operator[](difference_type diff) const {
        auto copy = *this;
        copy += diff;
        return *copy;
    }

    friend random_access_iterator_adapter operator+(random_access_iterator_adapter ra, difference_type diff) {
        ra += diff;
        return ra;
    }

    friend random_access_iterator_adapter operator+(difference_type diff, random_access_iterator_adapter ra) {
        ra += diff;
        return ra;
    }

    friend random_access_iterator_adapter operator-(random_access_iterator_adapter ra, difference_type diff) {
        ra -= diff;
        return ra;
    }

    friend difference_type operator-(random_access_iterator_adapter from, random_access_iterator_adapter to) {
        return from.derived.distance(to.derived);
    }

    friend bool operator<(random_access_iterator_adapter lhv, random_access_iterator_adapter rhv) {
        return lhv.derived.compare(rhv.derived) < 0;
    }

    friend bool operator>(random_access_iterator_adapter lhv, random_access_iterator_adapter rhv) {
        return lhv.derived.compare(rhv.derived) > 0;
    }

    friend bool operator<=(random_access_iterator_adapter lhv, random_access_iterator_adapter rhv) {
        return lhv.derived.compare(rhv.derived) <= 0;
    }

    friend bool operator>=(random_access_iterator_adapter lhv, random_access_iterator_adapter rhv) {
        return lhv.derived.compare(rhv.derived) >= 0;
    }

};

template<class SimpleIt>
forward_iterator_adapter<SimpleIt> adapt_simple_iterator(SimpleIt it, std::forward_iterator_tag) {
    return forward_iterator_adapter<SimpleIt>{ it };
}

template<class SimpleIt>
bidir_iterator_adapter<SimpleIt> adapt_simple_iterator(SimpleIt it, std::bidirectional_iterator_tag) {
    return bidir_iterator_adapter<SimpleIt>{ it };
}

template<class SimpleIt>
random_access_iterator_adapter<SimpleIt> adapt_simple_iterator(SimpleIt it, std::random_access_iterator_tag) {
    return random_access_iterator_adapter<SimpleIt>{ it };
}

} /* namespace essentials */
