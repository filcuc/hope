/*
    Copyright (C) 2018 Filippo Cucchetto.
    Contact: https://github.com/filcuc/hope

    This file is part of the Hope library.

    The Hope library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License.

    The Hope library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with the Hope library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <hope/object.h>

#include <hope/event.h>
#include <hope/eventloop.h>
#include <hope/signal.h>
#include <hope/thread.h>
#include <hope/private/eventhandlerdata.h>
#include <hope/private/threaddata.h>

#include <iostream>

using namespace hope;
using namespace detail;

Object::Object()
    : m_data(std::make_shared<EventHandlerData>(std::this_thread::get_id()))
{
    EventHandlerDataRegistry::instance().register_event_handler_data(this, m_data);
    {
        auto lock = m_data->lock();
        ThreadDataRegistry::instance().thread_data(m_data->m_thread_id)->register_event_handler(this);
    }
}

Object::~Object() {
    {
        auto lock = m_data->lock();
        if (m_data->m_thread_id != std::this_thread::get_id()) {
            std::cerr << "Destroying an object from different thread" << std::endl;
        }
        ThreadDataRegistry::instance().thread_data(m_data->m_thread_id)->unregister_event_handler(this);
    }
    EventHandlerDataRegistry::instance().unregister_event_handler_data(this);
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
    auto lock = m_data->lock();
    ThreadDataRegistry::instance().thread_data(m_data->m_thread_id)->unregister_event_handler(this);
    m_data->m_thread_id = thread;
    ThreadDataRegistry::instance().thread_data(m_data->m_thread_id)->register_event_handler(this);
}

void Object::on_event(Event* event) {
    if (auto signal_event = dynamic_cast<QueuedInvokationEventBase*>(event)) {
        if (signal_event->event_handler() == this) {
            signal_event->invoke();
        }
    }
}
