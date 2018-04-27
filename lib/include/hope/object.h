#pragma once

#include "eventhandler.h"

namespace hope {

class Thread;

class Object : public EventHandler {
public:
    Object();

    ~Object();

    std::thread::id thread_id() const final;

    void move_to_thread(Thread* thread);

    void move_to_thread(std::thread::id thread);

protected:
    void on_event(Event *event);

private:
    std::thread::id m_thread_id;
};

}
