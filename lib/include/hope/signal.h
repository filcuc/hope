#pragma once

#include "hope/connection.h"
#include "hope/event.h"
#include "hope/eventhandler.h"
#include "hope/eventloop.h"
#include "hope/private/threaddata.h"
#include "hope/private/indexsequence.h"
#include "hope/private/queuedinvokationevent.h"

#include <cassert>
#include <functional>
#include <map>
#include <iostream>

namespace hope {

template<class ... Args>
class Signal {
    using Handler = std::function<void(Args...)>;
public:
    Signal() : m_thread_id(std::this_thread::get_id()) {}
    Signal(const Signal& other) = delete;
    Signal(Signal&& other) noexcept = default;
    Signal& operator=(const Signal& other) = delete;
    Signal& operator=(Signal&& other) noexcept = default;

    void emit(Args&&... args) {
        for (const std::pair<Connection, Handler>& pair : m_handlers) {
            assert(pair.second);
            pair.second(std::forward<Args>(args)...);
        }
    }

    template<typename Handler>
    Connection connect(Handler handler) {
        auto it = m_handlers.emplace(get_next_connection_id(), handler);
        return it.first->first;
    }

    template<typename T, typename std::enable_if<std::is_base_of<EventHandler, T>::value, int>::type = 0>
    Connection connect(T* handler, void(T::*func)(Args...args)) {
        return connect([this, handler, func] (Args...args) {
            if (std::this_thread::get_id() != handler->thread_id()) /* QueuedConnectoin */ {
                auto event = make_queued_invokation_event(handler, func, std::move(args)...);
                ThreadDataRegistry::get_instance().thread_data(handler->thread_id())->push_event(std::move(event));
            } else {
                (handler->*func)(std::move(args)...);
            }
        });
    }

    void disconnect(Connection c) {
        m_handlers.erase(c);
    }

private:
    Connection get_next_connection_id() {
        return m_next_connection_id++;
    }

    const std::thread::id m_thread_id;
    std::map<Connection, Handler> m_handlers;
    int64_t m_next_connection_id = 0;
};

template <>
class Signal<void> {
    using Handler = std::function<void()>;

public:
    Signal() : m_thread_id(std::this_thread::get_id()) {}
    Signal(const Signal& other) = delete;
    Signal(Signal&& other) = default;
    Signal& operator=(const Signal& other) = delete;
    Signal& operator=(Signal&& other) = default;

    void emit() {
        for (const auto& pair : m_handlers) {
            assert(pair.second);
            pair.second();
        }
    }

    template<typename Handler>
    Connection connect(Handler handler) {
        auto it = m_handlers.emplace(get_next_connection_id(), handler);
        return it.first->first;
    }

    template<typename T, typename std::enable_if<std::is_base_of<EventHandler, T>::value, int>::type = 0>
    Connection connect(T* handler, void(T::*func)()) {
        return connect([this, handler, func] {
            if (std::this_thread::get_id() != handler->thread_id()) /* QueuedConnectoin */ {
                auto event = make_queued_invokation_event(handler, func);
                ThreadDataRegistry::get_instance().thread_data(handler->thread_id())->push_event(std::move(event));
            } else {
                (handler->*func)();
            }
        });
    }

    void disconnect(Connection c) {
        m_handlers.erase(c);
    }

private:
    Connection get_next_connection_id() {
        return m_next_connection_id++;
    }

    const std::thread::id m_thread_id;
    std::map<Connection, Handler> m_handlers;
    int64_t m_next_connection_id = 0;
};

}

