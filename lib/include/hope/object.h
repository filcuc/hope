#pragma once

#include "eventhandler.h"

namespace hope {

class Thread;

class Object : public EventHandler {
public:
    Object();

    ~Object();

    std::thread::id thread_id() const final;

    EventLoop *event_loop() const final;

    void move_to_thread(Thread* thread);

protected:
    void on_event(Event *event);

private:
    std::thread::id m_thread_id;
    EventLoop* m_event_loop;
};

}
