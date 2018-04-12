#include "hope/eventloop.h"

namespace hope {

EventLoop::EventLoop()
    : m_thread_id(std::this_thread::get_id())
{
}

EventLoop::~EventLoop() {
}

bool EventLoop::is_running() const {
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_is_running;
}

void EventLoop::push_event(std::unique_ptr<Event> event, EventLoop::TimePoint when) {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_events.emplace(std::move(when), std::move(event));
    m_cond.notify_one();
}

void EventLoop::push_event(std::unique_ptr<Event> event,
                           std::chrono::milliseconds duration,
                           EventLoop::TimePoint offset) {
    std::unique_lock<std::mutex> lock(m_mutex);
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
    loop();
    return m_exit_code;
}

void EventLoop::register_event_handler(EventHandler *handler) {
    Locker lock(m_dispatch_mutex);
    m_event_handlers.push_back(handler);
}

void EventLoop::unregister_event_handler(EventHandler *handler) {
    Locker lock(m_dispatch_mutex);
    auto it = std::find(m_event_handlers.begin(), m_event_handlers.end(), handler);
    if (it != m_event_handlers.end())
        m_event_handlers.erase(it);
}

std::thread::id EventLoop::thread_id() const
{
    return m_thread_id;
}

void EventLoop::loop() {
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
                return;
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

        {
            Locker lock(m_dispatch_mutex);
            for (auto& event : events) {
                for (const auto& handler : m_event_handlers) {
                    handler->on_event(event.get());
                }
            }
        }

        events.clear();
    }
}

}
