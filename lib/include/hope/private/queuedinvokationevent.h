#pragma once

#include "hope/private/indexsequence.h"
#include "hope/event.h"

#include <tuple>

namespace hope {

class EventHandler;

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
    QueuedInvokation(QueuedInvokation&&) noexcept = default;
    QueuedInvokation& operator=(const QueuedInvokation&) = delete;
    QueuedInvokation& operator=(QueuedInvokation&&) noexcept = default;

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
    QueuedInvokation(QueuedInvokation&&) noexcept = default;
    QueuedInvokation& operator=(const QueuedInvokation&) = delete;
    QueuedInvokation& operator=(QueuedInvokation&&) noexcept = default;

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

}
