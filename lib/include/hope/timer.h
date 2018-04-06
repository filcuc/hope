#pragma once

#include "eventhandler.h"
#include "eventloop.h"

#include <chrono>
#include <functional>

namespace hope {

class Timer : public EventHandler {
public:
    Timer();

    ~Timer() override;

    void set_duration(std::chrono::milliseconds duration);

    template <class T>
    void set_on_triggered(T&& handler) {
        m_handler = std::move(handler);
    }

    void start();

protected:
    void on_event(Event* event) override;

private:
    std::chrono::milliseconds m_duration;
    std::function<void()> m_handler;
};

}
