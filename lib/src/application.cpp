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

#include "hope/application.h"

#include "hope/private/eventhandlerdata.h"
#include "hope/private/threaddata.h"
#include "hope/private/queuedinvokationevent.h"

using namespace hope;
using namespace detail;

Application::Application()
    : m_data(std::make_shared<EventHandlerData>(std::this_thread::get_id()))
{
    EventHandlerDataRegistry::instance().register_event_handler_data(this, m_data);
    auto lock = EventHandlerData::lock(m_data);
    ThreadDataRegistry::instance().thread_data(m_data->m_thread_id)->register_event_handler(this);
}

Application::~Application()
{
    {
        auto lock = EventHandlerData::lock(m_data);
        if (m_data->m_thread_id != std::this_thread::get_id()) {
            std::cerr << "Destroying an application from different thread" << std::endl;
        }
        ThreadDataRegistry::instance().thread_data(m_data->m_thread_id)->unregister_event_handler(this);
    }
    EventHandlerDataRegistry::instance().unregister_event_handler_data(this);
}

void Application::quit(int exit_code)
{
   m_event_loop.quit(exit_code);
}

int Application::exec()
{
    return m_event_loop.exec();
}

void Application::on_event(Event *event)
{
    if (auto signal_event = dynamic_cast<QueuedInvokationEventBase*>(event)) {
        if (signal_event->event_handler() == this) {
            signal_event->invoke();
        }
    }
}

std::thread::id Application::thread_id() const {
    auto lock = EventHandlerData::lock(m_data);
    return m_data->m_thread_id;
}

