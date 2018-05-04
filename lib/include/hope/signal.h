/*
    Copyright (C) 2018 Filippo Cucchetto.
    Contact: https://github.com/filcuc/hope

    This file is part of the Hope library.

    The Hope library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License.

    The Hope library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with the Hope library.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "hope/connection.h"
#include "hope/event.h"
#include "hope/eventhandler.h"
#include "hope/eventloop.h"
#include "hope/private/threaddata.h"
#include "hope/private/indexsequence.h"
#include "hope/private/queuedinvokationevent.h"

#include <atomic>
#include <cassert>
#include <functional>
#include <iostream>
#include <map>

namespace hope {

namespace detail {

template<typename ...Args>
struct BaseInvoker {
    BaseInvoker()
        : valid(true)
    {}

    virtual ~BaseInvoker() = default;

    virtual void invoke_auto(Args...args) const = 0;
    virtual void invoke_direct(Args...args) const = 0;
    virtual void invoke_queued(Args...args) const = 0;

    std::atomic<bool> valid;
};

template<class Receiver, typename ...Args>
struct Invoker final : public BaseInvoker<Args...> {
    using ReceiverMemFunc = void(Receiver::*)(Args...);

    Invoker(Receiver* receiver, ReceiverMemFunc receiver_func)
        : receiver(receiver)
        , receiver_func(receiver_func)
    {}

    void invoke_auto(Args...args) const final {
        if (std::this_thread::get_id() != receiver->thread_id()) {
            invoke_queued(std::forward<Args>(args)...);
        } else {
            invoke_direct(std::forward<Args>(args)...);
        }
    }

    void invoke_queued(Args...args) const final {
        auto event = make_queued_invokation_event(receiver, receiver_func, std::move(args)...);
        ThreadDataRegistry::instance().thread_data(receiver->thread_id())->push_event(std::move(event));
    }

    void invoke_direct(Args...args) const final {
        (receiver->*receiver_func)(std::move(args)...);
    }

    Receiver* receiver = nullptr;
    ReceiverMemFunc receiver_func = nullptr;
};

template<class Receiver, typename ...Args>
std::shared_ptr<BaseInvoker<Args...>> make_invoker(Receiver* receiver, void(Receiver::*func)(Args...)) {
    return std::make_shared<Invoker<Receiver, Args...>>(receiver, func);
}

}


template<class ... Args>
class Signal {
public:
    using SignalInvoker = std::shared_ptr<detail::BaseInvoker<Args...>>;

    Signal() : m_thread_id(std::this_thread::get_id()) {}
    Signal(const Signal& other) = delete;
    Signal(Signal&& other) noexcept = default;
    Signal& operator=(const Signal& other) = delete;
    Signal& operator=(Signal&& other) noexcept = default;

    void emit(Args... args) {
        for (const auto& pair : handlers()) {
            if (pair.second->valid) {
                pair.second->invoke_auto(std::move(args)...);
            }
        }
    }

    template<typename Receiver, typename std::enable_if<std::is_base_of<EventHandler, Receiver>::value, int>::type = 0>
    Connection connect(Receiver* handler, void(Receiver::*func)(Args...args)) {
        std::lock_guard<std::mutex> lock(m_mutex);
        Connection result = get_next_connection_id();
        m_handlers.emplace(result, detail::make_invoker(handler, func));
        return result;
    }

    void disconnect(Connection c) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_handlers.find(c);
        if (it != m_handlers.end()) {
            it->second->m_valid = false;
            m_handlers.erase(it);
        }
    }

private:
    std::map<Connection, SignalInvoker> handlers() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_handlers;
    }
    
    Connection get_next_connection_id() {
        return m_next_connection_id++;
    }

    mutable std::mutex m_mutex;
    const std::thread::id m_thread_id;
    std::map<Connection, SignalInvoker> m_handlers;
    int64_t m_next_connection_id = 0;
};

}

