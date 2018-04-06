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

    bool is_running() const;

    void push_event(std::unique_ptr<Event> event, TimePoint when = Clock::now());
    void push_event(std::unique_ptr<Event> event, std::chrono::milliseconds duration, TimePoint offset = Clock::now());

    void quit(int exit_code = 0);

    int exec();

    void register_event_handler(EventHandler* handler);

    void unregister_event_handler(EventHandler *handler);

private:
    void loop();

    int m_exit_code = 0;
    bool m_exit = false;
    bool m_is_running = false;
    mutable Mutex m_mutex;
    mutable Mutex m_dispatch_mutex;
    std::condition_variable m_cond;
    std::map<TimePoint, std::unique_ptr<Event>> m_events;
    std::vector<EventHandler*> m_event_handlers;
};

}
