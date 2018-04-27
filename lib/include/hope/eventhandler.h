#pragma once

#include <thread>

namespace hope {

class Event;
class EventLoop;

extern thread_local EventLoop* m_current_event_loop;

class EventHandler {
public:
    virtual ~EventHandler() = default;

    virtual void on_event(Event* event) = 0;
    virtual std::thread::id thread_id() const = 0;
};

}
