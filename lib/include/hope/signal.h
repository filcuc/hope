#pragma once

#include "hope/event.h"
#include "hope/eventhandler.h"
#include "hope/eventloop.h"
#include "hope/private/indexsequence.h"

#include <cassert>
#include <functional>
#include <map>
#include <iostream>

namespace hope {

class QueuedInvokationEventBase : public Event {
public:
    virtual EventHandler* event_handler() = 0;
    virtual void invoke() = 0;
};

template<class Handler, class ...Args>
class QueuedInvokation final : public QueuedInvokationEventBase {
    using HandlerFuncPtr = void(Handler::* const)(Args...);
    using HandlerFuncArgs = std::tuple<Args...>;

public:
    QueuedInvokation(Handler* handler,
                     HandlerFuncPtr handler_func,
                     Args...args)
        : m_handler(handler)
        , m_handler_func(handler_func)
        , m_handler_func_args(std::move(args)...)
    {}

    QueuedInvokation() = default;
    QueuedInvokation(const QueuedInvokation&) = delete;
    QueuedInvokation(QueuedInvokation&&) = default;
    QueuedInvokation& operator=(const QueuedInvokation&) = delete;
    QueuedInvokation& operator=(QueuedInvokation&&) = default;

    EventHandler* event_handler() final {
        return m_handler;
    }

    void invoke() final {
        invoke_impl(m_handler_func_args);
    }

    template<typename Tuple, std::size_t... I>
    void invoke_impl(Tuple& a, std::index_sequence<I...>) {
        (m_handler->*m_handler_func)(std::move(std::get<I>(a))...);
    }

    template<typename ...T, typename Indices = std::make_index_sequence<sizeof... (T)>>
    void invoke_impl(std::tuple<T...>& t) {
        invoke_impl(t, Indices{});
    }

    Handler* const m_handler = nullptr;
    HandlerFuncPtr m_handler_func = nullptr;
    HandlerFuncArgs m_handler_func_args;
};

template<class Handler>
class QueuedInvokation<Handler, void> final : public QueuedInvokationEventBase {
    using HandlerFuncPtr = void(Handler::* const)();
public:
    QueuedInvokation(Handler* handler,
                     HandlerFuncPtr handler_func)
        : m_handler(handler)
        , m_handler_func(handler_func)
    {}

    QueuedInvokation() = default;
    QueuedInvokation(const QueuedInvokation&) = delete;
    QueuedInvokation(QueuedInvokation&&) = default;
    QueuedInvokation& operator=(const QueuedInvokation&) = delete;
    QueuedInvokation& operator=(QueuedInvokation&&) = default;

    EventHandler* event_handler() final {
        return m_handler;
    }

    void invoke() final {
        (m_handler->*m_handler_func)();
    }

private:
    Handler* const m_handler = nullptr;
    HandlerFuncPtr m_handler_func = nullptr;
};

template<class Handler, class ...Args>
std::unique_ptr<QueuedInvokation<Handler, Args...>> make_queued_invokation_event(Handler* handler,
                                                                void(Handler::*handler_func)(Args...),
                                                                Args...args) {
    return std::unique_ptr<QueuedInvokation<Handler, Args...>> (new QueuedInvokation<Handler, Args...>(handler, handler_func, std::move(args)...));
}

template<class Handler>
std::unique_ptr<QueuedInvokation<Handler, void>> make_queued_invokation_event(Handler* handler,
                                                                void(Handler::*handler_func)()) {
    return std::unique_ptr<QueuedInvokation<Handler, void>>(new QueuedInvokation<Handler, void>(handler, handler_func));
}

class Connection {
public:
    Connection(int64_t id)
        : m_id(id)
    {}

    int64_t id() const {
        return m_id;
    }

    bool operator==(const Connection& other) const {
        return m_id == other.m_id;
    }

    bool operator<(const Connection& other) const {
        return m_id < other.m_id;
    }

private:
    const int64_t m_id;
};


template<class ... Args>
class Signal {
    using Handler = std::function<void(Args...)>;
public:
    Signal() : m_thread_id(std::this_thread::get_id()) {}
    Signal(const Signal& other) = delete;
    Signal(Signal&& other) = default;
    Signal& operator=(const Signal& other) = delete;
    Signal& operator=(Signal&& other) = default;

    void emit(Args&&... args) {
        for (const std::pair<Connection, Handler>& pair : m_handlers) {
            assert(pair.second);
            pair.second(std::forward<Args>(args)...);
        }
    }

    template<typename Handler>
    Connection connect(Handler handler) {
        auto it = m_handlers.emplace(m_next_connection_id++, handler);
        return it.first->first;
    }

    template<typename T, typename std::enable_if<std::is_base_of<EventHandler, T>::value, int>::type = 0>
    Connection connect(T* handler, void(T::*func)(Args...args)) {
        return connect([this, handler, func] (Args...args) {
            if (std::this_thread::get_id() != handler->thread_id()) /* QueuedConnectoin */ {
                if (EventLoop* event_loop = handler->event_loop()) {
                    auto event = make_queued_invokation_event(handler, func, std::move(args)...);
                    event_loop->push_event(std::move(event));
                } else {
                    std::cerr << "Object " << handler << " has no event loop" << std::endl;
                }
            } else {
                (handler->*func)(std::move(args)...);
            }
        });
    }

    void disconnect(Connection c) {
        m_handlers.erase(c);
    }

private:
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
        for (const std::pair<Connection, Handler>& pair : m_handlers) {
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
                if (EventLoop* event_loop = handler->event_loop()) {
                    auto event = make_queued_invokation_event(handler, func);
                    event_loop->push_event(std::move(event));
                } else {
                    std::cerr << "Object " << handler << " has no event loop" << std::endl;
                }
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

