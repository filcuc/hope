#pragma once

#include <cassert>
#include <functional>
#include <map>

namespace hope {

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
    Signal() = default;
    Signal(const Signal& other) = delete;
    Signal(Signal&& other) = default;
    Signal& operator=(const Signal& other) = delete;
    Signal& operator=(Signal&& other) = default;

    void emit(Args&&... args) {
        for (const std::pair<Connection, Handler>& pair : m_handlers) {
            assert(pair->second);
            pair->second(std::forward<Args>(args)...);
        }
    }

    template<typename Handler>
    Connection connect(Handler handler) {
        auto it = m_handlers.emplace(m_next_connection_id++, handler);
        return it.first->first;
    }

    void disconnect(Connection c) {
        m_handlers.erase(c);
    }

private:
    std::map<Connection, Handler> m_handlers;
    int64_t m_next_connection_id = 0;
};

template <>
class Signal<void> {
    using Handler = std::function<void()>;

public:
    Signal() = default;
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
        auto it = m_handlers.emplace(m_next_connection_id++, handler);
        return it.first->first;
    }

    void disconnect(Connection c) {
        m_handlers.erase(c);
    }

private:
    std::map<Connection, Handler> m_handlers;
    int64_t m_next_connection_id = 0;
};

}

