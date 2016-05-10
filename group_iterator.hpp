/*
 * group_iterator.hpp
 *
 *  Created on: May 10, 2016
 *      Author: belyaev
 */

#ifndef GROUP_ITERATOR_HPP_
#define GROUP_ITERATOR_HPP_

#include "iterator_adapter.hpp"
#include "copy_assignable.hpp"

namespace essentials {
namespace iterations {

namespace group_iterator_impl {

template<class View>
View exhaust(const View& view) {
    auto begin_ = overloaded_begin(view);
    auto end_ = overloaded_end(view);
    while(!(begin_ == end_)) ++begin_;
    return View(begin_, end_);
}

template<class It, class Extractor>
using key_type = std::decay_t< decltype( std::declval<Extractor>()(*std::declval<It>()) ) >;

} /* namespace group_iterator_impl */

template<class It, class KeyExtractor, class KeyT>
struct grouping_iterator_child_simple:
        simple_iterator_facade<It> {
    std::shared_ptr<KeyT> key_;
    copy_assignable_function<KeyExtractor> extractor_;

    grouping_iterator_child_simple(It inner, const std::shared_ptr<KeyT>& key, KeyExtractor ex):
            simple_iterator_facade<It>(inner), key_(key), extractor_(ex) {}

    bool empty() const {
        if(!key_) return true;
        return !(extractor_(simple_iterator_facade<It>::value()) == *key_);
    }

    bool equals(grouping_iterator_child_simple other) const {
        if(simple_iterator_facade<It>::equals(other)) return true;
        if(empty()) return other.empty();
        return false;
    }
};

template<class It, class KeyExtractor>
using grouping_iterator_child_by_ext = 
    forward_iterator_adapter<grouping_iterator_child_simple<It, KeyExtractor, group_iterator_impl::key_type<It, KeyExtractor>>>;

template<class It, class KeyExtractor, class KeyT>
using grouping_iterator_child = 
    forward_iterator_adapter<grouping_iterator_child_simple<It, KeyExtractor, KeyT>>;

template<class It, class KeyExtractor, class KeyT>
auto group_iterator_child(It it, const std::shared_ptr<KeyT>& key, KeyExtractor ex) -> grouping_iterator_child<It, KeyExtractor, KeyT> {
    return adapt_simple_iterator(
        grouping_iterator_child_simple<It, KeyExtractor, KeyT>{it, key, ex},
        std::forward_iterator_tag{}
    );
}


template<class It, class KeyExtractor, class View>
struct grouping_iterator_parent_simple {
    It inner_;
    It innerEnd_;
    copy_assignable_function<KeyExtractor> extractor_;
    using key_t = group_iterator_impl::key_type<It, KeyExtractor>;
    std::shared_ptr<key_t> key_;
    View value_;

    grouping_iterator_parent_simple(It inner, It innerEnd, KeyExtractor ext):
            inner_(inner), innerEnd_(innerEnd), extractor_(ext), 
            key_((inner == innerEnd)? nullptr : std::make_shared<key_t>(extractor_(*inner))),
            value_(
                group_iterator_child<It, KeyExtractor, key_t>(inner_, key_, ext), 
                group_iterator_child<It, KeyExtractor, key_t>(innerEnd_, nullptr, ext)
            ) {}

    View value() const {
        return value_;
    }
    
    void next() {
        auto currentKey = key_;
        while(!(inner_ == innerEnd_) && (*currentKey == *key_)){
            ++inner_;
            if(!(inner_ == innerEnd_))
                currentKey = std::make_shared<key_t>(extractor_(*inner_));
        }
        key_ = (inner_ == innerEnd_)? nullptr : currentKey;
        value_ = View(
            group_iterator_child<It, KeyExtractor, key_t>(inner_, key_, extractor_), 
            group_iterator_child<It, KeyExtractor, key_t>(innerEnd_, nullptr, extractor_)
        );
    }

    bool equals(grouping_iterator_parent_simple other) const { 
        return inner_ == other.inner_;
    }

};

template<class It, class KeyExtractor, class View>
using grouping_iterator = forward_iterator_adapter<grouping_iterator_parent_simple<It, KeyExtractor, View>>;

template<class It, class KeyExtractor, class View>
auto group_iterator(It it, It itEnd, KeyExtractor ext) -> grouping_iterator<It, KeyExtractor, View> {
    return adapt_simple_iterator(
        grouping_iterator_parent_simple<It, KeyExtractor, View>{it, itEnd, ext},
        std::forward_iterator_tag{}
    );
}

} /* namespace iterations */
} /* namespace essentials */

#endif /* GROUP_ITERATOR_HPP_ */
