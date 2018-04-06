#pragma once

#include "event.h"
#include "eventhandler.h"

#include <algorithm>
#include <iostream>
#include <functional>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <deque>
#include <vector>
#include <map>

namespace hope {

class EventLoop {
public:
    using Clock = std::chrono::steady_clock;
    using Mutex = std::mutex;
    using Locker = std::unique_lock<Mutex>;
    using TimePoint = std::chrono::time_point<Clock>;

    EventLoop();

    ~EventLoop();

    bool is_running() const {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_is_running;
    }

    void push_event(Event event, TimePoint when = Clock::now()) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_events.emplace(std::move(when), std::move(event));
        m_cond.notify_one();
    }

    void quit(int exit_code = 0) {
        Locker lock(m_mutex);
        m_exit = true;
        m_exit_code = exit_code;
        m_cond.notify_one();
    }

    int exec() {
        std::thread t([this](){ loop();});
        t.join();
        return m_exit_code;
    }

    void register_event_handler(EventHandler* handler) {
        Locker lock(m_dispatch_mutex);
        m_event_handlers.push_back(handler);
    }

    void unregister_event_handler(EventHandler *handler) {
        Locker lock(m_dispatch_mutex);
        m_event_handlers.erase(std::remove(m_event_handlers.begin(), m_event_handlers.end(), handler));
    }

    void loop() {
        {
            Locker lock(m_mutex);
            m_is_running = true;
        }

        std::vector<Event> events;

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
                        handler->on_event(event);
                    }
                }
            }

            events.clear();

        }
    }


private:
    int m_exit_code = 0;
    bool m_exit = false;
    bool m_is_running = false;
    mutable Mutex m_mutex;
    mutable Mutex m_dispatch_mutex;
    std::condition_variable m_cond;
    std::map<TimePoint, Event> m_events;
    std::vector<EventHandler*> m_event_handlers;
};

}
