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

#include <hope/private/atomicwrapper.h>
#include <hope/event.h>
#include <hope/object.h>

#include <algorithm>
#include <thread>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <map>
#include <vector>

namespace hope {

namespace detail { class ObjectData; }
namespace test { class EventLoopTestHelper; }

class EventLoop : public Object {
public:
    using Clock = std::chrono::steady_clock;
    using Mutex = std::mutex;
    using Locker = std::unique_lock<Mutex>;
    using TimePoint = std::chrono::time_point<Clock>;

    EventLoop();
  
    ~EventLoop() override;

    bool is_running() const;

    void push_event(std::unique_ptr<Event> event, TimePoint when = Clock::now());
    void push_event(std::unique_ptr<Event> event, std::chrono::milliseconds duration, TimePoint offset = Clock::now());

    void quit(int exit_code = 0);

    int exec();

    void register_object(Object* object);

    void unregister_object(Object* object);

    void on_event(Event *event) final;

private:
    friend class EventLoopTestHelper;

    int loop();

    void process_events(const std::vector<std::unique_ptr<Event>>& events);

    void cleanup_objects();

    int m_exit_code = 0;
    bool m_exit = false;
    bool m_is_running = false;
    mutable Mutex m_mutex;
    mutable Mutex m_objects_mutex;
    std::condition_variable m_cond;
    std::multimap<TimePoint, std::unique_ptr<Event>> m_events;
    std::map<Object*, hope::detail::AtomicWrapper<bool>> m_objects;
};

}
