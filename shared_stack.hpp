/*
 * shared_stack.hpp
 *
 *  Created on: Nov 25, 2015
 *      Author: belyaev
 */

#ifndef SHARED_STACK_HPP
#define SHARED_STACK_HPP

#include <memory>

namespace essentials {
namespace iterations {

template<class T>
class shared_stack {
    struct body {
        const T head;
        std::shared_ptr<body> tail;

        body(const T& v, std::shared_ptr<body> tail): head(v), tail(tail) {}
    };

    std::shared_ptr<body> container = nullptr;

    shared_stack(std::shared_ptr<body> bptr): container(bptr) {};

public:
    shared_stack() = default;
    shared_stack(shared_stack&&) = default;
    shared_stack(const shared_stack&) = default;
    shared_stack& operator=(shared_stack&&) = default;
    shared_stack& operator=(const shared_stack&) = default;

    bool empty() const {
        return container == nullptr;
    }

    shared_stack push(const T& value) const {
        return shared_stack{ std::make_shared<body>(value, container) };
    }

    const T& top() const {
        return container->head;
    }

    shared_stack pop() const {
        return shared_stack{ container->tail };
    }


public:
    shared_stack reverse() const {
        shared_stack res;
        shared_stack self = *this;
        while(!self.empty()) {
            res = res.push(self.top());
            self = self.pop();
        }
        return std::move(res);
    }

    friend std::ostream& operator<<(std::ostream& ost, shared_stack ps) {
        if(ps.empty()) return ost << "<>";

        ost << "<" << ps.top();
        ps = ps.pop();
        while(!ps.empty()) {
            ost << ", " << ps.top();
            ps = ps.pop();
        }
        return ost << ">";
    }

};

} /* namespace iterations */
} /* namespace essentials */

#endif //SHARED_STACK_HPP
