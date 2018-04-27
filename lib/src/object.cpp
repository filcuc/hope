#include "hope/object.h"

#include "hope/event.h"
#include "hope/eventloop.h"
#include "hope/signal.h"
#include "hope/thread.h"
#include "hope/private/threaddata.h"

#include <iostream>

namespace hope {

Object::Object()
    : m_thread_id(std::this_thread::get_id())
{
    ThreadDataRegistry::get_instance().thread_data(m_thread_id)->register_event_handler(this);
}

Object::~Object() {
    if (m_thread_id != std::this_thread::get_id()) {
        std::cerr << "Destroying an object from different thread" << std::endl;
    }
    ThreadDataRegistry::get_instance().thread_data(m_thread_id)->unregister_event_handler(this);
}

std::thread::id Object::thread_id() const {
    return m_thread_id;
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
    ThreadDataRegistry::get_instance().thread_data(m_thread_id)->unregister_event_handler(this);
    m_thread_id = thread;
    ThreadDataRegistry::get_instance().thread_data(m_thread_id)->register_event_handler(this);
}

void Object::on_event(Event* event) {
    if (auto signal_event = dynamic_cast<QueuedInvokationEventBase*>(event)) {
        if (signal_event->event_handler() == this) {
            signal_event->invoke();
        }
    }
}

}
