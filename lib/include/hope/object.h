#pragma once

#include "eventhandler.h"

namespace hope {

class Object : public EventHandler {
public:
    Object();

    ~Object();

    std::thread::id thread_id() const final;

    EventLoop *event_loop() const final;

protected:
    void on_event(Event *event);

private:
    const std::thread::id m_thread_id;
    EventLoop* const m_event_loop;
};

}
