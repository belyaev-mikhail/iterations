/*
 * bfs_iterator.hpp
 *
 *  Created on: May 5, 2015
 *      Author: belyaev
 */

#ifndef ABSTRACT_ITERATOR_HPP
#define ABSTRACT_ITERATOR_HPP

#include <memory>

#include "iterator_adapter.hpp"

namespace essentials {
namespace iterations {

template<class ValueType, class IteratorCategory>
struct abstract_iterator_simple;

template<class ValueType>
struct abstract_iterator_simple<ValueType, std::forward_iterator_tag> {
    uintptr_t class_identifier;

    abstract_iterator_simple(uintptr_t class_identifier):
        class_identifier(class_identifier){};

    virtual ~abstract_iterator_simple() {}

    virtual ValueType value_impl() const = 0;
    inline ValueType value() const {
        return value_impl();
    }
    virtual void next_impl() = 0;
    inline void next() {
        next_impl();
    }
    virtual bool equals_impl(const abstract_iterator_simple&) const = 0;
    inline bool equals(const abstract_iterator_simple& a) const {
        return equals_impl(a);
    }

    virtual std::unique_ptr<abstract_iterator_simple> clone () const = 0;
};

template<class ValueType>
struct abstract_iterator_simple<ValueType, std::bidirectional_iterator_tag> {
    uintptr_t class_identifier;

    abstract_iterator_simple(uintptr_t class_identifier):
        class_identifier(class_identifier){};

    virtual ~abstract_iterator_simple() {}

    virtual ValueType value_impl() const = 0;
    inline ValueType value() const {
        return value_impl();
    }
    virtual void next_impl() = 0;
    inline void next() {
        next_impl();
    }
    virtual void prev_impl() = 0;
    inline void prev() {
        next_impl();
    }
    virtual bool equals_impl(const abstract_iterator_simple&) const;
    inline bool equals(const abstract_iterator_simple& a) const {
        return equals_impl(a);
    }

    virtual std::unique_ptr<abstract_iterator_simple> clone () const = 0;
};

template<class ValueType>
struct abstract_iterator_simple<ValueType, std::random_access_iterator_tag> {
    uintptr_t class_identifier;

    abstract_iterator_simple(uintptr_t class_identifier):
        class_identifier(class_identifier){};

    virtual ~abstract_iterator_simple() {}

    virtual ValueType value_impl() const = 0;
    inline ValueType value() const {
        return value_impl();
    }
    virtual void next_impl() = 0;
    inline void next() {
        next_impl();
    }
    virtual void prev_impl() = 0;
    inline void prev() {
        next_impl();
    }
    virtual void next_impl(size_t diff) = 0;
    inline void next(size_t diff) {
        next_impl(diff);
    }
    virtual void prev_impl(size_t diff) = 0;
    inline void prev(size_t diff) {
        next_impl(diff);
    }
    virtual bool equals_impl(const abstract_iterator_simple&) const = 0;
    inline bool equals(const abstract_iterator_simple& a) const {
        return equals_impl(a);
    }
    virtual size_t distance_impl(const abstract_iterator_simple&) const = 0;
    inline size_t distance(const abstract_iterator_simple& other) const {
        return distance_impl(other);
    }
    virtual int compare_impl(const abstract_iterator_simple&) const = 0;
    inline int compare(const abstract_iterator_simple& other) const {
        return compare_impl(other);
    }

    virtual std::unique_ptr<abstract_iterator_simple> clone () const = 0;
};

template<class It, class Category = iterator_category_for<It>>
struct concrete_iterator_simple;

template<class It>
struct concrete_iterator_simple<It, std::forward_iterator_tag>:
    abstract_iterator_simple<decltype(*std::declval<It>()), std::forward_iterator_tag> {
    using ValueType = decltype(*std::declval<It>());
    using base = abstract_iterator_simple<ValueType, std::forward_iterator_tag>;

    static uintptr_t class_id() {
        static char holder = 'A';
        return reinterpret_cast<uintptr_t>(&holder);
    }

    concrete_iterator_simple(const It& inner):
        base(class_id()), inner_(inner) {}

    It inner_;

    virtual ValueType value_impl() const { return *inner_; }
    virtual void next_impl() {
        ++inner_;
    }
    virtual bool equals_impl(const abstract_iterator_simple<decltype(*std::declval<It>()), std::forward_iterator_tag>& rhv) const {
        if(rhv.class_identifier != class_id()) return false;
        auto that = static_cast<const concrete_iterator_simple*>(&rhv);
        return this->inner_ == that->inner_;
    }

    virtual std::unique_ptr<base> clone () const{
        return std::unique_ptr<base>(new concrete_iterator_simple(inner_)); 
    }
};

template<class It>
struct concrete_iterator_simple<It, std::bidirectional_iterator_tag>:
    abstract_iterator_simple<decltype(*std::declval<It>()), std::bidirectional_iterator_tag> {

    using ValueType = decltype(*std::declval<It>());
    using base = abstract_iterator_simple<ValueType, std::bidirectional_iterator_tag>;

    static uintptr_t class_id() {
        static char holder = 'A';
        return reinterpret_cast<uintptr_t>(&holder);
    }

    concrete_iterator_simple(const It& inner):
        base(class_id()), inner_(inner) {}

    It inner_;

    virtual ValueType value_impl() const { return *inner_; }
    virtual void next_impl() { ++inner_; }
    virtual void prev_impl() { --inner_; }

    virtual bool equals_impl(const abstract_iterator_simple<decltype(*std::declval<It>()), std::bidirectional_iterator_tag>& rhv) const {
        if(rhv.class_identifier != class_id()) return false;
        auto that = static_cast<const concrete_iterator_simple*>(&rhv);
        return this->inner_ == that->inner_;
    }

    virtual std::unique_ptr<base> clone () const{
        return std::unique_ptr<base>(new concrete_iterator_simple(inner_)); 
    }
};

template<class It>
struct concrete_iterator_simple<It, std::random_access_iterator_tag>:
    abstract_iterator_simple<decltype(*std::declval<It>()), std::random_access_iterator_tag> {

    using ValueType = decltype(*std::declval<It>());
    using base = abstract_iterator_simple<ValueType, std::random_access_iterator_tag>;

    static uintptr_t class_id() {
        static char holder = 'A';
        return reinterpret_cast<uintptr_t>(&holder);
    }

    concrete_iterator_simple(const It& inner):
        base(class_id()), inner_(inner) {}

    It inner_;

    virtual ValueType value_impl() const { return *inner_; }
    virtual void next_impl() { ++inner_; }
    virtual void prev_impl() { --inner_; }
    virtual void next_impl(size_t diff) { inner_ += diff; }
    virtual void prev_impl(size_t diff) { inner_ -= diff; }

    virtual int compare_impl(const abstract_iterator_simple<decltype(*std::declval<It>()), std::random_access_iterator_tag>& rhv) const {
        if(rhv.class_identifier != class_id()) {
            auto us = class_id();
            auto them = rhv.class_identifier;
            return (us < them) ? (-1) :
                   (us > them) ?   1 :
                                   0;   
        }
            
        auto that = static_cast<const concrete_iterator_simple*>(&rhv);

        return (this->inner_ < that->inner_) ? (-1) :
               (this->inner_ > that->inner_) ?   1 :
                                                 0;    
    }

    virtual size_t distance_impl(const abstract_iterator_simple<decltype(*std::declval<It>()), std::random_access_iterator_tag>& rhv) const {
        if(rhv.class_identifier != class_id()) return (class_id() - rhv.class_identifier);
        auto that = static_cast<const concrete_iterator_simple*>(&rhv);

        return this->inner_ - that->inner_;
    }

    virtual bool equals_impl(const abstract_iterator_simple<decltype(*std::declval<It>()), std::random_access_iterator_tag>& rhv) const {
        if(rhv.class_identifier != class_id()) return false;
        auto that = static_cast<const concrete_iterator_simple*>(&rhv);
        return this->inner_ == that->inner_;
    }

    virtual std::unique_ptr<base> clone () const{
        return std::unique_ptr<base>(new concrete_iterator_simple(inner_)); 
    }
};

template<class ValueType, class Category>
struct abstract_layer2_iterator_simple {
    std::unique_ptr< abstract_iterator_simple<ValueType, Category> > base;



    template<class BaseIt>
    abstract_layer2_iterator_simple(const BaseIt& base)
        : base(new concrete_iterator_simple<BaseIt>(base)) {}

    abstract_layer2_iterator_simple(const abstract_layer2_iterator_simple& that):
        base(std::move(that.base->clone())) {}

    auto value() -> decltype(auto) { return base->value(); }
    auto value() const -> decltype(auto) { return base->value(); }
    void next() { base->next(); }
    void next(size_t diff) { base->next(diff); }
    void prev() { base->prev(); }
    void prev(size_t diff) { base->prev(diff); }
    bool equals(const abstract_layer2_iterator_simple& other) const { return base->equals(*other.base); }
    size_t distance(const abstract_layer2_iterator_simple& other) const {
        return base->distance(*other.base);
    }
    int compare(const abstract_layer2_iterator_simple& other) const {
        return base->compare(*other.base);
    }
};

template<class BaseIt>
using abstracting_iterator = iterator_adapter<
                                abstract_layer2_iterator_simple<
                                    decltype(*std::declval<BaseIt>()), 
                                    iterator_category_for<BaseIt>
                                >, 
                                iterator_category_for<BaseIt>
                             >;

template<class ValueType, class Category = std::forward_iterator_tag>
using generic_iterator = iterator_adapter<
                                abstract_layer2_iterator_simple<
                                    ValueType, 
                                    Category
                                >, 
                                Category
                          >;

template<class BaseIt>
auto abstract_iterator(const BaseIt& it) -> abstracting_iterator<BaseIt> {
    using Category = iterator_category_for<BaseIt>;
    return adapt_simple_iterator(
        abstract_layer2_iterator_simple<decltype(*it), Category>(it), 
        Category{}
    );
}


} /* namespace iterations */
} /* namespace essentials */


#endif /* ABSTRACT_ITERATOR_HPP */
