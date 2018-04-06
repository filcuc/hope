#pragma once

#include "eventhandler.h"
#include "eventloop.h"

#include <chrono>
#include <functional>

namespace hope {

class Timer : public EventHandler {
public:
    Timer() {
        if (m_current_event_loop)
            m_current_event_loop->register_event_handler(this);
    }

    void set_duration(std::chrono::milliseconds duration) {

    }

    template <class T>
    void set_on_triggered(T&& handler) {
        m_handler = std::move(handler);
    }

    void start() {
        auto t = m_current_event_loop;
    }

protected:
    void on_event(Event& event) {

    }

private:
    std::chrono::milliseconds m_duration;
    std::function<void()> m_handler;
};

}
