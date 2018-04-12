#include "hope/object.h"

#include "hope/event.h"
#include "hope/eventloop.h"
#include "hope/signal.h"
#include "hope/thread.h"
#include "hope/threaddata.h"

#include <iostream>

namespace hope {

Object::Object()
    : m_thread_id(std::this_thread::get_id())
{
    if (EventLoop* ev = event_loop()) {
        ev->register_event_handler(this);
    } else {
        std::cerr << "Object created without an event loop" << std::endl;
    }
}

Object::~Object() {
    if (m_thread_id != std::this_thread::get_id()) {
        std::cerr << "Destroying an object from different thread" << std::endl;
    }

    if (EventLoop* ev = event_loop()) {
        ev->unregister_event_handler(this);
    } else {
        std::cerr << "Object created without an event loop" << std::endl;
    }
}

std::thread::id Object::thread_id() const {
    return m_thread_id;
}

EventLoop *Object::event_loop() const {
    ThreadData data = ThreadDataRegistry::get_instance().thread_data(m_thread_id);
    return data.is_valid() ? data.event_loop() : nullptr;
}

void Object::move_to_thread(Thread* thread) {
    if (!thread) {
        std::cerr << "Passing null pointer to move to thread";
        return;
    }
    move_to_thread(thread->id());
}

void Object::move_to_thread(std::thread::id thread)
{
    if (auto current = event_loop()) {
        event_loop()->unregister_event_handler(this);
    }
    m_thread_id = thread;
    if (auto current = event_loop()) {
        event_loop()->register_event_handler(this);
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
