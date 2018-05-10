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

#include "hope/eventloop.h"
#include "hope/private/threaddata.h"

namespace hope {

class RegisterEvent final : public Event {
public:
    RegisterEvent(EventHandler* handler)
        : m_handler(handler)
    {}

    EventHandler* m_handler = nullptr;
};

EventLoop::EventLoop()
    : m_thread_id(std::this_thread::get_id())
{
    m_event_handlers.emplace(this, new std::atomic<bool>(true));
    ThreadDataRegistry::instance().current_thread_data()->set_event_loop(this);
}

EventLoop::~EventLoop() {
    m_event_handlers.erase(this);
    ThreadDataRegistry::instance().current_thread_data()->set_event_loop(nullptr);
}

bool EventLoop::is_running() const {
    Locker locker(m_mutex);
    return m_is_running;
}

void EventLoop::push_event(std::unique_ptr<Event> event, EventLoop::TimePoint when) {
    Locker lock(m_mutex);
    m_events.emplace(when, std::move(event));
    m_cond.notify_one();
}

void EventLoop::push_event(std::unique_ptr<Event> event,
                           std::chrono::milliseconds duration,
                           EventLoop::TimePoint offset) {
    Locker lock(m_mutex);
    m_events.emplace(offset + duration, std::move(event));
    m_cond.notify_one();
}

void EventLoop::quit(int exit_code) {
    Locker lock(m_mutex);
    m_exit = true;
    m_exit_code = exit_code;
    m_cond.notify_one();
}

int EventLoop::exec() {
    return loop();
}

void EventLoop::register_event_handler(EventHandler *handler) {
    // We queued the connection in order to not receive already queued events
    // In other words the handler will receive all the events after his registration
    std::unique_ptr<Event> event(new RegisterEvent(handler));
    push_event(std::move(event));
}

void EventLoop::unregister_event_handler(EventHandler *handler) {
    Locker lock(m_event_handlers_mutex);
    auto it = m_event_handlers.find(handler);
    if (it != m_event_handlers.end())
        *it->second = false;
}

std::thread::id EventLoop::thread_id() const
{
    Locker lock(m_mutex);
    return m_thread_id;
}

void EventLoop::on_event(Event *event)
{
    if (auto registerEvent = dynamic_cast<RegisterEvent*>(event)) {
        m_event_handlers.emplace(registerEvent->m_handler, new std::atomic<bool>(true));
    }
}

int EventLoop::loop() {
    {
        Locker lock(m_mutex);
        m_is_running = true;
    }

    std::vector<std::unique_ptr<Event>> events;

    while (true) {
        while (true) {
            Locker lock(m_mutex);
            if (m_exit) {
                m_is_running = false;
                return m_exit_code;
            } else if (m_events.empty()) {
                m_cond.wait(lock);
            } else {
                const TimePoint now = Clock::now();
                while (!m_events.empty() && m_events.begin()->first <= now) {
                    events.emplace_back(std::move(m_events.begin()->second));
                    m_events.erase(m_events.begin());
                }
                if (!events.empty()) {
                    break;
                }
                m_cond.wait_until(lock, m_events.begin()->first);
            }
        }

        for (auto& event : events) {
            for (const auto& handler : m_event_handlers) {
                if (handler.second)
                    handler.first->on_event(event.get());
            }
        }

        cleanup_handlers();

        events.clear();
    }
}

void EventLoop::cleanup_handlers()
{
    Locker lock(m_event_handlers_mutex);
    for (auto it = m_event_handlers.begin(); it != m_event_handlers.end(); ) {
        const std::unique_ptr<std::atomic<bool>>& valid = it->second;
        if (!*valid) {
            it = m_event_handlers.erase(it);
        } else {
            ++it;
        }
    }
}

}
