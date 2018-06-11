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

#include <hope/signal.h>
#include <hope/thread.h>
#include <hope/private/objectdata.h>
#include <hope/private/queuedinvokationevent.h>
#include <hope/private/threaddata.h>

#include <iostream>

using namespace hope;
using namespace detail;

Object::Object(bool init)
    : m_initialized(false)
    , m_terminated(false)
    , m_data(std::make_shared<ObjectData>(std::this_thread::get_id())) {
    if (init)
        initialize();
}

Object::Object()
    : Object(true) {
}

void Object::initialize() {
    if (m_initialized.exchange(true))
        return;
    ObjectDataRegistry::instance().register_object_data(this, m_data);
    {
        auto lock = m_data->lock();
        ThreadDataRegistry::instance().thread_data(m_data->m_thread_id)->register_object(this);
    }
}

void Object::terminate() {
    if (m_terminated.exchange(true))
        return;
    {
        auto lock = m_data->lock();
        if (m_data->m_thread_id != std::this_thread::get_id()) {
            std::cerr << "Destroying an object from different thread" << std::endl;
        }
        ThreadDataRegistry::instance().thread_data(m_data->m_thread_id)->unregister_object(this);
    }
    ObjectDataRegistry::instance().unregister_object_data(this);
}

Object::~Object() {
    terminate();
}

void Object::move_to_thread(Thread* thread) {
    if (!thread) {
        std::cerr << "Passing null pointer to move to thread" << std::endl;
        return;
    }
    move_to_thread(thread->id());
}

void Object::move_to_thread(std::thread::id thread) {
    auto lock = m_data->lock();
    ThreadDataRegistry::instance().thread_data(m_data->m_thread_id)->unregister_object(this);
    m_data->m_thread_id = thread;
    ThreadDataRegistry::instance().thread_data(m_data->m_thread_id)->register_object(this);
}

void Object::on_event(Event* event) {
    if (auto signal_event = dynamic_cast<QueuedInvokationEventBase*>(event)) {
        if (signal_event->object() == this) {
            signal_event->invoke();
        }
    }
}
