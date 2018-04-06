#pragma once

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

private:
    const int64_t m_id;
};


template<class ... Args>
class Signal {
public:
    void emit(Args&&... args) {
        if (m_handler)
            m_handler(std::forward<Args>(args)...);
    }

    template<typename Handler>
    void connect(Handler handler) {
        m_handler = std::move(handler);
    }

private:
    std::function<void(Args...)> m_handler;
};

template <>
class Signal<void> {
public:
    void emit() {
        if (m_handler)
            m_handler();
    }

    template<typename Handler>
    void connect(Handler handler) {
        m_handler = std::move(handler);
    }

private:
    std::function<void()> m_handler;
};

}

