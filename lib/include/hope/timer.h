#pragma once

#include "eventhandler.h"
#include "eventloop.h"

#include <chrono>
#include <functional>

namespace hope {

class Timer final : public EventHandler {
public:
    Timer();

    ~Timer() override;

    std::chrono::milliseconds duration() const;

    void set_duration(std::chrono::milliseconds duration);

    Signal<void>& triggered();

    void start();

    std::thread::id thread_id() const final;

protected:
    void on_event(Event* event) final;

private:
    const std::thread::id m_thread_id;
    std::chrono::milliseconds m_duration;
    Signal<void> m_triggered;
};

}
