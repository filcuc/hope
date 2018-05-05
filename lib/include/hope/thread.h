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

#include <condition_variable>
#include <thread>
#include <vector>

namespace hope {

class Object;
class EventLoop;

class Thread {
public:
    Thread();

    ~Thread();

    std::thread::id id() const;

    void start();

    void quit();

    void wait();

    void move_to_thread(std::unique_ptr<Object> obj);

private:
    void exec();

    enum class State {
        Starting,
        Started,
        Stopping,
        Stopped
    };

    mutable std::mutex m_mutex;
    State m_state = State::Stopped;
    std::condition_variable m_cond;
    std::thread m_thread;
    std::unique_ptr<EventLoop> m_event_loop = nullptr;
    std::vector<std::unique_ptr<Object>> m_children;
};

}
