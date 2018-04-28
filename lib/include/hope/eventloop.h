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

#pragma once

#include "hope/event.h"
#include "hope/eventhandler.h"

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

    std::thread::id thread_id() const;

private:
    int loop();

    const std::thread::id m_thread_id;
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
