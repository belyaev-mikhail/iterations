/*
 * shared_queue.hpp
 *
 *  Created on: Nov 25, 2015
 *      Author: belyaev
 */

#ifndef SHARED_QUEUE_HPP
#define SHARED_QUEUE_HPP

namespace essentials {
namespace iterations {

template<class T>
class shared_queue {
    shared_stack<T> input;
    shared_stack<T> output;


    shared_queue(shared_stack<T> input, shared_stack<T> output): input(input), output(output) {}
public:
    shared_queue() = default;
    shared_queue(const shared_queue&) = default;
    shared_queue(shared_queue&&) = default;

    shared_queue& operator=(const shared_queue&) = default;
    shared_queue& operator=(shared_queue&&) = default;

    bool empty() const {
        return output.empty();
    }

    shared_queue push(const T& value) const {
        if(empty()) return shared_queue{ input, output.push(value) };
        return shared_queue{ input.push(value), output };
    }

    shared_queue pop() const {
        if(empty()) return shared_queue{};

        auto o = output.pop();
        if(o.empty()) {
            return shared_queue{ shared_stack<T>{}, input.reverse() };
        }

        return shared_queue{ input, o };
    }

    const T& top() const {
        return output.top();
    }

    friend std::ostream& operator<<(std::ostream& ost, shared_queue ps) {
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

#endif //SHARED_QUEUE_HPP
