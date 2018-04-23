#pragma once

#include "eventhandler.h"
#include "hope/eventloop.h"
#include "hope/object.h"
#include "hope/signal.h"

#include <chrono>
#include <functional>

namespace hope {

class Timer final : public Object {
public:
    Timer();

    ~Timer() override;

    std::chrono::milliseconds duration() const;

    void set_duration(std::chrono::milliseconds duration);

    Signal<void>& triggered();

    void start();

protected:
    void on_event(Event* event) final;

private:
    std::chrono::milliseconds m_duration;
    Signal<void> m_triggered;
};

}
