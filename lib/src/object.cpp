#include "object.h"

#include "event.h"
#include "eventloop.h"
#include "signal.h"
#include "thread.h"

#include <iostream>

namespace hope {

Object::Object()
    : m_thread_id(std::this_thread::get_id())
    , m_event_loop(m_current_event_loop)
{
    if (m_event_loop)
        m_event_loop->register_event_handler(this);
}

Object::~Object() {
    if (m_thread_id != std::this_thread::get_id()) {
        std::cerr << "Destroying an object from different thread" << std::endl;
    }

    if (m_event_loop)
        m_event_loop->unregister_event_handler(this);
}

std::thread::id Object::thread_id() const {
    return m_thread_id;
}

EventLoop *Object::event_loop() const {
    return m_event_loop;
}

void Object::move_to_thread(Thread* thread) {
    if (!thread) {
        std::cerr << "Passing null pointer to move to thread";
        return;
    }
    m_thread_id = thread->id();
    m_event_loop->unregister_event_handler(this);
    m_event_loop = nullptr;
    if (auto event_loop = thread->event_loop()) {
        m_event_loop = event_loop;
        event_loop->register_event_handler(this);
    } else {
        std::cerr << "The given thread has no event loop" << std::endl;
    }
}

void Object::on_event(Event* event) {
    if (auto signal_event = dynamic_cast<QueuedInvokationEventBase*>(event)) {
        if (signal_event->event_handler() == this) {
            signal_event->invoke();
        }
    }
}

}
